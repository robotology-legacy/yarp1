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
/// $Id: YARPSocket.cpp,v 1.7 2003-04-24 08:49:32 gmetta Exp $
///
///

///
/// YARP incapsulates some socket (or any other OS communication features) into two 
///		externally visible classes YARPInputSocket and YARPOutputSocket
///
///	As of Apr 2003 this code is NOT tested under QNX.
///
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>


#ifndef __QNX__
#include <string>
using namespace std;
#ifndef __WIN_MSVC__
#include <unistd.h>  // just for gethostname
#else
#include <winsock2.h>
#endif
#else
#include "strng.h"
#include <unix.h>  // just for gethostname
#endif

#include <list>

///#include "TinySocket.h"
#include "YARPSocket.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPNameID.h"
#include "YARPScheduler.h"
#include "YARPTime.h"

///int TINY_VERBOSE = 0;
///#define DBG if (TINY_VERBOSE)
//#define DBG if (0)


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
///		ACE_DEBUG((LM_DEBUG, ">>>> header created with size %d\n", sizeof(MyMessageHeader)));
///		ACE_DEBUG((LM_DEBUG, ">>>> len part has size %d\n", sizeof(NetInt32)));
///		ACE_DEBUG((LM_DEBUG, ">>>> one char has size %d\n", sizeof(char)));
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


int YARPNetworkObject::getHostname(char *buffer, int buffer_length)
{
	int result = gethostname (buffer, buffer_length);
#ifndef __QNX4__
#ifndef __WIN__
#ifndef __QNX6__
	// QNX4 just doesn't have getdomainname or any obvious equivalent
	// cygwin version doesn't have getdomainname or any obvious equivalent
	if (result==0)
	{
		if (strchr(buffer,'.')==NULL)
		{
			int delta = strlen(buffer);
			buffer += delta;
			buffer_length -= delta;
			if (buffer_length>=1)
			{
				*buffer = '.';
				buffer++;
				buffer_length--;
			}
			result = getdomainname(buffer,buffer_length);
		}
	}
#endif
#endif
#endif
	return result;
}


///
/// structure:  socket thread handles an input connection.
///				_SocketThreadList is a simple stl container and handles the incoming connection (accept).
///				YARPInputSocket contains the list and provides the external interface.
///

class YARPInputSocket;
class _SocketThreadList;

///
/// CLASS DEFINITIONS.
///
///
///
///
class _SocketThread : public YARPThread
{
protected:
	int _available;
	ACE_SOCK_Stream *_stream;
	YARPUniqueNameID *_remote_endpoint;	

	_SocketThreadList *_owner;

	char *_extern_buffer;
	int _extern_length;
	char *_extern_reply_buffer;
	int _extern_reply_length;

	int _waiting;
	int _needs_reply;
	int _read_more;
	int _reply_preamble;
	YARPSemaphore _wakeup, _mutex, _reply_made;

	void _begin (YARPUniqueNameID *remid, ACE_SOCK_Stream *stream);

public:
	/// ctors.
	_SocketThread (YARPUniqueNameID *remid, ACE_SOCK_Stream *stream) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (remid, stream);
    }

	_SocketThread (void) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_begin (NULL, NULL);
    }

	~_SocketThread () {}

	/// needs a method to recycle the thread since
	/// thread creation might be a relatively costly process.

	/*
	// hack to deal with stl unpleasantness
	/// LATER: it might be required for STL.
	///
	SocketThread(const SocketThread& other)
	{
		owner = NULL;
		extern_buffer = NULL;
		extern_length = 0;
		extern_reply_buffer = NULL;
		extern_reply_length = 0;
		waiting = 0;
		needs_reply = 0;      
		available = 1;
		read_more = 0;
		reply_preamble = 0;
	}
	*/

	/// required by stl interface.
	int operator == (const _SocketThread& other) { return 0; }
	int operator != (const _SocketThread& other) { return 0; }
	int operator <  (const _SocketThread& other) { return 0; }

	void setOwner(_SocketThreadList& n_owner);

	/// call it reconnect (recycle the thread).
	/// the thread shouldn't be running.
	void reuse(YARPUniqueNameID *remid, ACE_SOCK_Stream *stream);

	/*
	/// this is now obsolete, REUSE does the job.
	void SetPID (ACE_HANDLE n_pid)
    {
		/// this might translate to something like:
		_stream.set_handle (n_pid)

		IFVERB printf("$$$ changing from %d to %d\n", socket.GetSocketPID(), n_pid);
		socket.ForcePID(n_pid);
		//owner = NULL;
		extern_buffer = NULL;
		extern_length = 0;
		waiting = 0;
		needs_reply = 0;
		read_more = 0;
		reply_preamble = 0;
    }
	*/

	ACE_HANDLE getID () const
    {
		return  _stream->get_handle (); /// socket.GetSocketPID();
    }

	virtual void End (void);

	/// thread Body.
	///	error check is not consistent.
	virtual void Body (void);

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
class _SocketThreadList : public YARPThread
{
private:
	/// int pid; what is this for?
	/// int assigned_port; replaced by ACE_SOCK_Addr
	
