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
/// $Id: YARPSocketMcast.cpp,v 1.11 2003-06-16 16:48:24 babybot Exp $
///
///

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocketMcast and YARPOutputSocketMcast
///
///	As of Apr 2003 this code is NOT tested under QNX.
///
///
///

///
///
///
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

#	include "strng.h"
#	include <string>
#	include <unix.h>  // just for gethostname

#endif

#include <list>

#include "YARPSocket.h"
#include "YARPSocketMcast.h"
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

///
/// structure:  
///      socket thread handles an input connection.
///		 _SocketThreadListMcast is a simple stl container and handles the incoming connection.
///		 YARPInputSocketMcast contains the list and provides the external interface.
///
/// because dgram are connectionless, we open a socket on a port to wait for connection and
///	then return the newly allocated port to the caller. The caller then instantiate the
///	usual (fake though) connection to the new port number (e.g. it starts sending data).
///	this maintains roughly a TCP-like structure and separates the incoming packets from 
///	multiple senders. there's a bit of handshaking at the beginning when the "connection" is
///	enstablished. additional calls to the name server might be required.
///
///

class YARPInputSocketMcast;
class _SocketThreadListMcast;

///
/// CLASS DEFINITIONS.
///
///
///
///
class _SocketThreadMcast : public YARPThread
{
protected:
	int _available;
	int _port;								/// udp port of the incoming udp "connection".
	ACE_SOCK_Dgram _local_socket;			/// a generic dgram socket.
	ACE_SOCK_Dgram_Mcast _mcast_socket;		/// the mcast socket.

	YARPUniqueNameID _local_addr;			/// both MCAST and UDP local addr.
	YARPUniqueNameID _mcast_addr;			/// mcast group.
	YARPUniqueNameID _remote_endpoint;		/// udp remote peer.

	_SocketThreadListMcast *_owner;

	char *_extern_buffer;
	int _extern_length;
	char *_extern_reply_buffer;
	int _extern_reply_length;

	int _waiting;
	int _needs_reply;
	int _read_more;
	int _reply_preamble;
	YARPSemaphore _wakeup, _mutex, _reply_made;

	int _begin (const YARPUniqueNameID *remid, const YARPUniqueNameID *group, int port);

public:
	/// ctors.
	_SocketThreadMcast (const YARPUniqueNameID *remid, const YARPUniqueNameID *group, int port) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (remid, group, port);
    }

	_SocketThreadMcast (void) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (NULL, NULL, 0);
    }

	~_SocketThreadMcast (void) {}

	/// needs a method to recycle the thread since
	/// thread creation might be a relatively costly process.

	/// required by stl interface.
	int operator == (const _SocketThreadMcast& other) { return 0; }
	int operator != (const _SocketThreadMcast& other) { return 0; }
	int operator <  (const _SocketThreadMcast& other) { return 0; }

	void setOwner(const _SocketThreadListMcast& n_owner);

	/// call it reconnect (recycle the thread).
	/// the thread shouldn't be running.
	int reuse(const YARPUniqueNameID& remid, const YARPUniqueNameID& group, int port);
	YARPUniqueNameID& getRemoteID(void) { return _remote_endpoint; }
	bool isMcast(void) const { return (_mcast_addr.getServiceType() == YARP_MCAST) ? true : false; }
	ACE_HANDLE getID () const {	return  _local_socket.get_handle (); }

	virtual void End (void);

	/// thread Body.
	///	error check is not consistent.
	virtual void Body (void);
	void BodyUdp (void);
	void BodyMcast (void);

	/// returns the last used port number for this thread.
	int getOldPortNumber (void) { return _port; } 

	/// 
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
class _SocketThreadListMcast : public YARPThread
{
private:
	ACE_INET_Addr _local_addr;
	ACE_SOCK_Dgram _acceptor_socket;

	list<_SocketThreadMcast *> _list;
	YARPSemaphore _new_data, _new_data_written;

	int _initialized;
	
	int *_ports;
	int _number_o_ports;
	int _last_assigned;		// index into array.

public:
	/// ctors
	_SocketThreadListMcast () : _local_addr (1111), _new_data(0), _new_data_written(0)
	{
		_ports = NULL;
		_number_o_ports = 0;
		_last_assigned = -1;
		_initialized = 0;
	}

	/// among other things, should correctly terminate threads.
	virtual ~_SocketThreadListMcast ();

