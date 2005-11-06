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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: Port.h,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

#ifndef __PORT_H_INC
#define __PORT_H_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPString.h>
#include <yarp/YARPNameID.h>
#include <yarp/YARPTime.h>

#include "yarp_private/mesh.h"
#include "yarp_private/Headers.h"

#include <yarp/YARPSemaphore.h>
#define Sema YARPSemaphore
#include <yarp/YARPThread.h>
#define Thread YARPThread
#include "yarp_private/Sendable.h"
#include "yarp_private/YARPFragments.h"
#define Fragment Fragments
#include "yarp_private/BlockSender.h"

#include <yarp/YARPNetworkTypes.h>

/**
 * \file Port.h contains the definition of the main part of the
 * port processing. It contains definition for the port and associated
 * threads.
 */

/**
 * Message tags, chosen to be reasonably human readable.
 */
enum
{
	MSG_ID_NULL         = 0,
	MSG_ID_DATA         = 'd',
	MSG_ID_GO           = 'g',
	MSG_ID_ATTACH_ME    = 'm',
	MSG_ID_DETACH       = '!',
	MSG_ID_ACK          = 'y',
	MSG_ID_NACK         = 'n',
	MSG_ID_ATTACH       = '/',
	MSG_ID_DETACH_ALL   = 'k',		// can only be used by SaySelfEnd --- WARNING.
	MSG_ID_DETACH_IN	= '~',
	MSG_ID_DUMP_CONNECTIONS = '*',
	MSG_ID_ERROR        = -1
};

/**
 * This is the OutputTarget: a thread that handles a single output connection.
 * These threads are part of a pool (MeshOf) managed by the Port main thread.
 * the thread receives the data buf through the usual sendable ref/pointer.
 * This class is all pretty much public since it's used as a big container.
 */
class OutputTarget : public BasedLink<OutputTarget>, public YARPBareThread
{
public:
	YARPUniqueNameID *target_pid;

	double check_tick;
	int ticking;
	
	int active;
	int sending;
	int add_header;
	int deactivate;
	int deactivated;
	int protocol_type;
	int require_ack;
	int allow_shmem;

	Sema something_to_send;
	Sema space_available;
	Sema mutex;

	CountedPtr<Sendable> p_sendable;
	
	int msg_type;
	char cmdname[2*YARP_STRING_LEN];

	YARPString network_name;
	YARPString own_name;

	/**
	 * Constructor. Initializes the thread object to some
	 * reasonable defaults.
	 */
	OutputTarget() : something_to_send(0), 
					 space_available(1),
					 mutex(1),
					 require_ack(0),
					 allow_shmem(1),
					 network_name(YARP_DEFAULT_NET),
					 own_name("__null")
    {
		target_pid = NULL;
		add_header = 1;  active = 1; sending = 0; 
		deactivate = 0;  deactivated = 0;
		check_tick = 0;  ticking = 0;
		protocol_type = YARP_NO_SERVICE_AVAILABLE;
		ACE_OS::memset (cmdname, 0, 2*YARP_STRING_LEN);
		msg_type = 0;
    }

	/**
	 * Destructor.
	 */
	virtual ~OutputTarget() 
	{ 
		End(); 
	}

	/**
	 * Asks for thread termination.
	 * @param dontkill is not used.
	 */
	virtual void End (int dontkill = -1);

	/**
	 * The thread body.
	 */
	virtual void Body();
	
	/**
	 * Starts the thread. It takes care of decrementing the
	 * thread mutex on startup (this is required by the thread body).
	 * @param stack_size is the size of the stack of the thread. This value
	 * is sent directly to the thread implementation. @see YARPBareThread.
	 */
	virtual void Begin(int stack_size=0)
	{
		// protect thread code at startup.
		WaitMutex ();
		YARPBareThread::Begin(stack_size);
	}

	/**
	 * Acquires the thread mutex. This is used for regulating
	 * the access to the thread shared data.
	 */
	void WaitMutex() { mutex.Wait(); }

	/**
	 * Releases the thread mutex.
	 */
	void PostMutex() { mutex.Post(); }

	/**
	 * A callback activated whenever a new message is sent.
	 */
	virtual void OnSend() {}

