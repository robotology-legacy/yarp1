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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSocketMulti.cpp,v 1.2 2003-07-08 22:37:44 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Handle_Set.h>
#include <ace/Time_Value.h>
#include <ace/Sched_Params.h>

#ifdef __QNX6__
#include <signal.h>
#endif

#ifndef __QNX__
/// WIN32, Linux

#	include <string>
using namespace std;

#ifndef __WIN_MSVC__
#	include <unistd.h>  // just for gethostname
#endif

#else

#	include <string>
#	include <unix.h>  // just for gethostname

#endif

#include <list>

#include "YARPSocket.h"
#include "YARPSocketMulti.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPNameID.h"
#include "YARPScheduler.h"
#include "YARPTime.h"

///
#define THIS_DBG 80
const int _MAGIC_NUMBER = 7777;

///
/// yarp message header.
///
#include "begin_pack_for_net.h"

struct MyMessageHeader
{
public:
	char key[2];
	NetInt32 len;
	char key2[2];

	MyMessageHeader()
	{
		len = 0;
		SetBad();
	}

	void SetGood()
    {
		key[0] = 'Y';
		key[1] = 'A';
		key2[0] = 'R';
		key2[1] = 'P';
    }
  
	void SetBad()
    {
		key[0] = 'y';
		key[1] = 'a';
		key2[0] = 'r';
		key2[1] = 'p';
    }

	void SetLength(int n_len)
	{
		len = n_len;
	}
  
	int GetLength()
	{
		if (key[0] == 'Y' && key[1] == 'A' && key2[0] == 'R' && key2[1] == 'P')
		{
			return len;
		}
		else
		{
			//	  printf("*** Bad header\n");
			return -1;
		}
	}
} PACKED_FOR_NET;

#include "end_pack_for_net.h"

class YARPInputSocketMulti;
class _SocketThreadListMulti;

///
/// CLASS DEFINITIONS.
///		it handles multiple incoming connections in different protocols.
///
///
///
class _SocketThreadMulti : public YARPBareThread
{
protected:
	int _available;
	int _port;								/// udp port of the incoming udp "connection".
	ACE_SOCK_Dgram _local_socket;			/// a generic dgram socket used for service 
											///		communication (control messages).
											/// this is UDP protocol.
	YARPUniqueNameSock _local_addr;			/// UDP local addr.
	YARPUniqueNameSock _remote_endpoint;	/// remote peer for UDP communication.

	void *_socket;							/// a general purpose connector.
	YARPUniqueNameID *_socket_addr;			/// general socket address description.

	_SocketThreadListMulti *_owner;

	char *_extern_buffer;
	int _extern_length;
	char *_extern_reply_buffer;
	int _extern_reply_length;

	/// unfortunately to avoid multiple packet send/recv in MCAST/UDP requires
	/// a buffer copy. Multiple mcast/udp packets is deleterious in terms of bandwidth
	/// consumption.
	///	LATER: Shared memory might not need this extra copy.
	char _local_buffer[MAX_PACKET];
	int _local_buffer_counter;

	int _waiting;
	int _needs_reply;
	int _read_more;
	int _reply_preamble;
	YARPSemaphore _wakeup, _mutex, _reply_made;

	/// <remid> is the UDP connection address.
	/// <socket> is the generic connection address (is UniqueNameSock ok?).
	int _begin (const YARPUniqueNameSock *remid, const YARPUniqueNameID *socket, int port);

public:
	/// ctors.
	_SocketThreadMulti (const YARPUniqueNameSock *remid, const YARPUniqueNameID *socket, int port) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (remid, socket, port);
    }

	_SocketThreadMulti (void) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (NULL, NULL, 0);
    }

	virtual ~_SocketThreadMulti (void) 
	{
		if (_socket == NULL)
			return;

		switch (_socket_addr->getServiceType())
		{
		case YARP_MCAST:
			{
				if (_socket != NULL)
				{
					ACE_SOCK_Dgram_Mcast& mcast = *((ACE_SOCK_Dgram_Mcast *)_socket);
					delete &mcast;
				}
			}
			break;

		case YARP_UDP:
			break;

		case YARP_SHMEM:
			{
				if (_socket != NULL)
				{
					ACE_MEM_Acceptor& a = *((ACE_MEM_Acceptor *)_socket);
					delete &a;
				}
			}
			break;

		default:
			ACE_DEBUG ((LM_DEBUG, "SocketThreadMulti::destructor : shouldn't happen, unrecognized protocol\n"));
			break;
		}
	}

	/// needs a method to recycle the thread since
	/// thread creation might be a relatively costly process.

	/// required by stl interface.
	int operator == (const _SocketThreadMulti& other) { ACE_UNUSED_ARG(other); return 0; }
	int operator != (const _SocketThreadMulti& other) { ACE_UNUSED_ARG(other); return 0; }
	int operator <  (const _SocketThreadMulti& other) { ACE_UNUSED_ARG(other); return 0; }

	void setOwner(const _SocketThreadListMulti& n_owner);

	/// call it reconnect (recycle the thread).
	/// the thread shouldn't be running.
	int reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port);

	YARPUniqueNameSock& getRemoteID(void) { return _remote_endpoint; }
	ACE_HANDLE getID () const {	return  _local_socket.get_handle (); }
	int getServiceType (void) const { return (_socket_addr != NULL) ? _socket_addr->getServiceType() : YARP_NO_SERVICE_AVAILABLE; }

	virtual void End (int dontkill = 0);

	/// thread Body.
	virtual void Body (void);
	void BodyUdp (void);
	void BodyMcast (void);
	void BodyShmem (void);

	/// returns the last used port number for this thread.
	/// the thread might have been assigned a port number out of a pool
	///		this is done to manage UDP connections.
	int getOldPortNumber (void) { return _port; } 

	/// ----> state flags
	inline void setAvailable (int flag) { _available = flag; }
	inline int isAvailable (void) const { return _available; }

	inline int isWaiting (void) const { return _waiting; }
	inline void setWaiting (int w) { _waiting = w; }

	inline void setExternalBuffer (char *b) { _extern_buffer = b; }
	inline int getExternalBufferLength (void) const { return _extern_length; }
	inline void setExternalBufferLength (int l) { _extern_length = l; }

	inline void setNeedsReply (int i) { _needs_reply = i; }
	inline void setReadMore (int i) { _read_more = i; }
	inline void setReplyPreamble (int i) { _reply_preamble = i; }

	inline void setExternalReplyBuffer (char *b) { _extern_reply_buffer = b; }
	inline int getExternalReplyLength (void) const { return _extern_reply_length; }
	inline void setExternalReplyLength (int l) { _extern_reply_length = l; }

	inline void waitOnMutex (void) { _mutex.Wait (); }
	inline void postToMutex (void) { _mutex.Post (); }
	inline void postToWakeup (void) { _wakeup.Post (); }
	inline void waitOnReplyMade (void) { _reply_made.Wait (); }
};

///
///
///
///
///
///
class _SocketThreadListMulti : public YARPBareThread
{
private:
	ACE_INET_Addr _local_addr;						/// local address of the acceptor.
	ACE_SOCK_Dgram _acceptor_socket;				/// the acceptor UDP socket.

	list<_SocketThreadMulti *> _list;				/// managed list of threads.
	YARPSemaphore _new_data, _new_data_written;

	int _initialized;
	