	///
	/// creates the acceptor socket and listens to port.
	/// simply prepares the socket.
	ACE_HANDLE connect(const YARPUniqueNameID& id);

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
		int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "acceptor thread at priority %d -> %d\n", GetPriority(), prio));
		if (SetPriority(prio) == YARP_FAIL)
		{
			ACE_DEBUG ((LM_DEBUG, "can't raise priority of acceptor thread, potential source of troubles\n"));
		}

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
int _SocketThreadMcast::_begin (const YARPUniqueNameID *remid, const YARPUniqueNameID *group, int port = 0)
{
	_owner = NULL;
	_extern_buffer = NULL;
	_extern_length = 0;
	_extern_reply_buffer = NULL;
	_extern_reply_length = 0;
	_waiting = 0;
	_needs_reply = 0;

	/// stores the address of the remote endpoint, the IP and port are used to reply to it
	/// and also check whether new data is coming from the same source.
	if (remid != NULL)
		_remote_endpoint = *remid;
	else
		_remote_endpoint.invalidate();

	if (group != NULL)
		_mcast_addr = *group;
	else
		_mcast_addr.invalidate();

	_port = port;
	if (port != 0)
	{
		/// listen to this new port.
		char buf[256];
		YARPNetworkObject::getHostname (buf, 256);
		_local_addr.getAddressRef().set (port, buf);
		_local_socket.open (_local_addr.getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled
		
		YARPNetworkObject::setSocketBufSize (_local_socket, MAX_PACKET);

		if (group->getServiceType() == YARP_MCAST)
		{
			_mcast_socket.open (_mcast_addr.getAddressRef(), 0, 1);	// reuse addr enabled
			YARPNetworkObject::setSocketBufSize (_mcast_socket, MAX_PACKET);
			_mcast_socket.join (_mcast_addr.getAddressRef(), 1, 0);
		}

		_local_addr.getNameID() = YARPNameID (YARP_UDP, _local_socket.get_handle());

		if (_local_socket.get_handle() == ACE_INVALID_HANDLE || _mcast_socket.get_handle() == ACE_INVALID_HANDLE)
		{
			return YARP_FAIL;
		}
		_available = 0;
	}
	else
	{
		_available = 1;
	}

	_read_more = 0;
	_reply_preamble = 0;

	return YARP_OK;
}

void _SocketThreadMcast::setOwner(const _SocketThreadListMcast& n_owner)
{
	_owner = (_SocketThreadListMcast *)(&n_owner);
}

/// call it reconnect (recycle the thread).
/// the thread shouldn't be running.
int _SocketThreadMcast::reuse(const YARPUniqueNameID& remid, const YARPUniqueNameID& group, int port)
{
	/// remid contains the mcast addr, or the remote endpoint in udp.
	/// port is the udp channel, for mcast it is used to shud down the connection.
	_remote_endpoint = remid;
	_mcast_addr = group;

	if (port != 0)
	{
		_local_socket.close ();
		_mcast_socket.close ();

		_port = port;

		/// listen to this new port.
		char buf[256];
		YARPNetworkObject::getHostname (buf, 256);
		_local_addr.getAddressRef().set (port, buf);
		_local_socket.open (_local_addr.getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled
		YARPNetworkObject::setSocketBufSize (_local_socket, MAX_PACKET);

		if (_mcast_addr.getServiceType() == YARP_MCAST)
		{
			_mcast_socket.open (_mcast_addr.getAddressRef(), 0, 1);	// reuse addr enabled
			YARPNetworkObject::setSocketBufSize (_mcast_socket, MAX_PACKET);
			_mcast_socket.join (_mcast_addr.getAddressRef(), 1, 0);
		}

		_local_addr.getNameID() = YARPNameID (YARP_UDP, _local_socket.get_handle());

		if (_local_socket.get_handle() == ACE_INVALID_HANDLE || _mcast_socket.get_handle() == ACE_INVALID_HANDLE)
		{
			return YARP_FAIL;
		}
		_available = 0;
	}
	else
	{
		_available = 1;
	}

	return YARP_OK;
}

void _SocketThreadMcast::End (void)
{
	YARPThread::End ();
	_mutex.Wait ();
	
	if (_local_socket.get_handle() != ACE_INVALID_HANDLE)
		_local_socket.close();

	if (_mcast_socket.get_handle() != ACE_INVALID_HANDLE)
		_mcast_socket.close();

	_mutex.Post ();
}



/// thread Body.
///	error check is not consistent.
void _SocketThreadMcast::Body (void)
{
#ifdef __QNX6__
	signal (SIGPIPE, SIG_IGN);
#endif

	int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "reader thread at priority %d -> %d\n", GetPriority(), prio));
	if (SetPriority(prio) == YARP_FAIL)
	{
		ACE_DEBUG ((LM_DEBUG, "can't raise priority of reader thread, potential source of troubles\n"));
	}

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;

	if (_mcast_addr.getServiceType() == YARP_NO_SERVICE_AVAILABLE)
	{
		BodyUdp ();			/// udp only connection.
	}
	else
	if (_mcast_addr.getServiceType() == YARP_MCAST)
	{
		BodyMcast ();		/// mcast + udp channel for service.
	}
	else
	{
		ACE_DEBUG ((LM_DEBUG, "troubles starting a reader thread\n"));
	}

	_mutex.Wait ();
	/// the socket is closed already.
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
    

void _SocketThreadMcast::BodyUdp (void)
{
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_INET_Addr incoming;
	int finished = 0;

	///
	while (!finished)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread of remote port %s:%d waiting\n", _remote_endpoint.getAddressRef().get_host_name(), _remote_endpoint.getAddressRef().get_port_number()));
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread waiting on port %d waiting\n", _local_addr.getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		hdr.SetBad();
		int r = YARP_FAIL;

		r = _local_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_name(), incoming.get_port_number()));

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
			continue;
		}

		/// this is supposed to read the header, r must be > 0
		if (r < 0 || 
			incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing (recv returned %d)\n", r));
			if (incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "returning because incoming diffs from remote addr\n"));
			}
			_local_socket.close ();
			finished = 1;
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMcast, len = %d\n", hdr.GetLength()));

		int len = hdr.GetLength();
		if (len < 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));
			_local_socket.close ();
			finished = 1;
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

				r = _local_socket.recv (_extern_buffer, len , incoming, 0, &timeout);
				if (incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? incoming address diffs from what I expected\n"));
					_local_socket.close ();
					finished = 1;
				}
				if (r < 0)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? recv failed, time out?\n"));
					_local_socket.close ();
					finished = 1;
				}
			
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received a buffer _SocketThreadMcast\n"));

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

					int rr = 0;
					if (_extern_reply_length == 0)
					{
						/// then do a select.
						ACE_Handle_Set set;
						set.reset ();
						set.set_bit (_local_socket.get_handle());
						rr = ACE_OS::select (int(_local_socket.get_handle())+1, set, 0, 0, &timeout);
						/// wait here until next valid chunck of data.
					}
					else
					{
						///
						int remaining = _extern_reply_length;
						char *tmp = _extern_reply_buffer;
						int retry = 0;
						do 
						{
							YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to recv %d\n", remaining));

							rr = _local_socket.recv (tmp, remaining, incoming, 0, &timeout);
							YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? last read got %d bytes\n", rr));
							if (rr < 0)
							{
								retry ++;
								if (retry > 5)
								{
									ACE_DEBUG ((LM_DEBUG, "retried 5 times, exit now\n"));
									break;
								}
								rr = 0;
							}
							else
							{
								remaining -= rr;
								tmp += rr;
								int ack = 0x01020304;
								int sent = _local_socket.send (&ack, sizeof(int), incoming);

								YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? acknowledged\n"));
							}
						}
						while (rr >= 0 && remaining > 0);

						///rr = _local_socket.recv (_extern_reply_buffer, _extern_reply_length, incoming, 0, &timeout); 
					}

					if (incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing %d\n", rr));
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? incoming address diffs from what I expected\n"));
						_local_socket.close ();
						finished = 1;
					}
					else
					if (_extern_reply_length > 0 && rr < 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing %d\n", rr));
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? recv failed\n"));
						_local_socket.close ();
						finished = 1;
					}
					else
					if (_extern_reply_length == 0 && rr <= 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing %d\n", rr));
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? select failed\n"));
						_local_socket.close ();
						finished = 1;
					}

					_read_more = 0;
					_reply_made.Post();
					_wakeup.Wait();
					_needs_reply = 0;
				}

				int was_preamble = 0;

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
					char bufack[] = "acknowledged";
					char *buf3 = bufack;
					int reply_len = 0;
					if (rep)
					{
						buf3 = _extern_reply_buffer;
						reply_len = _extern_reply_length;
					}

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? sending reply _SocketThreadMcast\n"));

					hdr2.SetLength(reply_len);
					_local_socket.send (&hdr2, sizeof(hdr2), _remote_endpoint.getAddressRef());

					if (reply_len > 0)
					{
						_local_socket.send (buf3, reply_len, _remote_endpoint.getAddressRef());
					}

					int curr_preamble = _reply_preamble;
					if (rep)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? POSTING reply made %d\n", curr_preamble));
						_reply_made.Post();
					}

					if (r >= 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener got %d bytes\n", r));
					}

					if (r < 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing because r = %d\n", r));
						_local_socket.close ();
						finished = 1;
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
	}	/// while !finished
}


