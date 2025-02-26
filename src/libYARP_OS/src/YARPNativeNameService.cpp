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
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNativeNameService.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///
//// #include <ace/OS.h>

///
///	this compiles into something similar for QNX6 that uses the global name server anyway.
///
#include <yarp/YARPNativeNameService.h>
#ifdef __QNX6__
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#endif

///
/// need to create a channel first, then register into the name server.
///
int YARPNativeEndpointManager::CreateQnetChannel (void)
{
#ifndef __QNX6__
	return YARP_FAIL;
#else
	int chid;
	if ((chid = ChannelCreate(0)) != -1)
	{
		ACE_DEBUG ((LM_DEBUG, "ChannelCreate: created channel with id : %d\n", chid));
		return chid;
	}	
	return YARP_FAIL;
#endif
}

///
///
///
int YARPNativeEndpointManager::CreateInputEndpoint (YARPUniqueNameID& name)
{
	ACE_UNUSED_ARG(name);
#ifndef __QNX6__
	return YARP_FAIL;
#else
	return YARP_OK;
#endif
}

int YARPNativeEndpointManager::CreateOutputEndpoint(YARPUniqueNameID& name)
{
#ifndef __QNX6__
	ACE_UNUSED_ARG(name);
	return YARP_FAIL;
#else
	YARPUniqueNameQnx& nameq = (YARPUniqueNameQnx&)name;

	int coid = ConnectAttach( 
						netmgr_strtond (nameq.getNode().c_str(),NULL),
						nameq.getPid(),
						nameq.getChannelID(),
						0,
						0);

	ACE_DEBUG ((LM_DEBUG, "ConnectAttach: %s(%d), %d %d, returned %d\n",
		nameq.getNode().c_str(),
		netmgr_strtond (nameq.getNode().c_str(), NULL),
		nameq.getPid(), nameq.getChannelID(),
		coid));

	if (coid != -1)
	{
		nameq.setRawIdentifier(coid);
		return YARP_OK; ///name.getNameID();
	}

	/// failed.
	///return YARPNameID();
	return YARP_OK;
#endif
}

int YARPNativeEndpointManager::ConnectEndpoints(YARPUniqueNameID& dest, const YARPString& own_name)
{
	ACE_UNUSED_ARG(dest);
	ACE_UNUSED_ARG(own_name);
#ifndef __QNX6__
	return YARP_FAIL;
#else
	return YARP_OK;
#endif
}

int YARPNativeEndpointManager::Close(YARPUniqueNameID& id)
{
#ifndef __QNX6__
	ACE_UNUSED_ARG(id);
	return YARP_FAIL;
#else
	return ConnectDetach (id.getRawIdentifier());
#endif
}

///
///
///
YARPUniqueNameID* YARPNativeNameService::RegisterName(const char *name)
{
	ACE_UNUSED_ARG(name);
	return NULL;
}

YARPUniqueNameID* YARPNativeNameService::LocateName(const char *name)
{
	ACE_UNUSED_ARG(name);
	return NULL;
}


int YARPNativeNameService::IsNonTrivial(void)
{
#ifndef __QNX6__
	return 0;
#else
	return 1;
#endif
}