	int *_ports;
	int _number_o_ports;
	int _last_assigned;		// index into array.

public:
	/// ctors
	_SocketThreadListMulti () : _local_addr (1111), _new_data(0), _new_data_written(0)
	{
		_ports = NULL;
		_number_o_ports = 0;
		_last_assigned = -1;
		_initialized = 0;
	}

	/// among other things, should correctly terminate threads.
	virtual ~_SocketThreadListMulti ();

	///
	/// creates the acceptor socket and listens to port.
	/// simply prepares the socket.
	ACE_HANDLE connect(const YARPUniqueNameSock& id);

	/// actually the assigned is what provided by the name server but this
	/// is done on another class.
	int getAssignedPort (void) { return _local_addr.get_port_number(); }
	ACE_HANDLE getID (void) { return (!_initialized) ? ACE_INVALID_HANDLE : _acceptor_socket.get_handle(); }

	void addSocket(void);
	int closeAll (void);

	/// closes a particular thread identified by its socket id. 
	///	the ACE_HANDLE is used as index into the list.
	int close (ACE_HANDLE reply_id);

	void declareDataAvailable (void);
	void declareDataWritten (void);

	virtual void Body (void)
	{
#if 0
		int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "acceptor thread at priority %d -> %d\n", GetPriority(), prio));
		if (SetPriority(prio) == YARP_FAIL)
		{
			ACE_DEBUG ((LM_DEBUG, "can't raise priority of acceptor thread, potential source of troubles\n"));
		}
#endif
		while (1)
		{
			addSocket();
		}
	}

	/// return the ACE_HANDLE as id for further reply.
	///
	int read(char *buf, int len, ACE_HANDLE *reply_pid = NULL);
	int pollingRead(char *buf, int len, ACE_HANDLE *reply_pid = NULL);
	int beginReply(ACE_HANDLE reply_pid, char *buf, int len);
	int reply(ACE_HANDLE reply_pid, char *buf, int len);

	/// this demands exact number of bytes.
	int receiveMore(ACE_HANDLE reply_pid, char *buf, int len);

	/// set a pool of port numbers to get ports for incoming connections.
	int setPool (int *ports, int number_o_ports) 
	{
		if (_ports != NULL) delete[] _ports;
		_ports = new int[number_o_ports];
		ACE_ASSERT (_ports != NULL);
		memcpy (_ports, ports, sizeof(int) * number_o_ports);
		_number_o_ports = number_o_ports;
		_last_assigned = -1;
		return YARP_OK;
	}
	
	int getNewPortNumberFromPool (void)
	{
		_last_assigned ++;
		if (_last_assigned >= _number_o_ports || _last_assigned < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "this should happen, _last_assigned port out of range!\n"));
			return 0;
		}

		return _ports[_last_assigned];
	}
};



///
/// METHODS IMPLEMENTATION
///
///
int _SocketThreadMulti::_begin (const YARPUniqueNameSock *remid, const YARPUniqueNameID *socket, int port = 0)
{
	_owner = NULL;
	_extern_buffer = NULL;
	_extern_length = 0;
	_extern_reply_buffer = NULL;
	_extern_reply_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_local_buffer_counter = 0;

	_port = port;
	if (port != 0)
	{
		ACE_ASSERT (remid != NULL && socket != NULL);
		ACE_ASSERT (remid->getServiceType() == YARP_UDP);

		_remote_endpoint = *remid;

		/// listen to this new port (this is the UDP channel).
		char myhostname[YARP_STRING_LEN];
		getHostname (myhostname, YARP_STRING_LEN);
		_local_addr.getAddressRef().set (port, myhostname);
		_local_socket.open (_local_addr.getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled
		
		YARPNetworkObject::setSocketBufSize (_local_socket, MAX_PACKET);

		switch (socket->getServiceType())
		{
		case YARP_MCAST:
			{
				_socket_addr = new YARPUniqueNameSock ((YARPUniqueNameSock*)socket);
				ACE_ASSERT (_socket_addr != NULL);

				/// build an MCAST socket.
				_socket = (void *)new ACE_SOCK_Dgram_Mcast;
				ACE_ASSERT (_socket != NULL);

				ACE_SOCK_Dgram_Mcast& mcast = *((ACE_SOCK_Dgram_Mcast *)_socket);
				
				if (_socket_addr != NULL)
				{
					mcast.open (((YARPUniqueNameSock&)_socket_addr).getAddressRef(), 0, 1);	// reuse addr enabled
					YARPNetworkObject::setSocketBufSize (mcast, MAX_PACKET);
					mcast.join (((YARPUniqueNameSock&)_socket_addr).getAddressRef(), 1, 0);

					if (mcast.get_handle() == ACE_INVALID_HANDLE)
						return YARP_FAIL;
				}
			}
			break;

		case YARP_UDP:
			{
				_socket_addr = new YARPUniqueNameSock ((YARPUniqueNameSock*)socket);
				ACE_ASSERT (_socket_addr != NULL);
				_socket = NULL;
			}
			break;

		case YARP_SHMEM:
			{
				_socket_addr = new YARPUniqueNameMem ((YARPUniqueNameMem*)socket);
				ACE_ASSERT (_socket_addr != NULL);

				_socket = (void *)new ACE_MEM_Acceptor (((YARPUniqueNameMem&)*_socket_addr).getAddressRef(), 1);
				ACE_ASSERT (_socket != NULL);
			}
			break;

		default:
			ACE_DEBUG ((LM_DEBUG, "SocketThreadMulti::_begin: protocol not implemented\n"));
			_socket = NULL;
			break;
		}

		_local_addr.setServiceType (YARP_UDP);
		_local_addr.setRawIdentifier (_local_socket.get_handle());

		if (_local_socket.get_handle() == ACE_INVALID_HANDLE)
		{
			return YARP_FAIL;
		}
		_available = 0;
	}
	else
	{
		_available = 1;
		_socket = NULL;
		_remote_endpoint.invalidate();
		_socket_addr = NULL;
	}

	_read_more = 0;
	_reply_preamble = 0;

	return YARP_OK;
}

void _SocketThreadMulti::setOwner(const _SocketThreadListMulti& n_owner)
{
	_owner = (_SocketThreadListMulti *)(&n_owner);
}

/// call it reconnect (recycle the thread).
/// the thread shouldn't be running.
int _SocketThreadMulti::reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port)
{
	/// remid is always UDP.
	ACE_ASSERT (remid->getServiceType() == YARP_UDP);

	/// test the old address first.
	switch (socket->getServiceType())
	{
	case YARP_MCAST:
		{
			if (_socket != NULL)
			{
				((ACE_SOCK_Dgram_Mcast *)_socket)->close ();
				delete (ACE_SOCK_Dgram_Mcast *)_socket;
				_socket = NULL;
			}

			if (_socket_addr != NULL) 
			{
				delete _socket_addr;
				_socket_addr = NULL;
			}
		}
		break;

	case YARP_UDP:
		break;

	case YARP_SHMEM:
		{
			if (_socket != NULL)
			{
				((ACE_MEM_Acceptor *)_socket)->close ();
				delete (ACE_MEM_Acceptor *)_socket;
				_socket = NULL;
			}

			if (_socket_addr != NULL) 
			{
				delete _socket_addr;
				_socket_addr = NULL;
			}
		}
		break;

	default:
		break;
	}

	if (port != 0)
	{
		_remote_endpoint = *remid;

		_local_socket.close ();

		_port = port;

		/// listen to this new port.
		char myhostname[YARP_STRING_LEN];
		getHostname (myhostname, YARP_STRING_LEN);
		_local_addr.getAddressRef().set (port, myhostname);
		_local_socket.open (_local_addr.getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled
		YARPNetworkObject::setSocketBufSize (_local_socket, MAX_PACKET);

		switch (socket->getServiceType())
		{
		case YARP_MCAST:
			{
				_socket_addr = new YARPUniqueNameSock ((YARPUniqueNameSock*)socket);
				ACE_ASSERT (_socket_addr != NULL);

				_socket = (void *)new ACE_SOCK_Dgram_Mcast;
				ACE_ASSERT (_socket != NULL);

				ACE_SOCK_Dgram_Mcast& mcast = *((ACE_SOCK_Dgram_Mcast *)_socket);

				mcast.open (((YARPUniqueNameSock&)_socket_addr).getAddressRef(), 0, 1);	// reuse addr enabled
				YARPNetworkObject::setSocketBufSize (mcast, MAX_PACKET);
				mcast.join (((YARPUniqueNameSock&)_socket_addr).getAddressRef(), 1, 0);
				if (mcast.get_handle() == ACE_INVALID_HANDLE)
					return YARP_FAIL;
			}
			break;

		case YARP_UDP:
			_socket_addr = new YARPUniqueNameSock ((YARPUniqueNameSock*)socket);
			ACE_ASSERT (_socket_addr != NULL);

			_socket = NULL;
			/// it doesn't require an extra socket because the protocol is the same of the
			///		service channel.
			break;

		case YARP_SHMEM:
			{
				_socket_addr = new YARPUniqueNameMem ((YARPUniqueNameMem*)socket);
				ACE_ASSERT (_socket_addr != NULL);

				_socket = (void *)new ACE_MEM_Acceptor (((YARPUniqueNameMem&)*_socket_addr).getAddressRef(), 1);
				ACE_ASSERT (_socket != NULL);
			}
			break;

		default:
			ACE_DEBUG ((LM_DEBUG, "SocketThreadMulti::reuse: protocol not implemented\n"));
			_socket = NULL;
			break;
		}

		_local_addr.getNameID() = YARPNameID (YARP_UDP, _local_socket.get_handle());

		if (_local_socket.get_handle() == ACE_INVALID_HANDLE)
		{
			return YARP_FAIL;
		}
		_available = 0;
	}
	else
	{
		_available = 1;
		_socket = NULL;
		_remote_endpoint.invalidate();
		_socket_addr = NULL;
	}

	return YARP_OK;
}

void _SocketThreadMulti::End (int dontkill /* = 0 */)
{
	ACE_UNUSED_ARG (dontkill);

	YARPBareThread::End ();
	_mutex.Wait ();
	
	if (_local_socket.get_handle() != ACE_INVALID_HANDLE)
		_local_socket.close();

	switch (_socket_addr->getServiceType())
	{
	case YARP_MCAST:
		{
			if (_socket != NULL)
			{
				ACE_SOCK_Dgram_Mcast& mcast = *((ACE_SOCK_Dgram_Mcast *)_socket);
				if (mcast.get_handle() != ACE_INVALID_HANDLE)
					mcast.close();
				delete &mcast;
				_socket = NULL;
			}

			if (_socket_addr != NULL)
			{
				delete _socket_addr;
				_socket_addr = NULL;
			}
		}
		break;

	case YARP_UDP:
		{
			if (_socket_addr != NULL)
			{
				delete _socket_addr;
				_socket_addr = NULL;
			}

			/// it doesn't require an extra socket because the protocol is the same of the
			///		service channel.
			_socket = NULL;
		}
		break;

	case YARP_SHMEM:
		{
			if (_socket != NULL)
			{
				ACE_MEM_Acceptor& a = *((ACE_MEM_Acceptor *)_socket);
				if (a.get_handle() != ACE_INVALID_HANDLE)
					a.close();
				delete &a;
				_socket = NULL;
			}
			
			if (_socket_addr != NULL)
			{
				delete _socket_addr;
				_socket_addr = NULL;
			}
		}
		break;

	default:
		{
			ACE_DEBUG ((LM_DEBUG, "SocketThreadMulti::End : protocol not implemented\n"));
			_socket = NULL;
		}
		break;
	}

	_mutex.Post ();
}



/// thread Body.
///	error check is not consistent.
void _SocketThreadMulti::Body (void)
{
#ifdef __QNX6__
	signal (SIGPIPE, SIG_IGN);
#endif

#if 0
	int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "reader thread at priority %d -> %d\n", GetPriority(), prio));
	if (SetPriority(prio) == YARP_FAIL)
	{
		ACE_DEBUG ((LM_DEBUG, "can't raise priority of reader thread, potential source of troubles\n"));
	}
