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
/// $Id: YARPNameService.cpp,v 1.15 2003-05-28 17:42:00 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/OS.h>

#include <string.h>
#include <fstream>

#ifndef __QNX4__
using namespace std;
#endif

#include "YARPNameService.h"
#include "YARPSemaphore.h"
#include "YARPSocket.h"
#include "YARPSocketNameService.h"
#include "YARPNativeNameService.h"
#include "YARPNameID.h"
#include "YARPNameClient.h"

#include "wide_nameloc.h"
#include "YARPNameID_defs.h"

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

static YARPSemaphore mutex(1);

/// not very elegant, legacy of old impl.
/// LATER: do it differently.
static bool _init_nameserver = true;
static YARPNameClient * _namer = NULL;
static YARPNameService _justtoinitialize;

///static int is_connected = 0, tried_to_connect = 0;
///static int registration_mode = YARP_NO_SERVICE_AVAILABLE;
///static YARPOutputSocket namer;

#define SCATTERSHOT


char * GetYarpRoot (void)
{
	char * ret = ACE_OS::getenv ("YARP_ROOT");

	if (ret == NULL)
	{
		ACE_DEBUG ((LM_DEBUG, "::GetYarpRoot : can't retrieve YARP_ROOT env variable\n"));
	}
	return ret;
}

///
///
///
///
int YARPNameService::ConnectNameServer(const char *name)
{
	return YARP_OK;
}

///
///
///
///
int YARPNameService::Initialize (void)
{
	if (!_init_nameserver)
	{
		ACE_DEBUG ((LM_DEBUG, "Name client service already initialized\n"));
		return YARP_FAIL;
	}

	/// handle the connection w/ the remote name server.
	char buf[256];
	ACE_ASSERT (GetYarpRoot() != NULL);
#ifdef __WIN32__
	ACE_OS::sprintf (buf, "%s\\%s\0", GetYarpRoot(), NAMER_CONFIG_FILE);
#else
	ACE_OS::sprintf (buf, "%s/%s\0", GetYarpRoot(), NAMER_CONFIG_FILE);
#endif

	ifstream fin(buf);
	if (fin.eof() || fin.fail())
	{
		_init_nameserver = true;
		return YARP_FAIL;
	}

	char hostname[256];
	ACE_OS::memset (hostname, 0, 256);
	int portnumber = -1;

	fin >> hostname >> portnumber;
	ACE_DEBUG ((LM_DEBUG, "name server at %s port %d\n", hostname, portnumber));

	_namer = new YARPNameClient (hostname, portnumber);
	ACE_ASSERT (_namer != NULL);

	_init_nameserver = false;
	return YARP_OK;
}

int YARPNameService::Finalize (void)
{
	if (!_init_nameserver)
	{
		if (_namer != NULL)
			delete _namer;

		_namer = NULL;
	}

	return YARP_OK;
}


YARPUniqueNameID YARPNameService::RegisterName(const char *name, int reg_type, int num_ports_needed)
{
	if (reg_type == YARP_QNET && !YARPNativeNameService::IsNonTrivial())
	{
		ACE_DEBUG ((LM_DEBUG, "YARPNameService: asked QNX under !QNX OS, of course, it failed\n"));
		return YARPUniqueNameID();
	}

	switch (reg_type)
	{
	case YARP_NO_SERVICE_AVAILABLE:
		break;

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		{
			return YARPSocketNameService::RegisterName (*_namer, name, reg_type, num_ports_needed);
		}		
		break;

	case YARP_QNET:
		{
			/// num_ports_needed is the channel ID here (somehow returned by the QNX channel creation proc).
			return YARPSocketNameService::RegisterName (*_namer, name, YARP_QNET, num_ports_needed);
		}
		break;

	case YARP_QNX4:
		{
			return YARPUniqueNameID ();
		}
		break;
	}

	return YARPUniqueNameID ();
}

YARPUniqueNameID YARPNameService::LocateName(const char *name, int name_type)
{
	/// goes through a sequence asking to the QNX native (if inside QNX)
	/// or otherwise it goes global.
	if (name_type == YARP_QNET && !YARPNativeNameService::IsNonTrivial())
	{
		ACE_DEBUG ((LM_DEBUG, "YARPNameService: asked QNX under !QNX OS, of course, it failed\n"));
		return YARPUniqueNameID();
	}

	/// search mode.
	return YARPSocketNameService::LocateName (*_namer, name, name_type);
}

/*
int YARPNameService::ReadOffset(int delta, int absolute)
{
  return YARPSocketNameService::ReadOffset(delta,absolute);
}
*/


YARPNameID YARPEndpointManager::CreateInputEndpoint(YARPUniqueNameID& name)
{
	switch (name.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		return YARPSocketEndpointManager::CreateInputEndpoint (name);

	case YARP_QNET:
		return YARPNativeEndpointManager::CreateInputEndpoint (name);
	}

	return YARPNameID();
}

YARPNameID YARPEndpointManager::CreateOutputEndpoint(YARPUniqueNameID& name)
{
	switch (name.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		return YARPSocketEndpointManager::CreateOutputEndpoint (name);

	case YARP_QNET:
		return YARPNativeEndpointManager::CreateOutputEndpoint (name);
	}

	return YARPNameID();
}

int YARPEndpointManager::ConnectEndpoints(YARPUniqueNameID& dest)
{
	switch (dest.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		return YARPSocketEndpointManager::ConnectEndpoints (dest);

	case YARP_QNET:
		return YARPNativeEndpointManager::ConnectEndpoints (dest);
	}

	return YARP_FAIL;
}

int YARPEndpointManager::Close(YARPUniqueNameID& endp)
{
	switch (endp.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		return YARPSocketEndpointManager::Close (endp);

	case YARP_QNET:
		return YARPNativeEndpointManager::Close (endp);
	}

	return YARP_FAIL;
}

int YARPEndpointManager::SetTCPNoDelay(const YARPUniqueNameID& endp)
{
	switch (endp.getServiceType())
	{
	case YARP_TCP:
		/// disables Nagle's algorithm.
		return YARPSocketEndpointManager::SetTCPNoDelay ();

	default:
		return YARP_FAIL;
	}

	return YARP_FAIL;
}