///
/// mcast receiver.
void _SocketThreadMcast::BodyMcast (void)
{
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_INET_Addr incoming;
	int finished = 0;
	char *local_buffer = new char[MAX_PACKET];
	ACE_ASSERT (local_buffer != 0);
	memset (local_buffer, 0, MAX_PACKET);
	int local_buffer_counter = 0;

	///
	while (!finished)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread of group %s:%d waiting\n", _mcast_addr.getAddressRef().get_host_addr(), _mcast_addr.getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		hdr.SetBad();
		int r = YARP_FAIL;

		ACE_Handle_Set set;
		set.reset ();
		set.set_bit (_local_socket.get_handle());
		set.set_bit (_mcast_socket.get_handle());
		int max = ((int)_local_socket.get_handle() > (int)_mcast_socket.get_handle()) 
			? (int)_local_socket.get_handle() : (int)_mcast_socket.get_handle();
		int rr = ACE_OS::select (max + 1, set, 0, 0, NULL);	///&timeout);	/// shouldn't timeout here.
		if (rr <= 0)
		{
			ACE_DEBUG ((LM_DEBUG, "main select failed\n"));
			ACE_DEBUG ((LM_DEBUG, "proper bailout required here\n"));

			if (local_buffer != NULL) delete[] local_buffer;
			return;
		}
		
		///
		if (rr >= 1 && set.is_set (_local_socket.get_handle()))
		{
			r = _local_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_name(), incoming.get_port_number()));

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
				_mcast_socket.close ();
				finished = 1;
				continue;
			}

			if (rr == 1)
				continue;
		}
		

		if (set.is_set (_mcast_socket.get_handle()))
		{
			iovec iov[1];
			iov[0].iov_base = local_buffer;
			iov[0].iov_len = MAX_PACKET;

			r = _mcast_socket.recv (iov, 1, incoming, 0);
			if (r < 0)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "lotta big troubles\n"));
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));

				/// close down sockets.
				_local_socket.close ();
				_mcast_socket.close ();
				finished = 1;
			}

			MyMessageHeader& hdr = *((MyMessageHeader *)local_buffer);
			local_buffer_counter = sizeof(hdr);

			/// maybe I need to have a big recv for the total number of bytes or more?
			///r = _mcast_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_name(), incoming.get_port_number()));

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMcast, len = %d\n", hdr.GetLength()));

			int len = hdr.GetLength();
			if (len < 0)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));

				/// close down sockets.
				_local_socket.close ();
				_mcast_socket.close ();
				finished = 1;
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

					///r = _mcast_socket.recv (_extern_buffer, len , incoming, 0, &timeout);
					/// shouldn't much different from the real recv.
					memcpy (_extern_buffer, local_buffer+local_buffer_counter, len); 
					local_buffer_counter += len;
					r = len;

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received a buffer _SocketThreadMcast\n"));

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

						int rr = 0;
						/// shouldn't happen for mcast thread (maybe...).
						/// ACE_ASSERT (_extern_reply_length != 0);

						if (_extern_reply_length == 0)
						{
							/// then do a select.
							///ACE_Handle_Set set;
							///set.reset ();
							///set.set_bit (_mcast_socket.get_handle());
							///rr = ACE_OS::select (int(_mcast_socket.get_handle())+1, set, 0, 0, &timeout);
							/// wait here until next valid chunck of data.

							/// ACE_DEBUG ((LM_DEBUG, "This shouldn't happen\n"));
						}
						else
						{
							///
							int remaining = _extern_reply_length;
							char *tmp = _extern_reply_buffer;
							int retry = 0;

							memcpy (_extern_reply_buffer, local_buffer+local_buffer_counter, _extern_reply_length);
							local_buffer_counter += _extern_reply_length;
							rr = _extern_reply_length;
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

	} /// while !finished

	if (local_buffer != NULL) delete[] local_buffer;
	/// returning because the thread is closing down.
}



