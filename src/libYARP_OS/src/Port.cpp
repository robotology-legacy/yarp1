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
/// $Id: Port.cpp,v 2.2 2006-01-13 23:12:00 eshuy Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>
#include <ace/Sched_Params.h>

#include <stdarg.h>

#include <yarp/YARPThread.h>
#include "yarp_private/mesh.h"

#include <yarp/YARPString.h>

#include "yarp_private/BlockSender.h"
#include "yarp_private/BlockReceiver.h"

#include "yarp_private/Port.h"

#include <yarp/debug.h>
#include "yarp_private/RefCounted.h"

#include <yarp/YARPNameService.h>
#include <yarp/YARPNativeNameService.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSocket.h>

#if defined(__QNX6__) || defined(__LINUX__)
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

///#define DEBUG_DISABLE_SHMEM 1

/// this is because SHMEM alloc is not implemented in ACE 5.3.something for QNX6.
/// need to figure out what's going on on newer versions.
#ifdef __QNX6__
#define DEBUG_DISABLE_SHMEM 1
#endif

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


int OutputTarget::ConnectMcast (const char *name)
{
	WaitMutex ();
	ACE_OS::memcpy (cmdname, name, ACE_OS::strlen(name));
	cmdname[ACE_OS::strlen(name)] = 0;
	msg_type = 1;
	something_to_send.Post();
	PostMutex ();

	YARPScheduler::yield();

	return YARP_OK;
}

int OutputTarget::DeactivateMcast (const char *name)
{
	WaitMutex ();
	ACE_OS::memcpy (cmdname, name, ACE_OS::strlen(name));
	cmdname[ACE_OS::strlen(name)] = 0;
	msg_type = 2;
	something_to_send.Post();
	PostMutex ();

	YARPScheduler::yield();

	return YARP_OK;
}

int OutputTarget::DeactivateMcastAll (void)
{
	WaitMutex ();
	msg_type = 3;
	something_to_send.Post();
	PostMutex ();

	/// I may actually join here!
	YARPScheduler::yield();
	
	return YARP_OK;
}

void OutputTarget::End (int dontkill /* =-1 */)
{
	AskForEnd ();	/// this thread doesn't respond to std AskForEnd() - not very elegant.
	Deactivate ();
	something_to_send.Post();
	Join ();
}

