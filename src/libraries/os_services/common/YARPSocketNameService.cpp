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
/// $Id: YARPSocketNameService.cpp,v 1.3 2003-04-18 09:25:48 gmetta Exp $
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
#include "YARPTime.h"
#include "ThreadInput.h"
#include "YARPNameID_defs.h"
#include "YARPNameService.h"

#define DBG(x) if ((x)>=40) 
//#define DBG(x) if (0) 


#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif


///
///
/// Linux implementation.
///
#ifdef __LINUX__			/////// LINUX

#include <pthread.h>

class ISHolder
{
public:
  YARPInputSocket sock;
  int offset;

  ISHolder() { offset = 0; }
};  

pthread_key_t registry_key;
pthread_once_t once_control = PTHREAD_ONCE_INIT;

static void destroy_from_key(void *ptr)
{
  if (ptr!=NULL)
    {
      delete ((ISHolder*)ptr);
    }
  DBG(5) ACE_OS::printf("^^^^^^^^ Destroyed an input socket handler\n");
}
   
static void init_registry_key(void)
{
  int status = pthread_key_create(&registry_key, destroy_from_key);
  DBG(5) ACE_OS::printf("^^^^^^^^ Initialized a key\n");
}

ISHolder *GetThreadData()
{
  void *key = pthread_getspecific(registry_key);
  return (ISHolder *) key;
}

YARPInputSocket *GetThreadSocket()
{
  ISHolder *data = GetThreadData();
  if (data!=NULL)
    {
      return &(data->sock);
    }
  return NULL;
}

int YARPSocketNameService::GetAssignedPort()
{
  int result = -1;
  YARPInputSocket *is = GetThreadSocket();
  if (is!=NULL)
    {
      result = is->GetAssignedPort();
    }
  return result;
}

/*  
int YARPNameService::ReadOffset(int delta, int absolute)
{
  // only one receiver possible per thread a-la qnx, so okay to do:
  int offset = 0;
  ISHolder *holder = GetThreadData();
  if (holder!=NULL)
    {
      int offset = holder->offset;
      if (absolute) { offset = deta; }
      else { offset += delta; }
      holder->offset = offset;
    }
  return delta;
}
*/

int YARPSocketNameService::RegisterName(const char *name)
{

  int name_status = -1;
  int result = -1;
  pthread_once(&once_control, &init_registry_key);

  void *key = pthread_getspecific(registry_key);
  assert(key==NULL);
  if (key==NULL)
    {
      ISHolder *holder = new ISHolder;
      assert(holder!=NULL);
      holder->sock.Register(name);
      result = holder->sock.GetAssignedPort();
      pthread_setspecific(registry_key, (void *)holder);
      DBG(5) ACE_OS::printf("^^^^^^^^ Made an input socket handler\n");
    } 

  return result;
}

YARPNameID YARPSocketNameService::LocateName(const char *name)
{
  YARPOutputSocket os;
  os.Connect(name);
  os.InhibitDisconnect();

  return YARPNameID(YARP_NAME_MODE_SOCKET,os.GetIdentifier());
}


#else				/////// NOT LINUX


/**************************************************************************
 qnx, cygwin, and NT version follows
 **************************************************************************/

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

