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
/// $Id: Port.cpp,v 1.21 2003-05-21 15:31:07 gmetta Exp $
///
///

//#include "debug-new.h"

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>
#include <ace/Sched_Params.h>

#include <stdarg.h>

#include "YARPThread.h"
#include "mesh.h"

#include "YARPString.h"

#include "BlockSender.h"
#include "BlockReceiver.h"

#include "Port.h"

#include "debug.h"
#include "RefCounted.h"

#include "YARPNameService.h"
#include "YARPNativeNameService.h"
#include "YARPSyncComm.h"
#include "YARPScheduler.h"
#include "YARPTime.h"

#ifdef __QNX6__
#include <signal.h>
#endif

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

Sema services_sema(1);

/// local adjustment, change for enabling/disabling single class debug.
#define THIS_DBG 80


void safe_printf(char *format,...)
{
	va_list arglist;
	services_sema.Wait();
	va_start(arglist,format);
	vprintf(format,arglist);
	va_end(arglist);
	services_sema.Post();
}

#define INT_SIZE (sizeof(int))
#define MXFER_SIZE (sizeof(struct _mxfer_entry))
#define HEADER_CT (4)
#define MAX_FRAGMENT (4)


///
/// prepares the connect command. the header is NewFragmentHeader. a sender is
/// a <vector>. The sender contains the YARPNameID of the destination. This is called
/// by Connect eventually. <pid> is the id of the port itself (self_id). the header
/// contains a NULL tag, the <buf> being the name of the destination (this triggers
/// the creation of a new OutputTarget). sender.End calls Fire and then the actual
///	send.
///
int Port::SendHelper(const YARPNameID& pid, const char *buf, int len, int tag)
{
	NewFragmentHeader *p_hdr;
	sender.Begin (pid);
	p_hdr = sender.AddHeader ();
	p_hdr->tag = tag;
	p_hdr->length = len;
	p_hdr->more = 0;
	p_hdr->first = 1;
	sender.Add ((char*)buf, len);
	return sender.End ();
}

///
/// just calls SendHelper to prepare the buffer.
/// 
int Port::SayServer (const YARPNameID& pid, const char *buf)
{
	int result = YARP_FAIL;
	if (pid.isValid())
	{
		result = SendHelper (pid, buf, ACE_OS::strlen(buf)+1, MSG_ID_NULL);
	}
	return result;
}

///
///
/// this is the thread which manages a single output connection.
///
void OutputTarget::Body ()
{
#ifdef __QNX6__
	signal (SIGPIPE, SIG_IGN);
#endif
	int success;
	NewFragmentHeader header;
	BlockSender sender;
	CountedPtr<Sendable> p_local_sendable;

	int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	ACE_DEBUG ((LM_DEBUG, "reader thread at priority %d -> %d\n", GetPriority(), prio));
	if (SetPriority(prio) == YARP_FAIL)
	{
		ACE_DEBUG ((LM_DEBUG, "can't raise priority of OutputTarget thread, potential source of troubles\n"));
	}

	target_pid = YARPNameService::LocateName (GetLabel().c_str());

	/// needed because the port changes from that of the incoming call
	/// it is used to accept data (pretend a connection to the remote).
	if (target_pid.getServiceType() == YARP_UDP)
	{
		/// local port number into P2[0] param.
		target_pid.allocP2 (1);
		target_pid.getP2Ptr()[0] = port_number;
	}

	YARPEndpointManager::CreateOutputEndpoint (target_pid);
	YARPEndpointManager::ConnectEndpoints (target_pid.getNameID());

#ifdef YARP_TCP_NO_DELAY
	/// disables Nagle's algorithm... 
	YARPEndpointManager::SetTCPNoDelay (target_pid.getNameID());
#endif

	/// MUST complain if connection fails.
	///
	///
	if (!target_pid.isValid())
	{
		WaitMutex();
		active = 0;
		PostMutex();
		while(1)
		{
			something_to_send.Wait();
			YARPScheduler::yield();
#ifdef UPDATED_PORT
			space_available.Post();
#endif
		}
	}

	while (!deactivated)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Waiting for sema to send\n"));
		something_to_send.Wait();

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Done waiting for sema to send\n"));

		WaitMutex();

		check_tick = YARPTime::GetTimeAsSeconds();
		ticking = 1;
		p_local_sendable.Set(p_sendable.Ptr());
		p_sendable.Reset();
		
		PostMutex();
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Waiting for sema to send <<< sema okay!\n"));

		sender.Begin(target_pid.getNameID());
		header.tag = MSG_ID_DATA;
		header.length = 0;
		header.first = 1;
		header.more = 0;
		
		if (add_header)
		{
			sender.Add ((char*)(&header), sizeof(header));
		}

		success = p_local_sendable.Ptr()->Write(sender);
		if (!success)
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** Fire failed\n"));
		success = success && sender.End();

		WaitMutex();

		if (deactivate)
		{
			active = 0;
			deactivated = 1;
		}

		if (!success)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "*** Send failed\n"));
			active = 0;
		}
		
		sending = 0;
		ticking = 0;
		PostMutex();
		p_local_sendable.Reset();