///
///
/// this is the thread which manages a single output connection.
///
void OutputTarget::Body ()
{
	/// implicit wait mutex for this thread.
	/// see overridden Begin()

#if defined(__QNX6__) || defined(__LINUX__)
	signal (SIGPIPE, SIG_IGN);
#endif
	int success = YARP_OK;
	NewFragmentHeader header;
	BlockSender sender;
	CountedPtr<Sendable> p_local_sendable;

	ACE_OS::memset (cmdname, 0, 2*YARP_STRING_LEN);
	msg_type = 0;

	/// needs to know what is the protocol of the owner.
	switch (protocol_type)
	{
	case YARP_QNET:
		{
			ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : starting a QNET sender thread\n"));

			/// LATER: must do proper bailout if locate fails.
			///
			target_pid = YARPNameService::LocateName (GetLabel().c_str());
			target_pid->setRequireAck(GetRequireAck());
			if (target_pid->getServiceType() != protocol_type)
			{
				/// problems.
				ACE_DEBUG ((LM_DEBUG, "troubles locating %s, the protocol is wrong\n", GetLabel().c_str()));

				YARPNameService::DeleteName(target_pid);
				target_pid = NULL;

				active = 0;
				deactivated = 1;
				PostMutex ();
				return;
			}

			YARPEndpointManager::CreateOutputEndpoint (*target_pid);
			YARPEndpointManager::ConnectEndpoints (*target_pid, own_name);
		}
		break;

	case YARP_TCP:
		{
			ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : starting a TCP sender thread\n"));

			target_pid = YARPNameService::LocateName (GetLabel().c_str(), network_name.c_str());
			target_pid->setRequireAck(GetRequireAck());
			/*
			  There is some kind of hack going on here that
			  prevents a clean use of nameserver (registering
			  ports with the protocol they actually use).
			  Adding a workaround.
			 */
			int use_workaround = 0;
			if (target_pid->isConsistent(YARP_TCP)) {
			  use_workaround = 1;
			}


			if (!(use_workaround||
			      target_pid->isConsistent(YARP_UDP)))
			{
			  if (target_pid->getServiceType()==YARP_NO_SERVICE_AVAILABLE) {
			    ACE_DEBUG ((LM_INFO, "*** FAILED to make connection to %s, port not found\n",GetLabel().c_str()));
			  } else {
			    ACE_DEBUG ((LM_INFO, "***** OutputTarget::Body : can't connect - target on different network, output thread 0x%x bailing out\n", GetIdentifier()));
			  }
				YARPNameService::DeleteName(target_pid);
				target_pid = NULL;

				active = 0;
				deactivated = 1;
				PostMutex ();
				return;	
			}

#ifndef DEBUG_DISABLE_SHMEM
			/// involves a query to dns or to the /etc/hosts file.
			char myhostname[YARP_STRING_LEN];
			getHostname (myhostname, YARP_STRING_LEN);
			ACE_INET_Addr local ((u_short)0, myhostname);

			/// this test is carried out by the nameserver relying on the
			/// subnetwork structure.
			char iplocal[17];
			ACE_OS::memset (iplocal, 0, 17);
			ACE_OS::memcpy (iplocal, local.get_host_addr(), 17);

#ifdef USE_YARP2
			// Test for locality is broken.
			// Is test for same-machine sufficient?
			bool same_machine = YARPNameService::VerifySame (((YARPUniqueNameSock *)target_pid)->getAddressRef().get_host_addr(), iplocal, network_name);
#else
			bool same_machine = YARPNameService::VerifyLocal (((YARPUniqueNameSock *)target_pid)->getAddressRef().get_host_addr(), iplocal, network_name.c_str());
#endif
			///if (((YARPUniqueNameSock *)target_pid)->getAddressRef().get_ip_address() == local.get_ip_address())

			bool accepted = true;

#ifdef USE_YARP2
			// YARP2 extension
			if (same_machine&&allow_shmem) {
			  accepted = (YARPNameService::CheckProperty(GetLabel().c_str(),"accepts", "shmem"))!=0;
			  if (!accepted) {
			    ACE_DEBUG ((LM_DEBUG, "switching to SHMEM mode is prohibited\n"));
			  }
			}
			// YARP2 extension ends			
#endif

			if (same_machine && allow_shmem && accepted)
			{
				/// going into SHMEM mode.
				protocol_type = YARP_SHMEM;
				target_pid->setServiceType (YARP_SHMEM);
				target_pid->setRequireAck(GetRequireAck());

				/// 
				ACE_DEBUG ((LM_DEBUG, "$$$$$ OutputTarget::Body : this goes into SHMEM mode\n"));
			}
			else
#endif
			{
				///
				/// LATER: do proper check. 
				/// 	must always succeed. Any protocol is fine but QNET
				/// 	for creating a socket connection.
				if (target_pid->getServiceType() != protocol_type &&
					target_pid->getServiceType() != YARP_UDP)
				{
					/// problems.
					ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : troubles locating %s, the protocol is wrong\n", GetLabel().c_str()));

					YARPNameService::DeleteName(target_pid);
					target_pid = NULL;

					ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : output thread 0x%x bailing out\n", GetIdentifier()));

					active = 0;
					deactivated = 1;
					PostMutex ();

					return;
					///target_pid->invalidate();
				}

				protocol_type = YARP_TCP;
				target_pid->setServiceType (YARP_TCP);
				target_pid->setRequireAck(GetRequireAck());
			}

			YARPEndpointManager::CreateOutputEndpoint (*target_pid);
			YARPEndpointManager::ConnectEndpoints (*target_pid, own_name);

//#ifdef DEBUG_DISABLE_SHMEM
#	ifdef YARP_TCP_NO_DELAY
			/// disables Nagle's algorithm...
			if (protocol_type == YARP_TCP)
				YARPEndpointManager::SetTCPNoDelay (*target_pid);
#	endif
//#endif
		}
		break;

	case YARP_UDP:
		{
			ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : starting a UDP sender thread\n"));

			target_pid = YARPNameService::LocateName (GetLabel().c_str(), network_name.c_str());
			target_pid->setRequireAck(GetRequireAck());
			if (!target_pid->isConsistent(YARP_UDP))
			{
			  if (target_pid->getServiceType()==YARP_NO_SERVICE_AVAILABLE) {
			    ACE_DEBUG ((LM_INFO, "*** FAILED to make connection to %s, port not found\n",GetLabel().c_str()));
			  } else {
			    ACE_DEBUG ((LM_INFO, "***** OutputTarget::Body : can't connect - target on different network, output thread 0x%x bailing out\n", GetIdentifier()));
			  }
				YARPNameService::DeleteName(target_pid);
				target_pid = NULL;

				active = 0;
				deactivated = 1;
				PostMutex ();

				return;	
			}

#ifndef DEBUG_DISABLE_SHMEM
			char myhostname[YARP_STRING_LEN];
			getHostname (myhostname, YARP_STRING_LEN);
			ACE_INET_Addr local ((u_short)0, myhostname);

			char iplocal[17];
			ACE_OS::memset (iplocal, 0, 17);
			ACE_OS::memcpy (iplocal, local.get_host_addr(), 17);

			/// this test is carried out by the name server.
			bool same_machine = YARPNameService::VerifyLocal (((YARPUniqueNameSock *)target_pid)->getAddressRef().get_host_addr(), iplocal, network_name.c_str());

			///if (((YARPUniqueNameSock *)target_pid)->getAddressRef().get_ip_address() == local.get_ip_address())
			if (same_machine && allow_shmem)
			{
				/// going into SHMEM mode.
				protocol_type = YARP_SHMEM;
				((YARPUniqueNameSock *)target_pid)->setServiceType (YARP_SHMEM);
				target_pid->setRequireAck(GetRequireAck());

				/// 
				ACE_DEBUG ((LM_DEBUG, "$$$$$ OutputTarget::Body : this goes into SHMEM mode\n"));
			}
			else
			{
				/// needed because the port changes from that of the incoming call
				/// it is used to accept data (pretend a connection to the remote).
				if (target_pid->getServiceType() != YARP_UDP)
				{
					/// problems.
					ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : troubles locating %s, the protocol is wrong\n", GetLabel().c_str()));
					YARPNameService::DeleteName(target_pid);
					target_pid = NULL;
	
					ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : output thread 0x%x bailing out\n", GetIdentifier()));

					active = 0;
					deactivated = 1;
					PostMutex ();

					return;	
					///target_pid->invalidate();
				}
			}
#else
			if (target_pid->getServiceType() != YARP_UDP)
			{
				/// problems.
				ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : troubles locating %s, the protocol is wrong\n", GetLabel().c_str()));
				///target_pid->invalidate();
				YARPNameService::DeleteName(target_pid);
				target_pid = NULL;

				ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : output thread 0x%x bailing out\n", GetIdentifier()));

				active = 0;
				deactivated = 1;
				PostMutex ();

				return;	
			}
#endif

			YARPEndpointManager::CreateOutputEndpoint (*target_pid);
			YARPEndpointManager::ConnectEndpoints (*target_pid, own_name);
		}
		break;

	case YARP_MCAST:
		{
			ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : starting a MCAST sender thread\n"));

			/// MCAST is different! Locate prepares the MCAST group and registers it.
			/// additional commands are sent to "clients" to ask to join the specific group.
			/// ALSO: VerifySame had been called before actually running the thread.

			target_pid = YARPNameService::LocateName(GetLabel().c_str(), network_name.c_str(), YARP_MCAST);
			target_pid->setRequireAck(GetRequireAck());
			YARPEndpointManager::CreateOutputEndpoint (*target_pid);

			if (target_pid->getServiceType() != YARP_MCAST)
			{
				/// problems.
				ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : troubles locating %s, the protocol is wrong\n", GetLabel().c_str()));
				///target_pid->invalidate();
				YARPNameService::DeleteName(target_pid);
				target_pid = NULL;
			
				ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : output thread 0x%x bailing out\n", GetIdentifier()));

				active = 0;
				deactivated = 1;
				PostMutex ();

				return;	
			}
		}
		break;
	}

	/// this wait for initialization.
	PostMutex ();
	YARP_DBG(THIS_DBG) ((LM_DEBUG, "Output target after initialization section\n"));

	/// if the address is not valid, terminates the thread.
	if (!target_pid->isValid())
	{
		/// 
		ACE_DEBUG ((LM_DEBUG, "***** OutputTarget, troubles, perhaps a process died without unregistering\n"));
		ACE_DEBUG ((LM_DEBUG, "***** OutputTarget, can't connect to the remote endpoint\n"));

		YARPNameService::DeleteName(target_pid);
		target_pid = NULL;
	
		ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : output thread 0x%x bailing out\n", GetIdentifier()));

		active = 0;
		deactivated = 1;
		PostMutex ();

		return;	
	}

	while (!deactivated)
	{
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Waiting for sema to send\n"));
		something_to_send.Wait();

		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Done waiting for sema to send\n"));

		WaitMutex();

		if (deactivate)
		{
			active = 0;
			deactivated = 1;
			PostMutex ();
			continue;
		}

		///
		/// MCAST commands follow: 
		///		- these are sent and processed by contacting the TCP channel of the port.
		if (protocol_type == YARP_MCAST)
		{
			/// LATER: need to figure out whether this is always ok.
			/// multiple connection requests might overlap?

			YARP_DBG(THIS_DBG) ((LM_DEBUG, "-------- msg_type is %d\n", msg_type));

			/// process connect/disconnect messages.
			switch (msg_type)
			{
			case 1:
				{
					/// connect
					YARPUniqueNameID* udp_channel = YARPNameService::LocateName(cmdname, network_name.c_str(), YARP_UDP);
					YARPEndpointManager::ConnectEndpoints (*udp_channel, own_name);
					YARPNameService::DeleteName (udp_channel);
				}
				break;

			case 2:
				{
					/// disconnect
					YARPUniqueNameID* udp_channel = YARPNameService::LocateName(cmdname, network_name.c_str(), YARP_UDP);
					if (udp_channel->getServiceType() == YARP_NO_SERVICE_AVAILABLE)
						udp_channel->setName(cmdname);

					/// first tries a Close of the endpoint.
					YARPEndpointManager::Close (*udp_channel);

					/// tests whether a thread termination is required.
					if (YARPEndpointManager::GetNumberOfClients () < 1)
					{
						active = 0;
						deactivated = 1;
					}

					YARPNameService::DeleteName (udp_channel);
				}
				break;

			case 3:
				{
					/// disconnect all
					YARPEndpointManager::CloseMcastAll ();
					active = 0;
					deactivated = 1;	/// perhaps deactivate = 1;
				}
				break;
			}

			/// if a message has been processed jump to the end of thread cycle.
			if (msg_type != 0)
			{
				msg_type = 0;
				PostMutex();
				continue;
			}

		}	/// end of if (protocol_type).


		///
		///
		check_tick = YARPTime::GetTimeAsSeconds();
		ticking = 1;
		p_local_sendable.Set(p_sendable.Ptr());
		p_sendable.Reset();
		
		PostMutex();
		YARP_DBG(THIS_DBG) ((LM_DEBUG, "Waiting for sema to send <<< sema okay!\n"));

		target_pid->getNameID().setRequireAck(GetRequireAck());
		sender.Begin(target_pid->getNameID());
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
		space_available.Post();

		OnSend();
	} /// while (!deactivated)

	if (protocol_type == YARP_MCAST)
	{
		YARPEndpointManager::CloseMcastAll ();
		/// unregister the mcast group.
		YARPNameService::UnregisterName (target_pid);
	}
	else
	{
		YARPEndpointManager::Close (*target_pid);
	}

	/// but see also what I said on closing the Port thread.
	YARPNameService::DeleteName(target_pid);

	ACE_DEBUG ((LM_DEBUG, "***** OutputTarget::Body : output thread 0x%x bailing out\n", GetIdentifier()));
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
	ACE_DEBUG ((LM_DEBUG, "***** _strange_select::Body : starting\n"));

#if defined(__QNX6__) || defined(__LINUX__)
	signal (SIGCHLD, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
#endif

	OutputTarget *target, *next;

	while (!IsTerminated())
	{
		_ready_to_go.Wait ();
		
		/// hopefully in wait on the sema, it returns from here.
		if (IsTerminated()) continue;

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
					ACE_DEBUG ((LM_INFO, "*** disconnecting %s and %s %s%s\n",
						_owner->name.c_str(), target->GetLabel().c_str(),
						deactivated ? "" : "(target stopped responding)",
						timeout?" (timeout)":""));

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
					ACE_DEBUG ((LM_INFO, "*** disconnecting %s and %s %s\n", 
						_owner->name.c_str(), target->GetLabel().c_str(),
						deactivated ? "" : "(target stopped responding)"));

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
				if (_owner->require_complete_send)
				{
					target->space_available.Wait ();
					target->space_available.Post ();
				}

				target->Share (_owner->p_sendable.Ptr());
				target->add_header = _owner->add_header;
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

	}	/// while !IsTerminated()

	/// 
	ACE_DEBUG ((LM_DEBUG, "***** _strange_select::Body : closing 0x%x\n", GetIdentifier()));
	_owner = NULL;
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
/// communication layer has sent it. Data is never copied [latest version actually makes
/// copies in preparing datagrams (UDP and MCAST)], only pointers (ref counted)
///	travel from the user layer to the comm layer. The only one copy is done at
/// the level of send/recv.
///

void Port::Body()
{
	int tag = 0;
	char *buf = NULL;
	Fragments cmd;
	OutputTarget *target = NULL, *next = NULL;
	BlockReceiver receiver;
	NewFragmentHeader hdr;
	int ok;
	int assume_data;
	int call_on_read = 0;

	YARPUniqueNameID* pid = NULL;
	name_set = 1;

	/// LATER: must actually jump to the end of the thread for proper cleanup instead of returning.
	/// this is the registration section.
	switch (protocol_type)
	{
	case YARP_QNET:
		{
			pid = YARPNameService::RegisterName(name.c_str(), network_name.c_str(), YARP_QNET, YARPNativeEndpointManager::CreateQnetChannel()); 
			if (pid->getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread (qnet)\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}

				_started = false;
				return;
			}
		}
		break;

	case YARP_TCP:
		{
			pid = YARPNameService::RegisterName(name.c_str(), network_name.c_str(), YARP_TCP, YARP_UDP_REGPORTS); 
			if (pid->getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread (tcp)\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}

				_started = false;
				return;
			}
		}
		break;

	case YARP_UDP:
		{
			pid = YARPNameService::RegisterName(name.c_str(), network_name.c_str(), YARP_UDP, YARP_UDP_REGPORTS); 
			if (pid->getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread (udp)\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}

				_started = false;
				return;
			}
		}
		break;

	case YARP_MCAST:
		{
			pid = YARPNameService::RegisterName(name.c_str(), network_name.c_str(), YARP_MCAST, YARP_UDP_REGPORTS);
			if (pid->getServiceType() == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread (mcast)\n"));
				name_set = 0;
				if (asleep)
				{
					asleep = 0;
					okay_to_send.Post();
				}

				_started = false;
				return;
			}
		}
		break;

	default:
		{
			ACE_DEBUG ((LM_DEBUG, "troubles in acquiring ports (?)\n"));
			name_set = 0;
			if (asleep)
			{
				asleep = 0;
				okay_to_send.Post();
			}
			return;
		}
		break;
	}

	YARPEndpointManager::CreateInputEndpoint (*pid);
	
	/// ok, ready to create the sending thread (_strange_select).
	/// all this to avoid sending a message through the port socket.
	tsender.Begin ();

	/// registration completed. now goes the receiver/command thread.
	if (asleep)
	{
		asleep = 0;
		okay_to_send.Post();
	}

	while (!IsTerminated())
    {
		receiver.Begin(pid->getNameID());

		receiver.Get();
		out_mutex.Wait();
		assume_data = !expect_header;
		out_mutex.Post();

		/// WARNING: cmd buffer is not zeroed anywhere, in case buffer is string
		///		it might be better to do it somewhere around here.

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
				
				if (ticking && now-started > 5)
				{
					active = 0;
					timeout = 1;
				}

				if (!active)
				{
					ACE_DEBUG ((LM_INFO, "*** disconnecting %s and %s %s%s\n",
						name.c_str(), target->GetLabel().c_str(),
						deactivated ? "" : "(target stopped responding)",
						timeout?" (timeout)":""));
					/// remove the port no, from the list of used ports.

					delete target;
				}
				target = next;
			}
			scanned = 1;
		}
		list_mutex.Post ();

		if (pid->isValid())
		{
			switch(tag)
			{
			case MSG_ID_ATTACH:
				{
					list_mutex.Wait ();
					
					/// mcast is handled differently.
					if (protocol_type != YARP_MCAST)
					{
						target = targets.GetByLabel (buf);
						if (target == NULL)
						{
							ACE_DEBUG ((LM_INFO, "*** connecting %s to %s\n", name.c_str(), buf));

							target = targets.NewLink(buf);
							ACE_ASSERT(target != NULL);
							
							target->network_name = network_name;
							target->target_pid = NULL;
							target->protocol_type = protocol_type;
							target->allow_shmem = allow_shmem;
							target->SetRequireAck (GetRequireAck());
							target->SetOwnName (name);

							target->Begin();
						}
						else
						{
							ACE_DEBUG ((LM_DEBUG, "Ignoring %s, already connected\n", buf));
						}
					}
					else
					{
						/// it requires an extra call to the name server.
						YARPUniqueNameID *rem_pid = YARPNameService::LocateName (buf);

						YARPString ifname;
					
						bool same_net = YARPNameService::VerifySame (((YARPUniqueNameSock *)rem_pid)->getAddressRef().get_host_addr(), network_name.c_str(), ifname);

#ifndef DEBUG_DISABLE_SHMEM
						char myhostname[YARP_STRING_LEN];
						getHostname (myhostname, YARP_STRING_LEN);
						ACE_INET_Addr local ((u_short)0, myhostname);
						
						char iplocal[17];
						ACE_OS::memset (iplocal, 0, 17);
						ACE_OS::strcpy (iplocal, local.get_host_addr());

						bool same_machine = YARPNameService::VerifyLocal (((YARPUniqueNameSock *)rem_pid)->getAddressRef().get_host_addr(), iplocal, network_name.c_str());

						if (same_net || same_machine)
						{
#else
						if (same_net)
						{
#endif

							
#ifndef DEBUG_DISABLE_SHMEM
							if (same_machine && allow_shmem)
							{
								/// go into TCP-SHMEM.
								target = targets.GetByLabel (buf);
								if (target == NULL)
								{
									ACE_DEBUG ((LM_INFO, "*** connecting SHMEM between %s and %s\n", name.c_str(), buf));

									target = targets.NewLink(buf);
									ACE_ASSERT(target != NULL);
		
									target->network_name = network_name;
									target->target_pid = NULL;
									target->protocol_type = YARP_UDP;
									target->allow_shmem = allow_shmem;
									target->SetOwnName (name);

									target->Begin();
								}
								else
								{
									ACE_DEBUG ((LM_DEBUG, "Ignoring %s, already connected\n", buf));
								}
							}
							else
#endif
							{
								/// mcast out port thread.
								target = targets.GetByLabel ("mcast-thread");
								if (target == NULL)
								{
									ACE_DEBUG ((LM_INFO, "*** connecting MCAST %s to %s\n", name.c_str(), buf));
									target = targets.NewLink("mcast-thread");

									ACE_ASSERT(target != NULL);
									target->network_name = network_name;
									target->target_pid = NULL;
									target->protocol_type = protocol_type;
									target->allow_shmem = allow_shmem;
									target->SetOwnName (name);

									target->Begin();

									target->ConnectMcast (buf);
								}
								else
								{
									ACE_DEBUG ((LM_INFO, "*** connecting MCAST %s to %s\n", name.c_str(), buf));
									target->ConnectMcast (buf);
								}
							}

						}	/// end if (same_net)
					}	/// if !MCAST 

					list_mutex.Post ();
				}
				break;

			case MSG_ID_DETACH:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Received detach request for %s\n", buf+1));

					list_mutex.Wait ();

					if (protocol_type != YARP_MCAST)
					{
						target = targets.GetByLabel(buf+1);
						if (target != NULL)
						{
							ACE_DEBUG ((LM_DEBUG, "Removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str()));
							target->Deactivate();
						}
					}
					else
					{
#ifndef DEBUG_DISABLE_SHMEM
						target = targets.GetByLabel(buf+1);
						if (target != NULL)
						{
							ACE_DEBUG ((LM_DEBUG, "Removing (SHMEM) connection between %s and %s\n", name.c_str(), target->GetLabel().c_str()));
							target->Deactivate();
						}
						else
#endif
						{
							/// mcast
							target = targets.GetByLabel("mcast-thread");
							if (target != NULL)
							{
								ACE_DEBUG ((LM_DEBUG, "Removing connection between %s and (%s) via %s\n", name.c_str(), buf+1, target->GetLabel().c_str()));
								
								target->DeactivateMcast(buf+1);
							}
						}
					}

					/// scan the list of outputs to delete disconnected ones!

					///
					/// don't I need a wait for completion of the Deactivate procedure here?

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
						
						if (ticking && now-started > 5)
						{
							active = 0;
							timeout = 1;
						}

						if (!active)
						{
							ACE_DEBUG ((LM_INFO, "disconnecting %s and %s %s%s\n",
								name.c_str(), target->GetLabel().c_str(),
								deactivated ? "" : "(target stopped responding)",
								timeout?" (timeout)":""));

							delete target;
						}
						target = next;
					}

					list_mutex.Post ();
				}
				break;

			case MSG_ID_DATA:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Gosh, someone sent me data! Me being %s in case you're curious\n", name.c_str()));

					out_mutex.Wait();
					receiving = 1;
					
					if (!ignore_data) 
					{
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
					}
					else
					{
						out_mutex.Post();
					}

					receiver.End();

					out_mutex.Wait();
					receiving = 0;
					
					if (!ignore_data) 
					{
						YARP_DBG(THIS_DBG) ((LM_DEBUG, "&&& Received. Switching with latest space\n"));
						p_receiver_latest.Switch(p_receiver_incoming);

						if (!has_input)
						{
							has_input = 1;
							something_to_read.Post();
						}
					}

					out_mutex.Post();

					if (!ignore_data) 
					{
						call_on_read = 1;
					}
				}
				break;

			case MSG_ID_GO:
				{
					ACE_DEBUG ((LM_ERROR, "this shouldn't happen, the new version doesn't accept MSG_ID_GO\n"));
				}
				break;

			case MSG_ID_DETACH_ALL:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Received detach_all request (%s)\n", name.c_str()));

					list_mutex.Wait ();

					if (protocol_type != YARP_MCAST)
					{
						target = targets.GetRoot();

						while (target != NULL)
						{
							next = target->GetMeshNext();
							ACE_DEBUG ((LM_INFO, "*** removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str()));
							target->Deactivate();
							target = next;
						}
					}
					else
					{
						OutputTarget *mtarget = targets.GetByLabel("mcast-thread");
#ifndef DEBUG_DISABLE_SHMEM
						/// takes care of the SHMEM connection
						target = targets.GetRoot();

						while (target != NULL)
						{
							next = target->GetMeshNext();
							ACE_DEBUG ((LM_INFO, "*** removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str()));
							if (target != mtarget && mtarget != NULL)
								target->Deactivate();
							target = next;
						}
#endif

						/// mcast
						if (mtarget != NULL)
						{
							ACE_DEBUG ((LM_INFO, "*** removing all MCAST connections\n"));
							mtarget->DeactivateMcastAll();
						}
					}

					list_mutex.Post ();

					/// signal the SelfEnd that the msg has been received.

					complete_msg_thread.Signal ();

					/// wait for closure of SelfEnd socket and relative thread.
					complete_terminate.Wait();

					/// only now asks for End.
					AskForEnd ();
				}
				break;

			case MSG_ID_DETACH_IN:
				{
					ACE_DEBUG ((LM_DEBUG, "*** received detach request for %s\n", buf+1));

					YARPUniqueNameID needclose;
					needclose = *pid;
					needclose.setName (buf+1);
					YARPEndpointManager::Close (needclose);
				}
				break;

			case MSG_ID_DUMP_CONNECTIONS:
				{
					ACE_DEBUG ((LM_INFO, "*** dumping connections for %s\n", name.c_str()));
					ACE_DEBUG ((LM_INFO, "*** output connections:\n"));

					list_mutex.Wait ();
					target = targets.GetRoot();
					int i = 0;
					while (target != NULL)
					{
						target->WaitMutex();
						ACE_DEBUG ((LM_INFO, "***   %d: %s %s:%d\n", 
							i, 
							target->GetLabel().c_str(),
							target->GetOwnAddress().get_host_addr(),
							target->GetOwnAddress().get_port_number()));
						target->PostMutex();
						next = target->GetMeshNext();
						target = next;
						i++;
					}

					list_mutex.Post ();

					ACE_DEBUG ((LM_INFO, "*** input connections:\n"));
					YARPEndpointManager::PrintConnections (*pid);
				}	
				break;

			case MSG_ID_ERROR:
				YARP_DBG(THIS_DBG) ((LM_DEBUG, "Error message received by %s!\n", name.c_str()));
				break;

			default:
				{
					YARP_DBG(THIS_DBG) ((LM_DEBUG, "Unknown message received by %s (tag is %d-->'%c', str is %s)!\n", name.c_str(), tag, tag, buf));
				}
				break;
			}
		}	/// end switch/case
      
		if (call_on_read)
		{
			OnRead();
			call_on_read = 0;
		}
	} /// if !terminated

	/// since this is started in this thread close it here.
	tsender.AskForEnd();
	tsender.pulseGo();
	tsender.Join();

	///
	/// tries to shut down the input socket threads.
	/// uses the special request for closing all input threads at once.
	///
	///
	YARPUniqueNameID needcloseall;
	needcloseall = *pid;
	needcloseall.setName ("__All__");
	YARPEndpointManager::Close (needcloseall);

	/// unregister the port name here.
	YARPNameService::UnregisterName (pid);

	/// free memory.
	YARPNameService::DeleteName (pid);
	pid = NULL;

	/// wakes up a potential thread waiting on a blocking Read().
	has_input = 1;
	something_to_read.Post();

	ACE_DEBUG ((LM_DEBUG, "***** main port thread 0x%x returning\n", GetIdentifier()));
}


///
/// this is called from YARPPort::Write to actually send something.
/// Currently it wakes the Port thread up through a socket connection that
///	Paul believes to be responsible for poor performance on Linux/NT.
///
void Port::Share(Sendable *nsendable)
{
	okay_to_send.Wait();
	out_mutex.Wait();
	p_sendable.Set(nsendable);
	out_mutex.Post();

	/// this simply pulses the mutex on the sender thread.
	/// cost an additional thread but hopefully saves a costly message
	/// through the port socket.
	tsender.pulseGo ();
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

	if (go && !IsTerminated())
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
}


int Port::IsSending ()
{
	int sending = pending;
	OutputTarget *target; //, *next;
	target = targets.GetRoot();

	while (target != NULL && !sending)
	{
		target->WaitMutex();
		sending |= target->sending;
		target->PostMutex();
		target = target->GetMeshNext();
	}

	return sending;
}


void Port::FinishSend ()
{
	OutputTarget *target;
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

///
/// this is called by YARPPort::Connect, and <buf> contains the
/// destination name, the self_is is the destination socket, being this
/// a command to the port. This connects to the port itself.
///
int Port::Say(const char *buf)
{
	int result = YARP_FAIL;
	if (self_id == NULL)
	{
		if (protocol_type == YARP_MCAST)
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), YARP_UDP);
			self_id->setServiceType (YARP_TCP);
			self_id->setRequireAck(require_ack);
			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
		else
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), protocol_type);
			if (self_id->getServiceType() != YARP_QNET)
				self_id->setServiceType (YARP_TCP);
			self_id->setRequireAck(require_ack);

			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
	}
	else
	/// silly but to guarantee self_id is !NULL.
	if (self_id != NULL && !self_id->isValid())
	{
		if (protocol_type == YARP_MCAST)
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), YARP_UDP);
			self_id->setServiceType (YARP_TCP);
			self_id->setRequireAck(require_ack);
			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
		else
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), protocol_type);
			if (self_id->getServiceType() != YARP_QNET)
				self_id->setServiceType (YARP_TCP);
			self_id->setRequireAck(require_ack);

			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
	}

	if (self_id->isValid())
	{
		result = SayServer(*self_id, buf);
	}

	/// deletes the endpoint.
	YARPEndpointManager::Close (*self_id);

	YARPNameService::DeleteName (self_id);
	self_id = NULL;
	return result;
}

