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
/// $Id: YARPSocketMulti.cpp,v 2.1 2006-01-13 23:13:34 eshuy Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Handle_Set.h>
#include <ace/Time_Value.h>
#include <ace/Sched_Params.h>
#include <yarp/YARPTime.h>

///
///
///
#if defined(__QNX6__)

#	include <signal.h>
#	include <unix.h>

#elif defined (__WIN32__)


#elif defined (__LINUX__) || defined(__DARWIN__)

#	include <unistd.h>  // just for gethostname

#else

#	error "Your architecture requires a configuration"

#endif


#include <yarp/YARPList.h>
#include <yarp/YARPSocket.h>
#include <yarp/YARPSocketMulti.h>
#include <yarp/YARPSocketNameService.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPString.h>

#include "yarp_private/Headers.h"

///
#define THIS_DBG 80

/**
 * \file YARPSocketMulti.cpp contains classes supporting an input socket/endpoint.
 * The input socket is a structure containing a thread for each input connection 
 * (possibly running at a different rate) and a manager thread polling periodically 
 * (not really polling though) to see which is ready to receive a message.
 * Perhaps there would be a more efficient implementation of this server pattern
 * but this looks efficient for now.
 */

/*
 * This flag enables new code to remove reply messages.
 */
#define DONT_WAIT_UP

class YARPInputSocketMulti;
class _SocketThreadListMulti;
class _SocketThreadMultiBase;
class _SocketThreadMultiTcp;
class _SocketThreadMultiUdp;
class _SocketThreadMultiMcast;
class _SocketThreadMultiShmem;

/**
 * 
 * This is a thread class that manages a list of communication threads of type 
 * _SocketThreadMultiBase.
 * SocketThreadListMulti manages a list of threads each communicating with a 
 * possibly distinct protocol. This allows a port to receive messages in any
 * of the YARP protocols (except QNET). A single instance of this class
 * is used within the YARPInputSocketMulti class which is just a cleaner 
 * interface.
 *
 */
class _SocketThreadListMulti : public YARPThread
{
private:
	/** local address of the acceptor. */
	ACE_INET_Addr _local_addr;
	/** the acceptor TCP socket. */
	ACE_SOCK_Acceptor _acceptor_socket;
	/** name of the interface (IP or symbolic). */	
	YARPString _interface;
	/** the name of the owner (symbolic). */
	YARPString _own_name;

	/** managed list of threads. */
	YARPList<_SocketThreadMultiBase *> _list;
	/** to protect the structure of the list of threads. */
	YARPSemaphore _listProtect;
	/** events on data available/written. */
	YARPSemaphore _new_data, _new_data_written;

	/** flag on whether the class has been initialized. */
	int _initialized;
	
	/** array of IP ports available to incoming connections. */
	int *_ports;
	/** total number of IP ports. */
	int _number_o_ports;
	/** index into array of ports. */
	int _last_assigned;

protected:
	void addSocket (void);

	virtual void Body (void)
	{
#if defined(__QNX__) || defined(__LINUX__)
		//signal( SIGCHLD, SIG_IGN );     /* Ignore condition */
#endif
		ACE_DEBUG ((LM_DEBUG, "***** _SocketThreadListMulti::Body : acceptor thread starting\n", GetIdentifier()));

		while (!IsTerminated())
		{
			addSocket();
		}

		ACE_DEBUG ((LM_DEBUG, "***** _SocketThreadListMulti::Body : acceptor thread 0x%x returning\n", GetIdentifier()));
	}

public:
	/**
	 * Constructor.
	 * This is the default constructor, just providing some default values.
	 */
	_SocketThreadListMulti () : _local_addr (1111), _new_data(0), _new_data_written(0), _listProtect(1)
	{
		_ports = NULL;
		_number_o_ports = 0;
		_last_assigned = -1;
		_initialized = 0;
		_own_name = "__null";
	}

	/**
	 * Destructor.
	 */
	virtual ~_SocketThreadListMulti ();

	/**
	 * Creates the acceptor socket and listens to port. This method doesn't really
	 * connect anywhere, it just prepares the listening socket.
	 * @param id is the YARPUniqueNameSock of the accepting connection.
	 * @return the ACE_HANDLE of the newly created socket.
	 */
	ACE_HANDLE connect(const YARPUniqueNameSock& id);

	/**
	 * Returns the port number assigned to this port for listening to incoming connections.
	 * The assigned port is provided by the name server. This is taken care of by another class.
	 * @return the assigned IP port number.
	 */
	int getAssignedPort (void) { return _local_addr.get_port_number(); }

	/**
	 * Gets the ACE_HANDLE of the acceptor socket.
	 * @return the acceptor socket handle if initialized, ACE_INVALID_HANDLE otherwise.
	 */
	ACE_HANDLE getID (void) { return (!_initialized) ? ACE_INVALID_HANDLE : _acceptor_socket.get_handle(); }

	/**
	 * Closes all incoming connections.
	 * @return YARP_OK on success.
	 */
	int closeAll (void);

	/**
	 * Closes a particular thread identified by its socket id, 
	 * the ACE_HANDLE is used as index into the list.
	 * @param reply_id is the handle of the socket whose thread is to be terminated.
	 * @return YARP_OK on success.
	 */
	int close (ACE_HANDLE reply_id);

	/**
	 * Closes a particular connection identified by a symbolic name.
	 * @param name is the name of the remote connection to be closed.
	 * @return YARP_OK on success.
	 */
	int closeByName (const YARPString& name);

	/**
	 * Posts on a semaphore to signal that new data is available for reading.
	 */
	void declareDataAvailable (void);

	/**
	 * Posts on a semaphore to signal that new data is on the buffer.
	 */
	void declareDataWritten (void);

	/**
	 * Called to terminate this thread and ask for termination of all
	 * communication threads.
	 * @param dontkill is not used.
	 */
	virtual void End (int donkill = -1)
	{
		if (_initialized)
		{
			YARPThread::AskForEnd ();

			ACE_SOCK_Connector connector;
			ACE_SOCK_Stream stream;
			ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
			int r = connector.connect (stream, _local_addr, 0, &timeout);
			if (r < 0)
			{
				// not much else I can do anyway.
				YARPThread::End(0);
				return;
			}

			MyMessageHeader hdr;
			hdr.SetBad ();
			stream.send_n (&hdr, sizeof(hdr), 0);
			stream.close ();

			YARPThread::Join ();
		}
	}

	/**
	 * Reads from any thread with data available on the receiving buffer.
	 * Note that it returns the ACE_HANDLE @a reply_pid as ID for further replies.
	 * @param buf is the pointer to a buffer of char where the message if to be
	 * stored.
	 * @param len is the length of the buffer.
	 * @param reply_pid is the ID of the receiving socket (ACE_HANDLE).
	 * @param whether the mutex to protect list access is used.
	 * @return YARP_OK on success.
	 */
	int read(char *buf, int len, ACE_HANDLE *reply_pid = NULL, bool safe = true);

	/**
	 * Polls from any thread with data available on the receiving buffer.
	 * Note that it returns the ACE_HANDLE @a reply_pid as ID for further replies. 
	 * This method is used to implement polling reads.
	 * @param buf is the pointer to a buffer of char where the message if to be
	 * stored.
	 * @param len is the length of the buffer.
	 * @param reply_pid is the ID of the receiving socket (ACE_HANDLE).
	 * @return YARP_OK on success.
	 */
	int pollingRead(char *buf, int len, ACE_HANDLE *reply_pid = NULL);

	/**
	 * Starts the reply to a received message.
	 * This uses the ID acquired by any previous read. 
	 * @param reply_id is ID of the socket where to reply to.
	 * @param buf is the pointer to a buffer of char containing the reply message.
	 * @param len is the length of the reply message in bytes.
	 * @return YARP_OK on success.
	 */
	int beginReply(ACE_HANDLE reply_pid, char *buf, int len);

	/**
	 * Completes a reply.
	 * This uses the ID acquired by any previous read. 
	 * @param reply_id is ID of the socket where to reply to.
	 * @param buf is the pointer to a buffer of char containing the reply message.
	 * @param len is the length of the reply message in bytes.
	 * @return YARP_OK on success.
	 */
	int reply(ACE_HANDLE reply_pid, char *buf, int len);

	/**
	 * Reads from the thread identified by @a reply_pid. This method can be called
	 * multiple times until all parts of a message have been received.
	 * @param reply_id is ID of the socket to listen from.
	 * @param buf is the pointer to a buffer where to store the message part.
	 * @param len is the length of the buffer.
	 * @return YARP_OK on success.
	 */
	int receiveMore(ACE_HANDLE reply_pid, char *buf, int len);

	/**
	 * Creates a pool of port numbers to be used by incoming connections.
	 * Note that these are IP port numbers assigned by the name server.
	 * @param ports is an array of port numbers.
	 * @param number_o_ports is the size of the array @a ports.
	 * @return YARP_OK on success.
	 */
	int setPool (int *ports, int number_o_ports) 
	{
		if (_ports != NULL) delete[] _ports;
		_ports = new int[number_o_ports];
		ACE_ASSERT (_ports != NULL);
		ACE_OS::memcpy (_ports, ports, sizeof(int) * number_o_ports);
		_number_o_ports = number_o_ports;
		_last_assigned = -1;
		return YARP_OK;
	}
	
	/**
	 * Gets a new unused port number from the pool.
	 * Note that these are IP port numbers assigned by the name server.
	 * @return a new port number on success, 0 on failure.
	 */
	int getNewPortNumberFromPool (void)
	{
		_last_assigned ++;
		if (_last_assigned >= _number_o_ports || _last_assigned < 0)
		{
			ACE_DEBUG ((LM_ERROR, "this shouldn't happen, _last_assigned port out of range!\n"));
			return 0;
		}

		return _ports[_last_assigned];
	}

	/**
	 * Sets the name of the interface cart. This is used by MCAST
	 * to determine to which network card to emit to (in case there's more
	 * than one). It can be an IP address (as in Windows) or a symbolic name 
	 * as for Linux/Unixes (e.g. le0, eth).
	 * @param the name of the interface card as a YARPString.
	 */
	void setInterfaceName (const YARPString& name) { _interface = name; }

	/**
	 * Returns the name of the owner port.
	 * @return the name of the owner port as a YARPString.
	 */
	YARPString getOwnerName (void) { return _own_name; }

	/**
	 * Prints the list of threads managed by the class.
	 * @return YARP_OK always.
	 */
	int printThreadList (void);
};


/**
 * A tentative rationalization of the input socket behavior. This is a base class
 * with a thread that handles an input connection. Specialized classes are
 * derived from this one and implement a particular protocol.
 */
class _SocketThreadMultiBase : public YARPBareThread
{
protected:
	/** whether the thread instance is available for recycling */
	int _available;
	/** the assigned port number */
	int _port;

	/** details of the remote peer address */
	YARPUniqueNameSock _remote_endpoint;
	/** the local enpoint address */
	YARPUniqueNameID *_socket_addr;

	/** pointer for upcalls */
	_SocketThreadListMulti *_owner;

	char *_extern_buffer;
	int _extern_length;
	char *_extern_reply_buffer;
	int _extern_reply_length;

	char _local_buffer[MAX_PACKET];
	int _local_buffer_counter;

	int _waiting;
	int _needs_reply;
	int _read_more;
	int _reply_preamble;

	YARPSemaphore _wakeup, _mutex, _reply_made;

