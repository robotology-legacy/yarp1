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
/// $Id: YARPSocketNameService.cpp,v 1.8 2004-07-12 12:04:19 eshuy Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>
#include <ace/Map_Manager.h>

#include <yarp/YARPSocketNameService.h>
#include <yarp/YARPSocket.h>
#include <yarp/YARPSocketDgram.h>
#include <yarp/YARPSocketMcast.h>
#include <yarp/YARPSocketMulti.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPNativeNameService.h>
#include <yarp/debug.h>


#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

#define THIS_DBG 50

#include <sys/types.h>

#ifndef __WIN32__
#include <unistd.h>
#endif

#include <yarp/YARPSemaphore.h>

#define times ignore

///
///
/// LATER: have a look at this. ACEize later...
///
#if defined(__QNX4__)

#include <sys/psinfo.h>
static int my_getpid()
{
	struct _psinfo2 info;
	qnx_getids (0, &info);
	return info.pid;
}

#elif defined(__QNX6__) || defined (__LINUX__)

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

#elif defined(__WIN32__)

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
	typedef ACE_Map_Manager<int, PYARPNetworkObject, ACE_Null_Mutex> SMap;

	SMap _map;

public:
	////
	_yarp_endpoint () : mutex (1) {}
	~_yarp_endpoint () {}

	/// get the socket from the map of all in sockets.
	inline YARPNetworkObject *GetThreadSocket(void)
	{
		int pid = my_gettid();
		YARPNetworkObject *result = NULL;
		mutex.Wait();
	
		if (_map.find(pid, result) == 0)
		{
			mutex.Post();
			return result;
		}

		mutex.Post();
		return NULL;
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
int YARPSocketEndpointManager::CreateInputEndpoint (YARPUniqueNameID& name)
{
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ checking for previous definitions\n"));
	YARPUniqueNameSock& sname = (YARPUniqueNameSock&)name;

	if (GetThreadSocket() == NULL)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ checks out okay\n"));
		int pid = my_gettid();
		_endpointmanager.mutex.Wait();

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ creating\n"));

		if (name.getServiceType() != YARP_TCP &&
			name.getServiceType() != YARP_UDP &&
			name.getServiceType() != YARP_MCAST)
		{
			ACE_DEBUG ((LM_DEBUG, "YARPSocketEndpointManager::CreateInputEndpoint, service type not supported\n"));
			return YARP_FAIL;
		}

		YARPNetworkObject *no = NULL;

		no = new YARPInputSocketMulti;
		ACE_ASSERT (no != NULL);

		_endpointmanager._map.rebind (pid, no);

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing input socket\n"));

		// prepare the socket for the connection (bind).
		((YARPInputSocketMulti *)no)->Prepare (sname, sname.getPorts(), sname.getNPorts());

		_endpointmanager.mutex.Post();
		
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ Made an input socket handler\n"));
		ACE_OS::fflush(stdout);

		sname.setRawIdentifier (no->GetIdentifier());
		return YARP_OK;
	}

	return YARP_OK;
}

///
///
///
int YARPSocketEndpointManager::CreateOutputEndpoint (YARPUniqueNameID& name)
{
	YARPUniqueNameSock& sname = (YARPUniqueNameSock&)name;

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
				no = new YARPOutputSocket;
				ACE_ASSERT (no != NULL);
				_endpointmanager._map.rebind (pid, no);

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing output socket\n"));

				// prepare the socket for the connection (bind).
				((YARPOutputSocket *)no)->Prepare (sname);
			}
			break;

		case YARP_UDP:
			{
				no = new YARPOutputSocketDgram;
				ACE_ASSERT (no != NULL);
				_endpointmanager._map.rebind (pid, no);

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing output socket\n"));

				/// uses the first port as port of the sender socket.
				((YARPOutputSocketDgram *)no)->Prepare (sname);
			}
			break;

		case YARP_MCAST:
			{
				no = new YARPOutputSocketMcast;
				ACE_ASSERT (no != NULL);
				_endpointmanager._map.rebind (pid, no);

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing output socket\n"));

				((YARPOutputSocketMcast *)no)->Prepare (sname);
			}
			break;

		case YARP_SHMEM:
			{
				no = new YARPOutputSocketMulti;
				ACE_ASSERT (no != NULL);
				_endpointmanager._map.rebind (pid, no);

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ preparing output socket\n"));

				/// this actually is only a shmem socket-like class.
				((YARPOutputSocketMulti *)no)->Prepare (sname);
			}
			break;

		default:
			ACE_DEBUG ((LM_DEBUG, "YARPSocketEndpointManager::CreateOutputEndpoint, service type not implemented\n"));
			return YARP_FAIL;
		}

		_endpointmanager.mutex.Post();
		
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "^^^^^^^^ Made an output socket handler\n"));
		ACE_OS::fflush(stdout);

		sname.setRawIdentifier (no->GetIdentifier());
		return YARP_OK;
	}

	return YARP_OK;
}

