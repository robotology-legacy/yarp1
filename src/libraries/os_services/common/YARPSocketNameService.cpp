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
///                    #paulfitz, pasa, nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSocketNameService.cpp,v 1.12 2003-05-16 00:04:51 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>


#ifdef __WIN_MSVC__
#	include <windows.h>
#endif

//#include <stdlib.h>
//#include <stdio.h>
//#include <assert.h>

#include "YARPSocketNameService.h"
#include "YARPSocket.h"
#include "YARPSocketDgram.h"
#include "YARPTime.h"
#include "ThreadInput.h"
#include "YARPNameID_defs.h"
#include "YARPNameService.h"
#include "YARPNativeNameService.h"
#include "debug.h"


#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

#define THIS_DBG 50

#include <sys/types.h>

#ifndef __WIN__
#include <unistd.h>
#endif

#include <map>

#ifdef __WIN_MSVC__
#include <process.h>
#include <functional>
using namespace std;
#endif

#include "YARPSemaphore.h"

#define times ignore

#ifndef __WIN__
#ifndef __QNX6__

#include <sys/psinfo.h>

#endif
#endif


///
///
/// LATER: have a look at this. ACEize later...
///
#ifndef __WIN__
#ifdef __QNX4__

static int my_getpid()
{
	struct _psinfo2 info;
	qnx_getids (0, &info);
	return info.pid;
}

#else //QNX6

#include <pthread.h>
static int my_getpid()
{
	return getpid();
}

/// returns the thread Id.
static int my_gettid()
{
	return (int)pthread_self();
}

#endif

#else

static int my_getpid()
{
#ifdef __WIN_MSVC__
	return GetCurrentThreadId();
#else
	return getpid();
#endif
}

static inline int my_gettid() { return my_getpid(); }

#endif


///
///
///
///
class _yarp_endpoint
{
public:
	YARPSemaphore mutex;

	typedef YARPNetworkObject *PYARPNetworkObject;
	typedef map<int, PYARPNetworkObject, less<int> > SMap;

	SMap _map;
	YARPInputSocket *test_global;

public:
	////
	_yarp_endpoint () : mutex (1)
	{
		test_global = NULL;
	}

	~_yarp_endpoint () {}


	/// get the socket from the map of all in sockets.
	inline YARPNetworkObject *GetThreadSocket(void)
	{
		int pid = my_gettid();
		YARPNetworkObject *result = NULL;
		mutex.Wait();
		if (_map.find(pid) != _map.end())
		{
			result = _map[pid];
		}
		mutex.Post();
		return result;
	}

	/// the calling thread gets it input socket if any, -1 if no socket is associated w/
	///		current thread.
	inline int GetAssignedPort(void)
	{
		int result = YARP_FAIL;
		YARPNetworkObject *is = GetThreadSocket();
		if (is != NULL)
		{
			result = is->GetAssignedPort ();
		}
		return result;
	}
};


///
///
/// global variables.
///
_yarp_endpoint _endpointmanager;		/// per-process instance.




///
///
///
///
YARPNetworkObject *YARPSocketEndpointManager::GetThreadSocket (void)
{
	return _endpointmanager.GetThreadSocket ();
}


///
/// socket creation.
YARPNameID YARPSocketEndpointManager::CreateInputEndpoint (YARPUniqueNameID& name)
{
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ checking for previous definitions\n"));

	if (GetThreadSocket() == NULL)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ checks out okay\n"));
		int pid = my_gettid();
		_endpointmanager.mutex.Wait();

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ creating\n"));

		YARPNetworkObject *no = NULL;
		switch (name.getServiceType())
		{
		case YARP_TCP:
			{
				_endpointmanager._map[pid] = new YARPInputSocket;
				no = _endpointmanager._map[pid];
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing input socket\n"));

				// prepare the socket for the connection (bind).
				no->Prepare (name);
			}
			break;

		case YARP_UDP:
			{
				_endpointmanager._map[pid] = new YARPInputSocketDgram;
				no = _endpointmanager._map[pid];

				/// P1 is the size of the pool (e.g. 11 (1+10))
				/// P2 is an array of ports (of size P1)
				/// cast is required because this version of prepare is not in base class (similar in spirit to a ctor).
				((YARPInputSocketDgram *)no)->Prepare (name, name.getP2Ptr(), name.getP1Ref());
			}
			break;

		default:
			ACE_DEBUG ((LM_DEBUG, "YARPSocketEndpointManager::CreateInputEndpoint, service type not implemented\n"));
			return YARPNameID();
			break;
		}

		
		//is->Register(name);
		// result = is->GetAssignedPort();
		_endpointmanager.mutex.Post();
		
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ Made an input socket handler\n"));
		ACE_OS::fflush(stdout);

		name.getNameID() = YARPNameID (name.getServiceType(), no->GetIdentifier());
		return name.getNameID();
	}

	/// if anything goes wrong, return an invalid NameID
	// return result;

	return YARPNameID ();
}