#ifdef UPDATED_PORT
		space_available.Post();
#endif
		OnSend();
	}

	YARPEndpointManager::Close (target_pid.getNameID());	
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "thread %d bailing out\n", GetIdentifier()));
}

///
///
///
/// hopefully an improvement. a thread that handles the fact that we can't wake up
/// Port::Body both on a socket and a mutex/semaphore.
///
/// this thread takes care of sending stuff to the list of output threads. 
/// it is simply activated by the Share method instead of the MSG_ID_GO command.
///
///
void _strange_select::Body ()
{
#ifdef __QNX6__
	signal (SIGCHLD, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
#endif

	int prio = ACE_Sched_Params::priority_max(ACE_SCHED_OTHER);
	///ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	///prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, prio, ACE_SCOPE_THREAD);

	ACE_DEBUG ((LM_DEBUG, "reader thread at priority %d -> %d\n", GetPriority(), prio));
	if (SetPriority(prio) == YARP_FAIL)
	{
		ACE_DEBUG ((LM_DEBUG, "can't raise priority of OutputTarget thread, potential source of troubles\n"));
	}

	OutputTarget *target, *next;

	while (!_terminate)
	{
		_ready_to_go.Wait ();

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Go! --- from a new thread --- \n"));

		/// access to targets must be protected, in case, the port
		/// thread decides to delete a link ;)
		/// this should only (on average) just cause a delay in receiving
		///	a command, while operations in the port thread should take less.
		///
		_owner->list_mutex.Wait ();

		int scanned = 0;
		if (!scanned)
		{
			double now = YARPTime::GetTimeAsSeconds ();
			target = _owner->targets.GetRoot ();
			while (target != NULL)
			{
				next = target->GetMeshNext ();

				target->WaitMutex ();
				
				int active = target->active;
				int deactivated = target->deactivated;
				int ticking = target->ticking;
				double started = target->check_tick;
				
				target->PostMutex ();
				
				int timeout = 0;
				
				if (ticking && now - started > 5)
				{
					active = 0;
					timeout = 1;
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "timeout expired %f\n", now-started));
				}

				if (!active)
				{
					ACE_DEBUG ((LM_DEBUG, "Removing connection between %s and %s (%s%s)\n",
						_owner->name.c_str(), target->GetLabel().c_str(),
						deactivated ? "as requested" : "target stopped responding",
						timeout?"/timeout":""));

///					if (_owner->protocol_type == YARP_UDP || _owner->protocol_type == YARP_MCAST)
///						_owner->output_pool.freeOne (target->GetAssignedPortNo ());

					delete target;
				}
				
				target = next;
			}
			scanned = 1;
		}

		// Send message to all listeners
		if (!scanned)
		{
			target = _owner->targets.GetRoot ();
			while (target != NULL)
			{
				next = target->GetMeshNext ();
				
				target->WaitMutex ();
				int active = target->active;
				int deactivated = target->deactivated;
				target->PostMutex ();

				if (!active)
				{
					ACE_DEBUG ((LM_DEBUG, "Removing connection between %s and %s (%s)\n", 
						_owner->name.c_str(), target->GetLabel().c_str(),
						deactivated ? "as requested" : "target stopped responding"));

///					if (_owner->protocol_type == YARP_UDP || _owner->protocol_type == YARP_MCAST)
///						_owner->output_pool.freeOne (target->GetAssignedPortNo ());

					delete target;
				}
				target = next;
			}
		}

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Sending a message from %s\n", _owner->name.c_str()));
		target = _owner->targets.GetRoot ();

		_owner->out_mutex.Wait ();

		while (target!=NULL)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "Sending a message from %s to target %s\n", _owner->name.c_str(), target->GetLabel().c_str()));
			if (_owner->p_sendable.Ptr() != NULL)
			{
#ifdef UPDATED_PORT
				if (_owner->require_complete_send)
				{
					target->space_available.Wait ();
					target->space_available.Post ();
				}

				target->Share (_owner->p_sendable.Ptr());
				target->add_header = _owner->add_header;
#else
				target->Share (_owner->p_sendable.Ptr());
				target->add_header = _owner->add_header;
#endif
			}
			else
			{
				ACE_DEBUG ((LM_DEBUG, "Delayed message skipped\n"));
			}
			
			target = target->GetMeshNext ();
		}

		/// releases the list only here
		/// 
		_owner->p_sendable.Reset ();
		
		_owner->out_mutex.Post ();
		_owner->pending = 0;
		_owner->okay_to_send.Post ();

		_owner->list_mutex.Post ();

	}	/// exit on _terminate == true

	_owner = NULL;
	_terminate = 0;
}