	ACE_INET_Addr _local_addr;
	ACE_SOCK_Acceptor _acceptor;

	list<_SocketThread *> _list;
	YARPSemaphore _new_data, _new_data_written;

	int _initialized;

public:
	/// ctors
	_SocketThreadList () : _local_addr (1111), _new_data(0), _new_data_written(0)
	{
		///pid = -1;  
		///system_resources = NULL; 
		///assigned_port = -1; 
		_initialized = 0;
	}

	/// what is this for?
	_SocketThreadList (const _SocketThreadList& other);

	/// among other things, should correctly terminate threads.
	virtual ~_SocketThreadList ();

	///
	/// creates the acceptor socket and listens to port.
	/// simply prepare the socket.
	ACE_HANDLE connect(const YARPUniqueNameID& id);

	/// actually the assigned is what provided by the name server but this
	/// is done on another class.
	int getAssignedPort (void) { return _local_addr.get_port_number(); }
	ACE_HANDLE getID (void) { return (!_initialized) ? ACE_INVALID_HANDLE : _acceptor.get_handle(); }

	void addSocket(void);
	int closeAll (void);

	/// closes a particular thread identified by its socket id. 
	///	the ACE_HANDLE is used as index into the list.
	int close (ACE_HANDLE reply_id);

	void declareDataAvailable (void);
	void declareDataWritten (void);

	virtual void Body (void)
	{
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

	// this demands exact number of bytes
	int receiveMore(ACE_HANDLE reply_pid, char *buf, int len);
};



///
/// METHODS IMPLEMENTATION
/// WARNING: requires a mutex to handle the stream deletion.
///
///
void _SocketThread::_begin (YARPUniqueNameID *remid, ACE_SOCK_Stream *stream)
{
	_owner = NULL;
	_extern_buffer = NULL;
	_extern_length = 0;
	_extern_reply_buffer = NULL;
	_extern_reply_length = 0;
	_waiting = 0;
	_needs_reply = 0;

	_remote_endpoint = remid;
	_stream = stream;

	_available = 1;
	_read_more = 0;
	_reply_preamble = 0;
}

void _SocketThread::setOwner(_SocketThreadList& n_owner)
{
	_owner = &n_owner;
}

/// call it reconnect (recycle the thread).
/// the thread shouldn't be running.
void _SocketThread::reuse(YARPUniqueNameID *remid, ACE_SOCK_Stream *stream)
{
	_remote_endpoint = remid;
	_stream = stream;
	_available = 0;
}

void _SocketThread::End (void)
{
	YARPThread::End ();
	_mutex.Wait ();
	if (_stream != NULL)
	{
		_stream->close ();
		delete _stream;
		_stream = NULL;
	}
	_mutex.Post ();
}

	/// thread Body.
	///	error check is not consistent.
void _SocketThread::Body (void)
{
	int finished = 0;

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;

	while (!finished)
	{
		ACE_DEBUG ((LM_DEBUG, "*** listener %d waiting\n", _remote_endpoint->getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		hdr.SetBad();
		int r = -1;

		r = _stream->recv_n (&hdr, sizeof(hdr), 0);
		//socket.Read((char*)(&hdr),sizeof(hdr),1);
		
		if (r < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "*** closing %d\n", r));
			_stream->close ();
			//socket.Close();
			finished = 1;
		}

		int len = hdr.GetLength();
		if (len < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));

			ACE_DEBUG ((LM_DEBUG, "*** closing\n", r));
			_stream->close ();
			//socket.Close();
			finished = 1;
		}