///
///
///
YARPNameID YARPSocketEndpointManager::CreateOutputEndpoint (YARPUniqueNameID& name)
{
	if (GetThreadSocket() == NULL)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ checks out okay\n"));
		int pid = my_gettid();
		_endpointmanager.mutex.Wait();

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ creating\n"));

		YARPNetworkObject *no = NULL;
		switch (name.getServiceType())
		{
		case YARP_TCP:
			{
				_endpointmanager._map[pid] = new YARPOutputSocket;
				no = (YARPNetworkObject *) _endpointmanager._map[pid];

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing output socket\n"));

				// prepare the socket for the connection (bind).
				no->Prepare (name);
			}
			break;

		case YARP_UDP:
				_endpointmanager._map[pid] = new YARPOutputSocketDgram;
				no = (YARPNetworkObject *) _endpointmanager._map[pid];

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing output socket\n"));

				// prepare the socket for the connection (bind).
				/// uses the first port as port of the sender socket.
				///ACE_ASSERT (name.getP1Ref() == 1);
				///((YARPOutputSocketDgram *)no)->Prepare (name); ///, name.getP2Ptr()[0]);
				no->Prepare (name); ///, name.getP2Ptr()[0]);
			break;

		default:
			ACE_DEBUG ((LM_DEBUG, "YARPSocketEndpointManager::CreateOutputEndpoint, service type not implemented\n"));
			break;
		}

		//is->Register(name);
		// result = is->GetAssignedPort();
		_endpointmanager.mutex.Post();
		
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ Made an output socket handler\n"));
		ACE_OS::fflush(stdout);

		name.getNameID() = YARPNameID (name.getServiceType(), no->GetIdentifier());

		return name.getNameID();
	}

	//	YARPOutputSocket os;

	/// connect?
	//	os.Connect(name);
	//	os.InhibitDisconnect();

	return YARPNameID ();
}

int YARPSocketEndpointManager::ConnectEndpoints(YARPNameID& dest)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "ConnectEndpoints: no socket associated with current thread\n"));
		return YARP_FAIL;
	}

	if (sock->getSocketType() != YARP_O_SOCKET)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "ConnectEndpoints: trying to connect an input socket\n"));
		return YARP_FAIL;
	}

	ACE_ASSERT (dest.getServiceType() == sock->GetServiceType());

	/// I've got a socket!
	///YARPOutputSocket *os = (YARPOutputSocket *)sock;
	sock->Connect ();
	dest.setRawIdentifier (sock->GetIdentifier ());

	return YARP_OK;
}

int YARPSocketEndpointManager::Close (void)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "CloseEndpoint: no socket associated with current thread\n"));
		return YARP_FAIL;
	}

	switch (sock->getSocketType())
	{
	case YARP_O_SOCKET:
		{
			///YARPOutputSocket *os = (YARPOutputSocket *)sock;
			sock->Close ();
			break;
		}

	case YARP_I_SOCKET:
		{
			///YARPInputSocket *is = (YARPInputSocket *)sock;
			sock->CloseAll ();
			break;
		}
	}

	return YARP_OK;
}

int YARPSocketEndpointManager::SetTCPNoDelay (void)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		return YARP_FAIL;
	}

	/// LATER: should check whether the function can actually be called for this socket.
	if (sock->getSocketType() == YARP_O_SOCKET && sock->GetServiceType() == YARP_TCP)
	{
		return ((YARPOutputSocket *)sock)->SetTCPNoDelay ();
	}

	return YARP_FAIL;
}

