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
/// $Id: YARPSocketDgram.cpp,v 1.33 2003-07-06 23:25:45 gmetta Exp $
///
///

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocketDgram and YARPOutputSocketDgram
///
///	As of Apr 2003 this code is NOT tested under QNX.
///
///
///

///
/// name server allocation policy for dgrams.
///	1) the input socket is created with a max number of acceptable connections.
///	2) the name server allocates a pool of ports for each IP asking a registration in UDP mode.
/// 3) the port simply sends back a port number for each connection attempt.
///	4) the port complains if a given remote is already connected.
///	5) this structure working in UDP mimics the corresponding TCP accept with less
///		exception handling though.
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
#	include <unix.h>  // just for gethostname

#endif

#include <list>

#include "YARPSocket.h"
#include "YARPSocketDgram.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPNameID.h"
#include "YARPScheduler.h"
#include "YARPTime.h"

///
#define THIS_DBG 80


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
///
///
const int _MAGIC_NUMBER = 7777;

///
/// structure:  
///      socket thread handles an input connection.
///		 _SocketThreadListDgram is a simple stl container and handles the incoming connection.
///		 YARPInputSocketDgram contains the list and provides the external interface.
///
/// because dgram are connectionless, we open a socket on a port to wait for connection and
///	then return the newly allocated port to the caller. The caller then instantiate the
///	usual (fake though) connection to the new port number (e.g. it starts sending data).
///	this maintains roughly a TCP-like structure and separates the incoming packets from 
///	multiple senders. there's a bit of handshaking at the beginning when the "connection" is
///	enstablished. additional calls to the name server might be required.
///
///

class YARPInputSocketDgram;
class _SocketThreadListDgram;

///
/// CLASS DEFINITIONS.
///
///
///
///
class _SocketThreadDgram : public YARPBareThread
{
protected:
	int _available;
	int _port;
	ACE_SOCK_Dgram _local_socket;
	YARPUniqueNameSock _local_addr;
	YARPUniqueNameSock _remote_endpoint;	

	_SocketThreadListDgram *_owner;

	char *_extern_buffer;
	int _extern_length;
	char *_extern_reply_buffer;
	int _extern_reply_length;

	int _waiting;
	int _needs_reply;
	int _read_more;
	int _reply_preamble;
	YARPSemaphore _wakeup, _mutex, _reply_made;

	char _local_buffer[MAX_PACKET];
	int _local_buffer_counter;

	int _begin (const YARPUniqueNameSock *remid, int port);

public:
	/// ctors.
	_SocketThreadDgram (const YARPUniqueNameSock *remid, int port) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (remid, port);
    }

	_SocketThreadDgram (void) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (NULL, 0);
    }

	~_SocketThreadDgram (void) {}

	/// needs a method to recycle the thread since
	/// thread creation might be a relatively costly process.

	/// required by stl interface.
	int operator == (const _SocketThreadDgram& other) { ACE_UNUSED_ARG(other); return 0; }
	int operator != (const _SocketThreadDgram& other) { ACE_UNUSED_ARG(other); return 0; }
	int operator <  (const _SocketThreadDgram& other) { ACE_UNUSED_ARG(other); return 0; }

	void setOwner(const _SocketThreadListDgram& n_owner);

	/// call it reconnect (recycle the thread).
	/// the thread shouldn't be running.
	int reuse(const YARPUniqueNameSock& remid, int port);
	
	YARPUniqueNameSock& getRemoteID(void) { return _remote_endpoint; }
	ACE_HANDLE getID () const {	return  _local_socket.get_handle (); }

	virtual void End (int donkill = 0);

	/// thread Body.
	///	error check is not consistent.
	virtual void Body (void);

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
class _SocketThreadListDgram : public YARPBareThread
{
private:
	ACE_INET_Addr _local_addr;
	ACE_SOCK_Dgram _acceptor_socket;

	list<_SocketThreadDgram *> _list;
	YARPSemaphore _new_data, _new_data_written;

	int _initialized;
	
	int *_ports;
	int _number_o_ports;
	int _last_assigned;		// index into array.

public:
	/// ctors
	_SocketThreadListDgram () : _local_addr (1111), _new_data(0), _new_data_written(0)
	{
		_ports = NULL;
		_number_o_ports = 0;
		_last_assigned = -1;
		_initialized = 0;
	}