	// prevent copies.
	_SocketThreadMultiBase (const _SocketThreadMultiBase&);
	void operator= (const _SocketThreadMultiBase&);

public:
	/**
	 * Constructor.
	 * It just initializes internal data to some useful default.
	 */
	_SocketThreadMultiBase (void) : _wakeup(0), _mutex(1), _reply_made(0)
    {
		_owner = NULL;
		_extern_buffer = NULL;
		_extern_length = 0;
		_extern_reply_buffer = NULL;
		_extern_reply_length = 0;
		_waiting = 0;
		_needs_reply = 0;
		_local_buffer_counter = 0;

		_port = 0;

		_available = 1;
		
		_remote_endpoint.invalidate();
		_socket_addr = NULL;

		_read_more = 0;
		_reply_preamble = 0;
    }

	/**
	 * Destructor.
	 * It tries to terminate the managed thread too.
	 */
	virtual ~_SocketThreadMultiBase (void) 
	{
		if (!IsTerminated()) End();
	}

	/**
	 * Asks the termination of the current thread.
	 * @param dontkill is not used.
	 */
	virtual void End (int dontkill = -1)
	{
		ACE_ASSERT (1 == 0);
	}

	/**
	 * Sets the owner of this class. The owner is the container instance managing 
	 * this thread.
	 * @param n_owner is a reference to an instance of the container class.
	 */
	void setOwner(const _SocketThreadListMulti& n_owner);

	/**
	 * Gets the remote endpoint ID. 
	 * @return the remote ID of the connection managed by the thread as a YARPUniqueNameSock
	 * reference.
	 */
	YARPUniqueNameSock& getRemoteID(void) { return _remote_endpoint; }

	/**
	 * Gets the name of the remote endpoint. 
	 * @return the remote name as a YARPString.
	 */
	YARPString getRemoteName(void) { return _socket_addr->getName(); }

	/**
	 * Gets the ID of the socket managed by this instance.
	 * @return the handle of the socket (ACE_HANDLE).
	 */
	ACE_HANDLE getID () const {	return  (_socket_addr != NULL) ? _socket_addr->getRawIdentifier () : ACE_INVALID_HANDLE; }

	/**
	 * Gets the require acknowledge flag. This is used to test whether the underlying
	 * communication protocol is requesting replies. Replies might slow down communication.
	 * @return 1 if replies are requested, 0 otherwise.
	 */
	int getRequireAck (void) const { return (_socket_addr != NULL) ? _socket_addr->getRequireAck() : 0; }

	/**
	 * Sets the require acknowledge flag. This is used to ask the underlying
	 * communication protocol to request replies. Replies might slow down communication.
	 * @param flag 1 to activate replies, 0 otherwise.
	 */
	void setRequireAck(int flag) { if (_socket_addr!=NULL) _socket_addr->setRequireAck(flag); }

	/**
	 * Returns the last used port number for this thread.
	 * @return the last port number used by this thread.
	 */
	int getOldPortNumber (void) { return _port; } 

	/**
	 * Sets the availability of this thread (terminated).
	 * @param flag if 1 the thread structure can be recycled.
	 */
	inline void setAvailable (int flag) { _available = flag; }

	/**
	 * Tests the availability of this thread (terminated).
	 * @return 1 if the thread structure can be recycled, 0 otherwise.
	 */
	inline int isAvailable (void) const { return _available; }

	/**
	 * Tests whether the thread is waiting for new data.
	 * @return 1 if the thread is waiting, 0 otherwise.
	 */
	inline int isWaiting (void) const { return _waiting; }

	/**
	 * Sets the wating flag to a specific value.
	 * @param w is the new value.
	 */
	inline void setWaiting (int w) { _waiting = w; }

	/**
	 * Sets the external buffer for the message. A pointer to
	 * a valid memory buffer is given to the thread to copy 
	 * (through a recv on a socket) the incoming message part into.
	 * @param b is the pointer to the buffer.
	 */
	inline void setExternalBuffer (char *b) { _extern_buffer = b; }

	/**
	 * Gets the length of the buffer. This method is used to
	 * determine how many bytes is the message composed of.
	 * @return the length of the buffer in bytes.
	 */
	inline int getExternalBufferLength (void) const { return _extern_length; }

	/**
	 * Sets the length of the buffer. This method is used to
	 * tell the thread how big is the buffer.
	 * @param l is the length of the buffer in bytes.
	 */
	inline void setExternalBufferLength (int l) { _extern_length = l; }

	/**
	 * Sets a flag to tell the thread code that a reply to the
	 * message is required.
	 * @param i 1 means that the reply is required, 0 otherwise.
	 */
	inline void setNeedsReply (int i) { _needs_reply = i; }

	/**
	 * Sets a flag to tell the thread code that another message
	 * part has to be read from the socket.
	 * @param i 1 means that there is more data to read, 0 otherwise.
	 */
	inline void setReadMore (int i) { _read_more = i; }

	/**
	 * Sets a flag to tell the thread code the next buffer contains
	 * the preamble of the reply message.
	 * @param i 1 means that the next buffer is the reply preamble, 0 otherwise.
	 */
	inline void setReplyPreamble (int i) { _reply_preamble = i; }

	/**
	 * Sets the external buffer for the reply to a message. A pointer to
	 * a valid memory buffer is given to the thread.
	 * @param b is the pointer to the buffer.
	 */
	inline void setExternalReplyBuffer (char *b) { _extern_reply_buffer = b; }

	/**
	 * Gets the length of the reply buffer. This method is used to
	 * determine how many bytes is the reply composed of.
	 * @return the length of the reply message in bytes.
	 */
	inline int getExternalReplyLength (void) const { return _extern_reply_length; }

	/**
	 * Sets the length of the reply buffer. This method tells the 
	 * thread code how long is the next reply message.
	 * @param l the length of the reply message in bytes.
	 */
	inline void setExternalReplyLength (int l) { _extern_reply_length = l; }

	/**
	 * The calling thread waits on a mutex. This method is used
	 * to access shared variables in the thread class.
	 */
	inline void waitOnMutex (void) { _mutex.Wait (); }

	/**
	 * The calling thread releases the mutex. This method is used
	 * to tell the thread that the access to shared variables is
	 * terminated.
	 */
	inline void postToMutex (void) { _mutex.Post (); }

	/**
	 * Posts the wakeup semaphore to tell the thread to continue processing.
	 */
	inline void postToWakeup (void) { _wakeup.Post (); }

	/**
	 * The caller blocks until a reply is completed.
	 */
	inline void waitOnReplyMade (void) { _reply_made.Wait (); }

	/**
	 * Cleans internal data structures. Used internally by reuse() to clean 
	 * addressing information.
	 */
	virtual void clearOldAddr (void) = 0;
	
	/**
	 * Prints the socket name and service type. To be used for debug.
	 */
	virtual void printSocketData (void) = 0;

	/**
	 * Gets the protocol type.
	 * @return the protocol type if successful, YARP_NO_SERVICE_AVAILABLE otherwise.
	 */
	virtual int getServiceType (void) const = 0;

	/**
	 * Initializes a thread instance. In spite of its name, this method is called both
	 * when starting up a new thread or to recycle an old one.
	 * @param remid is the descriptor of the incoming connection. It is always TCP. It 
	 * contains also the interface name for MCAST communication.
	 * @param socket is the descriptor of the local connection, and relative protocol.
	 * It contains the port number assigned from the pool, the name of the remote connection,
	 * and the protocol type (UDP, TCP, etc.).
	 * @param port is the port number (as requested from the pool).
	 * @return YARP_OK if successful.
	 */
	virtual int reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port) = 0;
};

///
/// IMPLEMENTATION.
///
void _SocketThreadMultiBase::setOwner(const _SocketThreadListMulti& n_owner)
{
	_owner = (_SocketThreadListMulti *)(&n_owner);
}



/**
 * A thread for managing an incoming connection in the tcp protocol.
 */
class _SocketThreadMultiTcp : public _SocketThreadMultiBase
{
protected:
	/** The tcp stream. */
	ACE_SOCK_Stream *_stream;	

public:
	/**
	 * Default constructor.
	 */
	_SocketThreadMultiTcp (void) : _SocketThreadMultiBase ()
	{
		_stream = NULL;
	}

	/**
	 * Destructor.
	 */
	virtual ~_SocketThreadMultiTcp () {}

	/**
	 * Asks the termination of the current thread.
	 * @param dontkill is not used.
	 */
	virtual void End (int dontkill = -1);

	/**
	 * The thread body.
	 */
	virtual void Body (void);

	/**
	 * Cleans internal data structures. Used internally by reuse() to clean 
	 * addressing information.
	 */
	virtual void clearOldAddr (void);

	/**
	 * Sets the TCP stream. For TCP connections this method is used to 
	 * tell the thread where to get data from.
	 * @param stream is the TCP stream where to read from.
	 */
	void setStream (ACE_SOCK_Stream *stream);

	/**
	 * Prints the socket name and service type. To be used for debug.
	 */
	void printSocketData (void);

	/**
	 * Gets the protocol type.
	 * @return the protocol type if successful, YARP_NO_SERVICE_AVAILABLE otherwise.
	 */
	virtual int getServiceType (void) const;

	/**
	 * Initializes a thread instance. In spite of its name, this method is called both
	 * when starting up a new thread or to recycle an old one.
	 * @param remid is the descriptor of the incoming connection. It is always TCP. It 
	 * contains also the interface name for MCAST communication.
	 * @param socket is the descriptor of the local connection, and relative protocol.
	 * It contains the port number assigned from the pool, the name of the remote connection,
	 * and the protocol type (UDP, TCP, etc.).
	 * @param port is the port number (as requested from the pool).
	 * @return YARP_OK if successful.
	 */
	virtual int reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port);
};