#endif

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;

	switch (_socket_addr->getServiceType())
	{
	case YARP_UDP:
		BodyUdp ();			/// udp only connection.
		break;

	case YARP_MCAST:
		BodyMcast ();		/// mcast + udp channel for service.
		break;

	case YARP_SHMEM:
		BodyShmem ();		/// shmem + udp channel for service.
		break;

	default:
		ACE_DEBUG ((LM_DEBUG, "troubles starting a reader thread\n"));
		break;
	}

	_mutex.Wait ();

	/// the socket is already closed.
	_available = 1;
	_mutex.Post ();

	ACE_DEBUG ((LM_DEBUG, "A reader thread returned\n"));

#ifdef __WIN32__
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? comms thread bailed out\n"));
#else
	/// what is this for?
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? comms thread %d bailed out\n", getpid()));
#endif
}

///
///
///
/// if connection dies the thread should be made available.
///
void _SocketThreadMulti::BodyShmem (void)
{
	ACE_Time_Value long_timeout (YARP_STALE_TIMEOUT, 0);

	int finished = 0;
	int r = YARP_FAIL;
	int was_preamble = 0;
	int len = 0;	

	char bufack[] = "acknowledged";
	char *buf3 = bufack;

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread about to accept SHMEM on port %d\n", ((YARPUniqueNameMem *)_socket_addr)->getAddressRef().get_port_number()));
	ACE_MEM_Acceptor& a = *((ACE_MEM_Acceptor *)_socket);
	ACE_MEM_Stream stream;
	a.accept (stream);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread accepted a new stream\n"));

	while (!finished)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread SHMEM of remote port %s:%d waiting\n", _remote_endpoint.getAddressRef().get_host_addr(), _remote_endpoint.getAddressRef().get_port_number()));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread SHMEM waiting on port %d waiting\n", _local_addr.getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		r = stream.recv_n (&hdr, sizeof(hdr), 0);

		/// this is supposed to read the header, r must be > 0
		if (r < 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing (recv returned %d)\n", r));

			_local_socket.close ();
			finished = 1;
			goto ShmemSocketMsgSkip;
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMulti, len = %d\n", hdr.GetLength()));

		len = hdr.GetLength();
		if (len < 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));
			
			_local_socket.close ();
			finished = 1;
			goto ShmemSocketMsgSkip;
		}

		if (len >= 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got a header\n"));
			if (_owner != NULL)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? and i am owned\n"));
				_mutex.Wait();
				_extern_buffer = NULL;
				_extern_length = len;

				/// ???
				_owner->declareDataAvailable();

				_waiting = 1;
				_mutex.Post();
				_wakeup.Wait();
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? woken up! (it means I can read more data from channel)\n"));
			}

			if (_extern_buffer != NULL)
			{
				if (len > _extern_length)
				{
					len = _extern_length;
				}

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read the data buffer\n"));

				r = stream.recv_n (_extern_buffer, len , 0);

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received a buffer _SocketThreadMulti\n"));

				_extern_length = r;
				int rep = _needs_reply;
				
				_owner->declareDataWritten();
				if (rep)
				{
					_wakeup.Wait();
					_needs_reply = 0;
				}

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read more =>>>> %d\n", _read_more));

				while (_read_more)
				{
					int rr = 0;

					/// this was r too, a bit confusing.
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read more data\n"));

					if (_extern_reply_length == 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read 0 len buffer, not waiting\n"));
					}
					else
					{
						rr = stream.recv_n (_extern_reply_buffer, _extern_reply_length, 0); 
					}

					_extern_reply_length = rr;
					_read_more = 0;
					_reply_made.Post();
					_wakeup.Wait();
					_needs_reply = 0;
				}

				was_preamble = 0;

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to go into sending reply\n"));

				do
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? ---> iterating send(s)\n"));
					if (_reply_preamble)
					{
						rep = 1;
					}

					MyMessageHeader hdr2;
					hdr2.SetGood();
					buf3 = bufack;
					int reply_len = 0;
					if (rep)
					{
						buf3 = _extern_reply_buffer;
						reply_len = _extern_reply_length;
					}

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? sending reply _SocketThreadMulti\n"));

					hdr2.SetLength(reply_len);
					stream.send_n (&hdr2, sizeof(hdr2), 0);

					if (reply_len > 0)
					{
						stream.send_n (buf3, reply_len, 0);
					}

					int curr_preamble = _reply_preamble;
					if (rep)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? POSTING reply made %d\n", curr_preamble));
						_reply_made.Post();
					}

					was_preamble = 0;
					if (curr_preamble)
					{
						was_preamble = 1;

						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? WAITING for post-preamble wakeup\n", r));
						_wakeup.Wait();
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? DONE WAITING for post-preamble wakeup\n", r));
						rep = 1;
					}
				}
				while (was_preamble);
			}
		}

