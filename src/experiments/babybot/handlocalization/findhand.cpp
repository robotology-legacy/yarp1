#include "findHand.h"

using namespace _logpolarParams;

const double __threshold = 10/255.0;
const double __thresholdMotor = 0.2;
const int _sCol = 256;
const int _npixels = _stheta*(_srho);//-_sfovea);

FindHand::FindHand(const YARPString &portName):
_inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_outPixelPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_handStatusPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_inPixelCoord(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_segmentedImagePort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
{
	_inPort.Register((portName+"/i:img").c_str());
	_outPort.Register((portName+"/o:img").c_str());
	_outPixelPort.Register((portName+"/o:pixel").c_str());
	_handStatusPort.Register((portName+"/i:hand").c_str());
	_armStatusPort.Register((portName+"/i:arm").c_str());
	_inPixelCoord.Register((portName+"/i:point").c_str());
	_segmentedImagePort.Register((portName+"/segmentation/o:img").c_str());

	_actualColored.Resize (_stheta, _srho);
	_segmented.Resize(_stheta, _srho);
	_actualGrayscale.Resize(_stheta, _srho);
	_motion.Resize(_stheta, _srho);
	_background.Resize(_stheta, _srho);
	_background.Zero();

	_handSpeed.Resize(6);
	_armSpeed.Resize(6);
	_pixelOut.Resize(4);
	
	_frame = 0;

	_x = 0;
	_y = 0;
	_alpha = 0.3;
	_shaking = false;

	_zeroCrossing = new ZeroCrossing[_npixels];
	_zeroCrossingHand= new ZeroCrossing[6];

	int i;
	for(i = 0; i < _npixels; i++)
		_zeroCrossing[i].setThreshold(__threshold);

	for(i = 0; i < 6; i ++)
		_zeroCrossingHand[i].setThreshold(__thresholdMotor);

	_output = new YARPImageOf<YarpPixelMono>[3];
	for(i = 0; i < 3; i ++)
	{
		_output[i].Resize(_stheta, _srho);
		_output[i].Zero();
	}
}

FindHand::~FindHand()
{
	delete [] _zeroCrossing;
	delete [] _zeroCrossingHand;

	int i;
	for(i = 0; i < 3; i ++)
		delete _output[i];
		
	delete [] _output;

}

void FindHand::Body()
{
	while (!IsTerminated())
	{
	_lock();
	// read ports
	_readInputPorts();
	// new image
	
	_actualLp.Refer (_inPort.Content());

	// reconstruct color
	_mapper.ReconstructColor (_actualLp, _actualColored);
	iplColorToGray(_actualColored, _actualGrayscale);
	// iplSubtract(_actualGrayscale, _background, _motion);
	// iplAbs(_motion, _motion);
	
	// background
	int i;
	unsigned char *bck = (unsigned char *) _background.GetAllocatedArray();
	unsigned char *img = (unsigned char *) _actualGrayscale.GetAllocatedArray();
	unsigned char *motion = (unsigned char *) _motion.GetAllocatedArray();
	for(i = 0; i < _background.GetAllocatedDataSize(); i++)
	{
		*motion = abs(*bck-*img);						// difference, img and background
		*bck = (*img)*_alpha + (*bck)*(1-_alpha);		// new background
		bck++;
		img++;
		motion++;
	}
	// store old img
	/*
	iplMultiplySScale(_actualGrayscale, _actualGrayscale, int (255*_alpha+0.5));
	iplMultiplySScale(_background, _background, int (255*(1-_alpha)+0.5));
	iplAdd(_background, _actualGrayscale, _background);*/
	

	_pixelOut(1) = _motion(_x, _y)/255.0;
	_pixelOut(3) = fabs(_handSpeed(4));

	_computePeriodicity();
		
	// prepare motion for display
	iplMultiplyS(_motion, _motion, 4/*_threshold*/);
	
	_writeOutputPorts();

	_unlock();

	_frame++;
	if ((_frame%100)==0)
		printf("frame #%5d\r", _frame);
	}
}

void FindHand::_dumpDetection()
{
	_pixelOut(4) = 0;
	_pixelOut(2) = 0;

	int x;
	int y = 0;

	// erase images
	int m;
	for(m = 0; m < 6; m++)
	{
		double motorMean;
		double motorStd;
		int nMotor;
		nMotor = _zeroCrossingHand[m].result(&motorMean, &motorStd);

		cout << "--> " << m << endl;
		cout << nMotor << "\t" << motorMean << "\t" << motorStd << endl;
	}

	_segmented.Zero();

	// for each pixel
	int j = 0;
	for(y = 0; y < _srho; y++)
		for(x = 0; x < _stheta; x++)
		{
			double motionMean;
			double motionStd;
			int n;
			n = _zeroCrossing[j].result(&motionMean, &motionStd);
			_zeroCrossing[j].reset();

			if ( (x == _x) && (y == _y) )
			{
				cout << "--> Selected pixel"<< endl;
				cout << n << "\t" << motionMean << "\t" << motionStd << endl;
			}
		
			int m;
			for(m = 0; m < 6; m++)
			{
				double motorMean;
				double motorStd;
				int nMotor;
				nMotor = _zeroCrossingHand[m].result(&motorMean, &motorStd);
				if ( (fabs(motionMean-motorMean) < 0.3) && (abs(nMotor-n) < 2) && (nMotor > 0) )
				{
					// segmented image, B/N
					_segmented(x,y) = 255;//_actualLp(x,y);
				}
			}
			j++;
		}

	// reset motor zero crossing
	for(m = 0; m < 6; m++)
	{
		_zeroCrossingHand[m].reset();

	}
	// display
	_segmentedImagePort.Content().Refer(_segmented);
	_segmentedImagePort.Write();
}