///
/// IMPLEMENTATION.
///
void _SocketThreadMultiTcp::Body (void)
{
#if defined(__QNX6__) || defined(__LINUX__)
	signal (SIGPIPE, SIG_IGN);
#endif

	_mutex.Wait();

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;
	
	// to store the thread remote peer name and recover it
	// before termination to print the quit message (see code below).
	YARPString ownname = getRemoteName();
	_mutex.Post();

	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	int r = YARP_FAIL, rr = YARP_FAIL;
	int was_preamble = 0;
	int len = 0, rep = 0;	
	int reply_len = 0;
	MyMessageHeader hdr2;
	char bufack[] = "acknowledged";
	char *buf3 = bufack;

	ACE_ASSERT (_stream != NULL);

	while (!IsTerminated())
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** listener %d waiting\n", _remote_endpoint.getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		hdr.SetBad();

		// I wonder whether this can be made to read the current packet instead of...
		// better packing all the sends in a single send_n - what about the NODELAY option?.
		r = 0;
		double now = YARPTime::GetTimeAsSeconds();
		double prev = now - 1000;
		while (r==0 && (now-prev>YARP_SHORT_SOCK_TIMEOUT/2.0)) 
		{
			// repeat loop so long as it is willing to block.
			ACE_Time_Value timeout (YARP_SHORT_SOCK_TIMEOUT, 0);
			prev = now;
			r = _stream->recv_n (&hdr, sizeof(hdr), &timeout);
			now = YARPTime::GetTimeAsSeconds();
			if (r == -1 && errno==ETIME && !IsTerminated()) 
			{
				r = 0;  
				prev = now - 1000;
			}
		}
		//r = _stream->recv_n (&hdr, sizeof(hdr), 0);
		
		if (r <= 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** closing %d\n", r));
			if (_stream != NULL)
			{
				_stream->close ();
				delete _stream;
				_stream = NULL;
			}

			AskForEnd();
			goto TcpSocketMsgSkip;
		}

		len = hdr.GetLength();
		if (len < 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** closing\n", r));
			if (_stream != NULL)
			{
				_stream->close ();
				delete _stream;
				_stream = NULL;
			}

			AskForEnd();
			goto TcpSocketMsgSkip;
		}

		if (len >= 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** got a header\n"));
			if (_owner != NULL)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** and i am owned\n"));
				_mutex.Wait();
				_extern_buffer = NULL;
				_extern_length = len;

				_owner->declareDataAvailable();

				_waiting = 1;
				_mutex.Post();
				_wakeup.Wait();

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** woken up!\n"));
			}

			if (_extern_buffer != NULL)
			{
				if (len > _extern_length)
				{
					len = _extern_length;
				}

				r = _stream->recv_n (_extern_buffer, len , 0); //, &timeout);
				_extern_length = r;
				rep = _needs_reply;
				if (r <= 0)
				{
					ACE_OS::memset (_extern_buffer, 0, _extern_length);
					AskForEnd();
				}

				_owner->declareDataWritten();
				if (rep)
				{
					_wakeup.Wait();
					_needs_reply = 0;
				}

				while (_read_more)
				{
					rr = 0;
					if (_extern_reply_length == 0)
					{
						// then do a select.
						ACE_Handle_Set set;
						set.reset ();
						set.set_bit (_stream->get_handle());
						ACE_OS::select (int(_stream->get_handle())+1, set); //, 0, 0, &timeout);
						/// wait here until next valid chunck of data.
					}
					else
					{
						rr = _stream->recv_n (_extern_reply_buffer, _extern_reply_length, 0); //, &timeout); 
						if (rr <= 0) AskForEnd();
					}

					_extern_reply_length = rr;

					_read_more = 0;
					_reply_made.Post();
					_wakeup.Wait();
					_needs_reply = 0;
				}

				was_preamble = 0;

#ifdef DONT_WAIT_UP
				if (getRequireAck())
#endif
				do
				{
					if (_reply_preamble)
					{
						rep = 1;
					}

					hdr2.SetGood();
					buf3 = bufack;
					reply_len = 0; //strlen(bufack)+1;

					if (rep)
					{
						buf3 = _extern_reply_buffer;
						reply_len = _extern_reply_length;
					}

					hdr2.SetLength(reply_len);
					rr = _stream->send_n (&hdr2, sizeof(hdr2), 0);
					if (rr <= 0) AskForEnd();

					if (reply_len > 0)
					{
						rr = _stream->send_n (buf3, reply_len);
						if (rr <= 0) AskForEnd();
					}

					int curr_preamble = _reply_preamble;
					if (rep)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** POSTING reply made %d\n", curr_preamble));
						_reply_made.Post();
					}

					was_preamble = 0;
					if (curr_preamble)
					{
						was_preamble = 1;

						YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** WAITING for post-preamble wakeup\n", r));
						_wakeup.Wait();
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** DONE WAITING for post-preamble wakeup\n", r));
						rep = 1;
					}
				}
				while (was_preamble);
			}
		}

TcpSocketMsgSkip:
		;
	}	// while (!IsTerminated)

	if (_owner != NULL && 
		ACE_OS::strncmp (ownname.c_str(), "explicit_connect", 16) != 0)
	{
		ACE_DEBUG ((LM_INFO, "*** detaching %s from %s [me being %s]\n", 
			ownname.c_str(), 
			_owner->getOwnerName().c_str(), 
			_owner->getOwnerName().c_str()));
	}

	_mutex.Wait ();
	_available = 1;
	_mutex.Post ();
}

void _SocketThreadMultiTcp::End (int dontkill /* = -1 */)
{
	ACE_UNUSED_ARG (dontkill);

	_mutex.Wait ();

	AskForEnd ();

	if (_stream != NULL)
	{
		_stream->close ();
		delete _stream;
		_stream = NULL;
	}

	if (_socket_addr != NULL)
	{
		delete _socket_addr;
		_socket_addr = NULL;
	}

	_mutex.Post ();

	// awful thing. to wakeup the reader...
	_wakeup.Post ();

	// the thread should have been terminated anyway!
	YARPBareThread::Join();
}


void _SocketThreadMultiTcp::setStream (ACE_SOCK_Stream *stream)
{
	ACE_ASSERT (stream != NULL);

	if (_stream != NULL)
	{
		_stream->close();
		delete _stream;
	}

	_stream = stream;
}

void _SocketThreadMultiTcp::clearOldAddr (void)
{
	if (_socket_addr != NULL)
	{
		// do not change _stream, this has been taken care by setTCPStream().

		if (_socket_addr != NULL) 
		{
			delete _socket_addr;
			_socket_addr = NULL;
		}
	}
}

void _SocketThreadMultiTcp::printSocketData (void)
{
	if (_socket_addr == NULL)
	{
		ACE_DEBUG ((LM_INFO, "socket is NULL %d ", isAvailable()));
	}
	else
	{
		ACE_DEBUG ((LM_INFO, "%s ", _socket_addr->getName().c_str()));
		YARPUniqueNameSock *a = (YARPUniqueNameSock *)_socket_addr;
		ACE_DEBUG ((LM_INFO, "%s:%d %d ", 
			_remote_endpoint.getAddressRef().get_host_addr(),
			a->getAddressRef().get_port_number(),
			isAvailable() ));
	}
}

int _SocketThreadMultiTcp::getServiceType (void) const
{ 
	const int type = (_socket_addr != NULL) ? _socket_addr->getServiceType() : YARP_NO_SERVICE_AVAILABLE; 
	if (_socket_addr != NULL) 
		ACE_ASSERT (type == YARP_TCP);

	return type;
}

// call it reconnect (recycle the thread).
// the thread shouldn't be running.
int _SocketThreadMultiTcp::reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port)
{
	// remid must be always TCP.
	ACE_ASSERT (remid->getServiceType() == YARP_TCP);

	clearOldAddr ();

	// making sure semaphores are in the right state.
	if (_wakeup.PollingWait())
		ACE_DEBUG ((LM_DEBUG, "****** reuse found _wakeup != 0\n"));

	if (port != 0)
	{
		// this is fine but the actual remote is the TCP here...
		// this gets the right value only after the first recv, no alternatives.
		_remote_endpoint = *remid;
		_port = port;

		// not quite needed for the addr itself (just stores the protocol -TCP-).
		_socket_addr = new YARPUniqueNameSock (*(YARPUniqueNameSock*)socket);
		ACE_ASSERT (_socket_addr != NULL);

		// externally provided during creation.
		ACE_ASSERT (_stream != NULL);

		if (_stream->get_handle() == ACE_INVALID_HANDLE)
		{
			_available = 1;
			return YARP_FAIL;
		}

		// since socket is NULL use the _stream ID.
		_socket_addr->setRawIdentifier (_stream->get_handle());
		_available = 0;
	}
	else
	{
		_available = 1;
		_remote_endpoint.invalidate();
		_socket_addr = NULL;

		if (_stream != NULL)
		{
			_stream->close ();
			delete _stream;
			_stream = NULL;
		}

		return YARP_FAIL;
	}

	return YARP_OK;
}

/**
 * A thread for managing an incoming connection in the udp protocol.
 */
class _SocketThreadMultiUdp : public _SocketThreadMultiBase
{
protected:
	/** the dgram socket */
	ACE_SOCK_Dgram * _socket;
	/** a generic TCP socket used for service communication (control messages). */
	ACE_SOCK_Acceptor _local_acceptor;

public:
	/**
	 * Default constructor.
	 */
	_SocketThreadMultiUdp (void) : _SocketThreadMultiBase ()
	{
		_socket = NULL;
	}

	/**
	 * Destructor.
	 */
	virtual ~_SocketThreadMultiUdp () {}

	/**
	 * Asks the termination of the current thread.
	 * @param dontkill is not used.
	 */
	virtual void End (int dontkill = -1);

	/**
	 * The thread body.
	 */
	virtual void Body (void);

	/**
	 * Cleans internal data structures. Used internally by reuse() to clean 
	 * addressing information.
	 */
	virtual void clearOldAddr (void);

	/**
	 * Prints the socket name and service type. To be used for debug.
	 */
	void printSocketData (void);

	/**
	 * Gets the protocol type.
	 * @return the protocol type if successful, YARP_NO_SERVICE_AVAILABLE otherwise.
	 */
	virtual int getServiceType (void) const;

	/**
	 * Initializes a thread instance. In spite of its name, this method is called both
	 * when starting up a new thread or to recycle an old one.
	 * @param remid is the descriptor of the incoming connection. It is always TCP. It 
	 * contains also the interface name for MCAST communication.
	 * @param socket is the descriptor of the local connection, and relative protocol.
	 * It contains the port number assigned from the pool, the name of the remote connection,
	 * and the protocol type (UDP, TCP, etc.).
	 * @param port is the port number (as requested from the pool).
	 * @return YARP_OK if successful.
	 */
	virtual int reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port);
};