	/**
	 * Passes a new Sendable to the thread (for sending it).
	 * @param nsendable is the pointer to the Sendable object.
	 */
	void Share(Sendable *nsendable)
	{
		WaitMutex();
		if (!sending)
		{
			space_available.Wait();
			p_sendable.Set(nsendable);
			something_to_send.Post();
			sending = 1;
		}
		PostMutex();
	}

	/**
	 * Asks the thread to close itself down.
	 */
	void Deactivate(void)
	{
		WaitMutex();
		deactivate = 1;
		// added to help close connection quickly.
		something_to_send.Post();
		PostMutex();

		// actually terminating the thread, then join.
		Join();
	}

	/**
	 * Connects an MCAST port. This is a special extra commands for MCAST protocol.
	 * It is required to forward clients MCAST request to join/release the group.
	 * @param name is the symbolic name to connect to.
	 * @return YARP_OK on success.
	 */
	int ConnectMcast (const char *name);

	/**
	 * Deactivates an MCAST connection. This is a special extra commands for MCAST protocol.
	 * It is required to forward clients MCAST request to join/release the group.
	 * @param name is the symbolic name to break the connection from.
	 * @return YARP_OK on success.
	 */
	int DeactivateMcast (const char *name);

	/**
	 * Deactivates all the MCAST connections. This is a special extra commands for 
	 * the MCAST protocol.
	 * @return YARP_OK on success.
	 */
	int DeactivateMcastAll (void);

	/**
	 * Sets the require acknowledge flag. This tells the output thread to expect 
	 * replies to messages.
	 * @param flag is 1 if the replies are needed, 0 otherwise.
	 */
	void SetRequireAck (int flag) { require_ack = flag; }

	/**
	 * Gets the require acknowledge flag. This tests the status of the flag.
	 * @return 1 if the replies are requested, 0 otherwise.
	 */
	int GetRequireAck (void) const { return require_ack; }

	/**
	 * Sets the name of the port owning this thread.
	 * @param s is the YARPString containing the symbolic name the port
	 * used to register to the name server.
	 */
	void SetOwnName (const YARPString& s) { own_name = s; }

	/**
	 * Requires the INET address the thread is using for communicating.
	 * If using sockets the return value is a true INET address, otherwise
	 * a plausible reply is produced. For QNET 0 is returned. This is used
	 * when asking information about the thread and its managed communication
	 * channel.
	 * @return the ACE_INET_Addr object of the current connection.
	 */
	ACE_INET_Addr GetOwnAddress (void)
	{
		if (target_pid != NULL)
			switch (target_pid->getServiceType())
			{
				case YARP_QNET:
					return ACE_INET_Addr((u_short)0);

				case YARP_TCP:
				case YARP_UDP:
				case YARP_MCAST:
					return ((YARPUniqueNameSock *)target_pid)->getAddressRef();

				case YARP_SHMEM:
					return ((YARPUniqueNameSock *)target_pid)->getAddressRef();
			}

		return ACE_INET_Addr((u_short)0);
	}
};

//
//
class Port;

/**
 * This is a thread that takes care of sending data. It waits for a
 * go signal (_ready_to_go), then it activates by scanning the list of
 * output targets, and eventually for all active targets sends the
 * data. This thread is meant to substitute a "select" with mutex/sema and 
 * socket demultiplexing capabilities (that doesn't exist in NT).
 * There's generally a single instance of this object per port. It is
 * created and destroyed from the main port Body().
 */
class _strange_select : public YARPBareThread
{
protected:
	YARPSemaphore _ready_to_go;
	Port *_owner;

public:
	/** 
	 * Constructor.
	 * @param o is the owner of the newly created port.
	 */
	_strange_select (Port *o) : YARPBareThread (), _ready_to_go(0) 
	{
		_owner = o;
		ACE_ASSERT (o != NULL); 
	}

	/**
	 * Constructor.
	 * The default constructor.
	 */
	_strange_select () : YARPBareThread (), _ready_to_go(0) { _owner = NULL; }

	/**
	 * Destructor.
	 */
	virtual ~_strange_select () {}

	/**
	 * The thread body.
	 */
	virtual void Body ();