///
///
///
int YARPSocketEndpointManager::ConnectEndpoints(YARPUniqueNameID& dest)
{
	YARPNetworkOutputObject *sock = (YARPNetworkOutputObject *)GetThreadSocket();
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

	ACE_ASSERT (dest.getServiceType() == sock->GetServiceType() ||
			    (dest.getServiceType() == YARP_UDP && sock->GetServiceType() == YARP_MCAST));

	sock->Connect (dest);

	dest.setRawIdentifier (sock->GetIdentifier ());

	return YARP_OK;
}

int YARPSocketEndpointManager::CloseMcastAll (void)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		ACE_DEBUG ((LM_DEBUG, "CloseMcastAll: no socket associated with current thread\n"));
		return YARP_FAIL;
	}

	if (sock->getSocketType() != YARP_O_SOCKET)
	{
		ACE_DEBUG ((LM_DEBUG, "CloseMcastAll: this thread doesn't hold an out socket\n"));
		return YARP_FAIL;
	}

	if (sock->GetServiceType() != YARP_MCAST)
	{
		ACE_DEBUG ((LM_DEBUG, "CloseMcastAll: this only applies to MCAST sockets\n"));
		return YARP_FAIL;
	}

	((YARPOutputSocketMcast *)sock)->CloseMcastAll ();

	int pid = my_gettid();
	_endpointmanager.mutex.Wait();
	_endpointmanager._map.unbind (pid, sock);
	delete sock;

	_endpointmanager.mutex.Post();

	return YARP_OK;
}

int YARPSocketEndpointManager::GetNumberOfClients (void)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "GetNumberOfClients: no socket associated with current thread\n"));
		return YARP_FAIL;
	}

	if (sock->getSocketType() == YARP_O_SOCKET)
	{
		/// need to test it's an MCAST socket for real. it might crash otherwise.
		return ((YARPOutputSocketMcast *)sock)->GetNumberOfClients();
	}
	else
	{
		return YARP_FAIL;
	}

	return YARP_FAIL;
}

int YARPSocketEndpointManager::Close (YARPUniqueNameID& dest)
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
			((YARPNetworkOutputObject *)sock)->Close (dest);
			break;
		}

	case YARP_I_SOCKET:
		{
			((YARPNetworkInputObject *)sock)->CloseAll ();
			break;
		}
	}

	/// this is tricky here. Apparently some of the pid's in some situations
	/// aren't released blocking the possibility of further connecting new sockets
	/// from the same thread.
	if (sock->GetIdentifier() == ACE_INVALID_HANDLE)
	{
		int pid = my_gettid();
		_endpointmanager.mutex.Wait();
		_endpointmanager._map.unbind (pid, sock);
		delete sock;
		_endpointmanager.mutex.Post();
	}

	dest.setRawIdentifier (ACE_INVALID_HANDLE);
	return YARP_OK;
}

int YARPSocketEndpointManager::SetTCPNoDelay (void)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		return YARP_FAIL;
	}

	if (sock->getSocketType() == YARP_O_SOCKET && sock->GetServiceType() == YARP_TCP)
	{
		return ((YARPOutputSocket *)sock)->SetTCPNoDelay ();
	}

	return YARP_FAIL;
}

#define NAMER_FAIL (new YARPUniqueNameID())