///
/// IMPLEMENTATION.
///
void _SocketThreadMultiUdp::Body (void)
{
#if defined(__QNX6__) || defined(__LINUX__)
	signal (SIGPIPE, SIG_IGN);
#endif

	_mutex.Wait();

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;
	
	// to store the thread remote peer name and recover it
	// before termination to print the quit message (see code below).
	YARPString ownname = getRemoteName();
	_mutex.Post();

	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_INET_Addr incoming;

	int r = YARP_FAIL;
	int was_preamble = 0;
	int len = 0;	

	char bufack[] = "acknowledged";
	char *buf3 = bufack;

	ACE_OS::memset (_local_buffer, 0, MAX_PACKET);
	_local_buffer_counter = 0;

	ACE_ASSERT (_socket != NULL);
	ACE_SOCK_Dgram& dgram_socket = *((ACE_SOCK_Dgram *)_socket);

	while (!IsTerminated())
	{
		MyMessageHeader& hdr = *((MyMessageHeader *)_local_buffer);

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread of remote port %s:%d waiting\n", _remote_endpoint.getAddressRef().get_host_addr(), _remote_endpoint.getAddressRef().get_port_number()));

		ACE_Handle_Set set;
		set.reset ();
		set.set_bit (_local_acceptor.get_handle());
		set.set_bit (dgram_socket.get_handle());
		const int max = ((int)_local_acceptor.get_handle() > (int)dgram_socket.get_handle()) 
			? (int)_local_acceptor.get_handle() : (int)dgram_socket.get_handle();

		const int rr = ACE_OS::select (max + 1, set, 0, 0, NULL); //&long_timeout);

		if (rr <= 0)
		{
			if (rr == 0)
			{
				ACE_DEBUG ((LM_DEBUG, "select on UDP reader timed out, perhaps this is a stale connection, closing down\n"));
			}
			else
			{
				ACE_DEBUG ((LM_DEBUG, "this is ok if the thread is closing down\n"));
			}

			_local_acceptor.close ();
			dgram_socket.close();

			AskForEnd();
			goto DgramSocketMsgSkip;
		}

		//
		if (rr >= 1 && set.is_set (_local_acceptor.get_handle()))
		{
			// accept the new TCP service connection.
			ACE_SOCK_Stream stream;
			_local_acceptor.accept (stream, &incoming);
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something on service channel from %s:%d waiting\n", incoming.get_host_addr(), incoming.get_port_number()));

			r = stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got a header %d\n", r));

			if (r >= 0)
			{
				if (hdr.GetLength() == (YARP_MAGIC_NUMBER + 1))
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "received a message to close the thread\n"));
					
					// a legitimate message to close down the thread.
					// sends reply to caller.
					hdr.SetGood ();
					hdr.SetLength (0);
					stream.send_n (&hdr, sizeof(hdr), 0, &timeout);
					stream.close();
					
					_local_acceptor.close ();

					// need to close (properly) also the dgram socket.
					dgram_socket.close ();
		
					AskForEnd();
					goto DgramSocketMsgSkip;
				}
				else
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "got garbage on thread service channel\n"));
				}
			}
			else
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "it seems the stream has been closed\n"));

				stream.close();
				
				_local_acceptor.close ();

				// need to close (properly) also the dgram socket.
				dgram_socket.close ();
	
				AskForEnd();
				goto DgramSocketMsgSkip;
			}

			if (rr == 1)
				goto DgramSocketMsgSkip;
				///continue;
		}

		if (set.is_set (dgram_socket.get_handle()))
		{
			iovec iov[1];
			iov[0].iov_base = _local_buffer;
			iov[0].iov_len = MAX_PACKET;

			r = dgram_socket.recv (iov, 1, incoming, 0);
			_local_buffer_counter = sizeof(MyMessageHeader);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something from %s:%d waiting\n", incoming.get_host_addr(), incoming.get_port_number()));

			// this is supposed to read the header, r must be > 0
			if (r <= 0 || incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing (recv returned %d)\n", r));
				if (incoming.get_host_addr() != _remote_endpoint.getAddressRef().get_host_addr())
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "returning because incoming diffs from remote addr\n"));
				}

				_local_acceptor.close ();
				dgram_socket.close ();

				AskForEnd();
				goto DgramSocketMsgSkip;
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMulti, len = %d\n", hdr.GetLength()));

			len = hdr.GetLength();
			if (len < 0)
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));
				
				_local_acceptor.close ();
				dgram_socket.close ();

				AskForEnd();
				goto DgramSocketMsgSkip;
			}

			if (len >= 0)
			{
				// set the new reply port.
				_remote_endpoint.getAddressRef() = incoming;

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got a header\n"));
				if (_owner != NULL)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? and i am owned\n"));
					_mutex.Wait();
					_extern_buffer = NULL;
					_extern_length = len;

					// ???
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

					ACE_OS::memcpy (_extern_buffer, _local_buffer+_local_buffer_counter, len); 
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
						// this was r too, a bit confusing.
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read more data\n"));

						if (_extern_reply_length == 0)
						{
							YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read 0 len buffer, not waiting\n"));
						}
						else
						{
							ACE_OS::memcpy (_extern_reply_buffer, _local_buffer+_local_buffer_counter, _extern_reply_length);
							_local_buffer_counter += _extern_reply_length;
						}

						_read_more = 0;
						_reply_made.Post();
						_wakeup.Wait();
						_needs_reply = 0;
					}

					_local_buffer_counter = 0;
				}
				else
				{
					// LATER:
					// need to send a reply even if the extern_buffer == NULL
					// otherwise the connection times out.
					ACE_DEBUG ((LM_DEBUG, "THIS IS A BUG ---> pls, tell Giorgio about it\n"));
				}
			}

		}	// if received on UDP socket.

DgramSocketMsgSkip:
		// this is to skip the rest of the message.
		;
	}	// while !finished

	if (_owner != NULL && 
		ACE_OS::strncmp (ownname.c_str(), "explicit_connect", 16) != 0)
	{
		ACE_DEBUG ((LM_INFO, "*** detaching %s from %s [me being %s]\n", 
			ownname.c_str(), 
			_owner->getOwnerName().c_str(), 
			_owner->getOwnerName().c_str()));
	}

	_mutex.Wait ();
	_available = 1;
	_mutex.Post ();
}

void _SocketThreadMultiUdp::End (int dontkill /* = -1 */)
{
	ACE_UNUSED_ARG (dontkill);

	_mutex.Wait ();

	//
	// closing strategy:
	//		- closes the sockets (acceptor if exists and channel proper).
	//		- wait for the thread to complain about failed communication and joins it.
	//
	//
	if (_local_acceptor.get_handle() != ACE_INVALID_HANDLE)
		_local_acceptor.close();

	AskForEnd();

	if (_socket != NULL)
	{
		ACE_SOCK_Dgram& dgram = *_socket;
		if (dgram.get_handle() != ACE_INVALID_HANDLE)
			dgram.close();
		delete &dgram;
		_socket = NULL;
	}

	if (_socket_addr != NULL)
	{
		delete _socket_addr;
		_socket_addr = NULL;
	}

	_mutex.Post ();

	// awful thing. to wakeup the reader...
	_wakeup.Post ();

	YARPBareThread::Join();
}

void _SocketThreadMultiUdp::clearOldAddr (void)
{
	if (_socket_addr != NULL)
	{
		if (_socket != NULL)
		{
			_socket->close ();
			delete _socket;
			_socket = NULL;
		}

		if (_socket_addr != NULL) 
		{
			delete _socket_addr;
			_socket_addr = NULL;
		}
	}
}

void _SocketThreadMultiUdp::printSocketData (void)
{
	if (_socket_addr == NULL)
	{
		ACE_DEBUG ((LM_INFO, "socket is NULL %d ", isAvailable()));
	}
	else
	{
		ACE_DEBUG ((LM_INFO, "%s ", _socket_addr->getName().c_str()));
		YARPUniqueNameSock *a = (YARPUniqueNameSock *)_socket_addr;
		ACE_DEBUG ((LM_INFO, "%s:%d %d ", 
			_remote_endpoint.getAddressRef().get_host_addr(),
			a->getAddressRef().get_port_number(),
			isAvailable() ));
	}
}

int _SocketThreadMultiUdp::getServiceType (void) const
{ 
	const int type = (_socket_addr != NULL) ? _socket_addr->getServiceType() : YARP_NO_SERVICE_AVAILABLE; 
	if (_socket_addr != NULL) 
		ACE_ASSERT (type == YARP_UDP);

	return type;
}

// call it reconnect (recycle the thread).
// the thread shouldn't be running.
int _SocketThreadMultiUdp::reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port)
{
	// remid must be always TCP.
	ACE_ASSERT (remid->getServiceType() == YARP_TCP);

	clearOldAddr ();

	// making sure semaphores are in the right state.
	if (_wakeup.PollingWait())
		ACE_DEBUG ((LM_DEBUG, "****** reuse found _wakeup != 0\n"));

	if (port != 0)
	{
		// this is fine but the actual remote is the TCP here and not the UDP.
		// this gets the right value only after the first recv, no alternatives.

		_remote_endpoint = *remid;

		//
		// LATER: is this needed, shouldn't be already closed.
		_local_acceptor.close ();
		_port = port;

		YARPString myhostip = _remote_endpoint.getAddressRef().get_host_addr();
		ACE_INET_Addr local (port, myhostip.c_str());

		// opens the local socket (for termination).
		if (_local_acceptor.open (local, 1) < 0)
		{
			_available = 1;
			return YARP_FAIL;
		}

		_socket_addr = new YARPUniqueNameSock (*(YARPUniqueNameSock*)socket);
		ACE_ASSERT (_socket_addr != NULL);

		// build the DGRAM socket.
		_socket = new ACE_SOCK_Dgram;
		ACE_ASSERT (_socket != NULL);
		
		ACE_SOCK_Dgram& dgram = *_socket;
		
		dgram.open (((YARPUniqueNameSock&)*_socket_addr).getAddressRef(), ACE_PROTOCOL_FAMILY_INET, 0, 1);	// reuse addr enabled
		YARPNetworkObject::setSocketBufSize (dgram, MAX_PACKET);

		if (dgram.get_handle() == ACE_INVALID_HANDLE)
		{
			_available = 1;
			return YARP_FAIL;
		}

		_socket_addr->setRawIdentifier (dgram.get_handle());
		_available = 0;
	}
	else
	{
		_available = 1;
		_socket = NULL;
		_remote_endpoint.invalidate();
		_socket_addr = NULL;

		return YARP_FAIL;
	}

	return YARP_OK;
}


/**
 * A thread for managing an incoming connection in the mcast protocol.
 */
class _SocketThreadMultiMcast : public _SocketThreadMultiBase
{
protected:
	/** the mcast socket. */
	ACE_SOCK_Dgram_Mcast * _socket;
	/** a generic TCP socket used for service communication (control messages). */
	ACE_SOCK_Acceptor _local_acceptor;

public:
	/**
	 * Default constructor.
	 */
	_SocketThreadMultiMcast (void) : _SocketThreadMultiBase ()
	{
		_socket = NULL;
	}

	/**
	 * Destructor.
	 */
	virtual ~_SocketThreadMultiMcast () {}

	/**
	 * Asks the termination of the current thread.
	 * @param dontkill is not used.
	 */
	virtual void End (int dontkill = -1);

	/**
	 * The thread body.
	 */
	virtual void Body (void);

	/**
	 * Cleans internal data structures. Used internally by reuse() to clean 
	 * addressing information.
	 */
	virtual void clearOldAddr (void);

	/**
	 * Prints the socket name and service type. To be used for debug.
	 */
	void printSocketData (void);

	/**
	 * Gets the protocol type.
	 * @return the protocol type if successful, YARP_NO_SERVICE_AVAILABLE otherwise.
	 */
	virtual int getServiceType (void) const;

	/**
	 * Initializes a thread instance. In spite of its name, this method is called both
	 * when starting up a new thread or to recycle an old one.
	 * @param remid is the descriptor of the incoming connection. It is always TCP. It 
	 * contains also the interface name for MCAST communication.
	 * @param socket is the descriptor of the local connection, and relative protocol.
	 * It contains the port number assigned from the pool, the name of the remote connection,
	 * and the protocol type (UDP, TCP, etc.).
	 * @param port is the port number (as requested from the pool).
	 * @return YARP_OK if successful.
	 */
	virtual int reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port);
};