///
/// this is the main port thread, it instantiates an input socket for receiving
///	both commands and data (in case it is input). it manages a list (mesh) of threads
/// (OutputTarget) for dealing with multiple targets output connections.
/// it synchros with other threads (the calling Read/Write - user level) by means of sema
///	it uses sema too for synchro with output threads. further the input socket
///	generates at least onother thread dealing with the acceptance of incoming connections
/// and a list of connected threads internally dealing with the actual stream.
/// data channels are represented by Sendables, object that derive from RefCounted.
/// the idea being that a particular Sendable must not be destroyed before the
/// communication layer has sent them. Data is never copied, only pointers (ref counted)
///	travel from the user layer to the comm layer. The only one copy is done at
/// the level of send/recv.
///
void Port::Body()
{
#ifdef __QNX6__
///	signal (SIGCHLD, SIG_IGN);
///	signal (SIGPIPE, SIG_IGN);
#endif

	int failed = 0;
	int tag = 0;
	char *buf;
	Fragments cmd;
	OutputTarget *target, *next;
	BlockReceiver receiver;
	NewFragmentHeader hdr;
	int ok;
	int assume_data;
	int call_on_read = 0;

	///MakeServer(name.c_str());
	YARPUniqueNameID pid;

	switch (protocol_type)
	{
	case YARP_QNET:
		{
			pid = YARPNameService::RegisterName(name.c_str(), YARP_QNET, YARPNativeEndpointManager::CreateQnetChannel()); 
			if (pid.getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}
				return;
			}
		}
		break;

	case YARP_TCP:
		{
			pid = YARPNameService::RegisterName(name.c_str(), YARP_TCP); 
			if (pid.getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}
				return;
			}
		}
		break;

	case YARP_UDP:
		{
			/// ask 11 ports overall:
			/// 1 as in channel for asking connections
			/// 10 for incoming connections
			/// there's a bit of messy handling of the numbers here to allocate the
			/// right things to the right places.
			///
			///
			pid = YARPNameService::RegisterName(name.c_str(), YARP_UDP, YARP_UDP_REGPORTS); 
			if (pid.getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}
				return;
			}

///			pid.getP1Ref() = 11;	// resize the in array of ports to 1 + 10.
///			output_pool.resize (pid.getP2Ptr() + 11, 10);	// dim the out array to the last 10 ports.
		}
		break;

	default:
		ACE_DEBUG ((LM_DEBUG, "troubles in acquiring ports (?)\n"));
		break;
	}

	YARPEndpointManager::CreateInputEndpoint (pid);
	
	/// ok, ready to create the sending thread (_strange_select).
	/// all this to avoid sending a message through the port socket.
	tsender.Begin ();