	/// among other things, should correctly terminate threads.
	virtual ~_SocketThreadListDgram ();

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
/// WARNING: requires a mutex to handle the stream deletion.
///
///
int _SocketThreadDgram::_begin (const YARPUniqueNameSock *remid, int port = 0)
{
	_owner = NULL;
	_extern_buffer = NULL;
	_extern_length = 0;
	_extern_reply_buffer = NULL;
	_extern_reply_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_local_buffer_counter = 0;

	/// stores the address of the remote endpoint, the IP and port are used to reply to it
	/// and also check whether new data is coming from the same source.
	if (remid != NULL)
		_remote_endpoint = *remid;
	else
		_remote_endpoint.invalidate();

	_port = port;
	if (port != 0)
	{
		/// listen to this new port.
		char myhostname[YARP_STRING_LEN];
		YARPNetworkObject::getHostname (myhostname, YARP_STRING_LEN);
		_local_addr.getAddressRef().set (port, myhostname);
		_local_socket.open (_local_addr.getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled?

		YARPNetworkObject::setSocketBufSize (_local_socket, MAX_PACKET);

		_local_addr.setRawIdentifier (_local_socket.get_handle());
		_local_addr.setServiceType (YARP_UDP);

		if (_local_socket.get_handle() == ACE_INVALID_HANDLE)
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

void _SocketThreadDgram::setOwner(const _SocketThreadListDgram& n_owner)
{
	_owner = (_SocketThreadListDgram *)(&n_owner);
}

/// call it reconnect (recycle the thread).
/// the thread shouldn't be running.
int _SocketThreadDgram::reuse(const YARPUniqueNameSock& remid, int port)
{
	_remote_endpoint = remid;

	if (_port != 0)
	{
		_local_socket.close();
	}

	if (port != 0)
	{
		_port = port;

		/// listen to this new port.
		char myhostname [YARP_STRING_LEN];
		YARPNetworkObject::getHostname (myhostname, YARP_STRING_LEN);
		_local_addr.getAddressRef().set (port, myhostname);
		_local_socket.open (_local_addr.getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled?

		YARPNetworkObject::setSocketBufSize (_local_socket, MAX_PACKET);

		///_local_addr.getNameID() = YARPNameID (YARP_UDP, _local_socket.get_handle());
		_local_addr.setRawIdentifier (_local_socket.get_handle());
		_local_addr.setServiceType (YARP_UDP);

		if (_local_socket.get_handle() == ACE_INVALID_HANDLE)
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

void _SocketThreadDgram::End (int dontkill /* = 0 */)
{
	ACE_UNUSED_ARG (dontkill);

	YARPBareThread::End ();
	_mutex.Wait ();
	if (_local_socket.get_handle() != ACE_INVALID_HANDLE)
	{
		_local_socket.close();
	}
	_mutex.Post ();
}




/// thread Body.
///	error check is not consistent.
void _SocketThreadDgram::Body (void)
{
#ifdef __QNX6__
	signal (SIGPIPE, SIG_IGN);
#endif
	///ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_Time_Value long_timeout (YARP_STALE_TIMEOUT, 0);

#if 0
	int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "reader thread at priority %d -> %d\n", GetPriority(), prio));
	if (SetPriority(prio) == YARP_FAIL)
	{
		ACE_DEBUG ((LM_DEBUG, "can't raise priority of reader thread, potential source of troubles\n"));
	}
#endif

	int finished = 0;

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;

	////
	//// all variables used in the loop.
	ACE_INET_Addr incoming;

	int r = YARP_FAIL;	///, rr = YARP_FAIL;
	int was_preamble = 0;

	char bufack[] = "acknowledged\0";
	char *buf3 = bufack;
	int reply_len = 0;
	int len = 0;

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
				ACE_DEBUG ((LM_DEBUG, "select on UDP reader timed out, perhaps this is a stale connection, closing down\n"));
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

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadDgram, len = %d\n", hdr.GetLength()));

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

					if (_extern_reply_length == 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read 0 len buffer, not waiting\n"));
					}
					else
					{
						memcpy (_extern_reply_buffer, _local_buffer + _local_buffer_counter, _extern_reply_length);
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
					reply_len = 0;
					if (rep)
					{
						buf3 = _extern_reply_buffer;
						reply_len = _extern_reply_length;
					}

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? sending reply _SocketThreadDgram\n"));

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

	/// exit thread.
	/// returning because the thread is closing down.

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
    

///
///
///

/// among other things, should correctly terminate threads.
_SocketThreadListDgram::~_SocketThreadListDgram (void)
{
	if (_ports != NULL) delete[] _ports;
	_ports = NULL;
	_number_o_ports = 0;
	_last_assigned = -1;

	for (list<_SocketThreadDgram *>::iterator it = _list.begin(); it != _list.end(); it++)
	{
		(*it)->End ();
		delete (*it);
	}

	_initialized = 0;
}


///
/// creates the acceptor socket and listens to port.
/// simply prepares the socket.
ACE_HANDLE _SocketThreadListDgram::connect (const YARPUniqueNameSock& id)
{
	_local_addr = ((YARPUniqueNameSock &)id).getAddressRef();
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


void _SocketThreadListDgram::addSocket (void)
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
		list<_SocketThreadDgram *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			if (!(*it_avail)->isAvailable ())
			{
				if ((*it_avail)->getRemoteID().getAddressRef() == incoming)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "thread already connected %s:%d\n", incoming.get_host_addr(), incoming.get_port_number()));

					/// needs to recycle the thread, and stop it first.
					/// reconnecting!

					///(*it_avail)->End ();
					///(*it_avail)->setAvailable (1);
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
				_list.push_back(new _SocketThreadDgram());
				it_avail = _list.end();
				it_avail--;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				(*it_avail)->reuse (YARPUniqueNameSock(YARP_UDP, incoming), port_number);
				(*it_avail)->Begin();
			}
			else
			{
				(*it_avail)->End();
				(*it_avail)->reuse (YARPUniqueNameSock(YARP_UDP, incoming), port_number);
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
int _SocketThreadListDgram::closeAll (void)
{
	ACE_ASSERT (_initialized != 0);

	list<_SocketThreadDgram *>::iterator it_avail;

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
int _SocketThreadListDgram::close (ACE_HANDLE reply_id)
{
	ACE_ASSERT (_initialized != 0);

	int result = -1;
	list<_SocketThreadDgram *>::iterator it_avail;

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

void _SocketThreadListDgram::declareDataAvailable (void)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "$$$ Declaring data available\n"));
	_new_data.Post ();	
}

void _SocketThreadListDgram::declareDataWritten (void)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "$$$ Declaring new data written\n"));
	_new_data_written.Post ();	
}


/// returns the ACE_HANDLE as id for further reply.
///	returns also the number of bytes read.
int _SocketThreadListDgram::read(char *buf, int len, ACE_HANDLE *reply_pid)
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

		list<_SocketThreadDgram *>::iterator it_avail;
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

int _SocketThreadListDgram::pollingRead(char *buf, int len, ACE_HANDLE *reply_pid)
{
	ACE_ASSERT (_initialized != 0);

	if (reply_pid != NULL)
	{
		*reply_pid = ACE_INVALID_HANDLE;
	}

	list<_SocketThreadDgram *>::iterator it_avail;
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

int _SocketThreadListDgram::beginReply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING REPLY of %d bytes\n", len));

	list<_SocketThreadDgram *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
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
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& FINISHED BEGINNING REPLY of %d bytes\n", len));

	return YARP_OK;
}

int _SocketThreadListDgram::reply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING FINAL REPLY of %d bytes\n", len));

	list<_SocketThreadDgram *>::iterator it_avail;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
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
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& FINISHED BEGINNING FINAL REPLY of %d bytes\n", len));

	return YARP_OK;
}

// this demands exact number of bytes
int _SocketThreadListDgram::receiveMore(ACE_HANDLE reply_pid, char *buf, int len)
{
	int result = YARP_FAIL;

	ACE_ASSERT (_initialized != 0);

	list<_SocketThreadDgram *>::iterator it_avail;
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
class ISDataDgram
{
public:
	_SocketThreadListDgram _list;
};

static ISDataDgram& ISDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((ISDataDgram*)x);
}

///
/// Input socket + stream + handling threads.
///
YARPInputSocketDgram::YARPInputSocketDgram (void) : YARPNetworkInputObject ()
{ 
	system_resources = NULL; 
	system_resources = new ISDataDgram;
	ACE_ASSERT (system_resources!=NULL);
	
	_socktype = YARP_I_SOCKET;
}

YARPInputSocketDgram::~YARPInputSocketDgram (void)
{
	CloseAll ();

	if (system_resources!=NULL)
	{
		delete ((ISDataDgram*)system_resources);
		system_resources = NULL;
	}
}

int YARPInputSocketDgram::Prepare (const YARPUniqueNameID& name, int *ports, int number_o_ports)
{
	ISDataDgram& d = ISDATA(system_resources);
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
int YARPInputSocketDgram::CloseAll (void)
{
	return ISDATA(system_resources)._list.closeAll();

	//int result = ISDATA(system_resources).owner.Close();
	//return result;
}


int YARPInputSocketDgram::Close (ACE_HANDLE reply_id)
{
	///int result = ISDATA(system_resources).owner.Close(reply_id);
	///return result;
	return ISDATA(system_resources)._list.close (reply_id);
}


int YARPInputSocketDgram::PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.pollingRead (buffer, buffer_length, reply_id);
}


int YARPInputSocketDgram::ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.read (buffer, buffer_length, reply_id);
}


int YARPInputSocketDgram::ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length)
{
	return ISDATA(system_resources)._list.receiveMore (reply_id, buffer, buffer_length);
}