		if (len >= 0)
		{
			ACE_DEBUG ((LM_DEBUG, "*** got a header\n"));
			if (_owner != NULL)
			{
				ACE_DEBUG ((LM_DEBUG, "*** and i am owned\n"));
				_mutex.Wait();
				_extern_buffer = NULL;
				_extern_length = len;

				/// ???
				_owner->declareDataAvailable();

				_waiting = 1;
				_mutex.Post();
				_wakeup.Wait();
				ACE_DEBUG ((LM_DEBUG, "*** woken up!\n"));
			}

			if (_extern_buffer != NULL)
			{
				if (len > _extern_length)
				{
					len = _extern_length;
				}

				r = _stream->recv_n (_extern_buffer, len , 0);
				// r = socket.Read(extern_buffer,len,1);
				_extern_length = r;
				int rep = _needs_reply;
				
				_owner->declareDataWritten();
				if (rep)
				{
					_wakeup.Wait();
					_needs_reply = 0;
				}

				while (_read_more)
				{
					/// this was r too, a bit confusing.
					int rr = _stream->recv_n (_extern_reply_buffer, _extern_reply_length, 0); 
					//socket.Read(extern_reply_buffer, extern_reply_length,1);
					_extern_reply_length = rr;
					_read_more = 0;
					_reply_made.Post();
					_wakeup.Wait();
					_needs_reply = 0;
				}

				int was_preamble = 0;

				do
				{
					if (_reply_preamble)
					{
						rep = 1;
					}

					MyMessageHeader hdr2;
					hdr2.SetGood();
					char bufack[] = "acknowledged";
					char *buf3 = bufack;
					int reply_len = 0; //strlen(bufack)+1;
					if (rep)
					{
						buf3 = _extern_reply_buffer;
						reply_len = _extern_reply_length;
					}

					hdr2.SetLength(reply_len);
					_stream->send_n (&hdr2, sizeof(hdr2), 0);
					//socket.Write((char*)(&hdr2),sizeof(hdr2));

					if (reply_len > 0)
					{
						//socket.Write(buf3,reply_len);
						_stream->send_n (buf3, reply_len);
					}

					int curr_preamble = _reply_preamble;
					if (rep)
					{
						ACE_DEBUG ((LM_DEBUG, "*** POSTING reply made %d\n", curr_preamble));
						_reply_made.Post();
					}

					if (r >= 0)
					{
						ACE_DEBUG ((LM_DEBUG, "*** listener got %d bytes\n", r));
					}

					if (r < 0)
					{
						ACE_DEBUG ((LM_DEBUG, "*** closing\n", r));
						_stream->close ();
						//socket.Close();
						finished = 1;
					}

					was_preamble = 0;
					if (curr_preamble)
					{
						was_preamble = 1;

						ACE_DEBUG ((LM_DEBUG, "*** WAITING for post-preamble wakeup\n", r));
						_wakeup.Wait();
						ACE_DEBUG ((LM_DEBUG, "*** DONE WAITING for post-preamble wakeup\n", r));
						rep = 1;
					}
				}
				while (was_preamble);
			}
		}

		//YARPTime::DelayInSeconds(1);
	}	///

	_mutex.Wait ();
	delete _stream;
	_stream = NULL;
	_available = 1;
	_mutex.Post ();

#ifdef __WIN32__
	ACE_DEBUG ((LM_DEBUG, "*** comms thread bailed out\n"));
#else
	/// what is this for?
	ACE_DEBUG ((LM_DEBUG, "*** comms thread %d bailed out\n", getpid()));
#endif
}
    

///
///
///

/// among other things, should correctly terminate threads.
_SocketThreadList::~_SocketThreadList ()
{
	for (list<_SocketThread *>::iterator it = _list.begin(); it != _list.end(); it++)
	{
		(*it)->End ();
		delete (*it);
	}

	_initialized = 0;
}


///
/// creates the acceptor socket and listens to port.
/// simply prepares the socket.
ACE_HANDLE _SocketThreadList::connect(const YARPUniqueNameID& id)
{
	_local_addr = ((YARPUniqueNameID &)id).getAddressRef();
	_acceptor.open (_local_addr, 1);	// REUSE_ADDR enabled.

	/// closes down any still open thread (just in case?).
	if (_initialized) closeAll ();

	// pid = sock;	/// if needed stores the ACE_HANDLE in pid.
	
	ACE_DEBUG ((LM_DEBUG, "server socket open on %s port %d\n", _local_addr.get_host_name(), _local_addr.get_port_number()));

	// pid = sock;

	Begin();

	_initialized = 1;

	//IFVERB printf("Server socket is %d\n", sock);
	return _acceptor.get_handle ();
}