ShmemSocketMsgSkip:
		/// this is to skip the rest of the message.
		;
	}	/// while !finished

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? SHMEM thread bailing out\n"));
}
    

void _SocketThreadMulti::BodyUdp (void)
{
	ACE_Time_Value long_timeout (YARP_STALE_TIMEOUT, 0);

	int finished = 0;
	
	ACE_INET_Addr incoming;

	int r = YARP_FAIL;
	int was_preamble = 0;
	int len = 0;	

	char bufack[] = "acknowledged";
	char *buf3 = bufack;

	memset (_local_buffer, 0, MAX_PACKET);
	_local_buffer_counter = 0;

	while (!finished)
	{
		MyMessageHeader& hdr = *((MyMessageHeader *)_local_buffer);

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread of remote port %s:%d waiting\n", _remote_endpoint.getAddressRef().get_host_addr(), _remote_endpoint.getAddressRef().get_port_number()));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread waiting on port %d waiting\n", _local_addr.getAddressRef().get_port_number()));

		ACE_Handle_Set set;
		set.reset ();
		set.set_bit (_local_socket.get_handle());
		int max = (int)_local_socket.get_handle();

		int rr = ACE_OS::select (max + 1, set, 0, 0, &long_timeout);

		if (rr <= 0)
		{
			if (rr == 0)
			{
				ACE_DEBUG ((LM_DEBUG, "select on MCAST-UDP reader timed out, perhaps this is a stale connection, closing down\n"));
			}
			else
			{
				ACE_DEBUG ((LM_DEBUG, "this is shouldn't happen, a bug???\n"));
			}

			_local_socket.close ();

			finished = 1;
			goto DgramSocketMsgSkip;
		}

		iovec iov[1];
		iov[0].iov_base = _local_buffer;
		iov[0].iov_len = MAX_PACKET;

		r = _local_socket.recv (iov, 1, incoming, 0);
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_addr(), incoming.get_port_number()));

		_local_buffer_counter = sizeof(MyMessageHeader);

		if (r >= 0 && hdr.GetLength() == (_MAGIC_NUMBER + 1))
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "received a message to close the thread\n"));
			/// a legitimate message to close down the thread.
			/// sends reply to caller.
			hdr.SetGood ();
			hdr.SetLength (0);
			_local_socket.send (&hdr, sizeof(hdr), incoming);

			_local_socket.close ();
			finished = 1;
			goto DgramSocketMsgSkip;
		}

		/// this is supposed to read the header, r must be > 0
		if (r < 0 || incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing (recv returned %d)\n", r));
			if (incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "returning because incoming diffs from remote addr\n"));
			}

			_local_socket.close ();
			finished = 1;
			goto DgramSocketMsgSkip;
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMulti, len = %d\n", hdr.GetLength()));

		len = hdr.GetLength();
		if (len < 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));
			
			_local_socket.close ();
			finished = 1;
			goto DgramSocketMsgSkip;
		}

		if (len >= 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got a header\n"));
			if (_owner != NULL)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? and i am owned\n"));
				_mutex.Wait();
				_extern_buffer = NULL;
				_extern_length = len;

				/// ???
				_owner->declareDataAvailable();

				_waiting = 1;
				_mutex.Post();
				_wakeup.Wait();
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? woken up! (it means I can read more data from channel)\n"));
			}

			if (_extern_buffer != NULL)
			{
				if (len > _extern_length)
				{
					len = _extern_length;
				}

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read the data buffer\n"));

				memcpy (_extern_buffer, _local_buffer+_local_buffer_counter, len); 
				_local_buffer_counter += len;
				r = len;
			
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received a buffer _SocketThreadMulti\n"));

				_extern_length = r;
				int rep = _needs_reply;
				
				_owner->declareDataWritten();
				if (rep)
				{
					_wakeup.Wait();
					_needs_reply = 0;
				}

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read more =>>>> %d\n", _read_more));

				while (_read_more)
				{
					/// this was r too, a bit confusing.
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read more data\n"));

					if (_extern_reply_length == 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read 0 len buffer, not waiting\n"));
					}
					else
					{
						memcpy (_extern_reply_buffer, _local_buffer+_local_buffer_counter, _extern_reply_length);
						_local_buffer_counter += _extern_reply_length;
					}

					_read_more = 0;
					_reply_made.Post();
					_wakeup.Wait();
					_needs_reply = 0;
				}

				was_preamble = 0;

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to go into sending reply\n"));

				/// creates a local buffer and sends it.
				_local_buffer_counter = 0;

				do
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? ---> iterating send(s)\n"));
					if (_reply_preamble)
					{
						rep = 1;
					}

					MyMessageHeader hdr2;
					hdr2.SetGood();
					buf3 = bufack;
					int reply_len = 0;
					if (rep)
					{
						buf3 = _extern_reply_buffer;
						reply_len = _extern_reply_length;
					}

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? sending reply _SocketThreadMulti\n"));

					hdr2.SetLength(reply_len);
					memcpy (_local_buffer + _local_buffer_counter, &hdr2, sizeof(hdr2));
					_local_buffer_counter += sizeof(hdr2);

					if (reply_len > 0)
					{
						memcpy (_local_buffer + _local_buffer_counter, buf3, reply_len);
						_local_buffer_counter += reply_len;
					}

					int curr_preamble = _reply_preamble;
					if (rep)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? POSTING reply made %d\n", curr_preamble));
						_reply_made.Post();
					}

					was_preamble = 0;
					if (curr_preamble)
					{
						was_preamble = 1;

						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? WAITING for post-preamble wakeup\n", r));
						_wakeup.Wait();
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? DONE WAITING for post-preamble wakeup\n", r));
						rep = 1;
					}
				}
				while (was_preamble);

				/// sends reply as a single packet.
				iovec iov;
				iov.iov_base = _local_buffer;
				iov.iov_len = _local_buffer_counter;

				_local_socket.send (&iov, 1, _remote_endpoint.getAddressRef(), 0);
			}
		}

