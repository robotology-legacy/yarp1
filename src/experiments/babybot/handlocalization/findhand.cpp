#include "findHand.h"

#include <YARPColorConverter.h>

using namespace _logpolarParams;

const double __threshold = 10/255.0;
const double __thresholdMotor = 0.2;
const int _sCol = 256;
const int _npixels = _stheta*(_srho);//-_sfovea);

FindHand::FindHand(const YARPString &portName):
_inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_outPixelPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_motorStatusPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_inPixelCoord(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_segmentedImagePort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_armDataPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_blobDetector((float) 0.35)
{
	_inPort.Register((portName+"/i:img").c_str(), "Net1");
	_outPort.Register((portName+"/o:img").c_str());
	_outPixelPort.Register((portName+"/o:pixel").c_str());
	_motorStatusPort.Register((portName+"/i:motor").c_str(), "Net0");
	_inPixelCoord.Register((portName+"/i:point").c_str());
	_segmentedImagePort.Register((portName+"/segmentation/o:img").c_str());
	_armDataPort.Register((portName+"/segmentation/o:armdata").c_str());

	_actualColored.Resize (_stheta, _srho);
	_detected.Resize(_stheta, _srho);
	_detectedColored.Resize(_stheta, _srho);
	_actualGrayscale.Resize(_stheta, _srho);
	_motion.Resize(_stheta, _srho);
	_background.Resize(_stheta, _srho);
	_background.Zero();

	_blob.Resize(_stheta, _srho);
	_blob.Zero();

	_blobDetector.resize(_stheta,_srho, _sfovea);
	_detectedCart.Resize(_xsize, _ysize);
	_detectedCartGrayscale.Resize(_xsize, _ysize);

	_motorStatus.resize(40, 6);

	_pixelOut.Resize(4);
	
	_frame = 0;
	_nSegmentations = 0;

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
	_mapper.ReconstructGrays (_actualLp, _actualGrayscale); //Colored);
		
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

	_pixelOut(1) = _motion(_x, _y)/255.0;
	_pixelOut(2) = 0.0;
	_pixelOut(3) = fabs(_motorStatus._velocity(4));
	_pixelOut(4) = 0.0;

	_computePeriodicity();

	//  prepare motion for display
	//	iplMultiplyS(_motion, _motion, 4/*_threshold*/);
	
	_writeOutputPorts();

	_unlock();

	_frame++;
	if ((_frame%100)==0)
		printf("frame #%5d\r", _frame);
	}
}

bool FindHand::_segmentation()
{
	_blob.Zero();
	char detected[128];
	char blob[128];
	char segmented[128];
	char complete[128];
	bool valid;
	_nSegmentations++;
	sprintf(detected, "%s%d.ppm", "y:\\zgarbage\\detected",_nSegmentations);
	sprintf(blob, "%s%d.ppm", "y:\\zgarbage\\blob", _nSegmentations);
	sprintf(segmented, "%s%d.ppm", "y:\\zgarbage\\segmented", _nSegmentations);
	sprintf(complete, "%s%d.ppm", "y:\\zgarbage\\complete", _nSegmentations);
	
	_blobDetector.filterLp(_detected);
	YARPImageFile::Write(detected, _detected);
	YARPImageOf<YarpPixelMono> &blobbed = _blobDetector.getSegmented();
	YARPImageFile::Write(blob, blobbed);
	
	YARPImageFile::Write(complete, _actualLp);

	_mask(blobbed, _detected);
	int t0;
	int r0;
	double a11, a12, a22;
	int x,y;
		
	_fit.fitEllipse(_detected, &t0, &r0, &a11, &a12, &a22);
	_fit.findEllipse(t0, r0, a11, a12, a22, _pointsBlob);
	_mapper.Logpolar2Cartesian(r0, t0, x, y);
	if (_pointsBlob.n > 10)
		valid = true;
	else 
		valid = false;

	_mask(_actualLp, _pointsBlob, _blob);
	_armData.reset();
	_armData.writeInt(x);
	_armData.writeInt(y);
	_armData.writeFloat(a11);
	_armData.writeFloat(a12);
	_armData.writeFloat(a22);
	
	if (valid)
		YARPImageFile::Write(segmented, _blob);

	return valid; 
}
/*
void FindHand::_segmentation()
{
	char detected[128];
	char blob[128];
	char segmented[128];
	char complete[128];
	_blob.Zero();

	_nSegmentations++;
	sprintf(detected, "%s%d.ppm", "y:\\zgarbage\\detected",_nSegmentations);
	sprintf(blob, "%s%d.ppm", "y:\\zgarbage\\blob", _nSegmentations);
	sprintf(segmented, "%s%d.ppm", "y:\\zgarbage\\segmented", _nSegmentations);
	sprintf(complete, "%s%d.ppm", "y:\\zgarbage\\complete", _nSegmentations);
	
	// _blobDetector.filterLp(_detected);
	YARPImageFile::Write(detected, _detected);
	// YARPImageOf<YarpPixelMono> &tmp = _blobDetector.getSegmented();

	// fit a circle in to the segmented region
	int r0, t0;
	double a11, a12, a22;
	_fit.fitEllipse(_detected, &t0,  &r0, &a11, &a12, &a22);
	_fit.plotEllipse(t0, r0, a11, a12, a22, _blob);
	_fit.findEllipse(t0, r0, a11, a12, a22, _pointsBlob);

	YARPImageFile::Write(blob, _blob);
	YARPImageFile::Write(complete, _actualLp);
	
	int k = 0;
	while(k<_pointsBlob.n)
	{
		int r,t;
		r = _pointsBlob.r[k];
		t = _pointsBlob.t[k];
		_blob(t,r) = _actualLp(t,r);
		k++;
	}
		
	YARPImageFile::Write(segmented, _blob);
}*/

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

	_detected.Zero();

	// for each pixel
	int j = 0;
	for(y = 0; y < _srho; y++)
	{
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
				if ( (fabs(motionMean-motorMean) < 0.3) &&
					(abs(nMotor-n) < 4) &&
					(nMotor > 3) /* &&
					(motorStd>2) &&
					(motionStd>1.5)*/ )
				{
					// segmented image, B/N
					_detected(x,y) = 255; //_actualLp(x,y);
				}
			}
			j++;
		}
	}

	// reset motor zero crossing
	for(m = 0; m < 6; m++)
	{
		_zeroCrossingHand[m].reset();

	}
	if (_segmentation())
		_sendSegmentation();
}