static int my_getpid()
{
	return getpid();
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

#endif


///
///
///
///
class _yarp_endpoint
{
public:
	YARPSemaphore mutex;

/*
	/// adapter class
	class ISHolder
	{
	public:
		YARPInputSocket sock;
		int operator==(const ISHolder& other) {	return sock.GetIdentifier() == other.sock.GetIdentifier(); }
		int operator!=(const ISHolder& other) {	return sock.GetIdentifier() != other.sock.GetIdentifier(); }
		int operator<(const ISHolder& other) { return sock.GetIdentifier() < other.sock.GetIdentifier(); }
	};
*/

	typedef YARPNetworkObject *PYARPNetworkObject;
	typedef map<int, PYARPNetworkObject, less<int> > SMap;

//	typedef YARPInputSocket *PYARPInputSocket;
//	typedef map<int, PYARPInputSocket, less<int> > ISMap;

//	ISMap is_map;
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
		int pid = my_getpid();
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
		int result = -1;
		YARPNetworkObject *is = GetThreadSocket();
		if (is != NULL)
		{
			if (is->getSocketType() == YARP_I_SOCKET)
				result = ((YARPInputSocket *)is)->GetAssignedPort();
/*
			else
			if (is->_socktype == YARP_O_SOCKET)
				result = ((YARPOutputSocket *)is)->GetAssignedPort();
*/
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

///int YARPSocketEndpointManager::GetAssignedPort(void)
///{
///	return _endpointmanager.GetAssignedPort ();
///}

///
/// socket creation.
YARPNameID YARPSocketEndpointManager::CreateInputEndpoint (YARPUniqueNameID& name)
{
	//int name_status = -1;
	//int result = -1;

	DBG(5) ACE_OS::printf("^^^^^^^^ checking for previous definitions\n");

	if (GetThreadSocket() == NULL)
	{
		DBG(5) ACE_OS::printf("^^^^^^^^ checks out okay\n");
		int pid = my_getpid();
		_endpointmanager.mutex.Wait();

		DBG(5) ACE_OS::printf("^^^^^^^^ creating\n");
		YARPInputSocket *is = new YARPInputSocket;
		_endpointmanager._map[pid] = is;
		
		DBG(5) ACE_OS::printf("^^^^^^^^ preparing input socket\n");

		// prepare the socket for the connection (bind).
		is->Prepare (name);
		//is->Register(name);
		// result = is->GetAssignedPort();
		_endpointmanager.mutex.Post();
		
		DBG(5) ACE_OS::printf("^^^^^^^^ Made an input socket handler\n");
		ACE_OS::fflush(stdout);

		name.getNameID() = YARPNameID (YARP_TCP, is->GetIdentifier());
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
		DBG(5) ACE_OS::printf("^^^^^^^^ checks out okay\n");
		int pid = my_getpid();
		_endpointmanager.mutex.Wait();

		DBG(5) ACE_OS::printf("^^^^^^^^ creating\n");
		YARPOutputSocket *os = new YARPOutputSocket;
		_endpointmanager._map[pid] = os;
		
		DBG(5) ACE_OS::printf("^^^^^^^^ preparing output socket\n");

		// prepare the socket for the connection (bind).
		os->Prepare (name);
		//is->Register(name);
		// result = is->GetAssignedPort();
		_endpointmanager.mutex.Post();
		
		DBG(5) ACE_OS::printf("^^^^^^^^ Made an output socket handler\n");
		ACE_OS::fflush(stdout);

		name.getNameID() = YARPNameID (YARP_TCP, os->GetIdentifier());

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
		ACE_DEBUG ((LM_DEBUG, "ConnectEndpoints: no socket associated with current thread\n"));
		return YARP_FAIL;
	}

	if (sock->getSocketType() != YARP_O_SOCKET)
	{
		ACE_DEBUG ((LM_DEBUG, "ConnectEndpoints: trying to connect an input socket\n"));
		return YARP_FAIL;
	}

	/// I've got a socket!
	YARPOutputSocket *os = (YARPOutputSocket *)sock;
	os->Connect ();
	dest.setRawIdentifier (os->GetIdentifier ());

	return YARP_OK;
}

int YARPSocketEndpointManager::Close (void)
{
	YARPNetworkObject *sock = GetThreadSocket();
	if (sock == NULL)
	{
		ACE_DEBUG ((LM_DEBUG, "CloseEndpoint: no socket associated with current thread\n"));
		return YARP_FAIL;
	}

	switch (sock->getSocketType())
	{
	case YARP_O_SOCKET:
		{
			YARPOutputSocket *os = (YARPOutputSocket *)sock;
			os->Close ();
			break;
		}

	case YARP_I_SOCKET:
		{
			YARPInputSocket *is = (YARPInputSocket *)sock;
			is->CloseAll ();
			break;
		}
	}

	return YARP_OK;
}

/// implementation of YARPSocketNameService.
///
///
/*
int YARPSocketNameService::GetAssignedPort()
{
	int result = -1;
	YARPInputSocket *is = GetThreadSocket();
	if (is!=NULL)
	{
		result = is->GetAssignedPort();
	}
	return result;
}
*/

YARPUniqueNameID YARPSocketNameService::RegisterName(NameClient& namer, const char *name, int reg_type)
{
	/// remote registration.
	///
	ACE_INET_Addr addr;
	ACE_INET_Addr reg_addr;

	char buf[256];
	YARPNetworkObject::GetHostName (buf, 256);

	///
	reg_addr.set((int)0, buf);
	string tname (name);
	if (namer.check_in (tname, reg_addr, addr) != YARP_OK)
	{
		ACE_DEBUG ((LM_DEBUG, ">>>>Problems registering %s\n", name));
		return YARPUniqueNameID ();	/// invalid name id.
	}
	
	return YARPUniqueNameID(YARP_TCP, addr);
}

//YARPNameID YARPSocketNameService::LocateName(const char *name)
YARPUniqueNameID YARPSocketNameService::LocateName(NameClient& namer, const char *name)
{
	/// handle the connection w/ the remote name server.
	///
	ACE_INET_Addr addr;

	///
	if (namer.query (name, addr) != YARP_OK)
	{
		ACE_DEBUG ((LM_DEBUG, ">>>>Problems locating %s\n", name));
		return YARPUniqueNameID ();	/// invalid name id.
	}
	
	return YARPUniqueNameID(YARP_TCP, addr);
	///return YARPUniqueNameID();
}

#endif