///
/// IMPLEMENTATION.
///
void _SocketThreadMultiMcast::Body (void)
{
#if defined(__QNX6__) || defined(__LINUX__)
	signal (SIGPIPE, SIG_IGN);
#endif

	_mutex.Wait();

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;
	
	// to store the thread remote peer name and recover it
	// before termination to print the quit message (see code below).
	YARPString ownname = getRemoteName();
	_mutex.Post();

	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ACE_INET_Addr incoming;
	
	ACE_OS::memset (_local_buffer, 0, MAX_PACKET);
	_local_buffer_counter = 0;

	ACE_ASSERT (_socket != NULL);
	ACE_SOCK_Dgram_Mcast& mcast_socket = *((ACE_SOCK_Dgram_Mcast *)_socket);

	//
	while (!IsTerminated())
	{
		YARPUniqueNameSock& addr = *((YARPUniqueNameSock *)_socket_addr);
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread of group %s:%d waiting\n", addr.getAddressRef().get_host_addr(), addr.getAddressRef().get_port_number()));

		MyMessageHeader hdr;
		hdr.SetBad();
		int r = YARP_FAIL;

		ACE_Handle_Set set;
		set.reset ();
		set.set_bit (_local_acceptor.get_handle());
		set.set_bit (mcast_socket.get_handle());
		int max = ((int)_local_acceptor.get_handle() > (int)mcast_socket.get_handle()) 
			? (int)_local_acceptor.get_handle() : (int)mcast_socket.get_handle();

		int rr = ACE_OS::select (max + 1, set, 0, 0, NULL); //&long_timeout);

		if (rr <= 0)
		{
			if (rr == 0)
			{
				ACE_DEBUG ((LM_DEBUG, "select on MCAST reader timed out, perhaps this is a stale connection, closing down\n"));
			}
			else
			{
				ACE_DEBUG ((LM_DEBUG, "this is ok if the thread is closing down\n"));
			}

			_local_acceptor.close ();
			mcast_socket.close ();
			
			AskForEnd();

			goto McastSocketMsgSkip;
		}
		
		//
		if (rr >= 1 && set.is_set (_local_acceptor.get_handle()))
		{
			// accept the new TCP service connection.
			ACE_SOCK_Stream stream;
			_local_acceptor.accept (stream, &incoming);
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? got something on service channel from %s:%d waiting\n", incoming.get_host_addr(), incoming.get_port_number()));

			r = stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);

			if (r >= 0)
			{
				if (hdr.GetLength() == (YARP_MAGIC_NUMBER + 1))
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "received a message to close the thread\n"));
					// a legitimate message to close down the thread.
					// sends reply to caller.
					hdr.SetGood ();
					hdr.SetLength (0);
					stream.send_n (&hdr, sizeof(hdr), 0, &timeout);
					stream.close();
					
					_local_acceptor.close ();

					// need to close (properly) also the mcast socket.
					mcast_socket.close ();
					
					AskForEnd();

					goto McastSocketMsgSkip;
				}
				else
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "received garbage on thread service channel\n"));
				}
			}
			else
			{
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "it looks like the stream has been closed\n"));

				stream.close();
				
				_local_acceptor.close ();

				// need to close (properly) also the mcast socket.
				mcast_socket.close ();
				
				AskForEnd();

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

				// close down sockets.
				_local_acceptor.close ();
				mcast_socket.close ();
				
				AskForEnd();
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

				// close down sockets.
				_local_acceptor.close ();
				mcast_socket.close ();
				
				AskForEnd();
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

					// shouldn't be much different from the real recv.
					ACE_OS::memcpy (_extern_buffer, _local_buffer + _local_buffer_counter, len); 
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
						// this was r too, a bit confusing.
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read more data\n"));

						if (_extern_reply_length != 0)
						{
							ACE_OS::memcpy (_extern_reply_buffer, _local_buffer + _local_buffer_counter, _extern_reply_length);
							_local_buffer_counter += _extern_reply_length;
						}

						_read_more = 0;
						_reply_made.Post();
						_wakeup.Wait();
						_needs_reply = 0;
					}

					// no reply here.

				} // if _extern_buf != NULL
			
			} // if len > 0

		} // if is_set (MCAST)...

McastSocketMsgSkip:
		;
	} // while !finished

	// returning because the thread is closing down.

	if (_owner != NULL && 
		ACE_OS::strncmp (ownname.c_str(), "explicit_connect", 16) != 0)
	{
		ACE_DEBUG ((LM_INFO, "*** detaching %s from %s [me being %s]\n", 
			ownname.c_str(), 
			_owner->getOwnerName().c_str(), 
			_owner->getOwnerName().c_str()));
	}

	_mutex.Wait ();
	_available = 1;
	_mutex.Post ();
}


void _SocketThreadMultiMcast::End (int dontkill /* = -1 */)
{
	ACE_UNUSED_ARG (dontkill);

	_mutex.Wait ();

	if (_local_acceptor.get_handle() != ACE_INVALID_HANDLE)
		_local_acceptor.close();

	AskForEnd ();

	if (_socket != NULL)
	{
		ACE_SOCK_Dgram_Mcast& mcast = *_socket;
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

	_mutex.Post ();

	// awful thing. to wakeup the reader...
	_wakeup.Post ();

	YARPBareThread::Join();
}

void _SocketThreadMultiMcast::clearOldAddr (void)
{
	if (_socket_addr != NULL)
	{
		if (_socket != NULL)
		{
			_socket->close ();
			delete _socket;
			_socket = NULL;
		}

		if (_socket_addr != NULL) 
		{
			delete _socket_addr;
			_socket_addr = NULL;
		}
	}
}

void _SocketThreadMultiMcast::printSocketData (void)
{
	if (_socket_addr == NULL)
	{
		ACE_DEBUG ((LM_INFO, "socket is NULL %d ", isAvailable()));
	}
	else
	{
		ACE_DEBUG ((LM_INFO, "%s ", _socket_addr->getName().c_str()));
		YARPUniqueNameSock *a = (YARPUniqueNameSock *)_socket_addr;
		ACE_DEBUG ((LM_INFO, "%s:%d %d ", 
			_remote_endpoint.getAddressRef().get_host_addr(),
			a->getAddressRef().get_port_number(),
			isAvailable() ));
	}
}

int _SocketThreadMultiMcast::getServiceType (void) const
{ 
	const int type = (_socket_addr != NULL) ? _socket_addr->getServiceType() : YARP_NO_SERVICE_AVAILABLE; 
	if (_socket_addr != NULL) 
		ACE_ASSERT (type == YARP_MCAST);

	return type;
}

// call it reconnect (recycle the thread).
// the thread shouldn't be running.
int _SocketThreadMultiMcast::reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port)
{
	// remid must be always TCP.
	ACE_ASSERT (remid->getServiceType() == YARP_TCP);

	clearOldAddr ();

	// making sure semaphores are in the right state.
	if (_wakeup.PollingWait())
		ACE_DEBUG ((LM_DEBUG, "****** reuse found _wakeup != 0\n"));

	if (port != 0)
	{
		// this is fine but the actual remote is the TCP here and not the UDP.
		// this gets the right value only after the first recv, no alternatives.

		_remote_endpoint = *remid;

		//
		// LATER: is this needed, shouldn't be already closed.
		_local_acceptor.close ();
		_port = port;

		YARPString myhostip = _remote_endpoint.getAddressRef().get_host_addr();
		ACE_INET_Addr local (port, myhostip.c_str());

		// opens the local socket (for termination).
		if (_local_acceptor.open (local, 1))
		{
			_available = 1;
			return YARP_FAIL;
		}

		_socket_addr = new YARPUniqueNameSock (*(YARPUniqueNameSock*)socket);
		ACE_ASSERT (_socket_addr != NULL);

		_socket = new ACE_SOCK_Dgram_Mcast;
		ACE_ASSERT (_socket != NULL);

		ACE_SOCK_Dgram_Mcast& mcast = *_socket;

		if (_remote_endpoint.getInterfaceName() != YARPString("default")) 
		{
			mcast.open (((YARPUniqueNameSock&)*_socket_addr).getAddressRef(), _remote_endpoint.getInterfaceName().c_str(), 1);
		} 
		else 
		{
			mcast.open (((YARPUniqueNameSock&)*_socket_addr).getAddressRef(), NULL, 1);
		}

		YARPNetworkObject::setSocketBufSize (mcast, MAX_PACKET);
		if (_remote_endpoint.getInterfaceName() != YARPString("default")) 
		{
			mcast.join (((YARPUniqueNameSock&)*_socket_addr).getAddressRef(), 1, _remote_endpoint.getInterfaceName().c_str());
		} 
		else 
		{
			mcast.join (((YARPUniqueNameSock&)*_socket_addr).getAddressRef(), 1, NULL);
		}

		if (mcast.get_handle() == ACE_INVALID_HANDLE)
		{
			_available = 1;
			return YARP_FAIL;
		}

		_socket_addr->setRawIdentifier (mcast.get_handle());
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


/**
 * A thread for managing an incoming connection in the shmem protocol.
 */
class _SocketThreadMultiShmem : public _SocketThreadMultiBase
{
protected:
	/** the acceptor of the shmem connection */
	ACE_MEM_Acceptor * _socket;
	/** ptr to the stream of the SHMEM connection. */
	ACE_MEM_Stream *_mstream;

public:
	/**
	 * Default constructor.
	 */
	_SocketThreadMultiShmem (void) : _SocketThreadMultiBase ()
	{
		_socket = NULL;
		_mstream = NULL;
	}

	/**
	 * Destructor.
	 */
	virtual ~_SocketThreadMultiShmem () {}

	/**
	 * Asks the termination of the current thread.
	 * @param dontkill is not used.
	 */
	virtual void End (int dontkill = -1);

	/**
	 * The thread body, not to be called directly :)
	 */
	virtual void Body (void);

	/**
	 * Cleans internal data structures. Used internally by reuse() to clean 
	 * addressing information.
	 */
	virtual void clearOldAddr (void);

	/**
	 * Closes a SHMEM connection.
	 * This specific method is used to ask termination of a SHMEM connection.
	 */
	void closeMemStream (void);

	/**
	 * Prints the socket name and service type. To be used for debug.
	 */
	void printSocketData (void);

	/**
	 * Gets the protocol type.
	 * @return the protocol type if successful, YARP_NO_SERVICE_AVAILABLE otherwise.
	 */
	virtual int getServiceType (void) const;

	/**
	 * Initializes a thread instance. In spite of its name, this method is called both
	 * when starting up a new thread or to recycle an old one.
	 * @param remid is the descriptor of the incoming connection. It is always TCP. It 
	 * contains also the interface name for MCAST communication.
	 * @param socket is the descriptor of the local connection, and relative protocol.
	 * It contains the port number assigned from the pool, the name of the remote connection,
	 * and the protocol type (UDP, TCP, etc.).
	 * @param port is the port number (as requested from the pool).
	 * @return YARP_OK if successful.
	 */
	virtual int reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port);
};

///
/// IMPLEMENTATION.
///
void _SocketThreadMultiShmem::Body (void)
{
#if defined(__QNX6__) || defined(__LINUX__)
	signal (SIGPIPE, SIG_IGN);
#endif

	_mutex.Wait();

	_extern_buffer = NULL;
	_extern_length = 0;
	_waiting = 0;
	_needs_reply = 0;
	_read_more = 0;
	_reply_preamble = 0;
	
	// to store the thread remote peer name and recover it
	// before termination to print the quit message (see code below).
	YARPString ownname = getRemoteName();
	_mutex.Post();

	int r = YARP_FAIL;
	int was_preamble = 0;
	int len = 0;	

	char bufack[] = "acknowledged";
	char *buf3 = bufack;

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread about to accept SHMEM on port %d (%s)\n", ((YARPUniqueNameMem *)_socket_addr)->getAddressRef().get_port_number(), ((YARPUniqueNameMem *)_socket_addr)->getAddressRef().get_host_addr()));
	ACE_MEM_Acceptor& a = *((ACE_MEM_Acceptor *)_socket);
	ACE_MEM_Stream stream;
	a.accept (stream);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread accepted a new stream\n"));

	_mutex.Wait();
	_mstream = &stream;
	_mutex.Post();

	while (!IsTerminated())
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? listener thread SHMEM of remote port %s:%d waiting\n", _remote_endpoint.getAddressRef().get_host_addr(), _remote_endpoint.getAddressRef().get_port_number()));

		MyMessageHeader hdr;

		r = 0;
		double now = YARPTime::GetTimeAsSeconds();
		double prev = now - 1000;
		while (r == 0 && (now-prev > YARP_SHORT_SOCK_TIMEOUT/2.0)) 
		{
			// repeat loop so long as it is willing to block.
			ACE_Time_Value timeout (YARP_SHORT_SOCK_TIMEOUT, 0);
			prev = now;
			r = stream.recv (&hdr, sizeof(hdr), &timeout);
			now = YARPTime::GetTimeAsSeconds();
			if (r == -1 && errno == ETIME && !IsTerminated()) 
			{
				r = 0;  
				prev = now - 1000;
			}
		}
		//r = stream.recv_n (&hdr, sizeof(hdr), 0);
		
		// this is supposed to read the header, r must be > 0
		if (r <= 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing (recv returned %d)\n", r));

			AskForEnd();
			goto ShmemSocketMsgSkip;
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? received header _SocketThreadMulti, len = %d\n", hdr.GetLength()));

		len = hdr.GetLength();
		if (len < 0)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "{{}} Corrupt/empty header received\n"));
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? closing thread\n"));
			
			AskForEnd();
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

				// ???
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
				if (r <= 0)
				{
					ACE_OS::memset (_extern_buffer, 0, _extern_length);
					AskForEnd();
				}

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
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to read more data\n"));

					if (_extern_reply_length == 0)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? read 0 len buffer\n"));
						rr = stream.recv_n (_extern_reply_buffer, 0, 0);
					}
					else
					{
						rr = stream.recv_n (_extern_reply_buffer, _extern_reply_length, 0); 
						if (rr <= 0) AskForEnd ();
					}

					_extern_reply_length = rr;
					_read_more = 0;
					_reply_made.Post();
					_wakeup.Wait();
					_needs_reply = 0;
				}

				was_preamble = 0;

				YARP_DBG(THIS_DBG) ((LM_DEBUG, "??? about to go into sending reply\n"));