DgramSocketMsgSkip:
		/// this is to skip the rest of the message.
		;
	}	/// while !finished
}


///
/// mcast receiver.
void _SocketThreadMulti::BodyMcast (void)
{
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_Time_Value long_timeout (YARP_STALE_TIMEOUT, 0);

	ACE_INET_Addr incoming;
	
	int finished = 0;
	memset (_local_buffer, 0, MAX_PACKET);
	_local_buffer_counter = 0;

	ACE_ASSERT (_socket != NULL);
	ACE_SOCK_Dgram_Mcast& mcast_socket = *((ACE_SOCK_Dgram_Mcast *)_socket);

	YARPUniqueNameSock& addr = *((YARPUniqueNameSock *)_socket_addr);
	///
	while (!finished)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread of group %s:%d waiting\n", addr.getAddressRef().get_host_addr(), addr.getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		hdr.SetBad();
		int r = YARP_FAIL;

		ACE_Handle_Set set;
		set.reset ();
		set.set_bit (_local_socket.get_handle());
		set.set_bit (mcast_socket.get_handle());
		int max = ((int)_local_socket.get_handle() > (int)mcast_socket.get_handle()) 
			? (int)_local_socket.get_handle() : (int)mcast_socket.get_handle();

		int rr = ACE_OS::select (max + 1, set, 0, 0, &long_timeout);

		if (rr <= 0)
		{
			if (rr == 0)
			{
				ACE_DEBUG ((LM_DEBUG, "select on MCAST reader timed out, perhaps this is a stale connection, closing down\n"));
			}
			else
			{
				ACE_DEBUG ((LM_DEBUG, "this is shouldn't happen, a bug???\n"));
			}

			_local_socket.close ();
			mcast_socket.close ();

			finished = 1;
			goto McastSocketMsgSkip;
		}
		
		///
		if (rr >= 1 && set.is_set (_local_socket.get_handle()))
		{
			r = _local_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_addr(), incoming.get_port_number()));

			if (r >= 0 && hdr.GetLength() == (_MAGIC_NUMBER + 1))
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "received a message to close the thread\n"));
				/// a legitimate message to close down the thread.
				/// sends reply to caller.
				hdr.SetGood ();
				hdr.SetLength (0);
				_local_socket.send (&hdr, sizeof(hdr), incoming);
				_local_socket.close ();

				/// need to close (properly) also the mcast socket.
				mcast_socket.close ();
	
				finished = 1;
				goto McastSocketMsgSkip;
			}

			if (rr == 1)
				goto McastSocketMsgSkip;
				///continue;
		}
		
		if (set.is_set (mcast_socket.get_handle()))
		{
			iovec iov[1];
			iov[0].iov_base = _local_buffer;
			iov[0].iov_len = MAX_PACKET;

			r = mcast_socket.recv (iov, 1, incoming, 0);
			if (r < 0)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "lotta big troubles\n"));
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));

				/// close down sockets.
				_local_socket.close ();
				mcast_socket.close ();
				finished = 1;
			}

			MyMessageHeader& hdr = *((MyMessageHeader *)_local_buffer);
			_local_buffer_counter = sizeof(hdr);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_addr(), incoming.get_port_number()));
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMulti, len = %d\n", hdr.GetLength()));

			int len = hdr.GetLength();
			if (len < 0)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));

				/// close down sockets.
				_local_socket.close ();
				mcast_socket.close ();
				finished = 1;
			}
			else
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got a header\n"));
				if (_owner != NULL)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? and i am owned\n"));
					_mutex.Wait();
					_extern_buffer = NULL;
					_extern_length = len;

					/// ???
					_owner->declareDataAvailable();

					_waiting = 1;
					_mutex.Post();
					_wakeup.Wait();
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? woken up! (it means I can read more data from channel)\n"));
				}

				if (_extern_buffer != NULL)
				{
					if (len > _extern_length)
					{
						len = _extern_length;
					}

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read the data buffer\n"));

					/// shouldn't be much different from the real recv.
					memcpy (_extern_buffer, _local_buffer + _local_buffer_counter, len); 
					_local_buffer_counter += len;
					_extern_length = len;
					int rep = _needs_reply;
					
					_owner->declareDataWritten();

					if (rep)
					{
						_wakeup.Wait();
						_needs_reply = 0;
					}

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read more =>>>> %d\n", _read_more));

					while (_read_more)
					{
						/// this was r too, a bit confusing.
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read more data\n"));

						if (_extern_reply_length != 0)
						{
							memcpy (_extern_reply_buffer, _local_buffer + _local_buffer_counter, _extern_reply_length);
							_local_buffer_counter += _extern_reply_length;
						}

						_read_more = 0;
						_reply_made.Post();
						_wakeup.Wait();
						_needs_reply = 0;
					}

					/// no reply here.

				} /// if _extern_buf != NULL
			
			} /// if len > 0

		} /// if is_set (MCAST)...

McastSocketMsgSkip:
		;
	} /// while !finished

	/// returning because the thread is closing down.
}



///
///
///

/// among other things, should correctly terminate threads.
_SocketThreadListMulti::~_SocketThreadListMulti (void)
{
	if (_ports != NULL) delete[] _ports;
	_ports = NULL;
	_number_o_ports = 0;
	_last_assigned = -1;

	for (list<_SocketThreadMulti *>::iterator it = _list.begin(); it != _list.end(); it++)
	{
		(*it)->End ();
		delete (*it);
	}

	_initialized = 0;
}