	/**
	 * Sets the owner of the thread.
	 * @param o is a pointer to the port object owning this thread.
	 */
	void setOwner (Port *o) { _owner = o; }

	/**
	 * Tells all other output threads that there's something ready
	 * to be sent.
	 */
	void pulseGo (void) { _ready_to_go.Post (); }
};


/**
 * This is the main port thread.
 *
 * It starts an input channel for receiving commands, it manages the list of output targets.
 * It creates and manages the _strange_select thread.
 * Commands are sent either remotely or locally (through a self connect procedure) through 
 * the input socket.
 * The GO command (a relics of some older implementation) is now handled efficiently by the 
 * _strange_select thread. Commands are sent to the port through the Say/SayServer.
 * Decision to send data is done through Fire/Share while reading is carried out by 
 * Acquire/Relinquish.
 * Ports objects are both input and output. An output port generally will only receive
 * commands from the input socket. An input port will never create an output thread/socket.
 * This class is mostly a container. The real access control/interface is done through
 * YARPPorts. @see YARPPort object.
 */
class Port : public Thread
{
public:
	friend class _strange_select;
	_strange_select tsender;

	// the protocol type UDP, TCP, etc.
	int protocol_type;

	// keep track of the thread status.
	bool _started;

	int name_set;
	int add_header;
	int expect_header;
	int require_ack;
	int allow_shmem;

	// the list of output threads.
	MeshOf<OutputTarget> targets;

	Sema something_to_send;
	Sema something_to_read;
	Sema okay_to_send;
	Sema wakeup;
	Sema list_mutex;
	Sema out_mutex;
	
	YARPEvent complete_terminate;
	YARPEvent complete_msg_thread;

	YARPString name;
	YARPString network_name;

	HeaderedBlockSender<NewFragmentHeader> sender;
	YARPUniqueNameID *self_id;

	int require_complete_send;

	/**
	 * Tells the port code whether to return before completing
	 * the send process.
	 * @param flag 1 to require the send to be completed before returning
	 * control to the caller.
	 */
	void RequireCompleteSend(int flag = 1)
	{
		require_complete_send = flag;
	}
  
	/**
	 * Counts the number of elements in the internal list of
	 * output connections.
	 * @return the number of connections in the list of threads.
	 */
	int CountClients(void)
	{
		int ct = 0;
		MeshLink *ptr;
		list_mutex.Wait();
		ptr = targets.GetRoot();
		while(ptr!=NULL)
		{
			ct++;
			ptr = ptr->BaseGetMeshNext();
		}
		list_mutex.Post();
		return ct;
	}

	CountedPtr<Sendable> p_sendable;

	int receiving;
	int accessing;
	int asleep;
	int has_input;
	int skip;
	int pending;
	int ignore_data;
	CountedPtr<Receivable> p_receiver_access;
	CountedPtr<Receivable> p_receiver_latest;
	CountedPtr<Receivable> p_receiver_incoming;

	/**
	 * The thread body. The port class is a thread and 
	 * contains code to take care of communication duties. 
	 */
	virtual void Body();

	/**
	 * Constructor.
	 * @param nname is the symbolic name of the port.
	 * @param autostart whether to start the port thread in the constructor (not used/tested).
	 * @param n_protocol_type is one of the protocols available to YARP.
	 */
	Port (const char *nname, int autostart = 1, int n_protocol_type = YARP_NO_SERVICE_AVAILABLE) : 
		tsender(this),
		protocol_type(n_protocol_type),
		something_to_send(0), 
		something_to_read(0),
		okay_to_send(1),
		wakeup(0),
		list_mutex(1),
		out_mutex(1),
		complete_terminate(0,0),
		complete_msg_thread(0,0),
		name(nname),
		require_ack(0),
		allow_shmem(1),
		ignore_data(0),
		network_name(YARP_DEFAULT_NET)
	{ 
		_started = false;
		self_id = NULL;
		skip=1; has_input = 0; asleep=0; name_set=0; accessing = 0; receiving=0;  
		add_header = 1;
		expect_header = 1;
		require_complete_send = 0;
		pending = 0;
		ACE_ASSERT (n_protocol_type != YARP_NO_SERVICE_AVAILABLE);
		if (autostart) Begin(); 
	}