/// implementation of YARPSocketNameService.
///	namer is the global istance of the name service interface class.
///	always allocates the output object.
///
YARPUniqueNameID* YARPSocketNameService::RegisterName(YARPNameClient& namer, const char *name, const char *network_name, int reg_type, int extra_param)
{
	/// remote registration.
	///
	ACE_INET_Addr addr;			/// returned by name service.
	ACE_INET_Addr reg_addr;		/// who I am.

	char myhostname[YARP_STRING_LEN];
	getHostname (myhostname, YARP_STRING_LEN);
	//printf("*** %s\n", myhostname);
	reg_addr.set((u_short)0, myhostname);
	
	YARPString tname (name);

	switch (reg_type)
	{
	case YARP_QNET:
		{
			/// hostname is used as usual, or rather the IP address?
			/// <name> is the name to be registered.
			/// <reg_type> self explaining
			/// <num_ports_needed> is used (recycled) as channel ID number.
			int& chid = extra_param;
			
			YARPNameQnx tmp;
			tmp.setName (name);
			/// LATER: isn't get_host_name here == myhostname?
			tmp.setAddr (YARPString(reg_addr.get_host_name()), my_getpid(), chid);

			ACE_DEBUG((LM_DEBUG, "registering name %s of (%s) protocol %s\n", name, reg_addr.get_host_addr(), servicetypeConverter(reg_type)));

			if (namer.check_in_qnx (tmp) != YARP_OK)
			{
			  //YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>>> problems registering %s\n", name));
			  ACE_DEBUG((LM_WARNING, "*** FAILED port %s - is the YARP name server running?\n", name));
				return NAMER_FAIL;	/// invalid name id.
			}

			YARPUniqueNameQnx *n = new YARPUniqueNameQnx (YARP_QNET);

			n->setName (name);
			n->setPid (my_getpid());
			n->setChannelID (chid);
			n->setNode (myhostname);

			return (YARPUniqueNameID *)n;
		}
		break;

		/// always does the check_in_udp to ask for a pool a ports and not simply for 1.
	case YARP_UDP:
	case YARP_TCP:
	case YARP_MCAST:
		{
			/// multi-interface handling - tries to get own IP addr from the nameserver for the
			/// requested symbolic network name.
			NetInt32 *ports = new NetInt32[extra_param];
			ACE_ASSERT (ports != NULL);
			memset (ports, 0, sizeof(int) * extra_param);

			/// uses the new IP for registration in the name server.
			YARPString new_ip, new_nic;
			YARPUniqueNameSock *n = new YARPUniqueNameSock (reg_type);

			//printf("Trying to get my (%s) IP addr from the name server (network %s ip %s)\n", myhostname, network_name, reg_addr.get_host_addr());
			
			if ((strcmp(reg_addr.get_host_addr(),"127.0.0.1")==0)||
			    (strcmp(reg_addr.get_host_addr(),"0.0.0.0")==0)) {
			  // localhost 127.0.0.1 IP is no good, no good at all
			  // dud 0.0.0.0 IP is no good, no good at all
			  // host name is marginally better
			  //printf("Trying with %s\n", myhostname);
			  namer.query_nic (myhostname, network_name, new_nic, new_ip);
			} else {
			  //printf("[2] Trying with %s\n", reg_addr.get_host_addr());
			  namer.query_nic (reg_addr.get_host_addr(), network_name, new_nic, new_ip);
			}
			
			//printf("my IP addr from the name server --> network %s ip %s\n", new_nic.c_str(), new_ip.c_str());
			
			if (strcmp (new_ip.c_str(), "0.0.0.0") == 0)
			{
				ACE_DEBUG ((LM_WARNING, ">>>> Can't find the net name %s\n", network_name));
				delete[] ports;
				delete n;
				return NAMER_FAIL;	/// invalid name id.
			}

			n->setInterfaceName (new_nic);
			reg_addr.set((u_short)0, new_ip.c_str());

			ACE_DEBUG((LM_DEBUG, "registering name %s of (%s) protocol %s\n", name, reg_addr.get_host_addr(), servicetypeConverter(reg_type)));

			if (namer.check_in_udp (tname, reg_addr, addr, ports, extra_param) != YARP_OK)
			{
			  //ACE_DEBUG ((LM_DEBUG, ">>>> Problems registering %s\n", name));
			  //ACE_DEBUG ((LM_DEBUG, ">>>> PLEASE CHECK THAT THE YARP NAME SERVER (yarp-service) IS RUNNING\n"));
			  ACE_DEBUG((LM_WARNING, "*** FAILED port %s - is the YARP name server running?\n", name));
				delete[] ports;
				delete n;
				return NAMER_FAIL;	/// invalid name id.
			}

			n->setName (name);
			n->getAddressRef() = addr;
			n->setPorts ((int *)ports, extra_param);
			delete[] ports;

			return (YARPUniqueNameID *)n;
		}
		break;

	default:
		{
			ACE_DEBUG ((LM_DEBUG, "protocol not supported, can't register name %s\n", name));
		}
		break;
	}

	return NAMER_FAIL;
}

