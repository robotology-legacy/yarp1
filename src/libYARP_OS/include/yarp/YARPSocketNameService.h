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
/// $Id: YARPSocketNameService.h,v 1.3 2004-07-09 13:45:59 eshuy Exp $
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

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPAll.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPNameClient.h>
#include <yarp/YARPNameID.h>
#include <yarp_private/YARPNameID_defs.h>
#include <yarp/YARPSocket.h>

///
///
/// LATER: This class becomes the real name service interface.
///
class YARPSocketNameService
{
public:
	/// register on the remote server and get the port and IP.
	static YARPUniqueNameID* RegisterName (YARPNameClient& namer, const char *name, const char *network_name, int reg_type = YARP_DEFAULT_PROTOCOL, int extra_param = YARP_PROTOCOL_REGPORTS);

	/// get a remote port and ID of a given channel.
	static YARPUniqueNameID* LocateName (YARPNameClient& namer, const char *name, const char *network_name, int reg_type = YARP_DEFAULT_PROTOCOL);

	/// unregister from the name server.
	static int UnregisterName (YARPNameClient& namer, const char *name, int reg_type);

	/// verifies <ip> and <netname> belong to the same network.
	static bool VerifySame (YARPNameClient& namer, const char *ip, const char *netname, YARPString& if_name);

	/// verifies <rem_ip> and <loc_ip> belong to the same machine.
	static bool VerifyLocal (YARPNameClient& namer, const char *rem_ip, const char *loc_ip, const char *netname);
};


///
///
/// endpoint manager, this handles the real socket/thread ID's.
///
class YARPSocketEndpointManager
{
public:
	static YARPNetworkObject *GetThreadSocket(void);

	/// allocates socket (either in or out).
	static int CreateInputEndpoint(YARPUniqueNameID& name);
	static int CreateOutputEndpoint(YARPUniqueNameID& name);
	static int ConnectEndpoints(YARPUniqueNameID& dest);
	static int Close(YARPUniqueNameID& dest);
	static int CloseMcastAll(void);
	static int GetNumberOfClients(void);
	static int SetTCPNoDelay(void);
};

#endif