///
/// this commands the port to terminate gracefully.
int Port::SaySelfEnd(void)
{
	int result = YARP_FAIL;
	if (self_id == NULL && name.c_str()[0] != '\0')
	{
		if (protocol_type == YARP_MCAST)
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), YARP_UDP);
			self_id->setServiceType (YARP_TCP);

			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
		else
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), protocol_type);
			if (self_id->getServiceType() != YARP_QNET)
				self_id->setServiceType (YARP_TCP);

			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
	}
	else
	/// silly but to guarantee self_id is !NULL.
	if (self_id != NULL && !self_id->isValid() && name.c_str()[0] != '\0')
	{
		if (protocol_type == YARP_MCAST)
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), YARP_UDP);
			self_id->setServiceType (YARP_TCP);

			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
		else
		{
			self_id = YARPNameService::LocateName(name.c_str(), network_name.c_str(), protocol_type);
			if (self_id->getServiceType() != YARP_QNET)
				self_id->setServiceType (YARP_TCP);

			YARPEndpointManager::CreateOutputEndpoint (*self_id);
			YARPEndpointManager::ConnectEndpoints (*self_id, name);
		}
	}

	if (self_id != NULL)
	{
		if (self_id->isValid())
		{
			// need to make sure the detach message will be read,
			// even if other messages are arriving

			Relinquish();
			out_mutex.Wait();
			ignore_data = 1;
			out_mutex.Post();
			Relinquish();

			result = SendHelper (*self_id, NULL, 0, MSG_ID_DETACH_ALL);
		}

		Relinquish();
		/// wait for message to be received.
		complete_msg_thread.Wait();

		/// deletes the endpoint.
		YARPEndpointManager::Close (*self_id);

		YARPNameService::DeleteName (self_id);
		self_id = NULL;

		///YARPScheduler::yield();

		/// tell the main thread to complete the termination function.
		complete_terminate.Signal();

		YARPThread::Join ();
	}

	return result;
}


#undef THIS_DBG