///
/// creates the acceptor socket and listens to port.
/// simply prepares the socket.
ACE_HANDLE _SocketThreadListMulti::connect (const YARPUniqueNameSock& id)
{
	_local_addr = ((YARPUniqueNameSock&)id).getAddressRef();
	_acceptor_socket.open (_local_addr, ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled

	if (_acceptor_socket.get_handle() == ACE_INVALID_HANDLE)
	{
		ACE_DEBUG ((LM_DEBUG, ">>>>> problems with opening receiving UDP at %s:%d\n", _local_addr.get_host_addr(), _local_addr.get_port_number()));
		return ACE_INVALID_HANDLE;

		/// and the thread is not started.
	}
	
	/// closes down any still open thread (just in case?).
	if (_initialized) closeAll ();

	_initialized = 1;

	Begin();

	return _acceptor_socket.get_handle ();
}


void _SocketThreadListMulti::addSocket (void)
{
	// need to keep calling this to get next incoming socket
	ACE_ASSERT (_initialized != 0);

#ifdef __QNX__
	signal( SIGCHLD, SIG_IGN );     /* Ignore condition */
#endif

	/// need to assert that _acceptor_socket is actually created and connected.
	/// ACE_ASSERT (_acceptor_socket != NULL);

	YARPScheduler::yield();
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "7777777 pre accept %d\n", errno));

	ACE_INET_Addr incoming;
	int port_number = 0;
	MyMessageHeader hdr;

	hdr.SetBad();

	int ra = -1;
	do
	{
		ra = _acceptor_socket.recv (&hdr, sizeof(hdr), incoming);		/// wait forever.
		if (ra < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "-------->>>>> acceptor_socket got garbage, trying again\n"));
			YARPTime::DelayInSeconds(1);
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "got something on acceptor socket...\n"));
	}
	while (ra == -1);

	/// MAGIC_NUMBER	-> UDP
	/// MAGIC_NUMBER+1	-> MCAST
	/// MAGIC_NUMBER+2	-> SHMEM
	///
	///
	if (hdr.GetLength() == _MAGIC_NUMBER)
	{
		/// checks whether <incoming> already tried a connection
		///		and it is still connected.
		list<_SocketThreadMulti *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if (!(*it_avail)->isAvailable ())
			{
				if ((*it_avail)->getRemoteID().getAddressRef().get_host_addr() == incoming.get_host_addr() &&
					(*it_avail)->getRemoteID().getAddressRef().get_port_number() == incoming.get_port_number())
				{
					ACE_DEBUG ((LM_DEBUG, "thread already connected %s:%d\n", incoming.get_host_addr(), incoming.get_port_number()));
					break;
				}
			}
		}

		/// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>> accepting a new socket from %s:%d\n", incoming.get_host_addr(), incoming.get_port_number()));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept %d, going to determine port number\n", errno));

		/// get a new available port number associated to this IP.
		/// recycle thread and port # if possible.
		int reusing = 0;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber (); 
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		if (port_number == 0)
		{
			/// failed connection, for any reason (likely too many connections).
			ACE_DEBUG ((LM_DEBUG, "777777 can't get a port, too many connections\n"));
			hdr.SetBad ();
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);
		}
		else
		{
			if (it_avail == _list.end())
			{
				_list.push_back(new _SocketThreadMulti());
				it_avail = _list.end();
				it_avail--;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				(*it_avail)->reuse (&YARPUniqueNameSock(YARP_UDP, incoming), &YARPUniqueNameSock(YARP_UDP), port_number);
				(*it_avail)->Begin();
			}
			else
			{
				(*it_avail)->End();
				(*it_avail)->reuse (&YARPUniqueNameSock(YARP_UDP, incoming), &YARPUniqueNameSock(YARP_UDP), port_number);
				(*it_avail)->Begin();
			}

			/// send reply to incoming socket.
			/// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}
	}
	else
	if (hdr.GetLength() == (_MAGIC_NUMBER + 1))
	{
		/// ask an MCAST connection.

		/// gets the group and port number of the incoming connection.
		/// gets 4 bytes w/ ip addr and 2 more bytes w/ port number.
		unsigned char smallbuf[6];
		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
		int r = _acceptor_socket.recv ((void *)smallbuf, 6, incoming, 0, &timeout);
		if (r < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "777777 connection failed\n"));

			/// didn't get the ip and port #.
			hdr.SetBad ();
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);
			return;
		}

		char ip[24];
		int port = smallbuf[4] * 256 + smallbuf[5];
		ACE_OS::sprintf (ip, "%u.%u.%u.%u:%u\0", smallbuf[0], smallbuf[1], smallbuf[2], smallbuf[3], port);
		ACE_INET_Addr group (ip);	/// problems here!!!!!!!!

		/// checks whether <incoming> already tried a connection
		///		and it is still connected.
		list<_SocketThreadMulti *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if (!(*it_avail)->isAvailable ())
			{
				if ((*it_avail)->getRemoteID().getAddressRef().get_host_addr() == group.get_host_addr() &&
					(*it_avail)->getRemoteID().getAddressRef().get_port_number() == group.get_port_number())
				{
					ACE_DEBUG ((LM_DEBUG, "thread already connected %s:%d\n", group.get_host_addr(), group.get_port_number()));
					break;
				}
			}
		}

		/// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>> accepting a new socket from %s:%d\n", incoming.get_host_addr(), incoming.get_port_number()));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept %d, going to determine port number\n", errno));

		/// get a new available port number associated to this IP.
		/// recycle thread and port # if possible.
		int reusing = 0;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber (); 
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		if (port_number == 0)
		{
			///
			hdr.SetBad ();
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);
		}
		else
		{
			if (it_avail == _list.end())
			{
				_list.push_back(new _SocketThreadMulti());
				it_avail = _list.end();
				it_avail--;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				(*it_avail)->reuse (&YARPUniqueNameSock(YARP_UDP, incoming), &YARPUniqueNameSock(YARP_MCAST, group), port_number);
				(*it_avail)->Begin();
			}
			else
			{
				(*it_avail)->End();
				(*it_avail)->reuse (&YARPUniqueNameSock(YARP_UDP, incoming), &YARPUniqueNameSock(YARP_MCAST, group), port_number);
				(*it_avail)->Begin();
			}

			/// send reply to incoming socket.
			/// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}
	}
	else
	if (hdr.GetLength() == (_MAGIC_NUMBER + 2))
	{
		/// ask a SHMEM connection.

		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

		/// checks whether <incoming> already tried a connection
		///		and it is still connected.
		list<_SocketThreadMulti *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if (!(*it_avail)->isAvailable ())
			{
				if ((*it_avail)->getRemoteID().getAddressRef().get_host_addr() == incoming.get_host_addr() &&
					(*it_avail)->getRemoteID().getAddressRef().get_port_number() == incoming.get_port_number())
				{
					ACE_DEBUG ((LM_DEBUG, "thread already connected %s:%d\n", incoming.get_host_addr(), incoming.get_port_number()));
					break;
				}
			}
		}

		/// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>> accepting a new socket from %s:%d\n", incoming.get_host_addr(), incoming.get_port_number()));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept %d, going to determine port number\n", errno));

		/// get a new available port number associated to this IP.
		/// recycle thread and port # if possible.
		int reusing = 0;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber (); 
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		if (port_number == 0)
		{
			/// failed connection, for any reason (likely too many connections).
			ACE_DEBUG ((LM_DEBUG, "777777 can't get a port, too many connections\n"));
			hdr.SetBad ();
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);
		}
		else
		{
			if (it_avail == _list.end())
			{
				_list.push_back(new _SocketThreadMulti());
				it_avail = _list.end();
				it_avail--;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				/// need a port number for SHMEM? or can I recycle the same as UDP, 'cause SHMEM messaging is TCP?
				(*it_avail)->reuse (&YARPUniqueNameSock(YARP_UDP, incoming), &YARPUniqueNameMem(YARP_SHMEM, port_number), port_number);
				(*it_avail)->Begin();
			}
			else
			{
				(*it_avail)->End();
				(*it_avail)->reuse (&YARPUniqueNameSock(YARP_UDP, incoming), &YARPUniqueNameMem(YARP_SHMEM, port_number), port_number);
				(*it_avail)->Begin();
			}

			/// send reply to incoming socket.
			/// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			_acceptor_socket.send (&hdr, sizeof(hdr), incoming);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}
	}
	else
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "corrupted header received, abort connection attempt, listening\n"));
	}
}

/// closes everything.
int _SocketThreadListMulti::closeAll (void)
{
	ACE_ASSERT (_initialized != 0);

	list<_SocketThreadMulti *>::iterator it_avail;

	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		/// it's a thread, ask for termination.
		(*it_avail)->End ();
	}

	/// terminates itself.
	End ();

	_acceptor_socket.close ();
	_initialized = 0;

	return 1;
}