///
///
///

/// among other things, should correctly terminate threads.
_SocketThreadListMcast::~_SocketThreadListMcast (void)
{
	if (_ports != NULL) delete[] _ports;
	_ports = NULL;
	_number_o_ports = 0;
	_last_assigned = -1;

	for (list<_SocketThreadMcast *>::iterator it = _list.begin(); it != _list.end(); it++)
	{
		(*it)->End ();
		delete (*it);
	}

	_initialized = 0;
}


///
/// creates the acceptor socket and listens to port.
/// simply prepares the socket.
ACE_HANDLE _SocketThreadListMcast::connect (const YARPUniqueNameID& id)
{
	_local_addr = ((YARPUniqueNameID &)id).getAddressRef();
	_acceptor_socket.open (_local_addr, ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled

	if (_acceptor_socket.get_handle() == ACE_INVALID_HANDLE)
	{
		ACE_DEBUG ((LM_DEBUG, ">>>>> problems with opening receiving UDP at %s:%d\n", _local_addr.get_host_name(), _local_addr.get_port_number()));
		/// ACE_DEBUG ((LM_DEBUG, "thread 0x%x (%P:%t) %p\n", GetCurrentThreadId(), "can't open socket"));
		return ACE_INVALID_HANDLE;

		/// and the thread is not started.
	}
	
	/// closes down any still open thread (just in case?).
	if (_initialized) closeAll ();

	/// ACE_DEBUG ((LM_DEBUG, "thread 0x%x server socket open on %s port %d\n", GetCurrentThreadId(), _local_addr.get_host_name(), _local_addr.get_port_number()));

	_initialized = 1;

	Begin();

	return _acceptor_socket.get_handle ();
}


void _SocketThreadListMcast::addSocket (void)
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

	if (hdr.GetLength() == _MAGIC_NUMBER)
	{
		/// checks whether <incoming> already tried a connection
		///		and it is still connected.
		list<_SocketThreadMcast *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if (!(*it_avail)->isAvailable ())
			{
				if ((*it_avail)->getRemoteID().getAddressRef().get_host_addr() == incoming.get_host_addr() &&
					(*it_avail)->getRemoteID().getAddressRef().get_port_number() == incoming.get_port_number())
				{
					ACE_DEBUG ((LM_DEBUG, "thread already connected %s:%d\n", incoming.get_host_name(), incoming.get_port_number()));
					break;
				}
			}
		}

		/// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>> accepting a new socket from %s:%d\n", incoming.get_host_name(), incoming.get_port_number()));
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
				_list.push_back(new _SocketThreadMcast());
				it_avail = _list.end();
				it_avail--;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				(*it_avail)->reuse (YARPUniqueNameID(YARP_UDP, incoming), YARPUniqueNameID(), port_number);
				(*it_avail)->Begin();
			}
			else
			{
				(*it_avail)->End();
				(*it_avail)->reuse (YARPUniqueNameID(YARP_UDP, incoming), YARPUniqueNameID(), port_number);
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
		list<_SocketThreadMcast *>::iterator it_avail;
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
		YARP_DBG(THIS_DBG) ((LM_DEBUG, ">>> accepting a new socket from %s:%d\n", incoming.get_host_name(), incoming.get_port_number()));
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
				_list.push_back(new _SocketThreadMcast());
				it_avail = _list.end();
				it_avail--;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				(*it_avail)->reuse (YARPUniqueNameID(YARP_UDP, incoming), YARPUniqueNameID(YARP_MCAST, group), port_number);
				(*it_avail)->Begin();
			}
			else
			{
				(*it_avail)->End();
				(*it_avail)->reuse (YARPUniqueNameID(YARP_UDP, incoming), YARPUniqueNameID(YARP_MCAST, group), port_number);
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
int _SocketThreadListMcast::closeAll (void)
{
	ACE_ASSERT (_initialized != 0);

	list<_SocketThreadMcast *>::iterator it_avail;

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
int _SocketThreadListMcast::close (ACE_HANDLE reply_id)
{
	ACE_ASSERT (_initialized != 0);

	int result = -1;
	list<_SocketThreadMcast *>::iterator it_avail;

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

void _SocketThreadListMcast::declareDataAvailable (void)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "$$$ Declaring data available\n"));
	_new_data.Post ();	
}