void _SocketThreadList::addSocket()
{
	// need to keep calling this to get next incoming socket
	ACE_ASSERT (_initialized != 0);

#ifdef __QNX__
	signal( SIGCHLD, SIG_IGN );     /* Ignore condition */
#endif

	/*
struct sockaddr sockaddr;
#ifndef __WIN__
#ifndef __QNX4__
#ifdef __QNX6__
socklen_t addrlen = sizeof(sockaddr);
#else
unsigned int addrlen = sizeof(sockaddr);
#endif
#else
int addrlen = sizeof(sockaddr);
#endif
#else
int addrlen = sizeof(sockaddr);
#endif 
int newsock;
	*/

	/// need to assert that _acceptor is actually created and connected.
	/// ACE_ASSERT (_acceptor != NULL);

	YARPScheduler::yield();
	ACE_DEBUG ((LM_DEBUG, "888888 pre accept %d\n", errno));

	ACE_INET_Addr incoming;
	ACE_SOCK_Stream *newstream = new ACE_SOCK_Stream;

	int ra = -1;
	do
	{
		ra = _acceptor.accept (*newstream, &incoming, 0);		/// wait forever.
		if (ra == -1)
		{
			ACE_DEBUG ((LM_DEBUG, "-------->>>>> accept got garbage, trying again\n"));
			YARPTime::DelayInSeconds (1);
		}
	}
	while (ra == -1);

	/// check accept return value.
	ACE_DEBUG ((LM_DEBUG, ">>> accepting a new socket %d (from in %s)\n", newstream->get_handle(), incoming.get_host_name()));
	ACE_DEBUG ((LM_DEBUG, "888888 post accept %d\n", errno));

	if (newstream->get_handle() == ACE_INVALID_HANDLE)
	{
		ACE_DEBUG ((LM_DEBUG, "invalid stream after accept, returning from addSocket\n"));
		return;
	}

/*
static int count = 0;
count ++;
if (count>=0)
{
  printf("======================================================\n");
  printf("RUNNING TESTS\n");
  TinySocket is;
  is.ForcePID(newsock);
  char buf[256];
  is.Read(buf,100);
  printf("======================================================\n");
}
*/

	// YARPScheduler::Yield();
	//assert(system_resources!=NULL);
	//if (newsock != -1) {
	//MySocketInfo& info = *((MySocketInfo*)system_resources);

	list<_SocketThread *>::iterator it_avail;
	int reusing = 0;
	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		if ((*it_avail)->isAvailable ())
		{
			reusing = 1;
			break;
		}
	}

	ACE_DEBUG ((LM_DEBUG, "888888 pre pushback %d\n", errno));

	if (it_avail == _list.end())
	{
		_list.push_back(new _SocketThread());
		it_avail = _list.end();
		it_avail--;
	}

	ACE_DEBUG ((LM_DEBUG, "888888 post pushback %d\n", errno));
	(*it_avail)->setAvailable (0);
	(*it_avail)->setOwner (*this);

	ACE_DEBUG ((LM_DEBUG, "888888 pre postbegin %d\n", errno));
	if (!reusing)
	{
		(*it_avail)->Begin();
	}
	else
	{
		(*it_avail)->End();
		(*it_avail)->Begin();
		//(*it_avail).wakeup.Post();
	}

	/// needs to be here, in case End destroys the old stream.
	(*it_avail)->reuse (&YARPUniqueNameID(YARP_TCP, incoming), newstream);

	ACE_DEBUG ((LM_DEBUG, "888888 post postbegin %d\n", errno));

	/*
	for (list<_SocketThread *>::iterator it = _list.begin(); it != _list.end(); it++)
	{
		ACE_DEBUG ((LM_DEBUG, "   pid %d...\n", (*it)->getID() ));
		//int pre = errno;
		//int v = eof((*it).GetID());
		//int post = errno;
		//IFVERB printf("   --> %d %d %d\n", pre, v, post);
	}
	*/
}