/// implementation of YARPSocketNameService.
///
///
YARPUniqueNameID YARPSocketNameService::RegisterName(YARPNameClient& namer, const char *name, int reg_type, int num_ports_needed)
{
	/// remote registration.
	///
	ACE_INET_Addr addr;
	ACE_INET_Addr reg_addr;
	YARPUniqueNameID n (reg_type, addr);

	char buf[256];
	YARPNetworkObject::getHostname (buf, 256);

	///
	int ret = reg_addr.set((u_short)0, buf);
	ACE_DEBUG((LM_DEBUG, "RegisterName - hostname in addr : %s\n", reg_addr.get_host_name()));
	string tname (name);

	switch (reg_type)
	{
	case YARP_QNET:
		{
			/// hostname is used as usual, or rather the IP address?
			/// <name> is the name to be registered.
			/// <reg_type> self explaining
			/// <num_ports_needed> is used (recycled) as channel ID number.
			int chid = num_ports_needed;
			
			YARPNameQnx tmp;
			tmp.setName(name);
			tmp.setAddr(string(reg_addr.get_host_name()), my_getpid(), chid);
			if (namer.check_in_qnx (tmp) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems registering %s\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}

			// _p2[0] = mypid, _p2[1] = chid.
			n.allocP2 (2);
			n.getP2Ptr()[0] = (NetInt32)my_getpid();
			n.getP2Ptr()[1] = (NetInt32)chid;
			n.getAddressRef() = reg_addr;	/// can't do anything better.
			n.setRawIdentifier((ACE_HANDLE)chid);
		}
		break;

	case YARP_TCP:
		{
			if (namer.check_in (tname, reg_addr, addr) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems registering %s\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}

			if (addr.get_port_number() == __portNotFound)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems registering (no port) %s\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}

			n.getAddressRef() = addr;
		}		
		break;

	case YARP_UDP:
		{
			NetInt32 *ports = new NetInt32[num_ports_needed];
			ACE_ASSERT (ports != NULL);
			memset (ports, 0, sizeof(int) * num_ports_needed);
			if (namer.check_in_udp (tname, reg_addr, addr, ports, num_ports_needed) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems registering %s\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}

			n.allocP2 (num_ports_needed);
			memcpy (n.getP2Ptr(), ports, sizeof(NetInt32) * num_ports_needed);
			n.getAddressRef() = addr;
			delete[] ports;
		}
		break;

	default:
		{
			ACE_DEBUG ((LM_DEBUG, "protocol not supported, can't register name %s\n", name));
			return YARPUniqueNameID();
		}
		break;
	}

	return n;
}

//YARPNameID YARPSocketNameService::LocateName(const char *name)
YARPUniqueNameID YARPSocketNameService::LocateName(YARPNameClient& namer, const char *name, int name_type)
{
	/// handle the connection w/ the remote name server.
	///
	ACE_INET_Addr addr;

	switch (name_type)
	{
	case YARP_QNET:
		{
			YARPNameQnx tmp;
			int reg_type = YARP_NO_SERVICE_AVAILABLE;
			if (namer.query_qnx (name, tmp, &reg_type) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems locating %s\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}

			if (reg_type != name_type)
			{
				ACE_DEBUG ((LM_DEBUG, ">>>>The requested type differs from the actual one %s\n", name));
				return YARPUniqueNameID ();
			}

			YARPUniqueNameID n(YARP_QNET, addr);
			n.allocP2 (2);
			n.getP2Ptr()[0] = (NetInt32)tmp.getPid();
			n.getP2Ptr()[1] = (NetInt32)tmp.getChan();
			n.getAddressRef().set ((u_short)0, tmp.getNode());	/// can't do anything better.
			n.setRawIdentifier((ACE_HANDLE)tmp.getChan());

			return n;
		}
		break;

	case YARP_TCP:
	case YARP_UDP:
		{
			///
			int reg_type = YARP_NO_SERVICE_AVAILABLE;
			if (namer.query (name, addr, &reg_type) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems locating %s\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}

			if (reg_type != name_type)
			{
				ACE_DEBUG ((LM_DEBUG, ">>>The requested type differs from the actual one %s\n", name));
				return YARPUniqueNameID ();
			}
			
			/// query must return also the registration type.
			///
			return YARPUniqueNameID (reg_type, addr);
		}
		break;

	case YARP_NO_SERVICE_AVAILABLE:
		{
			/// doing a search...
			/// QNET first.
			YARPNameQnx tmp;
			int reg_type = YARP_NO_SERVICE_AVAILABLE;
			
			if (YARPNativeNameService::IsNonTrivial())
			{
				if (namer.query_qnx (name, tmp, &reg_type) != YARP_OK)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems locating %s, perhaps the name service is not running\n", name));
					return YARPUniqueNameID ();	/// invalid name id.
				}

				/// found a legitimate QNET name.
				if (reg_type == YARP_QNET)
				{
					YARPUniqueNameID n(YARP_QNET, addr);
					n.allocP2 (2);
					n.getP2Ptr()[0] = (NetInt32)tmp.getPid();
					n.getP2Ptr()[1] = (NetInt32)tmp.getChan();
					n.getAddressRef().set ((u_short)0, tmp.getNode());	/// can't do anything better.
					n.setRawIdentifier((ACE_HANDLE)tmp.getChan());
					return n;
				}

				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems locating %s in QNET protocol\n", name));
			}

			reg_type = YARP_NO_SERVICE_AVAILABLE;
			if (namer.query (name, addr, &reg_type) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>>Problems locating %s also in TCP/UPD protocol\n", name));
				return YARPUniqueNameID ();	/// invalid name id.
			}
			
			/// query must return also the registration type.
			///
			return YARPUniqueNameID (reg_type, addr);
		}
		break;

	default:
		ACE_DEBUG ((LM_DEBUG, "troubles locating name: %s\n", name));
		break;
	}

	return YARPUniqueNameID();
}

#undef THIS_DBG
