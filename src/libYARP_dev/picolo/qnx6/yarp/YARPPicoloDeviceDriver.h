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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPPicoloDeviceDriver.h,v 1.2 2004-11-23 17:12:32 beltran Exp $
///
///

#ifndef __YARPPicoloDeviceDriverh__
#define __YARPPicoloDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPSemaphore.h>

#include <stdlib.h>
#include <string.h>
#include <bttv.h>

struct PicoloOpenParameters
{
	/// add here params for the open.
	PicoloOpenParameters()
	{
		_unit_number = 0;
		_video_type = 0;
		_size_x = 256;
		_size_y = 256;
		_offset_y = 0;
		_offset_x = 0;
	}

	int _unit_number;		/// board number 0, 1, 2, etc.
	int _video_type;		/// 0 composite, 1 svideo.
	int _size_x;			/// requested size x.
	int _size_y;			/// requested size y.
	int _offset_y;			/// y offset, with respect to the center 
	int _offset_x;			/// x offset, with respect to the center
};

class YARPPicoloDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPPicoloDeviceDriver>
{
	private:
		YARPPicoloDeviceDriver(const YARPPicoloDeviceDriver&);
		void operator=(const YARPPicoloDeviceDriver&);

	public:
		YARPPicoloDeviceDriver();
		~YARPPicoloDeviceDriver();

		// overload open, close
		virtual int open(void *d);
		virtual int close(void);

		virtual int acquireBuffer(void *);
		virtual int releaseBuffer(void *);
		virtual int waitOnNewFrame (void *cmd);
		virtual int getWidth(void *cmd);
		virtual int getHeight(void *cmd);
		virtual int setBright(void *cmd);
		virtual int setHue(void *cmd);
		virtual int setContrast(void *cmd);
		virtual int setSatU(void *cmd);
		virtual int setSatV(void *cmd);
		virtual int setLNotch(void * cmd);
		virtual int setLDec(void * cmd);
		virtual int setPeak(void * cmd);
		virtual int setCagc(void * cmd);
		virtual int setCkill(void * cmd);
		virtual int setRange(void * cmd);
		virtual int setYsleep(void * cmd);
		virtual int setCsleep(void * cmd);
		virtual int setCrush(void * cmd);
		virtual int setGamma(void * cmd);
		virtual int setDithFrame(void * cmd);

	protected:
		void *system_resources;
};
#endif