/// closes everything.
int _SocketThreadList::closeAll (void)
{
	ACE_ASSERT (_initialized != 0);

	list<_SocketThread *>::iterator it_avail;

	for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
	{
		/// it's a thread, ask for termination.
		(*it_avail)->End ();
	}

	/// terminates itself.
	End ();

	/// sockclose(pid);
	_acceptor.close ();
	_initialized = 0;

	///pid = 0;
	return 1;
}

/// closes a particular thread identified by its socket id. 
///	the ACE_HANDLE is used as index into the list.
int _SocketThreadList::close (ACE_HANDLE reply_id)
{
	ACE_ASSERT (_initialized != 0);

	int result = -1;
	list<_SocketThread *>::iterator it_avail;

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

	/*
	if (reply_id != -1)
	{
		/// MUST close the socket here.
		//sockclose(reply_id);
		IFVERB printf("!!!!!!!!!!!!! closed handle %d\n", reply_id);
	}
	*/
	return result;
}

void _SocketThreadList::declareDataAvailable (void)
{
	ACE_ASSERT (_initialized != 0);
	ACE_DEBUG ((LM_DEBUG, "$$$ Declaring data available\n"));
	_new_data.Post ();	
}

void _SocketThreadList::declareDataWritten (void)
{
	ACE_ASSERT (_initialized != 0);
	ACE_DEBUG ((LM_DEBUG, "$$$ Declaring new data written\n"));
	_new_data_written.Post ();	
}


/// return the ACE_HANDLE as id for further reply.
///
int _SocketThreadList::read(char *buf, int len, ACE_HANDLE *reply_pid)
{
	ACE_ASSERT (_initialized != 0);

	ACE_HANDLE save_pid = ACE_INVALID_HANDLE;

#ifdef __QNX__
	signal( SIGCHLD, SIG_IGN );     /* Ignore condition */
#endif

	int finished = 0;
	int result = -1;

	while (!finished)
	{
		ACE_DEBUG ((LM_DEBUG, "### Waiting for new data\n"));
		_new_data.Wait();

		ACE_DEBUG ((LM_DEBUG, "### Got new data\n"));

		list<_SocketThread *>::iterator it_avail;
		for (it_avail = _list.begin(); it_avail != _list.end(); it_avail++)
		{
			/// WARNING: isAvailable is read here without mut exclusion!
			if (!((*it_avail)->isAvailable()))
			{
				int work = 0, in_len = 0;
				(*it_avail)->waitOnMutex ();		/// _mutex.Wait();

				if ((*it_avail)->isWaiting())
				{
					ACE_HANDLE ((LM_DEBUG, "### Identified source of new data\n"));

					work = 1;
					(*it_avail)->setWaiting(0);						/// = 0;
					(*it_avail)->setExternalBuffer (buf);			///extern_buffer = buf;
					in_len = (*it_avail)->getExternalBufferLength (); ///extern_length;
					(*it_avail)->setExternalBufferLength (len);		///extern_length = len;

					(*it_avail)->setNeedsReply (1);

					(*it_avail)->postToWakeup ();	/// wakeup.Post();

					ACE_DEBUG ((LM_DEBUG, "### Waking up source of new data\n"));
				}
				
				(*it_avail)->postToMutex ();	/// mutex.Post();

				if (work)
				{
					ACE_DEBUG ((LM_DEBUG, "### Waiting for buffer write\n"));
					_new_data_written.Wait();

					save_pid = (*it_avail)->getID();
					ACE_DEBUG ((LM_DEBUG, "@@@ got data %d/%d\n", in_len, len));
					result = in_len;
					finished = -1;
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

int _SocketThreadList::pollingRead(char *buf, int len, ACE_HANDLE *reply_pid)
{
	ACE_ASSERT (_initialized != 0);

	if (reply_pid!=NULL)
	{
		*reply_pid = 0;
	}

	list<_SocketThread *>::iterator it_avail;
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

	return 0;
}

int _SocketThreadList::beginReply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);

	ACE_DEBUG ((LM_DEBUG, "&&& BEGINNING REPLY of %d bytes\n", len));

	list<_SocketThread *>::iterator it_avail;
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

	ACE_DEBUG ((LM_DEBUG, "&&& FINISHED BEGINNING REPLY of %d bytes\n", len));

	return 0;
}

int _SocketThreadList::reply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);
	ACE_DEBUG ((LM_DEBUG, "&&& BEGINNING FINAL REPLY of %d bytes\n", len));

	list<_SocketThread *>::iterator it_avail;
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

	ACE_DEBUG ((LM_DEBUG, "&&& FINISHED BEGINNING FINAL REPLY of %d bytes\n", len));

	return 0;
}

// this demands exact number of bytes
int _SocketThreadList::receiveMore(ACE_HANDLE reply_pid, char *buf, int len)
{
	int result = YARP_FAIL;

	ACE_ASSERT (_initialized != 0);

	list<_SocketThread *>::iterator it_avail;
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
class ISData
{
public:
	_SocketThreadList _list;
};

static ISData& ISDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((ISData*)x);
}


///
/// Input socket + stream + handling threads.
///
YARPInputSocket::YARPInputSocket()
{ 
	system_resources = NULL; 
//	identifier = -1; 
//	assigned_port = -1;
	system_resources = new ISData;
	ACE_ASSERT (system_resources!=NULL);
	
	_socktype = YARP_I_SOCKET;
}

/*
YARPInputSocket::YARPInputSocket(const YARPInputSocket& other)
{
	system_resources = NULL; 
	identifier = -1; 
	assigned_port = -1;
	system_resources = new ISData(ISDATA(other.system_resources));
	ACE_ASSERT(system_resources!=NULL);

	_socktype = YARP_I_SOCKET;
}
*/

YARPInputSocket::~YARPInputSocket()
{
	CloseAll ();

	if (system_resources!=NULL)
	{
		delete ((ISData*)system_resources);
		system_resources = NULL;
	}
}

int YARPInputSocket::Prepare (const YARPUniqueNameID& name)
{
	ISData& d = ISDATA(system_resources);
	d._list.connect (name);

	/// LATER: requires error handling here.
	return YARP_OK;
}

////
////
////
int YARPInputSocket::CloseAll (void)
{
	return ISDATA(system_resources)._list.closeAll();

	//int result = ISDATA(system_resources).owner.Close();
	//return result;
}

int YARPInputSocket::Close (ACE_HANDLE reply_id)
{
	///int result = ISDATA(system_resources).owner.Close(reply_id);
	///return result;
	return ISDATA(system_resources)._list.close (reply_id);
}

/* CHANGED INTO Connect.
int YARPInputSocket::Register(const char *name)
{
	DBG printf ("Registering port with name %s\n", name);
	int port = GetPort(name);
	DBG printf ("Registering port %d\n", port);
	int result = ISDATA(system_resources).owner.Connect(port);
	assigned_port = ISDATA(system_resources).owner.GetAssignedPort();
	DBG printf ("Assigned port is %d\n", assigned_port);
	return result;
}
*/

int YARPInputSocket::PollingReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.pollingRead (buffer, buffer_length, reply_id);
}


