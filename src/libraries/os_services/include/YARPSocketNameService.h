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
///
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
/// $Id: YARPSocketNameService.h,v 1.7 2003-05-12 23:32:43 gmetta Exp $
///
///

/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPSocketNameService_INC
#define YARPSocketNameService_INC

///
/// Socket name service/server interface and managin communication endpoints
///		is now implemented by two separate classes.
///
///	NOW means Apr 2003.
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include "YARPAll.h"
#include "YARPNameService.h"
#include "YARPNameClient.h"
#include "YARPNameID.h"
#include "YARPNameID_defs.h"
#include "YARPSocket.h"

///
///
/// LATER: This class becomes the real name service interface.
///
class YARPSocketNameService
{
public:
	/* zero if successful */
	/// register on the remote server and get the port and IP.
	static YARPUniqueNameID RegisterName (YARPNameClient& namer, const char *name, int reg_type = YARP_DEFAULT_PROTOCOL, int num_ports_needed = YARP_PROTOCOL_REGPORTS);

	/// get a remote port and ID of a given channel.
	static YARPUniqueNameID LocateName (YARPNameClient& namer, const char *name, int reg_type = YARP_DEFAULT_PROTOCOL);

	// static int GetAssignedPort();
	// static YARPNameID LocateName(const char *name);
};


///
///
/// endpoint manager, this handles the real socket/thread ID's.
///
class YARPSocketEndpointManager
{
public:
	static YARPNetworkObject *GetThreadSocket(void);

	///static int GetAssignedPort(void);	/// do I need it?

	/// allocates socket (either in or out).
	static YARPNameID CreateInputEndpoint(YARPUniqueNameID& name);
	static YARPNameID CreateOutputEndpoint(YARPUniqueNameID& name);
	static int ConnectEndpoints(YARPNameID& dest);
	static int Close(void);
	static int SetTCPNoDelay(void);
};

#endif