/// QNX6?
///	m_pid.raw_id = MakeServer(name.c_str());
///	m_pid.mode = 2;
  
	if (asleep)
	{
		asleep = 0;
		okay_to_send.Post();
	}

	while(1)
    {
		receiver.Begin(pid.getNameID());

///#ifdef __QNX6__     
///      receiver.pid.mode = m_pid.mode; //Carlos
///      receiver.pid.raw_id = m_pid.raw_id;
///#endif

		receiver.Get();		/// the original version.
		out_mutex.Wait();
		assume_data = !expect_header;
		out_mutex.Post();

		//if (asleep)
		//{
		//wakeup.Post();
		//}

		if (!assume_data)
		{
			ok = receiver.Get((char*)(&hdr),sizeof(hdr));
			if (ok || hdr.checker=='/')
			{
				if (hdr.checker == '~')
				{
					tag = hdr.tag;
					cmd.Require(hdr.length);
					if (tag != MSG_ID_DATA)
					{
						ok = receiver.Get(cmd.GetBuffer(),hdr.length);
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "Got some form of command ( %d, %d )\n", ((int)tag), ok));
					}
				}
				else
				{
					if (hdr.checker != '/')
					{
						ACE_DEBUG ((LM_ERROR, "Error - command received in unknown protocol ( %s )\n", name.c_str()));
						ok = 0;
					}
					else
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "%s received unsupported old format request: %s\n", name.c_str()));
					}
				}
			}

			if (!ok)
			{
				tag = MSG_ID_ERROR;
			}

			if (tag == MSG_ID_NULL)
			{
				buf = cmd.GetBuffer();
				tag = buf[0];
			}
			else
			{
				buf = cmd.GetBuffer();
			}
		}
		else
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "Auto assume data\n"));
			tag = MSG_ID_DATA;
		}

		// What was this for?
		//pid = 0;

		if (tag != MSG_ID_DATA)
		{
			receiver.End();
		}

		list_mutex.Wait ();
		int scanned = 0;
		if (!scanned)
		{
			double now = YARPTime::GetTimeAsSeconds();
			target = targets.GetRoot();
			while (target != NULL)
			{
				next = target->GetMeshNext();
				target->WaitMutex();
				int active = target->active;
				int deactivated = target->deactivated;
				int ticking = target->ticking;
				double started = target->check_tick;
				target->PostMutex();
				int timeout = 0;
				
				if (ticking && now-started>5)
				{
					active = 0;
					timeout = 1;
				}

				if (!active)
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Removing connection between %s and %s (%s%s)\n",
						name.c_str(), target->GetLabel().c_str(),
						deactivated ? "as requested" : "target stopped responding",
						timeout?"/timeout":""));
					/// remove the port no, from the list of used ports.

///					if (protocol_type == YARP_UDP || protocol_type == YARP_MCAST)
///						output_pool.freeOne (target->GetAssignedPortNo ());
					delete target;
				}
				target = next;
			}
			scanned = 1;
		}
        list_mutex.Post ();

		if (pid.isValid())
		{
			switch(tag)
			{
			case MSG_ID_ATTACH:
				{
					list_mutex.Wait ();

					target = targets.GetByLabel (buf);
					if (target == NULL)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "Starting connection between %s and %s\n", name.c_str(), buf));

						target = targets.NewLink(buf);
						
						ACE_ASSERT(target != NULL);
						
						target->target_pid.invalidate();
						
///						if (protocol_type == YARP_UDP || protocol_type == YARP_MCAST)
///						{
///							target->AssignPortNo (output_pool.getOne ());
///						}

						target->Begin();
					}
					else
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "Ignoring %s, already connected\n", buf));
					}

					list_mutex.Post ();
				}
				break;

			case MSG_ID_DETACH:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Received detach request for %s\n", buf+1));

					list_mutex.Wait ();

					target = targets.GetByLabel(buf+1);
					if (target != NULL)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "Removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str()));
						target->Deactivate();
					}

					list_mutex.Post ();
				}
				break;

			case MSG_ID_DATA:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Gosh, someone sent me data! Me being %s in case you're curious\n", name.c_str()));

					out_mutex.Wait();
					receiving = 1;
					
					while (p_receiver_incoming.Ptr() == NULL)
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& Waiting for incoming space\n"));
						// HIT - should have way to convey back skip
						// request to sender
						// (can't just ignore, don't know how many)
						// (components message has)
						
						asleep = 1;
						out_mutex.Post();
						wakeup.Wait();
						out_mutex.Wait();
					}

					asleep = 0;
					p_receiver_incoming.Ptr()->AddRef();
					out_mutex.Post();

					p_receiver_incoming.Ptr()->Read(receiver);
					p_receiver_incoming.Ptr()->RemoveRef();

					receiver.End();

					out_mutex.Wait();
					receiving = 0;
					
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& Received. Switching with latest space\n"));
					p_receiver_latest.Switch(p_receiver_incoming);
					
					if (!has_input)
					{
						has_input = 1;
						something_to_read.Post();
					}

					out_mutex.Post();
					//OnRead();
					call_on_read = 1;
				}
				break;

			case MSG_ID_GO:
				{
					ACE_DEBUG ((LM_ERROR, "this shouldn't get here, the new version don't accept MSG_ID_GO\n"));
				}
				break;

			case MSG_ID_DETACH_ALL:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Received detach_all request (%s)\n", name.c_str()));

					list_mutex.Wait ();

					target = targets.GetRoot();

					while (target!=NULL)
					{
						next = target->GetMeshNext();
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "Removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str()));
						target->Deactivate();
						target = next;
					}

					list_mutex.Post ();
				}
				break;

			case MSG_ID_ERROR:
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "Error message received by %s!\n", name.c_str()));
				break;

			default:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Unknown message received by %s (tag is %d-->'%c')!\n", name.c_str(), tag, tag));
				}
				break;
			}
		}	/// end switch/case
      
		if (call_on_read)
		{
			OnRead();
			call_on_read = 0;
		}
	} /// if it's valid
}