int YARPInputSocket::ReceiveBegin(char *buffer, int buffer_length, ACE_HANDLE *reply_id)
{
	return ISDATA(system_resources)._list.read (buffer, buffer_length, reply_id);
}


int YARPInputSocket::ReceiveContinue(ACE_HANDLE reply_id, char *buffer, int buffer_length)
{
	return ISDATA(system_resources)._list.receiveMore (reply_id, buffer, buffer_length);
}


int YARPInputSocket::ReceiveReplying(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.beginReply (reply_id, reply_buffer, reply_buffer_length);
}


int YARPInputSocket::ReceiveEnd(ACE_HANDLE reply_id, char *reply_buffer, int reply_buffer_length)
{
	return ISDATA(system_resources)._list.reply (reply_id, reply_buffer, reply_buffer_length);
}

ACE_HANDLE YARPInputSocket::GetIdentifier(void) const
{
	return ISDATA(system_resources)._list.getID ();
}

int YARPInputSocket::GetAssignedPort(void) const
{
	return ISDATA(system_resources)._list.getAssignedPort ();
}

///
/// Output socket + stream incapsulation.
///
///
class OSData
{
public:
	//TinySocket sock;

	ACE_INET_Addr _remote_addr;
	ACE_SOCK_Connector _connector;
	ACE_SOCK_Stream _stream;
};

static OSData& OSDATA(void *x)
{
	ACE_ASSERT (x != NULL);
	return *((OSData*)x);
}

