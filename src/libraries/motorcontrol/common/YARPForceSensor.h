// by nat June 2003

#ifndef __YARPForceSensorh__
#define __YARPForceSensorh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include "YARPJR3Adapter.h"

template <class  ADAPTER, class PARAMETERS>
class YARPForceSensor
{
	public:
		YARPForceSensor()
		{
			_reading = new short [6];
		}
		~YARPForceSensor()
		{
			delete [] _reading;
		}
		int initialize()
		{ return _adapter.initialize(_params); }
		int uninitialize()
		{ return _adapter.uninitialize(); }
		int read(double *f, double *t)
		{	
			short r[6];
			_adapter.IOCtl(CMDJR3ReadData, _reading);
			int i = 0;
			int j = 0;
			for(i = 0; i<3; i++)
			{
				f[i] = (double) r[j];
				j++;
				t[i] = (double) r[j];
				j++;
			}
			return YARP_OK;
		}

	protected:
		ADAPTER _adapter;
		PARAMETERS _params;

		short *_reading;
};

typedef YARPForceSensor<YARPJR3Adapter, YARPJR3Params> YARPBabybotForceSensor;

#endif