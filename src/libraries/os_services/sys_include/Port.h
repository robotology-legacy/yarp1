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
/// $Id: Port.h,v 1.24 2003-08-10 07:08:40 gmetta Exp $
///
///

#ifndef __PORT_H_INC
#define __PORT_H_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include "YARPString.h"
#include "YARPNameID.h"
#include "YARPTime.h"

#include "mesh.h"
#include "YARPSemaphore.h"
#define Sema YARPSemaphore
#include "YARPThread.h"
#define Thread YARPThread
#include "Sendable.h"
#include "YARPFragments.h"
#define Fragment Fragments
#include "BlockSender.h"

#include "YARPNetworkTypes.h"


// Message tags, chosen to be reasonably human readable
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
	MSG_ID_DETACH_ALL   = 'k',		/// can only be used by SaySelfEnd --- WARNING.
	MSG_ID_ERROR        = -1
};

///
/// this is the OutputTarget. A thread that handles a single out connection.
/// these are part of a pool (MeshOf) of threads handled by the Port main thread.
/// the thread receives the data buf through the usual sendable ref/pointer.
///
///
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
	int port_number;
	int protocol_type;

	Sema something_to_send;
#ifdef UPDATED_PORT
	Sema space_available;
#endif
	Sema mutex;

	CountedPtr<Sendable> p_sendable;
	
	int msg_type;
	char cmdname[2*YARP_STRING_LEN];

	OutputTarget() : something_to_send(0), 
#ifdef UPDATED_PORT
                   space_available(1),
#endif
                   mutex(1)
    {
		target_pid = NULL;
		add_header = 1;  active = 1; sending = 0; 
		deactivate = 0;  deactivated = 0;
		check_tick = 0;  ticking = 0;
		port_number = 0;
		protocol_type = YARP_NO_SERVICE_AVAILABLE;
		memset (cmdname, 0, 2*YARP_STRING_LEN);
		msg_type = 0;
    }

	virtual ~OutputTarget() 
	{ 
		End(); 
		ACE_OS::printf ("OutputTarget destroyed\n");
		/// supposedly closes the actual connection.
	}

	virtual void End (int dontkill = -1);

	virtual void Body();
	
	///
	virtual void Begin(int stack_size=0)
	{
		/// protect thread code at startup.
		WaitMutex ();
		YARPBareThread::Begin(stack_size);
	}

	void WaitMutex() { mutex.Wait(); }
	void PostMutex() { mutex.Post(); }

	virtual void OnSend() {}

	void AssignPortNo (int port) { port_number = port; }
	int GetAssignedPortNo (void) const { return port_number; }

	/// passes a new Sendable to the thread.
	void Share(Sendable *nsendable)
	{
		WaitMutex();
		if (!sending)
		{
#ifdef UPDATED_PORT
			space_available.Wait();
#endif
			p_sendable.Set(nsendable);
			something_to_send.Post();
			sending = 1;
		}
		PostMutex();
	}

	/// this simply asks the thread to close itself down.
	void Deactivate()
	{
		WaitMutex();
		deactivate = 1;
		PostMutex();
	}

	/// special extra commands for MCAST protocol.
	/// this is required to forward clients MCAST request to join/release the group.
	int ConnectMcast (const char *name);
	int DeactivateMcast (const char *name);
	int DeactivateMcastAll (void);
};


#include "begin_pack_for_net.h"
class NewFragmentHeader
{
public:
	NetInt32 length;
	unsigned char checker;
	char tag;
	char more;
	char first;

	NewFragmentHeader() { checker='~';}
} PACKED_FOR_NET;
#include "end_pack_for_net.h"

///
///
///
///
///
class Port;

///
/// this is a thread that takes care of sending data. It waits for a
///	go signal (_ready_to_go), it activates by scanning the list of
///	output targets, and eventually for all active targets sends the
/// data.
///	this thread is meant to substitute a "select" with mutex/sema and 
/// socket demultiplexing capabilities (that doesn't exist in NT).
///
class _strange_select : public YARPBareThread
{
protected:
	YARPSemaphore _ready_to_go;
	Port *_owner;

public:
	_strange_select (Port *o) : YARPBareThread (), _ready_to_go(0) 
	{
		_owner = o;
		ACE_ASSERT (o != NULL); 
	}

	_strange_select () : YARPBareThread (), _ready_to_go(0) { _owner = NULL; }

	virtual ~_strange_select () {}
	virtual void Body ();
	void setOwner (Port *o) { _owner = o; }
	void pulseGo (void) { _ready_to_go.Post (); }
};


///
/// this is the main port thread.
///	- starts an input channel for receiving commands.
/// - manages the list of output targets.
/// - manages the _strange_select thread.
/// - commands are sent either remotely or locally through the input socket.
///	- the GO command (data send) is handled by the _strange_select (efficiency).
///	- writing to the port is done through the Say/SayServer (for commands).
/// - writing the GO/data is done through Fire/Share.
/// - reading is done through Acquire/Relinquish.
///
///
class Port : public Thread
{
public:
	friend class _strange_select;
	_strange_select tsender;

