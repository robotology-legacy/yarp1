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
/// $Id: YARPTouchBoard.h,v 1.1 2004-07-28 17:17:35 babybot Exp $
///
///


// July 2003 -- by nat

// $Id: YARPTouchBoard.h,v 1.1 2004-07-28 17:17:35 babybot Exp $

#ifndef __YARPTOUCH_BOARD__
#define __YARPTOUCH_BOARD__

#include <ace/log_msg.h>
#include <yarp/YARPAndroidDeviceDriver.h>
#include <yarp/YARPTouchBoardParameters.h>
#include <string>	/// DEPRECATED!

#define YARP_TOUCH_BOARD_VERBOSE

#ifdef YARP_TOUCH_BOARD_VERBOSE
#define YARP_TOUCH_BOARD_VERBOSE_DEBUG(string) YARP_DEBUG("YARP_TOUCH_BOARD_VERBOSE_DEBUG:", string)
#else  YARP_TOUCH_BOARD_VERBOSE_DEBUG(string) YARP_NULL_DEBUG
#endif

class YARPTouchBoard : public YARPAndroidDeviceDriver
{
public:
	YARPTouchBoard()
	{
		_initialized = false;
	}
	
	~YARPTouchBoard()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(const std::string &filename)
	{
		_parameters.load("", filename);
		
		AndroidOpenParameters opar;
		opar.pport = _parameters._port;
		open(&opar);
	
		AndroidScanSetupParameters spar;
		spar.nBoard = _parameters._board;
		IOCtl(CMDScanSetup, &spar);
		
		_nSensors = _parameters._nS;

		_initialized = true;
		return YARP_OK;
	}

	int read(double *out)
	{
		IOCtl(CMDAIReadScan, _temp_char);
	
		_convertOutput(_temp_char, out);

		return YARP_OK;
	}

	int _convertOutput(const unsigned char *in, double *out)
	{
		for(int i = 0; i < _nSensors; i++)
			out[i] = 255-in[_parameters._lut[i]];
		return YARP_OK;
	}

	int uninitialize()
	{
		close();

		_initialized = false;
		return YARP_OK;
	}
	
private:

	bool _initialized;
	unsigned char _temp_char[__AndroidNInput];
	int _nSensors;
	YARPTouchBoardParameters _parameters;
};

#endif