///
/// I need an extra method here to verify whether the two addresses are on the 
///	same machine.
bool YARPSocketNameService::VerifySame (YARPNameClient& namer, const char *ip, const char *netname, YARPString& if_name)
{
	YARPString new_ip, new_nic;
	namer.query_nic (ip, netname, new_nic, new_ip);
	if_name = new_nic;
	//if (strcmp (new_ip.c_str(), ip) == 0)
	//return true;


	ACE_INET_Addr addr1, addr2;
	addr1.set((short unsigned int)0,(const char *)new_ip.c_str());
	addr2.set((short unsigned int)0,(const char *)ip);
	if (addr1==addr2) {
	  return true;
	}

	return false;
}

bool YARPSocketNameService::VerifyLocal (YARPNameClient& namer, const char *rem_ip, const char *loc_ip, const char *netname)
{
	YARPString ip1, nic1;
	YARPString ip2, nic2;
	namer.query_nic (rem_ip, netname, nic1, ip1);
	namer.query_nic (loc_ip, netname, nic2, ip2);

	//if (strcmp (ip1.c_str(), ip2.c_str()) == 0 && strcmp (ip1.c_str(), "0.0.0.0") != 0)
	//return true;

	if (strcmp (ip1.c_str(), "0.0.0.0") != 0) {
	  ACE_INET_Addr addr1, addr2;
	  addr1.set((short unsigned int)0,(const char *)ip1.c_str());
	  addr2.set((short unsigned int)0,(const char *)ip2.c_str());
	  if (addr1==addr2) {
	    return true;
	  }
	}


	return false;
}

///
///
///
int YARPSocketNameService::UnregisterName (YARPNameClient& namer, const char *name, int name_type)
{
	switch (name_type)
	{
	case YARP_QNET:
		return namer.check_out_qnx (name);

	default:
		return namer.check_out (name);
	}

	return YARP_OK;
}