int YARPInputSocketDgram::ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.beginReply (reply_id, reply_buffer, reply_buffer_length);
}


int YARPInputSocketDgram::ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.reply (reply_id, reply_buffer, reply_buffer_length);
}

ACE_HANDLE YARPInputSocketDgram::GetIdentifier(void) const
{
	return ISDATA(system_resources)._list.getID ();
}

int YARPInputSocketDgram::GetAssignedPort(void) const
{
	return ISDATA(system_resources)._list.getAssignedPort ();
}

///
/// Output socket + stream incapsulation.
///
///
class OSDataDgram
{
public:
	ACE_INET_Addr _remote_acceptor_store;
	ACE_INET_Addr _remote_addr;
	ACE_INET_Addr _local_addr;
	ACE_SOCK_Dgram _connector_socket;

	MyMessageHeader _hdr;

	enum { _iov_buffer_size = 256 };
	iovec _iov[_iov_buffer_size];
	int _num_elements;
	int _overall_msg_size;

	char _buffer[MAX_PACKET];
};

static OSDataDgram& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSDataDgram*)x);
}

YARPOutputSocketDgram::YARPOutputSocketDgram (void) : YARPNetworkOutputObject ()
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSDataDgram;
	ACE_ASSERT(system_resources!=NULL);
	OSDataDgram& d = OSDATA(system_resources);

	_socktype = YARP_O_SOCKET;

	memset (d._iov, 0, sizeof(iovec) * d._iov_buffer_size);
	d._num_elements = 0;
	d._overall_msg_size = 0;

	d._hdr.SetBad();

	/// I hate this but there's no much choice unless the protocol is changed to allow
	/// for nice iovec style sync comm.
	memset (d._buffer, 0, MAX_PACKET);
}

