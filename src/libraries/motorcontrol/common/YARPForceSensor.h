/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPForceSensor.h,v 1.8 2004-01-17 00:15:15 gmetta Exp $
///
///

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