///
/// search for a name in the name service.
/// 
YARPUniqueNameID* YARPSocketNameService::LocateName(YARPNameClient& namer, const char *name, const char *network_name, int name_type)
{
	/// handle the connection w/ the remote name server.
	///
	ACE_INET_Addr addr;
	YARPString sname = name;

	switch (name_type)
	{
	case YARP_QNET:
		{
			YARPNameQnx tmp;
			int reg_type = YARP_NO_SERVICE_AVAILABLE;
			if (namer.query_qnx (sname, tmp, &reg_type) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> problems locating %s\n", sname.c_str()));
				return NAMER_FAIL;	/// invalid name id.
			}

			if (reg_type != name_type)
			{
				ACE_DEBUG ((LM_WARNING, ">>>> the requested type differs from the actual one %s\n", sname.c_str()));
				return NAMER_FAIL;
			}

			YARPUniqueNameQnx *n = new YARPUniqueNameQnx(YARP_QNET);

			n->setName (name);
			n->setPid (tmp.getPid());
			n->setChannelID (tmp.getChan());
			n->setNode (tmp.getNode());

			return (YARPUniqueNameID *)n;
		}
		break;

	case YARP_TCP:
	case YARP_UDP:
		{
			///
			int reg_type = YARP_NO_SERVICE_AVAILABLE;
			if (namer.query (sname, addr, &reg_type) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> Problems locating %s\n", sname.c_str()));
				return NAMER_FAIL;	/// invalid name id.
			}

			if (network_name != NULL)
			{
				YARPString if_name;
				if (!VerifySame (namer, addr.get_host_addr(), network_name, if_name))
				{
					YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> the requested name doesn't belong to the target network %s\n", sname.c_str()));
					return NAMER_FAIL;
				}
			}

			YARPUniqueNameSock *n = new YARPUniqueNameSock (reg_type);
			
			n->setName (name);
			n->getAddressRef() = addr;

			return (YARPUniqueNameID *)n;
		}
		break;

	case YARP_MCAST:
		{
			ACE_ASSERT (network_name != NULL);

			/// 1. use the queried name to register an MCAST entry (ip + port # returned).
			/// - use the queried name + a postfix (is this a suitable strategy?) + a rnd number.
			/// - if it exists generate a new rnd number.

			/// 2. return it. This is used to call the CreateOutputEndpoint().

			/// 3. the corresponding UDP entry is queried separately and used in the 
			///		ConnectEndpoint().
			ACE_OS::srand (ACE_OS::time(NULL));

			ACE_INET_Addr addr;
			char fullname[YARP_STRING_LEN];
			int ret = YARP_FAIL;
			int reg_type = YARP_NO_SERVICE_AVAILABLE;

			do
			{
				ACE_OS::sprintf (fullname, "%s_mcast_%d\0", name, ACE_OS::rand());
				ret = namer.query (fullname, addr, &reg_type);
				if (ret != YARP_OK)
				{
					ACE_DEBUG ((LM_WARNING, ">>>> troubles registering %s\n", fullname));
					return NAMER_FAIL;
				}
			}
			while (addr.get_port_number() != 0);

			/// can't locate a name -> it's a new one.
			YARPString tname (fullname);
			if (namer.check_in_mcast(tname, addr) != YARP_OK)
			{
				ACE_DEBUG ((LM_WARNING, ">>>> can't get an MCAST group for %s\n", sname.c_str()));
				return NAMER_FAIL;
			}

			ACE_DEBUG((LM_DEBUG, "registering group name - hostname in addr : %s\n", addr.get_host_addr()));

			ACE_INET_Addr reg_addr;		/// who I am.
			char myhostname[YARP_STRING_LEN];
			getHostname (myhostname, YARP_STRING_LEN);
			reg_addr.set((u_short)0, myhostname);
			YARPString new_ip, new_nic;
			namer.query_nic (reg_addr.get_host_addr(), network_name, new_nic, new_ip);

			YARPUniqueNameSock *n = new YARPUniqueNameSock (YARP_MCAST);

			n->setName (fullname);
			n->setInterfaceName (new_nic);
			n->getAddressRef() = addr;

			return (YARPUniqueNameID *)n;
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
				if (namer.query_qnx (sname, tmp, &reg_type) != YARP_OK)
				{
					YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> problems locating %s, perhaps the name service is not running\n", sname.c_str()));
					return NAMER_FAIL;	/// invalid name id.
				}

				/// found a legitimate QNET name.
				if (reg_type == YARP_QNET)
				{
					YARPUniqueNameQnx *n = new YARPUniqueNameQnx(YARP_QNET);
					
					n->setName (name);
					n->setPid (tmp.getPid());
					n->setChannelID (tmp.getChan());
					n->setNode (tmp.getNode());

					return (YARPUniqueNameID *)n;
				}

				YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> problems locating %s in QNET protocol\n", sname.c_str()));
			}

			reg_type = YARP_NO_SERVICE_AVAILABLE;
			if (namer.query (sname, addr, &reg_type) != YARP_OK)
			{
				YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> problems locating %s also in TCP/UDP protocol\n", sname.c_str()));
				return NAMER_FAIL;	/// invalid name id.
			}
			
			if (network_name != NULL)
			{
				YARPString if_name;
				if (!VerifySame (namer, addr.get_host_addr(), network_name, if_name))
				{
					YARP_DBG(THIS_DBG) ((LM_WARNING, ">>>> the requested name doesn't belong to the target network %s\n", sname.c_str()));
					return NAMER_FAIL;
				}
			}

			/// query must return also the registration type.
			///
			YARPUniqueNameSock *n = new YARPUniqueNameSock (reg_type);
			n->setName (name);
			n->getAddressRef() = addr;

			return (YARPUniqueNameID *)n;
		}
		break;

	default:
		ACE_DEBUG ((LM_WARNING, ">>>> troubles locating name: %s\n", sname.c_str()));
		break;
	}

	return NAMER_FAIL;
}

#undef THIS_DBG