#ifdef DONT_WAIT_UP
				if (getRequireAck())
#endif
				do
				{
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
		// this is to skip the rest of the message.
		;
	}	// while !finished

	_mutex.Wait();
	_mstream->close ();
	_mstream = NULL;
	_mutex.Post();

	if (_owner != NULL && 
		ACE_OS::strncmp (ownname.c_str(), "explicit_connect", 16) != 0)
	{
		ACE_DEBUG ((LM_INFO, "*** detaching %s from %s [me being %s]\n", 
			ownname.c_str(), 
			_owner->getOwnerName().c_str(), 
			_owner->getOwnerName().c_str()));
	}

	_mutex.Wait ();
	_available = 1;
	_mutex.Post ();

}

void _SocketThreadMultiShmem::End (int dontkill /* = -1 */)
{
	ACE_UNUSED_ARG (dontkill);

	_mutex.Wait ();

	AskForEnd();

	// this closes the stream and hopefully forces the thread to close too.
	closeMemStream();

	if (_socket != NULL)
	{
		ACE_MEM_Acceptor& a = *_socket;
		if (a.get_handle() != ACE_INVALID_HANDLE)
		{
			a.close();
		}
		delete &a;
		_socket = NULL;
	}
	
	if (_socket_addr != NULL)
	{
		delete _socket_addr;
		_socket_addr = NULL;
	}

	_mutex.Post ();

	// awful thing. to wakeup the reader...
	_wakeup.Post ();

	YARPBareThread::Join();
}

void _SocketThreadMultiShmem::clearOldAddr (void)
{
	if (_socket_addr != NULL)
	{
		if (_socket != NULL)
		{
			_socket->close ();
			delete _socket;
			_socket = NULL;
		}

		if (_socket_addr != NULL) 
		{
			delete _socket_addr;
			_socket_addr = NULL;
		}
	}
}

void _SocketThreadMultiShmem::closeMemStream (void)
{
	if (_mstream != NULL) _mstream->close();
}

void _SocketThreadMultiShmem::printSocketData (void)
{
	if (_socket_addr == NULL)
	{
		ACE_DEBUG ((LM_INFO, "socket is NULL %d ", isAvailable()));
	}
	else
	{
		YARPUniqueNameMem *a = (YARPUniqueNameMem *)_socket_addr;
		ACE_DEBUG ((LM_INFO, "%s:%d %d ", 
			_remote_endpoint.getAddressRef().get_host_addr(),
			a->getAddressRef().get_local_addr().get_port_number(),
			isAvailable() ));
	}
}

int _SocketThreadMultiShmem::getServiceType (void) const
{ 
	const int type = (_socket_addr != NULL) ? _socket_addr->getServiceType() : YARP_NO_SERVICE_AVAILABLE; 
	if (_socket_addr != NULL) 
		ACE_ASSERT (type == YARP_SHMEM);

	return type;
}

