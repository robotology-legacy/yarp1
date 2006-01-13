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
/// $Id: YARPNameService.cpp,v 2.2 2006-01-13 23:14:21 eshuy Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/OS.h>

#include <fstream>

#include <yarp/YARPNameService.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPSocket.h>
#include <yarp/YARPSocketNameService.h>
#include <yarp/YARPNativeNameService.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPNameClient.h>

#include <yarp/wide_nameloc.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPString.h>

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

static YARPSemaphore mutex(1);

/// not very elegant, legacy of old impl.
/// LATER: do it differently.
static bool _init_nameserver = true;
static YARPNameClient * _namer = NULL;

///#define SCATTERSHOT

// this is really bad - should use real strings
static int name_buffer_set = 0;
static char name_buffer[1000];

// general function
char * GetYarpRoot (void)
{
  if (name_buffer_set) {
    return name_buffer;
  }
  char * ret = getenv ("YARP_ROOT");
  if (ret == NULL)
    {
      ACE_DEBUG ((LM_DEBUG, "::GetYarpRoot : can't retrieve YARP_ROOT env variable, using ~/.yarp instead\n"));
      ret = getenv("HOME");
      if (ret!=NULL) {
	sprintf(name_buffer,"%s/.yarp",ret);
	ret = name_buffer;
      }
    }
  return ret;
}

/*
char * GetYarpRoot (void)
{
	char * ret = ACE_OS::getenv ("YARP_ROOT");

	if (ret == NULL)
	{
		ACE_DEBUG ((LM_DEBUG, "::GetYarpRoot : can't retrieve YARP_ROOT env variable\n"));
	}
	return ret;
}
*/

///
///
///
///
int YARPNameService::ConnectNameServer(const char *name)
{
	ACE_UNUSED_ARG (name);
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
	char buf[YARP_STRING_LEN];
	ACE_ASSERT (GetYarpRoot() != NULL);
	ACE_OS::sprintf (buf, "%s/%s\0", GetYarpRoot(), NAMER_CONFIG_FILE);

	ifstream fin(buf);
	if (fin.eof() || fin.fail())
	{
		_init_nameserver = true;
		return YARP_FAIL;
	}

	char hostname[YARP_STRING_LEN];
	ACE_OS::memset (hostname, 0, YARP_STRING_LEN);
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

#define NAMER_FAIL (new YARPUniqueNameID())

YARPUniqueNameID* YARPNameService::RegisterName(const char *name, const char *network_name, int reg_type, int num_ports_needed)
{
  const char *reg_type_text = "unknown";
  switch (reg_type) {
  case YARP_NO_SERVICE_AVAILABLE:
    reg_type_text = "none";
    break;
  case YARP_TCP:
    reg_type_text = "tcp";
    break;
  case YARP_UDP:
    reg_type_text = "udp";
    break;
  case YARP_MCAST:
    reg_type_text = "mcast";
    break;
  case YARP_QNET:
    reg_type_text = "qnet";
    break;
  case YARP_QNX4:
    reg_type_text = "qnx4";
    break;
  }

  ACE_DEBUG((LM_INFO, "*** making port %s on %s network [%s, %d port%s]\n", name, network_name, reg_type_text, num_ports_needed,
	  (num_ports_needed==1)?"":"s"));
	if (reg_type == YARP_QNET && !YARPNativeNameService::IsNonTrivial())
	{
		ACE_DEBUG ((LM_WARNING, "YARPNameService: asked QNX under !QNX OS, of course, it failed\n"));
		return NAMER_FAIL;
	}

	switch (reg_type)
	{
	case YARP_NO_SERVICE_AVAILABLE:
		break;

	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		{
			return YARPSocketNameService::RegisterName (*_namer, name, network_name, reg_type, num_ports_needed);
		}		
		break;

	case YARP_QNET:
		{
			/// num_ports_needed is the channel ID here (somehow returned by the QNX channel creation proc).
			return YARPSocketNameService::RegisterName (*_namer, name, network_name, YARP_QNET, num_ports_needed);
		}
		break;

	case YARP_QNX4:
		{
			return NAMER_FAIL;
		}
		break;
	}

	return NAMER_FAIL;
}

YARPUniqueNameID* YARPNameService::LocateName(const char *name, const char *network_name /*= NULL */, int name_type /*= YARP_NO_SERVICE_AVAILABLE*/)
{
	/// goes through a sequence asking to the QNX native (if inside QNX)
	/// or otherwise it goes global.
	if (name_type == YARP_QNET && !YARPNativeNameService::IsNonTrivial())
	{
		ACE_DEBUG ((LM_WARNING, "YARPNameService: asked QNX under !QNX OS, of course, it failed\n"));
		return NAMER_FAIL;
	}

	/// search mode.
	return YARPSocketNameService::LocateName (*_namer, name, network_name, name_type);
}

bool YARPNameService::VerifySame(const char *ip, const char *network_name, YARPString& ifname)
{
	return YARPSocketNameService::VerifySame(*_namer, ip, network_name, ifname);
}

bool YARPNameService::VerifyLocal (const char *rem_ip, const char *loc_ip, const char *network_name)
{
	return YARPSocketNameService::VerifyLocal(*_namer, rem_ip, loc_ip, network_name);
}

int YARPNameService::DeleteName(YARPUniqueNameID* pid)
{
	if (pid != NULL) delete pid;
	return YARP_OK;
}

int YARPNameService::UnregisterName (YARPUniqueNameID* pid)
{
	return YARPSocketNameService::UnregisterName (*_namer, pid->getName().c_str(), pid->getServiceType());
}

///
///
///
int YARPEndpointManager::CreateInputEndpoint(YARPUniqueNameID& name)
{
	switch (name.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
		return YARPSocketEndpointManager::CreateInputEndpoint (name);

	case YARP_QNET:
		return YARPNativeEndpointManager::CreateInputEndpoint (name);

	default:
		ACE_DEBUG ((LM_WARNING, "it doesn't exist an input endpoint for the protocol specified\n"));
	}

	return YARP_FAIL;
}

int YARPEndpointManager::CreateOutputEndpoint(YARPUniqueNameID& name)
{
	switch (name.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketEndpointManager::CreateOutputEndpoint (name);

	case YARP_QNET:
		return YARPNativeEndpointManager::CreateOutputEndpoint (name);
	}

	return YARP_FAIL;
}

int YARPEndpointManager::ConnectEndpoints(YARPUniqueNameID& dest, const YARPString& own_name)
{
	switch (dest.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
		return YARPSocketEndpointManager::ConnectEndpoints (dest, own_name);

	case YARP_QNET:
		return YARPNativeEndpointManager::ConnectEndpoints (dest, own_name);
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
	case YARP_SHMEM:
	default:
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

/// should fail if !MCAST.
int YARPEndpointManager::CloseMcastAll(void)
{
	return YARPSocketEndpointManager::CloseMcastAll();
}

///
int YARPEndpointManager::GetNumberOfClients(void)
{
	return YARPSocketEndpointManager::GetNumberOfClients();
}

///
int YARPEndpointManager::PrintConnections(const YARPUniqueNameID& endp)
{
	switch (endp.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
	case YARP_MCAST:
	case YARP_SHMEM:
	default:
		return YARPSocketEndpointManager::PrintConnections ();

	case YARP_QNET:
		return YARP_FAIL;
	}

	return YARP_FAIL;
}


int YARPNameService::CheckProperty(const char *name,
				   const char *key,
				   const char *value) {
  if (_namer!=NULL) {
    return _namer->check(name,key,value);
  }
  return -1;
}