void _SocketThreadListMcast::declareDataWritten (void)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "$$$ Declaring new data written\n"));
	_new_data_written.Post ();	
}


/// returns the ACE_HANDLE as id for further reply.
///	returns also the number of bytes read.
int _SocketThreadListMcast::read(char *buf, int len, ACE_HANDLE *reply_pid)
{
	ACE_ASSERT (_initialized != 0);

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

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Got new data\n"));

		list<_SocketThreadMcast *>::iterator it_avail;
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

int _SocketThreadListMcast::pollingRead(char *buf, int len, ACE_HANDLE *reply_pid)
{
	ACE_ASSERT (_initialized != 0);

	if (reply_pid != NULL)
	{
		*reply_pid = ACE_INVALID_HANDLE;
	}

	list<_SocketThreadMcast *>::iterator it_avail;
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

int _SocketThreadListMcast::beginReply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING REPLY of %d bytes\n", len));

	list<_SocketThreadMcast *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				if (!(*it_avail)->isMcast())
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

int _SocketThreadListMcast::reply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING FINAL REPLY of %d bytes\n", len));

	list<_SocketThreadMcast *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				if (!(*it_avail)->isMcast())
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
int _SocketThreadListMcast::receiveMore(ACE_HANDLE reply_pid, char *buf, int len)
{
	int result = YARP_FAIL;

	ACE_ASSERT (_initialized != 0);

	list<_SocketThreadMcast *>::iterator it_avail;
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
class ISDataMcast
{
public:
	_SocketThreadListMcast _list;
};

static ISDataMcast& ISDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((ISDataMcast*)x);
}

///
/// Input socket + stream + handling threads.
///
YARPInputSocketMcast::YARPInputSocketMcast (void)
{ 
	system_resources = NULL; 
	system_resources = new ISDataMcast;
	ACE_ASSERT (system_resources!=NULL);
	
	_socktype = YARP_I_SOCKET;
}

YARPInputSocketMcast::~YARPInputSocketMcast (void)
{
	CloseAll ();

	if (system_resources!=NULL)
	{
		delete ((ISDataMcast*)system_resources);
		system_resources = NULL;
	}
}

int YARPInputSocketMcast::Prepare (const YARPUniqueNameID& name, int *ports, int number_o_ports)
{
	ISDataMcast& d = ISDATA(system_resources);
	ACE_ASSERT (ports != NULL && number_o_ports >= 2);
	ACE_ASSERT (((YARPUniqueNameID&)name).getAddressRef().get_port_number() == ports[0]);

	d._list.connect (name);
	/// set the ports -1 that is used as pricipal receiving port.
	d._list.setPool (ports+1, number_o_ports-1);

	/// LATER: requires error handling here.
	return YARP_OK;
}

////
////
////
int YARPInputSocketMcast::CloseAll (void)
{
	return ISDATA(system_resources)._list.closeAll();
}


int YARPInputSocketMcast::Close (ACE_HANDLE reply_id)
{
	return ISDATA(system_resources)._list.close (reply_id);
}


int YARPInputSocketMcast::PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.pollingRead (buffer, buffer_length, reply_id);
}


int YARPInputSocketMcast::ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.read (buffer, buffer_length, reply_id);
}