	/**
	 * Constructor. Default contructor with default initialization values.
	 */
	Port (void) : 
		tsender(this),
		something_to_send(0), 
		something_to_read(0),
		okay_to_send(1),
		wakeup(0),
		list_mutex(1),
		out_mutex(1),
		require_ack(0),
		allow_shmem(1),
		ignore_data(0),
		complete_terminate(0,0),
		complete_msg_thread(0,0),
		network_name(YARP_DEFAULT_NET)
	{
		_started = false;
		self_id = NULL;
		skip=1; has_input = 0; asleep=0; name_set=0; accessing = 0; receiving=0;  
		add_header = 1;
		expect_header = 1;
		require_complete_send = 0;
		pending = 0;
		protocol_type = YARP_NO_SERVICE_AVAILABLE;
	}

	/**
	 * Destructor.
	 */
	virtual ~Port();

	/**
	 * Starts the port thread.
	 * @param stack_size is the thread stack size in bytes. Use 0
	 * as default value. This value is passed to the underlying thread
	 * implementation.
	 */
	virtual void Begin (int stack_size = 0)
	{
		list_mutex.Wait();
		if (!_started)
		{
			_started = true;
			list_mutex.Post();
			YARPBareThread::Begin (stack_size);
		}
		else
			list_mutex.Post();
	}

	/**
	 * Asks the thread to terminate. This initiates a complex
	 * shutdown procedure that talks to all existing threads and tries
	 * to shut them down nicely.
	 * @param dontkill is ignored.
	 */
	virtual void End(int dontkill = -1)
	{
		ACE_UNUSED_ARG(dontkill);
		list_mutex.Wait();
		if (_started)
		{
			_started = false;
			list_mutex.Post();

			// sends a message to unblock the thread from the read and close connections.
			// SaySelfEnd does the AskForEnd/Join calls.
			SaySelfEnd ();
		}
		else
			list_mutex.Post();
	}

	/**
	 * Asks the protocol used by the port.
	 * @return a reference to the protocol of the port.
	 */
	inline int& GetProtocolTypeRef() { return protocol_type; }

	/**
	 * Sets the port name and register the port to the name server.
	 * Calling this method has various effects: the name is checked and
	 * then assigned to internal variables, then the thread is started
	 * which register the names to the name server as one of its first
	 * actions.
	 * @param nname is the port symbolic name.
	 * @param netname is the network name when multiple NICs are present.
	 * @return YARP_OK if successful.
	 */
	int SetName(const char *nname, const char *netname)
	{
		if (nname == NULL ||
			nname[0] != '/')
		{
			ACE_DEBUG ((LM_WARNING, "  please respect the port syntax for names:\n"));
			ACE_DEBUG ((LM_WARNING, "   - the name requires a leading /[forward slash]\n"));
			return YARP_FAIL;
		}

		int ret = YARP_OK;
		name = nname;
		network_name = netname;

		asleep = 1;
		okay_to_send.Wait();
		ACE_ASSERT (protocol_type != YARP_NO_SERVICE_AVAILABLE);

		Begin();
		
		okay_to_send.Wait();
		if (!name_set) { ret = YARP_FAIL; }
		okay_to_send.Post();

		return ret;
	}

	/**
	 * Helper function to send a message to the port thread or to another port.
	 * @param pid is the ID of the destination.
	 * @param buf is the buffer containing the message to be sent.
	 * @param len is the length in bytes of the message buffer.
	 * @param tag is the command type.
	 * @return YARP_OK on success.
	 */
	int SendHelper(const YARPNameID& pid, const char *buf, int len, int tag = MSG_ID_NULL);

	/**
	 * Connects to the port thread and sends a message.
	 * @param pid is the destination address of the message.
	 * @param buf is a zero terminated string.
	 * @return YARP_OK on success.
	 */
	int SayServer(const YARPNameID& pid, const char *buf);

	/**
	 * Self connects to the thread and sends a message.
	 * @param buf is a zero terminated string.
	 * @return YARP_OK on success.
	 */
	int Say(const char *buf);

	/**
	 * Self connects to the thread and sends a termination request.
	 * The request is propagated to all threads spawned by the port thread.
	 * @return YARP_OK on success.
	 */
	int SaySelfEnd(void);

