#ifndef __FIND_HAND__
#define __FIND_HAND__

#include <YARPLogpolar.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPImages.h>
#include <YARPSemaphore.h>
#include "zerocrossing.h"

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
	YARPInputPortOf<YVector> _handStatusPort;
	YARPInputPortOf<YVector> _armStatusPort;

	YARPImageOf<YarpPixelMono> _actualLp;
	YARPImageOf<YarpPixelBGR> _actualColored;
	YARPImageOf<YarpPixelMono> _segmented;
	YARPImageOf<YarpPixelMono> _actualGrayscale;
	YARPImageOf<YarpPixelMono> _motion;
	YARPImageOf<YarpPixelMono> *_output;

	YARPImageOf<YarpPixelMono> _background;

	YARPLogpolar _mapper;

	unsigned char _threshold;
	
	YVector _handSpeed;
	YVector _armSpeed;
	YVector _pixelOut;
	YVector _zeros;
	void Body();

	int _x;
	int _y;
	double _alpha;

	ZeroCrossing *_zeroCrossing;
	ZeroCrossing *_zeroCrossingHand;
	
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
	inline _lock()
	{ _mutex.Wait(); }
	inline _unlock()
	{ _mutex.Post(); }

	int _frame;
	bool _shaking;
	YARPSemaphore _mutex;

	void _dumpDetection();
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
		}

		///////////////// read hand status
		if (_handStatusPort.Read(0))
			memcpy(_handSpeed.data(), _handStatusPort.Content().data(), 6*sizeof(double));
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

		for(y = 0/*_logpolarParams::_sfovea*/; y<_logpolarParams::_srho; y++)
			for(x = 0; x<_logpolarParams::_stheta; x++)
			{
				_zeroCrossing[j].find(_motion(x,y)/255.0, &dummy);
				if ( (x == _x) && (y == _y) )
				{
					_pixelOut(2) = dummy;
				}
				j++;
			}
			
		_zeroCrossingHand[0].find(fabs(_handSpeed(1)), &dummy);
		_zeroCrossingHand[1].find(fabs(_handSpeed(2)), &dummy);
		_zeroCrossingHand[2].find(fabs(_handSpeed(3)), &dummy);
		_zeroCrossingHand[3].find(fabs(_handSpeed(4)), &dummy);
		_zeroCrossingHand[4].find(fabs(_handSpeed(5)), _pixelOut.data()+3);//_pixelOut.data()+1);
		_zeroCrossingHand[5].find(fabs(_handSpeed(6)), &dummy);
	}
};

#endif