YARPOutputSocket::YARPOutputSocket()
{ 
	system_resources = NULL;
	identifier = ACE_INVALID_HANDLE;
	system_resources = new OSData;
	ACE_ASSERT(system_resources!=NULL);

	_socktype = YARP_O_SOCKET;
}

YARPOutputSocket::~YARPOutputSocket()
{
	Close ();

	if (system_resources != NULL)
	{
		delete ((OSData*)system_resources);
		system_resources = NULL;
	}
}


int YARPOutputSocket::Close (void)
{
	return OSDATA(system_resources)._stream.close ();

/*
	int result = OSDATA(system_resources).sock.Close();
	return result;
*/
}


int YARPOutputSocket::Prepare (const YARPUniqueNameID& name)
{
	OSDATA(system_resources)._remote_addr = ((YARPUniqueNameID&)name).getAddressRef();
	return YARP_OK;
}

int YARPOutputSocket::SetTCPNoDelay (void)
{
	OSData& d = OSDATA(system_resources);
	int one = 1;
	return d._stream.set_option (ACE_IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));
}

int YARPOutputSocket::Connect (void)
{
	OSData& d = OSDATA(system_resources);
	ACE_DEBUG ((LM_DEBUG, "Connecting to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_name()));

	d._connector.connect (d._stream, d._remote_addr);

	identifier = d._stream.get_handle ();

	//identifier = OSDATA(system_resources).sock.GetSocketPID();
	//return identifier;
	
	return YARP_OK;

/*
	string machine = GetBase(name);
	int port = GetPort(name);
	const char *str;
#ifndef __QNX__
	str = machine.c_str();
#else
	str = machine.AsChars();
#endif
	DBG printf ("Connecting to port %d on %s\n", port, str);
	OSDATA(system_resources).sock.Connect(str,port);
	identifier = OSDATA(system_resources).sock.GetSocketPID();
	return identifier;
*/
}


int YARPOutputSocket::SendBegin(char *buffer, int buffer_length)
{
	MyMessageHeader hdr;
	hdr.SetGood ();
    hdr.SetLength (buffer_length);

	int sent = -1;
	sent = OSDATA(system_resources)._stream.send_n ((const void *)(&hdr), sizeof(hdr), 0);
	if (sent != sizeof(hdr))
		return YARP_FAIL;

	sent = OSDATA(system_resources)._stream.send_n (buffer, buffer_length, 0);
	if (sent != buffer_length)
		return YARP_FAIL;

	return YARP_OK;
	//return SendBlock(OSDATA(system_resources).sock,buffer,buffer_length);
}


int YARPOutputSocket::SendContinue(char *buffer, int buffer_length)
{
	/// without header.
	int sent = OSDATA(system_resources)._stream.send_n (buffer, buffer_length, 0);
	if (sent != buffer_length)
		return YARP_FAIL;

	return YARP_OK;

	//return SendBlock(OSDATA(system_resources).sock,buffer,buffer_length,0);
}

/// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocket::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
	MyMessageHeader hdr2;
	hdr2.SetBad ();

	int result = -1;
	int r = OSDATA(system_resources)._stream.recv_n ((void *)(&hdr2), sizeof(hdr2), 0);
	if (r == sizeof(hdr2))
	{
		int len2 = hdr2.GetLength();
		if (len2 > 0)
		{
			if (len2 < reply_buffer_length)
			{
				reply_buffer_length = len2;
			}

			result = OSDATA(system_resources)._stream.recv_n ((void *)reply_buffer, reply_buffer_length, 0);
		}
		else
		{
			if (len2 == 0) { result = 0; }
		}
	}
	return result;

	//return ::SendBeginEnd(OSDATA(system_resources).sock,reply_buffer,reply_buffer_length);
}


int YARPOutputSocket::SendEnd(char *reply_buffer, int reply_buffer_length)
{
	return SendReceivingReply (reply_buffer, reply_buffer_length);
	//return ::SendEnd(OSDATA(system_resources).sock,reply_buffer,reply_buffer_length);
}

/// ACE I guess requires the object all the time.
/*
void YARPOutputSocket::InhibitDisconnect()
{
	OSDATA(system_resources).sock.InhibitDisconnect();
}
*/

ACE_HANDLE YARPOutputSocket::GetIdentifier(void) const
{
	return identifier;
	///return _connector.get_handle ();
	//return OSDATA(system_resources).sock.GetSocketPID();
}