	/**
	 * Sets a flag to tell the communication layer to add headers to
	 * the messages.
	 * @param flag is the value of the flag.
	 */
	void AddHeaders(int flag)
    {
		out_mutex.Wait();
		add_header = flag;
		out_mutex.Post();
    }

	/**
	 * Sets a flag to tell the communication layer whether to expect
	 * headers in messages.
	 * @param flag is the value of the flag.
	 */
	void ExpectHeaders(int flag)
    {
		out_mutex.Wait();
		expect_header = flag;
		out_mutex.Post();
    }

	/**
	 * Skips...
	 * @param flag tells the thread whether to skip (what?).
	 */
	void SetSkip(int flag)
    {
		skip = flag;
    }

	/**
	 * Callback function of ports. 
	 * The user can override this method in a derived class to
	 * do something useful upon message reception.
	 * @see YARPPort
	 */
	virtual void OnRead() {}

	/**
	 * Sends a new message. After the message buffer has been
	 * conveyed to the thread, this methods tells the thread code
	 * that it's time to send it.
	 */
	void Fire (void)
    {
		okay_to_send.Wait(); 
		out_mutex.Wait();
		pending = 1;
		out_mutex.Post();
		tsender.pulseGo ();
    }

	/**
	 * Sends a message to the thread asking for termination of
	 * all active output connections.
	 */
	void Deactivate (void)
    {
		char buf[2] = { MSG_ID_DETACH_ALL, 0 };
		okay_to_send.Wait(); 
		Say(buf);
    }

	/**
	 * Shares a Sendable with the thread. The calling thread
	 * tells the port code that the object pointed by @a nsendable
	 * is ready to be sent.
	 * @param nsendable is a pointer to the object to be sent.
	 */
	void Share (Sendable *nsendable);

	/**
	 * Gets a pointer to the access buffered object.
	 * @param nreceiver is a Receivable (a type of content). In particular
	 * this is the last received buffer.
	 */
	void TakeReceiverAccess (Receivable *nreceiver);

	/**
	 * Gets a pointer to the latest buffered object.
	 * @param nreceiver is a Receivable (a type of content). In particular
	 * this is the last received buffer but one.
	 */
	void TakeReceiverLatest (Receivable *nreceiver);

	/**
	 * Gets a pointer to the incoming buffered object.
	 * @param nreceiver is a Receivable (a type of content).
	 */
	void TakeReceiverIncoming (Receivable *nreceiver);

	/**
	 * Acquires the buffer. This operation is needed to
	 * get one of the latest buffers filled with message content
	 * by the communication layer. This method is used to build a Read()
	 * on the port data. @see YARPPort.
	 * @param wait tells the code whether to block or just poll.
	 */
	Sendable *Acquire (int wait = 1);

	/**
	 * Tells the underlying code that reading is terminated.
	 * After this operation a new content is ready to be accessed by
	 * user code.
	 */
	void Relinquish (void);

	/**
	 * The caller waits undefinitedly for a new message to arrive.
	 */
	void WaitInput (void) {	something_to_read.Wait(); }

	/**
	 * Tells the caller whether the port is sending a message.
	 * @return 1 if sending, 0 otherwise.
	 */
	int IsSending (void);

	/**
	 * Waits for a send to be completed.
	 */
	void FinishSend (void);

	/**
	 * Sets the require acknowledge flag into the communication code.
	 * @param flag is 1 if the code has to expect replies back from the remote peer.
	 */
	void SetRequireAck(int flag) { require_ack = flag; }

	/**
	 * Gets the require acknowledge flag.
	 * @return 1 if the communication code is using replies.
	 */
	int GetRequireAck() { return require_ack; }

	/**
	 * Sets the allow shared memory flag into the communication code.
	 * @param flag is 1 if the code has to permit shared memory connections with the remote peer.
	 */
	void SetAllowShmem(int flag) { allow_shmem = flag; }

	/**
	 * Gets the allow shared memory flag.
	 * @return 1 if the communication code allows establishing shared memory connections.
	 */
	int GetAllowShmem(void) { return allow_shmem; }
};


#define InputPort Port
#define OutputPort Port


#endif