int YARPInputSocketMcast::ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length)
{
	return ISDATA(system_resources)._list.receiveMore (reply_id, buffer, buffer_length);
}


int YARPInputSocketMcast::ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.beginReply (reply_id, reply_buffer, reply_buffer_length);
}


int YARPInputSocketMcast::ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.reply (reply_id, reply_buffer, reply_buffer_length);
}

ACE_HANDLE YARPInputSocketMcast::GetIdentifier(void) const
{
	return ISDATA(system_resources)._list.getID ();
}

int YARPInputSocketMcast::GetAssignedPort(void) const
{
	return ISDATA(system_resources)._list.getAssignedPort ();
}

///
/// Output socket + stream incapsulation.
///
///
class OSDataMcast
{
public:
	ACE_INET_Addr _mcast_addr;
	ACE_SOCK_Dgram_Mcast _connector_socket;
	
	ACE_SOCK_Dgram _udp_socket;

	enum { _max_num_clients = 256 };
	ACE_INET_Addr _clients[_max_num_clients];
	string _client_names[_max_num_clients];

	MyMessageHeader _hdr;

	enum { _iov_buffer_size = 256 };
	iovec _iov[_iov_buffer_size];
	int _num_elements;
	int _overall_msg_size;
};

static OSDataMcast& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSDataMcast*)x);
}

YARPOutputSocketMcast::YARPOutputSocketMcast (void)
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSDataMcast;
	ACE_ASSERT(system_resources!=NULL);

	_socktype = YARP_O_SOCKET;

	int i;
	for (i = 0; i < OSDATA(system_resources)._max_num_clients; i++)
	{
		OSDATA(system_resources)._clients[i].set ((u_short)0, INADDR_ANY);
	}

	OSDATA(system_resources)._num_elements = 0;
	OSDATA(system_resources)._overall_msg_size = 0;
	OSDATA(system_resources)._hdr.SetBad();
}

YARPOutputSocketMcast::~YARPOutputSocketMcast (void)
{
	OSDataMcast& d = OSDATA(system_resources);
	int i;
	for (i = 0; i < d._max_num_clients; i++)
		Close (YARPUniqueNameID(YARP_NO_SERVICE_AVAILABLE, d._clients[i]));

	ACE_DEBUG ((LM_DEBUG, "Pretending to close all connections to port %d on %s\n", 
		d._mcast_addr.get_port_number(), 
		d._mcast_addr.get_host_name()));

	d._connector_socket.leave (d._mcast_addr, 0);

	if (system_resources != NULL)
	{
		delete ((OSDataMcast*)system_resources);
		system_resources = NULL;
	}
}

int YARPOutputSocketMcast::CloseMcastAll (void)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// send the header.
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (_MAGIC_NUMBER + 1);

	/// calling gethostname is not required since I can use INET_ADDR_ANY
	char buf[256];
	YARPNetworkObject::getHostname (buf, 256);
	ACE_INET_Addr local ((u_short)0, buf);
	d._udp_socket.open (local, ACE_PROTOCOL_FAMILY_INET, 0, 1);

	int i;
	for (i = 0; i < d._max_num_clients; i++)
	{
		if (d._clients[i].get_port_number() != 0)
		{
			d._udp_socket.send (&hdr, sizeof(hdr), d._clients[i]);
			
			/// wait response.
			hdr.SetBad ();
			ACE_INET_Addr incoming;
			int r = d._udp_socket.recv (&hdr, sizeof(hdr), incoming);
			if (r < 0)
			{
				ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._clients[i].get_host_addr(), d._clients[i].get_port_number()));
			}
			d._clients[i].set ((u_short)0, INADDR_ANY);
		}
	}

	///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
	d._udp_socket.close ();

	return YARP_OK;
}