/// closes a particular thread identified by its socket id. 
///	the ACE_HANDLE is used as index into the list.
int _SocketThreadListMulti::close (ACE_HANDLE reply_id)
{
	ACE_ASSERT (_initialized != 0);

	int result = -1;
	list<_SocketThreadMulti *>::iterator it_avail;

	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable ()))
		{
			if ((*it_avail)->getID () == reply_id)
			{
				(*it_avail)->End ();
				(*it_avail)->setAvailable (1);
				result = 0;
			}
		}
	}

	return result;
}

void _SocketThreadListMulti::declareDataAvailable (void)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "$$$ Declaring data available\n"));
	_new_data.Post ();	
}

void _SocketThreadListMulti::declareDataWritten (void)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "$$$ Declaring new data written\n"));
	_new_data_written.Post ();	
}


/// returns the ACE_HANDLE as id for further reply.
///	returns also the number of bytes read.
int _SocketThreadListMulti::read(char *buf, int len, ACE_HANDLE *reply_pid)
{
	////ACE_ASSERT (_initialized != 0);

	ACE_HANDLE save_pid = ACE_INVALID_HANDLE;

#ifdef __QNX__
	signal( SIGCHLD, SIG_IGN );     /* Ignore condition */
#endif

	int finished = 0;
	int result = YARP_FAIL;

	while (!finished)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Waiting for new data\n"));
		_new_data.Wait();

		/// checks for proper initialization. maybe should move the wait for new data
		/// at the very beginning in this case?
		ACE_ASSERT (_initialized != 0);

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Got new data\n"));

		list<_SocketThreadMulti *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			/// WARNING: isAvailable is read here without mut exclusion!
			if (!((*it_avail)->isAvailable()))
			{
				int work = 0, in_len = 0;
				(*it_avail)->waitOnMutex ();		/// _mutex.Wait();

				if ((*it_avail)->isWaiting())
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Identified source of new data\n"));

					work = 1;
					(*it_avail)->setWaiting(0);						/// = 0;
					(*it_avail)->setExternalBuffer (buf);			///extern_buffer = buf;
					in_len = (*it_avail)->getExternalBufferLength (); ///extern_length;
					(*it_avail)->setExternalBufferLength (len);		///extern_length = len;

					(*it_avail)->setNeedsReply (1);

					(*it_avail)->postToWakeup ();	/// wakeup.Post();

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Waking up source of new data\n"));
				}
				
				(*it_avail)->postToMutex ();	/// mutex.Post();

				if (work)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Waiting for buffer write\n"));
					_new_data_written.Wait();

					save_pid = (*it_avail)->getID();
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "@@@ got data %d/%d\n", in_len, len));
					result = in_len;
					finished = 1;
					break;
				}
			}
		}
	}

	if (reply_pid != NULL)
	{
		*reply_pid = save_pid;
	}

	return result;
}

int _SocketThreadListMulti::pollingRead(char *buf, int len, ACE_HANDLE *reply_pid)
{
	ACE_ASSERT (_initialized != 0);

	if (reply_pid != NULL)
	{
		*reply_pid = ACE_INVALID_HANDLE;
	}

	list<_SocketThreadMulti *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->isWaiting())
			{
				return read (buf, len, reply_pid);
			}
		}
	}

	return YARP_OK;
}

int _SocketThreadListMulti::beginReply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING REPLY of %d bytes\n", len));

	list<_SocketThreadMulti *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				if ((*it_avail)->getServiceType () != YARP_MCAST)
				{
					(*it_avail)->waitOnMutex ();					///mutex.Wait();

					(*it_avail)->setExternalReplyBuffer (buf);		///extern_reply_buffer = buf;
					(*it_avail)->setExternalReplyLength (len);		///extern_reply_length = len;
					(*it_avail)->setReadMore (0);					///read_more = 0;
					(*it_avail)->setNeedsReply (1);					///needs_reply = 1;
					(*it_avail)->setReplyPreamble (1);				///reply_preamble = 1;

					(*it_avail)->postToWakeup ();					///wakeup.Post();
					(*it_avail)->postToMutex ();					///mutex.Post();
					(*it_avail)->waitOnReplyMade ();				///reply_made.Wait();
				}
				else
				{
					memset (buf, 0, len);
				}
			}
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& FINISHED BEGINNING REPLY of %d bytes\n", len));

	return YARP_OK;
}

int _SocketThreadListMulti::reply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING FINAL REPLY of %d bytes\n", len));

	list<_SocketThreadMulti *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				if ((*it_avail)->getServiceType() != YARP_MCAST)
				{
					(*it_avail)->waitOnMutex ();				///mutex.Wait();
					(*it_avail)->setExternalReplyBuffer (buf);	///extern_reply_buffer = buf;
					(*it_avail)->setExternalReplyLength (len);	///extern_reply_length = len;
					(*it_avail)->setReadMore (0);				///read_more = 0;
					(*it_avail)->setNeedsReply (1);				///needs_reply = 1;
					(*it_avail)->setReplyPreamble (0);			///reply_preamble = 0;
					(*it_avail)->postToWakeup ();				///wakeup.Post();
					(*it_avail)->postToMutex ();				///mutex.Post();
					(*it_avail)->waitOnReplyMade ();			///reply_made.Wait();
				}
				else
				{
					memset (buf, 0, len);
					(*it_avail)->postToWakeup ();				///wakeup.Post();
				}
			}
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& FINISHED BEGINNING FINAL REPLY of %d bytes\n", len));

	return YARP_OK;
}

// this demands exact number of bytes
int _SocketThreadListMulti::receiveMore(ACE_HANDLE reply_pid, char *buf, int len)
{
	int result = YARP_FAIL;

	ACE_ASSERT (_initialized != 0);

	list<_SocketThreadMulti *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				(*it_avail)->waitOnMutex ();
				(*it_avail)->setExternalReplyBuffer (buf);
				(*it_avail)->setExternalReplyLength (len);
				(*it_avail)->setReadMore (1);
				(*it_avail)->postToWakeup ();
				(*it_avail)->postToMutex ();
				(*it_avail)->waitOnReplyMade ();
				result = (*it_avail)->getExternalReplyLength ();
			}
		}
	}

	return result;
}



///
///
///
class ISDataMulti
{
public:
	_SocketThreadListMulti _list;
};

static ISDataMulti& ISDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((ISDataMulti*)x);
}

///
/// Input socket handling threads.
///
YARPInputSocketMulti::YARPInputSocketMulti (void) : YARPNetworkInputObject ()
{ 
	system_resources = NULL; 
	system_resources = new ISDataMulti;
	ACE_ASSERT (system_resources!=NULL);
	
	_socktype = YARP_I_SOCKET;
}

YARPInputSocketMulti::~YARPInputSocketMulti (void)
{
	CloseAll ();

	if (system_resources!=NULL)
	{
		delete ((ISDataMulti*)system_resources);
		system_resources = NULL;
	}
}

int YARPInputSocketMulti::Prepare (const YARPUniqueNameID& name, int *ports, int number_o_ports)
{
	ISDataMulti& d = ISDATA(system_resources);
	ACE_ASSERT (ports != NULL && number_o_ports >= 2);
	ACE_ASSERT (((YARPUniqueNameSock&)name).getAddressRef().get_port_number() == ports[0]);

	d._list.connect ((const YARPUniqueNameSock&)name);
	/// set the ports -1 that is used as pricipal receiving port.
	d._list.setPool (ports+1, number_o_ports-1);

	/// LATER: requires error handling here.
	return YARP_OK;
}

