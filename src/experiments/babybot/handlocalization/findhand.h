#ifndef __FIND_HAND__
#define __FIND_HAND__

#include <yarp/YARPLogpolar.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPBlobDetector.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPConicFitter.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPThread.h>

#include "zerocrossing.h"

#include <iostream>
using namespace std;


class FindHand: public YARPThread
{
public:
	FindHand(const YARPString &portName = "/handlocalization");
	~FindHand();

	
	YARPInputPortOf<YARPGenericImage> _inPort;
	YARPOutputPortOf<YARPGenericImage> _outPort;
	YARPOutputPortOf<YARPGenericImage> _segmentedImagePort;
	YARPOutputPortOf<YVector> _outPixelPort;
	YARPInputPortOf<int[2]> _inPixelCoord;
	YARPOutputPortOf<YARPBabyBottle> _armDataPort;
	YARPInputPortOf<YARPControlBoardNetworkData> _motorStatusPort;

	YARPImageOf<YarpPixelMono> _actualLp;
	YARPImageOf<YarpPixelMono> _blob;
	YARPImageOf<YarpPixelBGR> _actualColored;
	YARPImageOf<YarpPixelMono> _detected;
	YARPImageOf<YarpPixelMono> _actualGrayscale;
	YARPImageOf<YarpPixelMono> _motion;
	YARPImageOf<YarpPixelBGR> _detectedCart;
	YARPImageOf<YarpPixelBGR> _detectedColored;
	YARPImageOf<YarpPixelMono> _detectedCartGrayscale;
	YARPImageOf<YarpPixelMono> *_output;
	YARPLpShapeRegion _pointsBlob;

	YARPImageOf<YarpPixelMono> _background;

	YARPBlobDetector _blobDetector;

	YARPLogpolar _mapper;

	unsigned char _threshold;
	
	YARPControlBoardNetworkData _motorStatus;
	YARPBabyBottle _armData;
	YVector _pixelOut;
	YVector _zeros;
	void Body();

	int _x;
	int _y;
	double _alpha;

	ZeroCrossing *_zeroCrossing;
	ZeroCrossing *_zeroCrossingHand;
	
	YARPLpConicFitter _fit;
	
	void startShake()
	{
		_lock();
		_shaking = true;
		_unlock();
	}

	void stopShake()
	{
		_lock();
		_shaking = false;
		_dumpDetection();
		_unlock();
	}

private:
	inline void _lock()
	{ _mutex.Wait(); }
	inline void _unlock()
	{ _mutex.Post(); }

	int _frame;
	int _nSegmentations;
	bool _shaking;
	YARPSemaphore _mutex;

	void _dumpDetection();
	bool _segmentation();
	void _sendSegmentation()
	{
		// display
		// _segmentedImagePort.Content().Refer(_actualLp);
		_segmentedImagePort.Content().Refer(_blob);
		// _segmentedImagePort.Content().Refer(_detected);
		_segmentedImagePort.Write();

		_armDataPort.Content() = _armData;
		_armDataPort.Content().display();
		_armDataPort.Write();
	}

	void _mask(YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out)
	{
		int rho;
		int theta;
		unsigned char *tmpIn;
		unsigned char *tmpOut;
		for(rho = 0; rho<_logpolarParams::_srho; rho++)
		{
			tmpIn = (unsigned char *) in.GetArray()[rho];
			tmpOut = (unsigned char *) out.GetArray()[rho];
			for(theta = 0; theta<_logpolarParams::_stheta; theta++)
			{
				if (*tmpIn == 0)
					*tmpOut = 0;
				tmpIn++;
				tmpOut++;
			}
		}
	}

	void _mask(YARPImageOf<YarpPixelMono> &in, const YARPLpShapeRegion &mask, YARPImageOf<YarpPixelMono> &out)
	{
		int k = 0;
		while(k<mask.n)
		{
			int r,t;
			r = mask.r[k];
			t = mask.t[k];
			out(t,r) = in(t,r);
			k++;
		}
	}

	void _readInputPorts()
	{
		// input images
		_inPort.Read();
		////////////////////////// pixel coord
		if (_inPixelCoord.Read(0))
		{
			_x = _inPixelCoord.Content()[0];
			_y = _inPixelCoord.Content()[1];

			if (_x > (_logpolarParams::_stheta-1) ) _x = (_logpolarParams::_stheta-1);
			if (_y > (_logpolarParams::_srho-1) ) _y = (_logpolarParams::_srho-1);

			cout << _x << '\t' << _y << endl;
		}

		///////////////// read hand status
		if (_motorStatusPort.Read(0))
		{
			_motorStatus = _motorStatusPort.Content();
		}
	}

	void _writeOutputPorts()
	{
		_outPixelPort.Content() = _pixelOut;
		_outPixelPort.Write();
		_outPort.Content().Refer(_motion);
		_outPort.Write();
	}
	
	void _computePeriodicity()
	{
		if (!_shaking)
			return;

		int x;
		int y;
		int j = 0;
		double dummy;

		_zeroCrossingHand[0].find(fabs(_motorStatus._velocity(1)), &dummy);
		_zeroCrossingHand[1].find(fabs(_motorStatus._velocity(2)), &dummy);
		_zeroCrossingHand[2].find(fabs(_motorStatus._velocity(3)), &dummy);
		_zeroCrossingHand[3].find(fabs(_motorStatus._velocity(4)), _pixelOut.data()+3);
		_zeroCrossingHand[4].find(fabs(_motorStatus._velocity(5)), &dummy); //_pixelOut.data()+3);//_pixelOut.data()+1);
		_zeroCrossingHand[5].find(fabs(_motorStatus._velocity(6)), &dummy);

		unsigned char *motion;
		for(y = 0/*_logpolarParams::_sfovea*/; y<_logpolarParams::_srho; y++)
		{
			motion = (unsigned char *) _motion.GetArray()[y];
			for(x = 0; x<_logpolarParams::_stheta; x++)
			{
				_zeroCrossing[j].find((*motion)/255.0, &dummy);

				double motionMean;
				double motionStd;
				int n;
				n = _zeroCrossing[j].result(&motionMean, &motionStd);

				if ( (x == _x) && (y == _y) )
				{
					_pixelOut(2) = dummy;
				}

				/*
				//////////// detection
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
						_detected(x,y) = 255;//_actualLp(x,y);
					}
					else
						_detected(x,y) = 0;
				}*/
				j++;
				motion++;
			}
		}	
		// display
	//	_segmentedImagePort.Content().Refer(_detected);
	//	_segmentedImagePort.Write();
	}
};
#endif