/// closes down a specific connetion (tell the remote thread the connection is going down.).
int YARPOutputSocketMcast::Close (const YARPUniqueNameID& name)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// send the header.
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (_MAGIC_NUMBER + 1);

	/// calling gethostname is not required since I can use INET_ADDR_ANY
	char buf[256];
	YARPNetworkObject::getHostname (buf, 256);
	ACE_INET_Addr local ((u_short)0, buf);
	d._udp_socket.open (local, ACE_PROTOCOL_FAMILY_INET, 0, 1);

	int i;
	int j = -1;
	ACE_INET_Addr& nm = ((YARPUniqueNameID &)name).getAddressRef();
	char *sname = (char *)((YARPUniqueNameID &)name).getP2Ptr();

	for (i = 0; i < d._max_num_clients; i++)
	{
		if (d._clients[i].get_host_addr() == nm.get_host_addr() &&
			d._client_names[i].compare(sname) == 0)
		{
			j = i;
			break;
		}
	}

	if (j == -1)
	{
		///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
		d._udp_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "the specific name is not connected %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	d._udp_socket.send (&hdr, sizeof(hdr), d._clients[j]);

	/// wait response.
	hdr.SetBad ();
	ACE_INET_Addr incoming;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

	int r = d._udp_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
	if (r < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._clients[j].get_host_addr(), d._clients[j].get_port_number()));

		d._clients[j].set ((u_short)0, INADDR_ANY);
		d._client_names[j].erase(d._client_names[j].begin(), d._client_names[j].end());
	
		///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
		d._udp_socket.close ();
		return YARP_FAIL;
	}

	d._clients[j].set ((u_short)0, INADDR_ANY);
	d._client_names[j].erase(d._client_names[j].begin(), d._client_names[j].end());

	///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
	d._udp_socket.close ();

	return YARP_OK;
}


/// this is called only once to create the group.
int YARPOutputSocketMcast::Prepare (const YARPUniqueNameID& name) ///, int local_port)
{
	OSDataMcast& d = OSDATA(system_resources);
	d._mcast_addr = ((YARPUniqueNameID&)name).getAddressRef();

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Starting mcast group on port %d and ip %s\n", 
		d._mcast_addr.get_port_number(), 
		d._mcast_addr.get_host_addr()));

	int r = d._connector_socket.open (d._mcast_addr, 0, 1);		/// reuse addr on, netif = 0.
	if (r == -1)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot open mcast socket %s:%d\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number()));
		return YARP_FAIL;
	}

	int ret = YARPNetworkObject::setSocketBufSize (d._connector_socket, MAX_PACKET);
	if (ret != YARP_OK)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot set buffer size to %d\n", MAX_PACKET));
		return YARP_FAIL;
	}


	/// no loopback?
#if 0
	r = d._connector_socket.join (d._mcast_addr, 1, 0);
	if (r == -1)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot joint mcast group %s:%d\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number()));
		return YARP_FAIL;
	}
#endif

	identifier = d._connector_socket.get_handle ();

	return YARP_OK;
}

