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
/// $Id: YARPNameService.cpp,v 1.13 2003-05-18 19:15:31 gmetta Exp $
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

#if 0
	int con = 0;
	mutex.Wait ();
	con = is_connected || (name == NULL && tried_to_connect);
	mutex.Post ();

#ifdef SCATTERSHOT
	if (tried_to_connect)
	{
		mutex.Wait ();
		namer.Close ();
		mutex.Post ();
	}
	con = 0;
#endif
  
	if (!con)
	{
		char aux_name[256];
		mutex.Wait ();
		if (name == NULL)
		{
			ifstream fin(NAMER_CONFIG_FILE);
			is_connected = 0;
			while (!fin.eof() && !is_connected)
			{
				fin.getline(aux_name,sizeof(aux_name));
				if (ACE_OS::strchr(aux_name,'|')!=NULL)
				{
					//		  fprintf(stderr, 
					//			  "Trying name server \"%s\"...\n", 
					//			 aux_name);
					if (namer.Connect(aux_name) != -1)
					{
						name = aux_name;
						is_connected = 1;
					}
				}
			}
		}

		if (name == NULL)
		{
			ACE_OS::fprintf(stderr,"Could not connect name server, check %s\n", NAMER_CONFIG_FILE);
		}
		tried_to_connect = 1;
		mutex.Post();
	}
	return 0;

#endif
}

#if 0
int ConnectNameServer(YARPOutputSocket& namer)
{
  int is_connected = 0;
  if (1)
    {
      char aux_name[256];
	{
	  ifstream fin(NAMER_CONFIG_FILE);
	  is_connected = 0;
	  while (!fin.eof() && !is_connected)
	    {
	      fin.getline(aux_name,sizeof(aux_name));
		  if (ACE_OS::strchr(aux_name,'|')!=NULL)
		{
       //	  fprintf(stderr, 
//			  "Trying name server \"%s\"...\n", 
//			 aux_name);
		  if (namer.Connect(aux_name) != -1)
		    {
		      is_connected = 1;
		    }
		}
	    }
	}
      if (!is_connected)
	{
		  ACE_OS::fprintf(stderr,"Could not connect name server, check %s\n", NAMER_CONFIG_FILE);
	}
    }
  return is_connected;
}
#endif

#if 0
YARPNameID YARPNameService::GetRegistration()
{
	mutex.Wait();
	int mode = registration_mode;
	mutex.Post();
	return YARPNameID(registration_mode,0);
}
#endif

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
#if 0
			int native = 1;
			if (!(native && YARPNativeNameService::IsNonTrivial()))
			{
				int all_result = YARPSocketNameService::RegisterName(name);
				int in_port = YARPSocketNameService::GetAssignedPort();

				//ConnectNameServer(NULL);
				YARPOutputSocket namer;
				::ConnectNameServer(namer);

				NameServiceHeader hdr;
				hdr.port = in_port;
				hdr.machine_length = 0;
				hdr.key_length = ACE_OS::strlen(name)+1;
				hdr.spare2 = hdr.spare1 = -1;
				char hostname[256] = "localhost";
				YARPNetworkObject::GetHostName(hostname,sizeof(hostname));
				hdr.machine_length = ACE_OS::strlen(hostname)+1;
				hdr.request_type = NAME_REGISTER;

				mutex.Wait();
				namer.SendBegin((char*)(&hdr),sizeof(hdr));
				namer.SendContinue(hostname,hdr.machine_length);
				namer.SendContinue((char*)name,hdr.key_length);
				char reply[256];
				int result = namer.SendEnd(reply,sizeof(reply));
				registration_mode = YARP_NAME_MODE_SOCKET;
				mutex.Post();

				if (result>=0)
				{
					ACE_OS::fprintf(stderr,"Registered <%s> as [%s]\n", name, reply);
				}
				else
				{
					ACE_OS::fprintf(stderr,"Couldn't contact name server to register %s\n", name);
				}

				ACE_OS::fprintf(stderr,"Remote name registration of <%s>\n", name);
				return all_result;
			}
			else
			{
				// Otherwise, native registration
				ACE_OS::fprintf(stderr,"Native name registration of <%s>\n", name);
				int all_result = YARPNativeNameService::RegisterName(name);
				mutex.Wait();
				registration_mode = YARP_NAME_MODE_NATIVE;
				mutex.Post();
				return all_result;
			}
#endif
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

#if 0
  YARPNameID all_result;

  if (ACE_OS::strchr(name,'|')==NULL)
    {
      if (native && YARPNativeNameService::IsNonTrivial())
	{
	  all_result = YARPNativeNameService::LocateName(name);
	  ACE_OS::fprintf(stderr,"Native name lookup of <%s>\n", name);
	}
      
      if (!all_result.IsValid())
	{
	  ACE_OS::fprintf(stderr,"Remote name lookup of <%s>\n", name);
	  //ConnectNameServer(NULL);
	  YARPOutputSocket namer;
	  ::ConnectNameServer(namer);
	  
	  NameServiceHeader hdr;
	  hdr.port = 0;
	  hdr.machine_length = 0;
	  hdr.key_length = ACE_OS::strlen(name)+1;
	  hdr.spare2 = hdr.spare1 = -1;
	  hdr.request_type = NAME_LOOKUP;
	  
	  mutex.Wait();
	  namer.SendBegin((char*)(&hdr),sizeof(hdr));
	  namer.SendContinue((char*)name,hdr.key_length);
	  char reply[256];
	  int result = namer.SendEnd(reply,sizeof(reply));
	  mutex.Post();
	  if (result>=0 && reply[0]!='?')
	    {
	      ACE_OS::fprintf(stderr,"Name <%s> is being directed to [%s]\n", name, reply);
	      all_result = YARPSocketNameService::LocateName(reply);
	    }
	  else
	    {
	      if (result>=0)
		{
		  ACE_OS::fprintf(stderr,"Couldn't find <%s>\n", name);
		}
	      else
		{
		  ACE_OS::fprintf(stderr,"Couldn't connect to name service to lookup <%s>\n", name);
		}
	    }
	}
    }
  else
    {
      ACE_OS::fprintf(stderr,"Socket name lookup of <%s>\n", name);
      all_result = YARPSocketNameService::LocateName(name);
    }

  return all_result;

#endif
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
		return YARPSocketEndpointManager::CreateOutputEndpoint (name);

	case YARP_QNET:
		return YARPNativeEndpointManager::CreateOutputEndpoint (name);
	}

	return YARPNameID();
}

int YARPEndpointManager::ConnectEndpoints(YARPNameID& dest)
{
	switch (dest.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
		return YARPSocketEndpointManager::ConnectEndpoints (dest);

	case YARP_QNET:
		return YARPNativeEndpointManager::ConnectEndpoints (dest);
	}

	return YARP_FAIL;
}

int YARPEndpointManager::Close(const YARPNameID& endp)
{
	switch (endp.getServiceType())
	{
	case YARP_TCP:
	case YARP_UDP:
		return YARPSocketEndpointManager::Close ();

	case YARP_QNET:
		return YARPNativeEndpointManager::Close (endp);
	}

	return YARP_FAIL;
}

int YARPEndpointManager::SetTCPNoDelay(const YARPNameID& endp)
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