////
////
////
int YARPInputSocketMulti::CloseAll (void)
{
	return ISDATA(system_resources)._list.closeAll();
}


int YARPInputSocketMulti::Close (ACE_HANDLE reply_id)
{
	return ISDATA(system_resources)._list.close (reply_id);
}


int YARPInputSocketMulti::PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.pollingRead (buffer, buffer_length, reply_id);
}


int YARPInputSocketMulti::ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.read (buffer, buffer_length, reply_id);
}


int YARPInputSocketMulti::ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length)
{
	return ISDATA(system_resources)._list.receiveMore (reply_id, buffer, buffer_length);
}


int YARPInputSocketMulti::ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.beginReply (reply_id, reply_buffer, reply_buffer_length);
}


int YARPInputSocketMulti::ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.reply (reply_id, reply_buffer, reply_buffer_length);
}

ACE_HANDLE YARPInputSocketMulti::GetIdentifier(void) const
{
	return ISDATA(system_resources)._list.getID ();
}

int YARPInputSocketMulti::GetAssignedPort(void) const
{
	return ISDATA(system_resources)._list.getAssignedPort ();
}


///
/// Output SHMEM socket + stream incapsulation.
///
///
class OSDataMulti
{
public:
	ACE_INET_Addr _mem_addr;
	ACE_MEM_Connector _connector_socket;
	ACE_MEM_Stream _stream;
	
	ACE_SOCK_Dgram _udp_socket;
	ACE_INET_Addr _remote_addr;
	ACE_INET_Addr _local_addr;
};

static OSDataMulti& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSDataMulti*)x);
}

YARPOutputSocketMulti::YARPOutputSocketMulti (void) : YARPNetworkOutputObject ()
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSDataMulti;
	ACE_ASSERT(system_resources!=NULL);

	_socktype = YARP_O_SOCKET;
}

YARPOutputSocketMulti::~YARPOutputSocketMulti (void)
{
	OSDataMulti& d = OSDATA(system_resources);

	Close (YARPUniqueNameSock(YARP_NO_SERVICE_AVAILABLE, d._remote_addr));

	if (system_resources != NULL)
	{
		delete ((OSDataMulti*)system_resources);
		system_resources = NULL;
	}
}


/// closes down a specific connetion (tell the remote thread the connection is going down.).
int YARPOutputSocketMulti::Close (const YARPUniqueNameID& name)
{
	OSDataMulti& d = OSDATA(system_resources);

	ACE_DEBUG ((LM_DEBUG, "Pretending to close all connections to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	if (name.getServiceType() != YARP_SHMEM)
	{
		/// send the header.
		MyMessageHeader hdr;
		hdr.SetGood ();
		hdr.SetLength (_MAGIC_NUMBER + 1);

		d._udp_socket.open (d._local_addr, ACE_PROTOCOL_FAMILY_INET, 0, 1);
		d._udp_socket.send (&hdr, sizeof(hdr), d._remote_addr);

		/// wait response.
		hdr.SetBad ();
		ACE_INET_Addr incoming;
		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

		int r = d._udp_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
		if (r < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		}

		d._udp_socket.close ();
	}
	else
	{
		d._stream.close ();
	}

	identifier = ACE_INVALID_HANDLE;

	return YARP_OK;
}


/// this is called only once to create the group.
int YARPOutputSocketMulti::Prepare (const YARPUniqueNameID& name)
{
	OSDataMulti& d = OSDATA(system_resources);
	
	char myhostname[YARP_STRING_LEN];
	getHostname (myhostname, YARP_STRING_LEN);
	d._local_addr.set ((u_short)0, myhostname);
	d._remote_addr = ((YARPUniqueNameSock&)name).getAddressRef();

	identifier = ACE_INVALID_HANDLE;

	return YARP_OK;
}

///
/// this can be called many many times to ask receivers to join to mcast group.
/// name is the remote we're asking to join.
int YARPOutputSocketMulti::Connect (const YARPUniqueNameID& name)
{
	OSDataMulti& d = OSDATA(system_resources);

	/// send the header.
	int port_number = 0;
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (_MAGIC_NUMBER + 2);

	/// verifies it's a new connection.
	ACE_INET_Addr nm = ((YARPUniqueNameSock&)name).getAddressRef();

	d._udp_socket.open (d._local_addr, ACE_PROTOCOL_FAMILY_INET, 0, 1);
	d._udp_socket.send (&hdr, sizeof(hdr), nm);
	
	/// wait response.
	hdr.SetBad ();
	ACE_INET_Addr incoming;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	int r = d._udp_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
	if (r < 0)
	{
		d._udp_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	port_number = hdr.GetLength();
	if (port_number == -1)
	{
		/// there might be a real -1 port number -> 65535.
		d._udp_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "got garbage back from remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	d._udp_socket.close ();

	/// the connect changes the remote port number to the actual assigned channel.
	/// should close on this address.
	d._remote_addr.set_port_number (port_number);
	d._mem_addr.set (port_number, d._local_addr.get_host_addr());

	/// at this point the remote should be listening on port_number
	r = d._connector_socket.connect (d._stream, d._mem_addr);
	if (r == -1)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot connect shmem socket %s:%d\n", d._mem_addr.get_host_addr(), d._mem_addr.get_port_number()));
		identifier = ACE_INVALID_HANDLE;
		return YARP_FAIL;
	}

	identifier = d._stream.get_handle ();

	return YARP_OK;
}


int YARPOutputSocketMulti::SendBegin(char *buffer, int buffer_length)
{
	OSDataMulti& d = OSDATA(system_resources);

	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (buffer_length);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "sending to: %s:%d\n", d._mem_addr.get_host_addr(), d._mem_addr.get_port_number()));

	int sent = -1;
	sent = d._stream.send_n ((char *)&hdr, sizeof(hdr), 0);
	if (sent != sizeof(hdr))
		return YARP_FAIL;

	sent = d._stream.send_n (buffer, buffer_length, 0);
	if (sent != buffer_length)
		return YARP_FAIL;

	return YARP_OK;
}


int YARPOutputSocketMulti::SendContinue(char *buffer, int buffer_length)
{
	OSDataMulti& d = OSDATA(system_resources);

	int sent = d._stream.send_n (buffer, buffer_length, 0);
	if (sent != buffer_length)
		return YARP_FAIL;

	return YARP_OK;
}

/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocketMulti::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
	OSDataMulti& d = OSDATA(system_resources);
	MyMessageHeader hdr2;
	hdr2.SetBad ();

	int result = -1;
	int r = d._stream.recv_n ((void *)(&hdr2), sizeof(hdr2), 0);
	if (r == sizeof(hdr2))
	{
		int len2 = hdr2.GetLength();
		if (len2 > 0)
		{
			if (len2 < reply_buffer_length)
			{
				reply_buffer_length = len2;
			}

			result = d._stream.recv_n ((void *)reply_buffer, reply_buffer_length, 0);
		}
		else
		{
			if (len2 == 0) { result = 0; }
		}
	}
	return result;
}

int YARPOutputSocketMulti::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	return SendReceivingReply (reply_buffer, reply_buffer_length);
}


ACE_HANDLE YARPOutputSocketMulti::GetIdentifier(void) const
{
	return identifier;
}


#undef THIS_DBG