// call it reconnect (recycle the thread).
// the thread shouldn't be running.
int _SocketThreadMultiShmem::reuse(const YARPUniqueNameSock* remid, const YARPUniqueNameID* socket, int port)
{
	// remid must be always TCP.
	ACE_ASSERT (remid->getServiceType() == YARP_TCP);

	clearOldAddr ();

	// making sure semaphores are in the right state.
	if (_wakeup.PollingWait())
		ACE_DEBUG ((LM_DEBUG, "****** reuse found _wakeup != 0\n"));

	if (port != 0)
	{
		// this is fine but the actual remote is the TCP here and not the UDP.
		// this gets the right value only after the first recv, no alternatives.

		_remote_endpoint = *remid;
		_port = port;

		_socket_addr = new YARPUniqueNameMem (*(YARPUniqueNameMem*)socket);
		((YARPUniqueNameMem*)_socket_addr)->getAddressRef().set(port); // just in case 

		ACE_ASSERT (_socket_addr != NULL);

		_socket = new ACE_MEM_Acceptor (((YARPUniqueNameMem&)*_socket_addr).getAddressRef(), 1);
		ACE_ASSERT (_socket != NULL);

		// the size of the SHMEM buffer and the delivery strategy.
		_socket->init_buffer_size (2 * MAX_SHMEM_BUFFER);
		_socket->preferred_strategy (ACE_MEM_IO::Reactive);

		_socket_addr->setRawIdentifier (((ACE_MEM_Acceptor *)_socket)->get_handle());
		if (_socket_addr->getRawIdentifier() == ACE_INVALID_HANDLE)
		{
			_available = 1;
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




///
/// METHODS IMPLEMENTATION
///
///

// among other things, should correctly terminate threads.
_SocketThreadListMulti::~_SocketThreadListMulti (void)
{
	if (_ports != NULL) delete[] _ports;
	_ports = NULL;
	_number_o_ports = 0;
	_last_assigned = -1;
	_initialized = 0;
}


//
// creates the acceptor socket and listens to port.
// simply prepares the socket.
ACE_HANDLE _SocketThreadListMulti::connect (const YARPUniqueNameSock& id)
{
	_local_addr = ((YARPUniqueNameSock&)id).getAddressRef();

	// for YARP2 flexibility, don't select a particular IP
#ifdef USE_YARP2
	ACE_INET_Addr flex;
	flex.set_port_number(_local_addr.get_port_number());
	_local_addr.set(flex);
#endif
	// YARP2 change finished

	_acceptor_socket.open (_local_addr, 1);	// reuse addr enabled.

	if (_acceptor_socket.get_handle() == ACE_INVALID_HANDLE)
	{
	  ACE_DEBUG ((LM_WARNING, "*** FAILED to open tcp connection (%s:%d)\n", _local_addr.get_host_addr(), _local_addr.get_port_number()));
	  ACE_DEBUG ((LM_WARNING, "*** --- This can happen if the requested socket-port is already in use.\n", _local_addr.get_host_addr(), _local_addr.get_port_number()));
	  ACE_DEBUG ((LM_WARNING, "*** --- This, in turn, can happen if the name server is restarted...\n", _local_addr.get_host_addr(), _local_addr.get_port_number()));
	  ACE_DEBUG ((LM_WARNING, "*** --- without first stopping any processes that talked to the old server.\n", _local_addr.get_host_addr(), _local_addr.get_port_number()));
	  return ACE_INVALID_HANDLE;

		// and the thread is not started.
	}
	
	// closes down any still open thread (just in case?).
	if (_initialized) closeAll ();

	_own_name = id.getName();
	_initialized = 1;

	Begin();

	return _acceptor_socket.get_handle ();
}

//
//
//
int _SocketThreadListMulti::printThreadList (void)
{
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
	it_avail.go_head();

	int i = 0;
	for (; !it_avail.done(); it_avail++)
	{
		(*it_avail)->waitOnMutex ();

		ACE_DEBUG ((LM_INFO, "***   %d: ", i));
		(*it_avail)->printSocketData ();
		ACE_DEBUG ((LM_INFO, "\n"));
		i++;

		(*it_avail)->postToMutex ();
	}

	return YARP_OK;
}

//
// The list management is a bit odd now. Threads are typed and can't be
// recycled directly. A wise strategy since creating a new thread has
// negligible cost would be to destroy and re-create whenever a new one
// is needed.
//
//
void _SocketThreadListMulti::addSocket (void)
{
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Hello good evening and welcome to addSocket\n"));

	// need to keep calling this to get next incoming socket
	ACE_ASSERT (_initialized != 0);

	YARPScheduler::yield();
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "7777777 pre accept %d\n", errno));

	ACE_SOCK_Stream *stream = new ACE_SOCK_Stream;
	ACE_ASSERT (stream != NULL);
	ACE_INET_Addr incoming;
	int port_number = 0;
	MyMessageHeader hdr;

	hdr.SetBad();

	int ra = -1;
	do
	{

		ra = _acceptor_socket.accept (*stream, &incoming);

		if (ra < 0)
		{
			ACE_DEBUG ((LM_ERROR, "*** acceptor_socket got garbage, trying again\n"));
			YARPTime::DelayInSeconds(1);
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "got something on acceptor socket...\n"));

		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
		ra = stream->recv_n (&hdr, sizeof(hdr), 0, &timeout);

		if (ra <= 0)
		{
			ACE_DEBUG ((LM_ERROR, "*** something weird happened while receiving header, trying again\n"));
			ra = -1;
		}
	}
	while (ra == -1);

	// MAGIC_NUMBER	-> UDP
	// MAGIC_NUMBER+1	-> MCAST
	// MAGIC_NUMBER+2	-> SHMEM
	// MAGIC_NUMBER+3  -> TCP
	//
	int magic = ((hdr.GetLength()-YARP_MAGIC_NUMBER) & 15) + YARP_MAGIC_NUMBER;
	const int flags = ((hdr.GetLength()-YARP_MAGIC_NUMBER) & (255-15));
	const int req_ack = flags!=0;

	// acquire the incoming connection name.
	// this information is later used to do per-connection detach, useful also to remove
	// stale connections.
	//
	NetInt32 name_len = 0;
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
	ra = stream->recv_n (&name_len, sizeof(NetInt32), 0, &timeout);
	if (ra <= 0)
	{
		ACE_DEBUG ((LM_DEBUG, "*** something weird happened while receiving the remote name length, bailing out\n"));
		stream->close ();
		delete stream;
		return;
	}

	char *tmpbuf = new char[name_len+1];
	ACE_ASSERT (tmpbuf != 0);
	tmpbuf[name_len] = 0;

	ra = stream->recv_n (tmpbuf, name_len, 0, &timeout);
	if (ra <= 0)
	{
		ACE_DEBUG ((LM_DEBUG, "*** something weird happened while receiving the remote name, bailing out\n"));
		stream->close ();
		delete stream;
		delete[] tmpbuf;
		return;
	}

	YARPString buf(tmpbuf, name_len);
	delete[] tmpbuf;

	if (magic == YARP_MAGIC_NUMBER)
	{
		_listProtect.Wait();

		// checks whether <incoming> already tried a connection
		//		and it is still connected.
		YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
		it_avail.go_head();

		// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept errno (%x), going to determine port number\n", errno));

		// get a new available port number associated to this IP.
		// recycle thread and port # if possible.
		int reusing = 0;
		for (; !it_avail.done(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber ();
				if ((*it_avail)->getServiceType() != YARP_UDP)
				{
					delete (*it_avail);
					*it_avail = new _SocketThreadMultiUdp();
					ACE_ASSERT (*it_avail != NULL);
				}
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		ACE_DEBUG ((LM_INFO, "*** accepting from %s UDP:%s:%d assigned port %d\n", buf.c_str(), incoming.get_host_addr(), incoming.get_port_number(), port_number));

		if (port_number == 0)
		{
			printThreadList ();

			// failed connection, for any reason (likely too many connections).
			ACE_DEBUG ((LM_ERROR, "*** error, can't get a port, too many connections\n"));
			hdr.SetBad ();
			stream->send_n (&hdr, sizeof(hdr), 0);
		}
		else
		{
			if (it_avail.done())
			{
				_list.push_back(new _SocketThreadMultiUdp());
				it_avail.go_tail();
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "1777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameSock temp_sock(YARP_UDP, ACE_INET_Addr(port_number));
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
			}
			else
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameSock temp_sock(YARP_UDP, ACE_INET_Addr(port_number));
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				(*it_avail)->CleanState ();
				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
			}

			// send reply to incoming socket.
			// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			stream->send_n (&hdr, sizeof(hdr), 0);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}

		_listProtect.Post();

		stream->close ();
		delete stream;
	}
	else
	if (magic == (YARP_MAGIC_NUMBER + 1))
	{
		// ask an MCAST connection.

		// gets the group and port number of the incoming connection.
		// gets 4 bytes w/ ip addr and 2 more bytes w/ port number.
		unsigned char smallbuf[6];
		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);
		int r = stream->recv_n ((void *)smallbuf, 6, 0, &timeout);

		if (r <= 0)
		{
			ACE_DEBUG ((LM_DEBUG, "777777 connection failed\n"));

			// didn't get the ip and port #.
			hdr.SetBad ();
			stream->send_n (&hdr, sizeof(hdr), 0);
			stream->close ();
			delete stream;
			return;
		}

		char ip[24];
		int port = smallbuf[4] * 256 + smallbuf[5];
		ACE_OS::sprintf (ip, "%u.%u.%u.%u:%u\0", smallbuf[0], smallbuf[1], smallbuf[2], smallbuf[3], port);
		ACE_INET_Addr group (ip);

		_listProtect.Wait();

		// checks whether <incoming> already tried a connection
		//		and it is still connected.
		YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
		it_avail.go_head();

		// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept %d, going to determine port number\n", errno));

		// get a new available port number associated to this IP.
		// recycle thread and port # if possible.
		int reusing = 0;
		for (; !it_avail.done(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber (); 
				if ((*it_avail)->getServiceType() != YARP_MCAST)
				{
					delete (*it_avail);
					*it_avail = new _SocketThreadMultiMcast();
					ACE_ASSERT (*it_avail != NULL);
				}
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		ACE_DEBUG ((LM_INFO, "*** accepting from %s MCAST:%s:%d assigned port %d\n", buf.c_str(), incoming.get_host_addr(), incoming.get_port_number(), port_number));

		if (port_number == 0)
		{
			printThreadList ();

			//
			ACE_DEBUG ((LM_ERROR, "*** error, can't get a port, too many connections\n"));
			hdr.SetBad ();
			stream->send_n (&hdr, sizeof(hdr), 0);
		}
		else
		{
			if (it_avail.done())
			{
				_list.push_back(new _SocketThreadMultiMcast());
				it_avail.go_tail();
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "2777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameSock temp_sock(YARP_MCAST, group);
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
			}
			else
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameSock temp_sock(YARP_MCAST, group);
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				(*it_avail)->CleanState ();
				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
			}

			// send reply to incoming socket.
			// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			stream->send_n (&hdr, sizeof(hdr), 0);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}

		_listProtect.Post();
		stream->close();
		delete stream;
	}
	else
	if (magic == (YARP_MAGIC_NUMBER + 2))
	{
		// ask a SHMEM connection.

		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

		_listProtect.Wait();

		// checks whether <incoming> already tried a connection
		//		and it is still connected.
		YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
		it_avail.go_head();

		// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept %d, going to determine port number\n", errno));

		// get a new available port number associated to this IP.
		// recycle thread and port # if possible.
		int reusing = 0;
		for (; !it_avail.done(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber (); 
				if ((*it_avail)->getServiceType() != YARP_SHMEM)
				{
					delete (*it_avail);
					*it_avail = new _SocketThreadMultiShmem();
					ACE_ASSERT (*it_avail != NULL);
				}
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		ACE_DEBUG ((LM_INFO, "*** accepting from %s SHMEM:%s:%d assigned port %d\n", buf.c_str(), incoming.get_host_addr(), incoming.get_port_number(), port_number));

		if (port_number == 0)
		{
			printThreadList ();

			// failed connection, for any reason (likely too many connections).
			ACE_DEBUG ((LM_ERROR, "*** error, can't get a port, too many connections\n"));
			hdr.SetBad ();
			stream->send_n (&hdr, sizeof(hdr), 0);
		}
		else
		{
			if (it_avail.done())
			{
				_list.push_back(new _SocketThreadMultiShmem());
				it_avail.go_tail();
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "3777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameMem temp_sock(YARP_SHMEM, port_number);
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				// need a port number for SHMEM? or can I recycle the same as UDP, 'cause SHMEM messaging is TCP?
				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "3777777 Begin %d new thread ready to go on port %d\n", __LINE__, port_number));
			}
			else
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameMem temp_sock(YARP_SHMEM, port_number);
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				(*it_avail)->CleanState ();
				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "3777777 Begin %d new thread ready to go on port %d\n", __LINE__, port_number));
			}

			// send reply to incoming socket.
			// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			stream->send_n (&hdr, sizeof(hdr), 0);

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}

		_listProtect.Post();
		stream->close ();
		delete stream;
	}
	else
	if (magic == (YARP_MAGIC_NUMBER + 3))
	{
		// 
		// LATER: optimize port # usage. the TCP connection is actually keeping a 
		// fictious port use although it's not really used since the SOCK_Stream is 
		// already connected. Optimize by freeing the port number.
		//
		_listProtect.Wait();
		YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);

		// check accept return value.
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post accept %d, going to determine port number\n", errno));

		// get a new available port number associated to this IP.
		// recycle thread and port # if possible.
		int reusing = 0;
		for (; !it_avail.done(); it_avail++)
		{
			if ((*it_avail)->isAvailable ())
			{
				reusing = 1;
				port_number = (*it_avail)->getOldPortNumber (); 
				if ((*it_avail)->getServiceType() != YARP_TCP)
				{
					delete (*it_avail);
					*it_avail = new _SocketThreadMultiTcp();
					ACE_ASSERT (*it_avail != NULL);
				}
				break;
			}
		}

		if (!reusing)
			port_number = getNewPortNumberFromPool ();

		if (ACE_OS::strncmp(buf.c_str(), "explicit_connect", 16) != 0)
			ACE_DEBUG ((LM_INFO, "*** accepting from %s TCP:%s:%d assigned port %d\n", buf.c_str(), incoming.get_host_addr(), incoming.get_port_number(), port_number));

		if (port_number == 0)
		{
			printThreadList ();

			// failed connection, for any reason (likely too many connections).
			ACE_DEBUG ((LM_ERROR, "*** error, can't get a port, too many connections\n"));
			hdr.SetBad ();
			stream->send_n (&hdr, sizeof(hdr), 0);
			stream->close ();
			delete stream;
		}
		else
		{
			if (it_avail.done())
			{
				_list.push_back(new _SocketThreadMultiTcp());
				it_avail.go_tail();
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "4777777 new thread ready to go on port %d\n", port_number));
			(*it_avail)->setAvailable (0);
			(*it_avail)->setOwner (*this);

			// send reply to incoming socket.
			// LATER: to refuse the connection simply send back a SetBad() header.
			hdr.SetGood ();
			hdr.SetLength (port_number);
			stream->send_n (&hdr, sizeof(hdr), 0);

			// passes control of stream to the communication thread.
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 pre postbegin %d\n", errno));
			if (!reusing)
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameSock temp_sock(YARP_TCP, ACE_INET_Addr(port_number));
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				((_SocketThreadMultiTcp *)*it_avail)->setStream (stream);
				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
			}
			else
			{
				YARPUniqueNameSock temp(YARP_TCP); //, incoming);
				YARPUniqueNameSock temp_sock(YARP_TCP, ACE_INET_Addr(port_number));
				temp.setInterfaceName (_interface);
				temp.getAddressRef().set ((u_short)0, _local_addr.get_host_addr());
				temp_sock.setName (buf);

				(*it_avail)->CleanState ();
				((_SocketThreadMultiTcp *)*it_avail)->setStream (stream);
				(*it_avail)->reuse (&temp, &temp_sock, port_number);
				(*it_avail)->setRequireAck(req_ack);
				(*it_avail)->Begin();
			}

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "777777 post postbegin %d\n", errno));
		}

		_listProtect.Post();
	}
	else
	{
		YARP_DBG(THIS_DBG) ((LM_ERROR, "corrupted header received, abort connection attempt, listening\n"));

		stream->close ();
		delete stream;

		AskForEnd();
	}
}

// closes everything.
int _SocketThreadListMulti::closeAll (void)
{
	ACE_ASSERT (_initialized != 0);

	_listProtect.Wait();
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);

	for (; !it_avail.done(); it_avail++)
	{
		// it's a thread, ask for termination.
		(*it_avail)->End ();
	}
	_listProtect.Post();

	// terminates itself.
	End ();

	_acceptor_socket.close ();
	_initialized = 0;

	return 1;
}

// closes a particular thread identified by its socket id. 
//	the ACE_HANDLE is used as index into the list.
int _SocketThreadListMulti::close (ACE_HANDLE reply_id)
{
	ACE_ASSERT (_initialized != 0);

	int result = YARP_FAIL;

	_listProtect.Wait();
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);

	for (; !it_avail.done(); it_avail++)
	{
		if (!((*it_avail)->isAvailable ()))
		{
			if ((*it_avail)->getID () == reply_id)
			{
				(*it_avail)->End ();
				(*it_avail)->setAvailable (1);
				result = YARP_OK;
			}
		}
	}
	_listProtect.Post();

	return result;
}

// closes a particular connection identified by a symbolic name.
//	the YARPString is used as index into the list.
int _SocketThreadListMulti::closeByName (const YARPString& name)
{
	ACE_ASSERT (_initialized != 0);

	int result = YARP_FAIL;

	_listProtect.Wait();
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);

	for (; !it_avail.done(); it_avail++)
	{
		if (!((*it_avail)->isAvailable ()))
		{
			if (ACE_OS::strcmp((*it_avail)->getRemoteName().c_str(), name.c_str()) == 0)
			{
				(*it_avail)->End ();
				(*it_avail)->setAvailable (1);
				result = YARP_OK;
			}
		}
	}
	_listProtect.Post();

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