///
/// this can be called many many times to ask receivers to join to mcast group.
/// name is the remote we're asking to join.
int YARPOutputSocketMcast::Connect (const YARPUniqueNameID& name)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// send the header.
	int port_number = 0;
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (_MAGIC_NUMBER + 1);

	/// verifies it's a new connection.
	ACE_INET_Addr nm = ((YARPUniqueNameID&)name).getAddressRef();

	char *sname = (char *)((YARPUniqueNameID&)name).getP2Ptr();
	int i, firstempty = -1;
	for (i = 0; i < d._max_num_clients; i++)
	{
		if (d._clients[i].get_host_addr() == nm.get_host_addr() &&
			d._client_names[i].compare(sname) == 0)
		{
			/// it's already there...
			ACE_DEBUG ((LM_DEBUG, "the specific client is already connected %s:%d\n", d._clients[i].get_host_addr(), d._clients[i].get_port_number()));

			/// tries to shut down the connection first.
			if (Close (name) == YARP_FAIL)
			{
				ACE_DEBUG ((LM_DEBUG, "can't close the mcast connection, it can happen if the server died unexpectedly\n"));

				/// erases the client entry anyway.
				d._clients[i].set ((u_short)0, INADDR_ANY);
				d._client_names[i].erase(d._client_names[i].begin(), d._client_names[i].end());
			}

			/// 250 ms delay.
			YARPTime::DelayInSeconds (.25);

			///d._udp_socket.close();
			///return YARP_FAIL;
		}

		if (d._clients[i].get_port_number() == 0 && firstempty < 0)
		{
			firstempty = i;	
		}
	}

	char buf[256];
	YARPNetworkObject::getHostname (buf, 256);
	ACE_INET_Addr local ((u_short)0, buf);
	d._udp_socket.open (local, ACE_PROTOCOL_FAMILY_INET, 0, 1);

	/// ask for a connection.
	d._udp_socket.send (&hdr, sizeof(hdr), nm);
	
	/// send mcast ip and port #.
	/// exactly 6 bytes.
	int ip = d._mcast_addr.get_ip_address();
	buf[0] = (ip & 0xff000000) >> 24;
	buf[1] = (ip & 0x00ff0000) >> 16;
	buf[2] = (ip & 0x0000ff00) >> 8;
	buf[3] = (ip & 0x000000ff);
	short portn = d._mcast_addr.get_port_number();
	buf[4] = (portn & 0xff00) >> 8;
	buf[5] = (portn & 0x00ff);
	d._udp_socket.send (buf, 6, nm);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "supposedly sent %s:%d\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number()));

	/// wait response.
	hdr.SetBad ();
	ACE_INET_Addr incoming;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	int r = d._udp_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
	if (r < 0)
	{
		///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
		d._udp_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	d._clients[firstempty] = nm;

	port_number = hdr.GetLength();
	if (port_number == -1)
	{
		/// there might be a real -1 port number -> 65535.
		///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
		d._udp_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "got garbage back from remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	/// the connect changes the remote port number to the actual assigned channel.
	d._clients[firstempty].set_port_number (port_number);

	/// stores also the full symbolic name as index.
	string& s = d._client_names[firstempty];
	s.erase(s.begin(), s.end());
	s = string(sname);

	///ACE_OS::shutdown (d._udp_socket.get_handle(), ACE_SHUTDOWN_BOTH);
	d._udp_socket.close ();

	return YARP_OK;
}


int YARPOutputSocketMcast::SendBegin(char *buffer, int buffer_length)
{
	OSDataMcast& d = OSDATA(system_resources);
	d._hdr.SetGood ();
    d._hdr.SetLength (buffer_length);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "sending to: %s:%d\n", d._mcast_addr.get_host_addr(), d._mcast_addr.get_port_number()));

	///d._num_elements = 0;

	d._iov[0].iov_base = (char *)&d._hdr;
	d._iov[0].iov_len = sizeof(d._hdr);
	d._iov[1].iov_base = buffer;
	d._iov[1].iov_len = buffer_length;
	d._num_elements = 2;
	d._overall_msg_size = buffer_length + sizeof(d._hdr);

	return YARP_OK;
}


int YARPOutputSocketMcast::SendContinue(char *buffer, int buffer_length)
{
	OSDataMcast& d = OSDATA(system_resources);

	d._iov[d._num_elements].iov_base = buffer;
	d._iov[d._num_elements].iov_len = buffer_length;
	d._num_elements ++;
	d._overall_msg_size += buffer_length;

	return YARP_OK;
}

/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocketMcast::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
	memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
}

int YARPOutputSocketMcast::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	OSDataMcast& d = OSDATA(system_resources);

	/// the send can fail because the total buf is too big for the IP stack buffer.
	/// assert that situation (use the MAX_PACKET_SIZE as limit for sending msgs).
	/// NOTE: mcast is efficient but should be only used for relatively small messages.
	///	LATER: should set the socket buffer to handle bigger messages, at least the
	///		size of a 128sq color image.

	if (d._overall_msg_size > MAX_PACKET)
	{
		ACE_DEBUG ((LM_DEBUG, "Implementation limit, pls, you should refrain from sending big MCAST packets\n"));
		ACE_DEBUG ((LM_DEBUG, "Actual size is %d, allowed %d\n", d._overall_msg_size, MAX_PACKET));
		ACE_ASSERT (1 == 0);
	}

///	int x = *((int *)d._iov[d._num_elements-1].iov_base);
///	ACE_DEBUG ((LM_DEBUG, "about to send %d elements, last is %d\n", d._num_elements, x));

	int sent = d._connector_socket.send (d._iov, d._num_elements, 0);
	if (sent < 0)
		return YARP_FAIL;

	memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;

	///return SendReceivingReply (reply_buffer, reply_buffer_length);
}


ACE_HANDLE YARPOutputSocketMcast::GetIdentifier(void) const
{
	return identifier;
}

#undef THIS_DBG
