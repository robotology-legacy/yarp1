/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #by nat June 2003#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPGenericForceSensor.h,v 1.2 2004-09-10 15:14:06 babybot Exp $
///
///

#ifndef __YARPGenericForceSensorh__
#define __YARPGenericForceSensorh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>


template <class  ADAPTER, class PARAMETERS>
class YARPGenericForceSensor
{
	public:
		YARPGenericForceSensor()
		{
			_reading = new short [6];
		}

		~YARPGenericForceSensor()
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
		
		int uninitialize() { return _adapter.uninitialize(); }

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

		///int read(YVector &f) { return read(f.data()); }
		///int read(YVector &f, YVector &t) { return read(f.data(), t.data()); }

	protected:
		ADAPTER _adapter;
		PARAMETERS _params;

		short *_reading;

};

#endif