// returns the ACE_HANDLE as id for further reply.
//	returns also the number of bytes read.
int _SocketThreadListMulti::read(char *buf, int len, ACE_HANDLE *reply_pid, bool safe)
{
	ACE_HANDLE save_pid = ACE_INVALID_HANDLE;

	int finished = 0;
	int result = YARP_FAIL;

	while (!finished)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Waiting for new data\n"));
		_new_data.Wait();

		// checks for proper initialization. maybe should move the wait for new data
		// at the very beginning in this case?
		ACE_ASSERT (_initialized != 0);

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Got new data\n"));

		if (safe) _listProtect.Wait();

		YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
		for (; !it_avail.done(); it_avail++)
		{
			// WARNING: isAvailable is read here without mut exclusion!
			if (!((*it_avail)->isAvailable()))
			{
				int work = 0, in_len = 0;
				(*it_avail)->waitOnMutex ();		// _mutex.Wait();

				if ((*it_avail)->isWaiting())
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Identified source of new data\n"));

					work = 1;
					(*it_avail)->setWaiting(0);							// = 0;
					(*it_avail)->setExternalBuffer (buf);				///extern_buffer = buf;
					in_len = (*it_avail)->getExternalBufferLength ();	///extern_length;
					(*it_avail)->setExternalBufferLength (len);			///extern_length = len;

					(*it_avail)->setNeedsReply (1);

					(*it_avail)->postToWakeup ();	// wakeup.Post();

					YARP_DBG(THIS_DBG) ((LM_DEBUG, "### Waking up source of new data\n"));
				}
				
				(*it_avail)->postToMutex ();	// mutex.Post();

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

		if (safe) _listProtect.Post();
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

	_listProtect.Wait();

	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
	for (; !it_avail.done(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->isWaiting())
			{
				// called with false because I'm already using the mutex here.
				return read (buf, len, reply_pid, false);
			}
		}
	}

	_listProtect.Post();

	return YARP_OK;
}

int _SocketThreadListMulti::beginReply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING REPLY of %d bytes\n", len));

	_listProtect.Wait();
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
	for (; !it_avail.done(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				if ((*it_avail)->getServiceType () != YARP_MCAST &&
#ifdef DONT_WAIT_UP
				    ((*it_avail)->getRequireAck() || ((*it_avail)->getServiceType () != YARP_TCP && (*it_avail)->getServiceType () != YARP_SHMEM)) &&
#endif
					(*it_avail)->getServiceType () != YARP_UDP)
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
					ACE_OS::memset (buf, 0, len);
				}
			}
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& FINISHED BEGINNING REPLY of %d bytes\n", len));
	_listProtect.Post();

	return YARP_OK;
}

int _SocketThreadListMulti::reply(ACE_HANDLE reply_pid, char *buf, int len)
{
	ACE_ASSERT (_initialized != 0);
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& BEGINNING FINAL REPLY of %d bytes\n", len));

	_listProtect.Wait();
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
	for (; !it_avail.done(); it_avail++)
	{
		if (!((*it_avail)->isAvailable()))
		{
			if ((*it_avail)->getID() == reply_pid)
			{
				if ((*it_avail)->getServiceType() != YARP_MCAST &&
#ifdef DONT_WAIT_UP
				    ((*it_avail)->getRequireAck() || ((*it_avail)->getServiceType () != YARP_TCP && (*it_avail)->getServiceType () != YARP_SHMEM)) &&
#endif
					(*it_avail)->getServiceType () != YARP_UDP)
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
					ACE_OS::memset (buf, 0, len);
					(*it_avail)->postToWakeup ();				///wakeup.Post();
				}
			}
		}
	}

	YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& FINISHED BEGINNING FINAL REPLY of %d bytes\n", len));
	_listProtect.Post();

	return YARP_OK;
}

// this demands exact number of bytes
int _SocketThreadListMulti::receiveMore(ACE_HANDLE reply_pid, char *buf, int len)
{
	int result = YARP_FAIL;

	ACE_ASSERT (_initialized != 0);

	_listProtect.Wait();
	YARPList<_SocketThreadMultiBase *>::iterator it_avail(_list);
	for (; !it_avail.done(); it_avail++)
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
	_listProtect.Post();

	return result;
}



/**
 * A class to hide the management object of the YARPInputSocketMulti.
 */
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

//
// Input socket handling threads.
//
YARPInputSocketMulti::YARPInputSocketMulti (void) : YARPNetworkInputObject ()
{ 
	system_resources = new ISDataMulti;
	ACE_ASSERT (system_resources != NULL);
	
	_socktype = YARP_I_SOCKET;
}

YARPInputSocketMulti::~YARPInputSocketMulti (void)
{
	if (system_resources != NULL)
	{
		///CloseAll ();
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
	// set the ports -1 that is used as pricipal receiving port.
	d._list.setPool (ports+1, number_o_ports-1);
	d._list.setInterfaceName (((YARPUniqueNameSock&)name).getInterfaceName());

	// LATER: requires error handling here.
	return YARP_OK;
}

///
///
///
int YARPInputSocketMulti::CloseAll (void)
{
	return ISDATA(system_resources)._list.closeAll();
}

int YARPInputSocketMulti::Close (ACE_HANDLE reply_id)
{
	return ISDATA(system_resources)._list.close (reply_id);
}

int YARPInputSocketMulti::CloseByName (const YARPString& name)
{
	return ISDATA(system_resources)._list.closeByName(name);
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

int YARPInputSocketMulti::PrintThreadList(void)
{
	return ISDATA(system_resources)._list.printThreadList();
}

/**
 * This is a convenience class that incapsulates the output SHMEM socket and 
 * its associated stream. This is private and the user shouldn't be concerned with.
 */
class OSDataMulti
{
public:
	ACE_INET_Addr _mem_addr;
	ACE_MEM_Connector _connector_socket;
	ACE_MEM_Stream _stream;
	
	ACE_SOCK_Connector _service_socket;

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

	if (identifier != ACE_INVALID_HANDLE)
		Close (YARPUniqueNameSock(YARP_SHMEM, d._remote_addr));

	if (system_resources != NULL)
	{
		delete ((OSDataMulti*)system_resources);
		system_resources = NULL;
	}
}


// closes down a specific connetion (tell the remote thread the connection is going down.).
int YARPOutputSocketMulti::Close (const YARPUniqueNameID& name)
{
	OSDataMulti& d = OSDATA(system_resources);

	ACE_DEBUG ((LM_DEBUG, "Pretending to close all connections to port %d on %s\n", 
		d._remote_addr.get_port_number(), 
		d._remote_addr.get_host_addr()));

	if (name.getServiceType() != YARP_SHMEM)
	{
		ACE_ASSERT (1 == 0);

		// send the header.
		MyMessageHeader hdr;
		hdr.SetGood ();
		hdr.SetLength (YARP_MAGIC_NUMBER + 1);
		
		ACE_SOCK_Stream stream;
		ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

		int r = d._service_socket.connect (stream, d._remote_addr, &timeout);
		if (r < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "cannot connect to remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
			identifier = ACE_INVALID_HANDLE;
			return YARP_FAIL;
		}

		stream.send_n (&hdr, sizeof(hdr), 0);

		// wait response.
		hdr.SetBad ();
		r = stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);
		if (r < 0)
		{
			ACE_DEBUG ((LM_DEBUG, "cannot handshake with remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
			identifier = ACE_INVALID_HANDLE;
			stream.close ();
			return YARP_FAIL;
		}

		stream.close ();
	}
	else
	{
		d._stream.close ();
	}

	identifier = ACE_INVALID_HANDLE;

	return YARP_OK;
}


// this is called only once to prepare the socket and addr.
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

//
// this can be called many many times to ask receivers to join to mcast group.
// name is the remote we're asking to join.
int YARPOutputSocketMulti::Connect (const YARPUniqueNameID& name, const YARPString& own_name)
{

	OSDataMulti& d = OSDATA(system_resources);
	ACE_Time_Value timeout (YARP_SOCK_TIMEOUT, 0);

	// send the header.
	int port_number = 0;
	MyMessageHeader hdr;
	hdr.SetGood ();
	hdr.SetLength (YARP_MAGIC_NUMBER + 2 + 128*name.getRequireAck());

	// verifies it's a new connection.
	ACE_INET_Addr nm = ((YARPUniqueNameSock&)name).getAddressRef();
	ACE_SOCK_Stream stream;

	int r = d._service_socket.connect (stream, nm, &timeout);
	if (r < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "cannot connect to remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		return YARP_FAIL;
	}

	stream.send_n (&hdr, sizeof(hdr), 0);

	// fine, now send the name of the connection.
	NetInt32 name_len = own_name.length();
	stream.send_n (&name_len, sizeof(NetInt32), 0);
	stream.send_n (own_name.c_str(), name_len, 0);

	// wait response.
	hdr.SetBad ();
	r = stream.recv_n (&hdr, sizeof(hdr), 0, &timeout);
	if (r < 0)
	{
		ACE_DEBUG ((LM_ERROR, "*** error, cannot handshake with remote %s:%d\n", nm.get_host_addr(), nm.get_port_number()));
		stream.close ();
		return YARP_FAIL;
	}

	port_number = hdr.GetLength();
	if (port_number == -1)
	{
		// there might be a real -1 port number -> 65535.
		ACE_DEBUG ((LM_ERROR, "*** error, got garbage back from remote %s:%d\n", d._remote_addr.get_host_addr(), d._remote_addr.get_port_number()));
		stream.close ();
		return YARP_FAIL;
	}

	stream.close ();

	// the connect changes the remote port number to the actual assigned channel.
	// should close on this address.
	d._remote_addr.set_port_number (port_number);

	//d._mem_addr.set(d._local_addr);
	//d._mem_addr.set_port_number(port_number);
	
	d._mem_addr.set (port_number, d._local_addr.get_host_addr());

	// at this point the remote should be listening on port_number

	// Sets the delivery strategy.
	d._connector_socket.preferred_strategy (ACE_MEM_IO::Reactive);

	r = d._connector_socket.connect (d._stream, d._mem_addr);

	if (r == -1)
	{
	  ACE_DEBUG ((LM_ERROR, "*** CHECK that your machine's domain name is configured\n"));
	  ACE_DEBUG ((LM_ERROR, "*** A lack of domain name can make the two sides of a connection seem ...\n"));
	  ACE_DEBUG ((LM_ERROR, "*** ... to be on different machines, and prevent shared memory from kicking in\n"));
	  ACE_DEBUG ((LM_DEBUG, "cannot connect shmem socket %s:%d, error code is %d\n", d._mem_addr.get_host_addr(), d._mem_addr.get_port_number(),r));

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

// I'm afraid the reply might end up being costly to streaming communication.
int YARPOutputSocketMulti::SendReceivingReply(char *reply_buffer, int reply_buffer_length)
{
#ifdef DONT_WAIT_UP
  if (getRequireAck()) 
  {
#endif
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
#ifdef DONT_WAIT_UP
  } else {
	ACE_OS::memset (reply_buffer, 0, reply_buffer_length);
	return reply_buffer_length;
  }
#endif
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