///
/// this is called from YARPPort::Write to actually send something.
/// Currently it wakes the Port thread up through a socket connection that
///	Paul believes to be responsible for poor performance on Linux/NT.
///
void Port::Share(Sendable *nsendable)
{
#ifdef UPDATED_PORT
	okay_to_send.Wait();
	out_mutex.Wait();
	p_sendable.Set(nsendable);
	out_mutex.Post();

#if 0
	char buf[2] = { MSG_ID_GO, 0 };
	Say(buf);
#endif

	/// this simply pulses the mutex on the sender thread.
	/// costed an additional thread but hopefully saves a costly message
	/// through the port socket.
	tsender.pulseGo ();

#else
	out_mutex.Wait();
	p_sendable.Set(nsendable);
	out_mutex.Post();
	Fire();
#endif
}


void Port::TakeReceiverAccess(Receivable *nreceiver)
{
	out_mutex.Wait();
	ACE_ASSERT (!accessing);
	p_receiver_access.Set(nreceiver);
	out_mutex.Post();
}


void Port::TakeReceiverLatest(Receivable *nreceiver)
{
	out_mutex.Wait();
	p_receiver_latest.Set(nreceiver);
	out_mutex.Post();
}


void Port::TakeReceiverIncoming(Receivable *nreceiver)
{
	out_mutex.Wait();
	ACE_ASSERT (!receiving);
	p_receiver_incoming.Set(nreceiver);
	out_mutex.Post();
}

///
/// this together with Relinquish is part of the Read process.
///
///
///
Sendable *Port::Acquire(int wait)
{
	int go = 0;
	Sendable *result = NULL;
	if (wait)
	{
		out_mutex.Wait();
		while (!has_input)
		{
			out_mutex.Post();
			something_to_read.Wait();
			out_mutex.Wait();
		}
		out_mutex.Post();
		go = 1;
	}
	else
	{
		// HIT unfinished
		go = something_to_read.PollingWait();
	}

	if (go)
	{
		out_mutex.Wait();
		ACE_ASSERT (!accessing);
		if (p_receiver_latest.Ptr()!=NULL)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& READ from latest space\n"));
			p_receiver_latest.Switch(p_receiver_access);
			has_input = 0;
		}
		result = p_receiver_access.Ptr();

		if (asleep)
		{
			wakeup.Post();
		}
		accessing = 1;
		out_mutex.Post();
	}

	return result;
}

void Port::Relinquish()
{
	out_mutex.Wait();
	if (accessing)
    {
		if (p_receiver_incoming.Ptr()==NULL)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& READ relinquish to incoming space\n"));
			p_receiver_incoming.Switch(p_receiver_access);
		}
		else 
		if (p_receiver_latest.Ptr()==NULL)
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& READ relinquish to latest space\n"));
			p_receiver_latest.Switch(p_receiver_access);
		}
		else
		{
			YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& READ relinquish to access space\n"));
		}
	
		if (asleep)
		{
			wakeup.Post();
		}
		accessing = 0;
	}
	out_mutex.Post();
}


Port::~Port()
{
	tsender.End ();

	OutputTarget *target, *next;
	target = targets.GetRoot();

	while (target != NULL)
	{
		next = target->GetMeshNext();
		target->End();
		delete target;
		target = next;
	}

	End();

	/// if I knew the pid of the connection, I should probably close down the channel properly.
}


int Port::IsSending ()
{
	//printf("sending?? pending=%d\n", pending);
	int sending = pending;
	OutputTarget *target; //, *next;
	target = targets.GetRoot();

	while (target != NULL && !sending)
	{
		target->WaitMutex();
		//printf(">>> sending?? %s->sending=%d\n", target->GetLabel().c_str(), target->sending);
		sending |= target->sending;
		target->PostMutex();
		target = target->GetMeshNext();
	}

	return sending;
}


void Port::FinishSend ()
{
	int sending = 0;
	OutputTarget *target; //, *next;
	target = targets.GetRoot();

	while (pending)
	{
		// wait for a sister process (at same priority level) to catch up
		YARPScheduler::yield();
	}

	while (target!=NULL)
	{
		target->space_available.Wait();
		target->space_available.Post();
		target = target->GetMeshNext();
	}
}


#undef THIS_DBG
