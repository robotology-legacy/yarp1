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
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: CollectorCommands.h,v 1.8 2006-01-12 15:48:46 claudio72 Exp $
///
///

#ifndef __CollectorCommandsh__
#define __CollectorCommandsh__

// ------------------------- WARNING
// so far, there is NO GUARANTEE on the streaming frequency. It seems
// that, connecting all peripherals, we get to a reasonable 25Hz, but
// I have found no sensible means to stabilise it - not even putting
// a delay in the streaming thread. TO BE EVENTUALLY FIXED.
// -------------------------

#include<yarp/begin_pack_for_net.h>

typedef enum {
	CCmdFailed         = 0,
	CCmdSucceeded      = 1,
	CCmdGetData        = 2,
	CCmdStartStreaming = 3,
	CCmdStopStreaming  = 4,
	CCmdConnect        = 5,
	CCmdDisconnect     = 6,
	CCmdQuit           = 7
} CollectorCommand;

typedef struct CollectorNumericalDataStruct {
	CollectorNumericalDataStruct() { clean(); };
	void clean() {
		tracker0Data.clean();
		tracker1Data.clean();
		GTData.clean();
		gloveData.clean();
		pressureData.clean();
	};
	TrackerData     tracker0Data;
	TrackerData     tracker1Data;
	GazeTrackerData GTData;
	DataGloveData   gloveData;
	PresSensData    pressureData;
} CollectorNumericalData;

typedef YARPImageOf<YarpPixelBGR> CollectorImage;

const int HardwareUseDataGlove = 0x01;
const int HardwareUseTracker0  = 0x02;
const int HardwareUseTracker1  = 0x04;
const int HardwareUsePresSens  = 0x08;
const int HardwareUseCamera0   = 0x10;
const int HardwareUseCamera1   = 0x20;
const int HardwareUseGT        = 0x40;

#include<yarp/end_pack_for_net.h>

#endif
