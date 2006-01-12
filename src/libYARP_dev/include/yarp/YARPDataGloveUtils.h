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
/// $Id: YARPDataGloveUtils.h,v 1.3 2006-01-12 15:47:48 claudio72 Exp $
///
///

#ifndef __YARPDataGloveUtilsh__
#define __YARPDataGloveUtilsh__

#include <yarp/YARPConfig.h>

enum DataGloveCmd
{
	DGCMDGetData = 1,
	DGCMDStartStreaming = 2,
	DGCMDStopStreaming = 3,
	DGCMDGetLed = 4,
	DGCMDGetSwitch = 5,
	DGCMDResetGlove = 6,
	DGCMDSetLed = 7,
	
	DGCMDNCmds = 8
};

#include<yarp/begin_pack_for_net.h>

struct DataGloveData {
	DataGloveData(){ clean(); };
	void clean() {
		thumb[0]=0;  thumb[1]=0;  thumb[2]=0;
		index[0]=0;  index[1]=0;  index[2]=0;
		middle[0]=0; middle[1]=0; middle[2]=0;
		ring[0]=0;   ring[1]=0;   ring[2]=0;
		pinkie[0]=0; pinkie[1]=0; pinkie[2]=0;
		abduction[0]=0; abduction[1]=0; abduction[2]=0; abduction[3]=0; abduction[4]=0;
		palmArch=0;
		wrist[0]=0; wrist[1]=0;
	};
	int thumb[3];	// [inner, middle, outer]
	int index[3];	// [inner, middle, outer]
	int middle[3];	// [inner, middle, outer]
	int ring[3];	// [inner, middle, outer]
	int pinkie[3];	// [inner, middle, outer]

	int abduction[5]; // [thumb-index, index-middle, middle-ring, ring-pinkie, palm]

	int palmArch;
	int wrist[2]; // [pitch, yaw]
};

#include<yarp/end_pack_for_net.h>

#endif