	/// the protocol type UDP, TCP, etc.
	int protocol_type;

	/// keep track of the thread status.
	bool _started;

	int name_set;
	int add_header;
	int expect_header;
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
	HeaderedBlockSender<NewFragmentHeader> sender;
	YARPUniqueNameID *self_id;

#ifdef UPDATED_PORT
	int require_complete_send;
	void RequireCompleteSend(int flag = 1)
	{
		require_complete_send = flag;
	}
  
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
#endif

	CountedPtr<Sendable> p_sendable;

	int receiving;
	int accessing;
	int asleep;
	int has_input;
	int skip;
	int pending;
	CountedPtr<Receivable> p_receiver_access;
	CountedPtr<Receivable> p_receiver_latest;
	CountedPtr<Receivable> p_receiver_incoming;


	///
	virtual void Body();

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
		name(nname)
	{ 
		_started = false;
		self_id = NULL;
		skip=1; has_input = 0; asleep=0; name_set=0; accessing = 0; receiving=0;  
		add_header = 1;
		expect_header = 1;
#ifdef UPDATED_PORT
		require_complete_send = 0;
#endif
		pending = 0;
		ACE_ASSERT (n_protocol_type != YARP_NO_SERVICE_AVAILABLE);
		if (autostart) Begin(); 
	}

	Port (void) : 
		tsender(this),
		something_to_send(0), 
		something_to_read(0),
		okay_to_send(1),
		wakeup(0),
		list_mutex(1),
		out_mutex(1),
		complete_terminate(0,0),
		complete_msg_thread(0,0)
	{
		_started = false;
		self_id = NULL;
		skip=1; has_input = 0; asleep=0; name_set=0; accessing = 0; receiving=0;  
		add_header = 1;
		expect_header = 1;
#ifdef UPDATED_PORT
		require_complete_send = 0;
#endif
		pending = 0;
		protocol_type = YARP_NO_SERVICE_AVAILABLE;
	}

	virtual ~Port();

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

	virtual void End(int dontkill = -1)
	{
		ACE_UNUSED_ARG(dontkill);

		list_mutex.Wait();
		if (_started)
		{
			_started = false;
			list_mutex.Post();

			/// sends a message to unblock the thread from the read and close connections.
			/// SaySelfEnd does the AskForEnd/Join calls.
			SaySelfEnd ();
		}
		else
			list_mutex.Post();
	}

	inline int& GetProtocolTypeRef() { return protocol_type; }

	int SetName(const char *nname)
	{
		if (nname == NULL ||
			nname[0] != '/')
		{
			ACE_DEBUG ((LM_DEBUG, "  please respect the port syntax for names:\n"));
			ACE_DEBUG ((LM_DEBUG, "   - the name requires a leading /[slash]\n"));
			return YARP_FAIL;
		}

		int ret = YARP_OK;
		name = nname;
		asleep = 1;
		okay_to_send.Wait();
		ACE_ASSERT (protocol_type != YARP_NO_SERVICE_AVAILABLE);

		Begin();
		
		okay_to_send.Wait();
		if (!name_set) { ret = YARP_FAIL; }
		okay_to_send.Post();
		return ret;
	}
  
	int SendHelper(const YARPNameID& pid, const char *buf, int len, int tag = MSG_ID_NULL);
	int SayServer(const YARPNameID& pid, const char *buf);
	int Say(const char *buf);
	int SaySelfEnd(void);

	void AddHeaders(int flag)
    {
		out_mutex.Wait();
		add_header = flag;
		out_mutex.Post();
    }

	void ExpectHeaders(int flag)
    {
		out_mutex.Wait();
		expect_header = flag;
		out_mutex.Post();
    }

	void SetSkip(int flag)
    {
		skip = flag;
    }

	virtual void OnRead() {}

	void Fire (void)
    {
		okay_to_send.Wait(); 
		out_mutex.Wait();
		pending = 1;
		out_mutex.Post();
		tsender.pulseGo ();
    }

	void Deactivate (void)
    {
		char buf[2] = { MSG_ID_DETACH_ALL, 0 };
		okay_to_send.Wait(); 
		Say(buf);
    }

	void Share (Sendable *nsendable);

	void TakeReceiverAccess (Receivable *nreceiver);
	void TakeReceiverLatest (Receivable *nreceiver);
	void TakeReceiverIncoming (Receivable *nreceiver);

	Sendable *Acquire (int wait = 1);
	void Relinquish (void);

	void WaitInput (void)
    {
		something_to_read.Wait();
    }

	int IsSending (void);
	void FinishSend (void);
};


#define InputPort Port
#define OutputPort Port


#endif

