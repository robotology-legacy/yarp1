// by nat June 2003

#ifndef __YARPForceSensorh__
#define __YARPForceSensorh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include "YARPJR3Adapter.h"
#include <YARPMath.h>

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
		int initialize(const std::string file)
		{ 
			int ret;
			ret = _params.load(std::string(""),file);
			if (ret == YARP_FAIL)
				return ret;

			ret = _adapter.initialize();
			return ret;
		}
		int uninitialize()
		{ return _adapter.uninitialize(); }

		int readRaw(double *f, double *t)
		{	
			_adapter.IOCtl(CMDJR3ReadData, _reading);
			int i = 0;
			int j = 0;
			for(i = 0; i<3; i++)
			{
				f[i] = (double) _reading[j];
				t[i] = (double) _reading[j+3];
				j++;
			}
			return YARP_OK;
		}

		int read(double *f, double *t)
		{	
			_adapter.IOCtl(CMDJR3ReadData, _reading);
			int i = 0;
			int j = 0;
			
			for(i = 0; i < 3; i++)
			{
				// matrix product between _reading and _R
				f[i] = (_reading[0]/_params._max) * _params._R[i][0];
				f[i] += (_reading[1]/_params._max) * _params._R[i][1];
				f[i] += (_reading[2]/_params._max) * _params._R[i][2];
			
				t[i] = (_reading[3]/_params._max) * _params._R[i][0];
				t[i] += (_reading[4]/_params._max) * _params._R[i][1];
				t[i] += (_reading[5]/_params._max) * _params._R[i][2];
			}
						
			return YARP_OK;
		}

		int read(double *f)
		{	
			_adapter.IOCtl(CMDJR3ReadData, _reading);
			int i = 0;
			int j = 0;
			
			for(i = 0; i < 3; i++)
			{
				// matrix product between _reading and _R
				f[i] = (_reading[0]/_params._max) * _params._R[i][0];
				f[i] += (_reading[1]/_params._max) * _params._R[i][1];
				f[i] += (_reading[2]/_params._max) * _params._R[i][2];
			
				f[i+3] = (_reading[3]/_params._max) * _params._R[i][0];
				f[i+3] += (_reading[4]/_params._max) * _params._R[i][1];
				f[i+3] += (_reading[5]/_params._max) * _params._R[i][2];
			}
						
			return YARP_OK;
		}

		int read(YVector &f)
		{ return read(f.data()); }

		int read(YVector &f, YVector &t)
		{ return read(f.data(), t.data()); }

	protected:
		ADAPTER _adapter;
		PARAMETERS _params;

		short *_reading;

};

typedef YARPForceSensor<YARPJR3Adapter, YARPJR3Params> YARPBabybotForceSensor;

#endif