YARPOutputSocketDgram::~YARPOutputSocketDgram (void)
{
	Close (YARPUniqueNameSock());

	if (system_resources != NULL)
	{
		delete ((OSDataDgram*)system_resources);
		system_resources = NULL;
	}
}


int YARPOutputSocketDgram::Close (const YARPUniqueNameID& name)
{
	ACE_UNUSED_ARG (name);

	OSDataDgram& d = OSDATA(system_resources);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Pretending to close a connection to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	/// send the header.
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (_MAGIC_NUMBER + 1);
	d._connector_socket.send (&hdr, sizeof(hdr), d._remote_addr);

	/// wait response.
	hdr.SetBad ();
	ACE_INET_Addr incoming;
	int r = d._connector_socket.recv (&hdr, sizeof(hdr), incoming);
	if (r < 0)
	{
		d._connector_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	hdr.GetLength();
	d._remote_addr.set ((u_short)0);
	d._remote_acceptor_store.set ((u_short)0);

	return d._connector_socket.close ();
}


int YARPOutputSocketDgram::Prepare (const YARPUniqueNameID& name)
{
	/// local_port might not be needed by the socket layer.
	char buf[YARP_STRING_LEN];
	YARPNetworkObject::getHostname (buf, YARP_STRING_LEN);
	OSDATA(system_resources)._local_addr.set ((u_short)0, buf);
	OSDATA(system_resources)._remote_addr = ((YARPUniqueNameSock&)name).getAddressRef();
	return YARP_OK;
}

///
/// pretend a connection.
int YARPOutputSocketDgram::Connect (const YARPUniqueNameID& name)
{
	ACE_UNUSED_ARG (name);

	OSDataDgram& d = OSDATA(system_resources);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Pretending a connection to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	int r = d._connector_socket.open (d._local_addr, ACE_PROTOCOL_FAMILY_INET, 0, 1);
	if (r == -1)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot open dgram socket %s:%d\n", d._local_addr.get_host_addr(), d._local_addr.get_port_number()));
		return YARP_FAIL;
	}

	/// send the header.
	int port_number = 0;
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (_MAGIC_NUMBER);
	d._connector_socket.send (&hdr, sizeof(hdr), d._remote_addr);

	/// wait response.
	hdr.SetBad ();
	ACE_INET_Addr incoming;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	r = d._connector_socket.recv (&hdr, sizeof(hdr), incoming, 0, &timeout);
	if (r < 0)
	{
		d._connector_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	/// stores the remote acceptor address for future use (e.g. closing the connection).
	d._remote_acceptor_store = d._remote_addr;

	port_number = hdr.GetLength();
	if (port_number == -1)
	{
		/// there might be a real -1 port number -> 65535.
		d._connector_socket.close ();
		ACE_DEBUG ((LM_DEBUG, "got garbage back from remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		return YARP_FAIL;
	}

	/// the connect changes the remote port number to the actual assigned channel.
	d._remote_addr.set_port_number (port_number);
	identifier = d._connector_socket.get_handle ();

	YARPNetworkObject::setSocketBufSize (d._connector_socket, MAX_PACKET);

	return YARP_OK;
}


int YARPOutputSocketDgram::SendBegin(char *buffer, int buffer_length)
{
	OSDataDgram& d = OSDATA(system_resources);
	d._hdr.SetGood ();
    d._hdr.SetLength (buffer_length);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "sending to: %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));

	d._iov[0].iov_base = (char *)&d._hdr;
	d._iov[0].iov_len = sizeof(d._hdr);
	d._iov[1].iov_base = buffer;
	d._iov[1].iov_len = buffer_length;
	d._num_elements = 2;
	d._overall_msg_size = buffer_length + sizeof(d._hdr);

	return YARP_OK;
}


int YARPOutputSocketDgram::SendContinue(char *buffer, int buffer_length)
{
	OSDataDgram& d = OSDATA(system_resources);

	d._iov[d._num_elements].iov_base = buffer;
	d._iov[d._num_elements].iov_len = buffer_length;
	d._num_elements ++;
	d._overall_msg_size += buffer_length;
		
	return YARP_OK;
}


/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocketDgram::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
	OSDataDgram& d = OSDATA(system_resources);
	int result = YARP_FAIL;

	if (d._overall_msg_size != 0)
	{
		/// it needs to send the message first.
		if (d._overall_msg_size > MAX_PACKET)
		{
			ACE_DEBUG ((LM_DEBUG, "Implementation limit, pls, you should refrain from sending big MCAST packets\n"));
			ACE_DEBUG ((LM_DEBUG, "Actual size is %d, allowed %d\n", d._overall_msg_size, MAX_PACKET));
			ACE_ASSERT (1 == 0);
		}

		int sent = d._connector_socket.send (d._iov, d._num_elements, d._remote_addr, 0);
		if (sent < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "Actual send to %s:%d failed\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
			return YARP_FAIL;
		}

		d._overall_msg_size = 0;
		d._num_elements = 0;

		d._iov[0].iov_base = d._buffer;
		d._iov[0].iov_len = MAX_PACKET;
		ACE_INET_Addr incoming;
		int r = d._connector_socket.recv (d._iov, 1, incoming, 0);
		if (r > 0)
		{
			MyMessageHeader *hdr2 = (MyMessageHeader *)(d._iov[0].iov_base);
			d._iov[0].iov_base = ((char *)d._iov[0].iov_base) + sizeof(MyMessageHeader);

			int len2 = hdr2->GetLength();
			if (len2 > 0)
			{
				if (len2 < reply_buffer_length)
				{
					reply_buffer_length = len2;
				}

				memcpy (reply_buffer, d._iov[0].iov_base, reply_buffer_length);
				d._iov[0].iov_base = ((char *)d._iov[0].iov_base) + reply_buffer_length;
				result = reply_buffer_length;
			}
			else
			{
				if (len2 == 0) { result = 0; }
			}
		}
		else
		{
			ACE_DEBUG ((LM_DEBUG, "Recv of reply msg failed\n"));
			return YARP_FAIL;
		}
	}
	else
	{
		MyMessageHeader *hdr2 = (MyMessageHeader *)(d._iov[0].iov_base);
		d._iov[0].iov_base = ((char *)d._iov[0].iov_base) + sizeof(MyMessageHeader);

		int len2 = hdr2->GetLength();
		if (len2 > 0)
		{
			if (len2 < reply_buffer_length)
			{
				reply_buffer_length = len2;
			}

			memcpy (reply_buffer, d._iov[0].iov_base, reply_buffer_length);
			d._iov[0].iov_base = ((char *)d._iov[0].iov_base) + reply_buffer_length;
			result = reply_buffer_length;
		}
		else
		{
			if (len2 == 0) { result = 0; }
		}
	}

	return result;
}


int YARPOutputSocketDgram::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	return SendReceivingReply (reply_buffer, reply_buffer_length);
}


ACE_HANDLE YARPOutputSocketDgram::GetIdentifier(void) const
{
	return identifier;
}

#undef THIS_DBG
