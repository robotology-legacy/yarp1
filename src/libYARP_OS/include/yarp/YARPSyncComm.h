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
/// $Id: YARPSyncComm.h,v 1.2 2004-07-02 08:47:06 eshuy Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
*/

#ifndef YARPSyncComm_INC
#define YARPSyncComm_INC

#include <yarp/conf/YARPConfig.h>
#include <yarp/YARPAll.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPMultipartMessage.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
/// encapsulating either QNet or Socket comm layer.
///
class YARPSyncComm
{
public:
	static int Send(const YARPNameID& dest, char *buffer, int buffer_length, char *return_buffer, int return_buffer_length);
	static YARPNameID BlockingReceive(const YARPNameID& src, char *buffer, int buffer_length);
	static YARPNameID PollingReceive(const YARPNameID& src, char *buffer, int buffer_length);
	static int ContinuedReceive(const YARPNameID& src, char *buffer, int buffer_length);
	static int Reply(const YARPNameID& src, char *buffer, int buffer_length);
	static int Send(const YARPNameID& dest, YARPMultipartMessage& msg, YARPMultipartMessage& return_msg);
	static YARPNameID BlockingReceive(const YARPNameID& src, YARPMultipartMessage& msg);
	static YARPNameID PollingReceive(const YARPNameID& src, YARPMultipartMessage& msg);
	static int Reply(const YARPNameID& src, YARPMultipartMessage& msg);
};

#endif

