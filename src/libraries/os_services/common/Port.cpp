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
/// $Id: Port.cpp,v 1.5 2003-04-18 15:51:46 gmetta Exp $
///
///

//#include "debug-new.h"

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

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
#include "YARPSyncComm.h"
#include "YARPScheduler.h"
#include "YARPTime.h"


#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

Sema services_sema(1);


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

#if 0
///
///
/// return type was YARPNameID
YARPUniqueNameID GetServer(const char *name)
{
	///
	return YARPNameService::LocateName(name);
}

/// return type was int.
YARPUniqueNameID MakeServer(const char *name)
{
	return YARPNameService::RegisterName(name);
}
#endif

int Port::SendHelper(const YARPNameID& pid, const char *buf, int len, int tag)
{
	NewFragmentHeader *p_hdr;
	sender.Begin(pid);
	p_hdr = sender.AddHeader();
	p_hdr->tag = tag;
	p_hdr->length = len;
	p_hdr->more = 0;
	p_hdr->first = 1;
	sender.Add((char*)buf,len);
	return sender.End();
}

int Port::SayServer(const YARPNameID& pid, const char *buf)
{
	int result = YARP_FAIL;
	if (pid.isValid())
	{
		result = SendHelper(pid, buf, strlen(buf)+1);
	}
	return result;
}

#if 0
////
//// changed return type
YARPUniqueNameID Port::GetServer(const char *name)
{
	return ::GetServer(name);
}
#endif

#if 0
YARPUniqueNameID Port::MakeServer(const char *name)
{
	return ::MakeServer(name);
}
#endif

void OutputTarget::Body()
{
	int success;
	NewFragmentHeader header;
	BlockSender sender;
	CountedPtr<Sendable> p_local_sendable;

	///YARPUniqueNameID target_pid = GetServer(GetLabel().c_str());
	YARPUniqueNameID target_pid = YARPNameService::LocateName(GetLabel().c_str());
	YARPEndpointManager::CreateOutputEndpoint (target_pid);
	YARPEndpointManager::ConnectEndpoints (target_pid);

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

	while(1)
	{
		dbg_printf(80)("Waiting for sema to send\n");
		something_to_send.Wait();

		dbg_printf(80)("Done waiting for sema to send\n");
		WaitMutex();

		check_tick = YARPTime::GetTimeAsSeconds();
		ticking = 1;
		p_local_sendable.Set(p_sendable.Ptr());
		p_sendable.Reset();
		
		PostMutex();
		dbg_printf(80)("Waiting for sema to send <<< sema okay!\n");

		sender.Begin(target_pid.getNameID());
		header.tag = MSG_ID_DATA;
		header.length = 0;
		header.first = 1;
		header.more = 0;
		
		if (add_header)
		{
			sender.Add((char*)(&header), sizeof(header));
		}

		success = p_local_sendable.Ptr()->Write(sender);
		DBG(45) if (!success) cout << "*** Fire failed" << endl;
		success = success && sender.End();

		WaitMutex();

		if (deactivate)
		{
			active = 0;
			deactivated = 1;
		}

		if (!success)
		{
			DBG(45) cout << "*** Send failed" << endl;
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
}


void Port::Body()
{
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
	YARPUniqueNameID pid = YARPNameService::RegisterName(name.c_str()); 
	if (pid.getServiceType() == YARP_NO_SERVICE_AVAILABLE)
	{
		ACE_DEBUG ((LM_DEBUG, ">>> registration failed, bailing out port thread\n"));
		name_set = 0;
		okay_to_send.Post();
		return;
	}

	YARPEndpointManager::CreateInputEndpoint (pid);
	
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

		receiver.Get();
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
						DBG(95) cout << "Got some form of command (" << ((int)tag) << "," << ok << ")" << endl;
					}
				}
				else
				{
					if (hdr.checker != '/')
					{
						cerr << "Error - command received in unknown protocol (" << name << ")"	<< endl;
						ok = 0;
					}
					else
					{
						dbg_printf(0)("%s received unsupported old format request: %s\n", name.c_str());
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
			dbg_printf(95)("Auto assume data\n");
			tag = MSG_ID_DATA;
		}

		// What was this for?
		//pid = 0;

		if (tag!=MSG_ID_DATA)
		{
			receiver.End();
		}

      
		int scanned = 0;
		if (!scanned)
		{
			double now = YARPTime::GetTimeAsSeconds();
			target = targets.GetRoot();
			while (target!=NULL)
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
					dbg_printf(40)("Removing connection between %s and %s (%s%s)\n",
						name.c_str(), target->GetLabel().c_str(),
						deactivated ? "as requested" : "target stopped responding",
						timeout?"/timeout":"");
					delete target;
				}
				target = next;
			}
			scanned = 1;
		}
            
		if (pid.isValid())
		{
			switch(tag)
			{
			case MSG_ID_ATTACH:
				{
					target = targets.GetByLabel(buf);
					if (target==NULL)
					{
						dbg_fprintf(40)(stderr, "Starting connection between %s and %s\n", name.c_str(), buf);

						target = targets.NewLink(buf);
						
						ACE_ASSERT(target!=NULL);
						
						target->target_pid.invalidate();
						target->Begin();
					}
					else
					{
						dbg_printf(45)("Ignoring %s, already connected\n", buf);
					}
				}
				break;

			case MSG_ID_DETACH:
				{
					dbg_printf(70)("Received detach request for %s\n", buf+1);
					target = targets.GetByLabel(buf+1);
					if (target!=NULL)
					{
						dbg_printf(70)("Removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str());
						target->Deactivate();
					}
				}
				break;

			case MSG_ID_DATA:
				{
					dbg_printf(70)("Gosh, someone sent me data! Me being %s in case you're curious\n", name.c_str());

					out_mutex.Wait();
					receiving = 1;
					
					while (p_receiver_incoming.Ptr() == NULL)
					{
						dbg_printf(70)("%%% Waiting for incoming space\n");
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
					
					dbg_printf(70)("%%% Received. Switching with latest space\n");
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
					// Send message to all listeners
					if (!scanned)
					{
						target = targets.GetRoot();
						while (target!=NULL)
						{
							next = target->GetMeshNext();
							target->WaitMutex();
							int active = target->active;
							int deactivated = target->deactivated;
							target->PostMutex();
							if (!active)
							{
								dbg_printf(40)("Removing connection between %s and %s (%s)\n", 
									name.c_str(), target->GetLabel().c_str(),
									deactivated ? "as requested" : "target stopped responding");
								delete target;
							}
							target = next;
						}
					}

					dbg_printf(95)("Sending a message from %s\n", name.c_str());
					target = targets.GetRoot();
					out_mutex.Wait();

					while (target!=NULL)
					{
						dbg_printf(80)("Sending a message from %s to target %s\n",
						name.c_str(), target->GetLabel().c_str());
						if (p_sendable.Ptr() != NULL)
						{
	#ifdef UPDATED_PORT
							if (require_complete_send)
							{
								target->space_available.Wait();
								target->space_available.Post();
							}

							target->Share(p_sendable.Ptr());
							target->add_header = add_header;
	#else
							target->Share(p_sendable.Ptr());
							target->add_header = add_header;
	#endif
						}
						else
						{
							dbg_printf(45)("Delayed message skipped\n");
						}
						
						target = target->GetMeshNext();
					}

					p_sendable.Reset();
					out_mutex.Post();
					pending = 0;
					okay_to_send.Post();
				}
				break;

			case MSG_ID_DETACH_ALL:
				{
					dbg_printf(70)("Received detach_all request (%s)\n", name.c_str());
					target = targets.GetRoot();

					while (target!=NULL)
					{
						next = target->GetMeshNext();
						dbg_printf(70)("Removing connection between %s and %s\n", name.c_str(), target->GetLabel().c_str());
						target->Deactivate();
						target = next;
					}
				}
				break;

			case MSG_ID_ERROR:
				dbg_printf(50)("Error message received by %s!\n", name.c_str());
				break;

			default:
				{
					dbg_printf(80)("Unknown message received by %s (tag is %d-->'%c')!\n", name.c_str(), tag, tag);
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



void Port::Share(Sendable *nsendable)
{
#ifdef UPDATED_PORT
	//printf("*** debug 1\n");
	okay_to_send.Wait();
	//printf("*** debug 2\n");
	out_mutex.Wait();
	//printf("*** debug 3\n");
	p_sendable.Set(nsendable);
	//printf("*** debug 4\n");
	out_mutex.Post();
	//printf("*** debug 5\n");
	char buf[2] = {MSG_ID_GO, 0};
	//printf("*** debug 6\n");
	Say(buf);
	//printf("*** debug 7\n");
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
	assert(!accessing);
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
	assert(!receiving);
	p_receiver_incoming.Set(nreceiver);
	out_mutex.Post();
}

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
		assert(!accessing);
		if (p_receiver_latest.Ptr()!=NULL)
		{
			dbg_printf(70)("%%% READ from latest space\n");
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
			dbg_printf(70)("%%% READ relinquish to incoming space\n");
			p_receiver_incoming.Switch(p_receiver_access);
		}
		else 
		if (p_receiver_latest.Ptr()==NULL)
		{
			dbg_printf(70)("%%% READ relinquish to latest space\n");
			p_receiver_latest.Switch(p_receiver_access);
		}
		else
		{
			dbg_printf(70)("%%% READ relinquish to access space\n");
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
	OutputTarget *target, *next;
	target = targets.GetRoot();

	while (target!=NULL)
	{
		next = target->GetMeshNext();
		target->End();
		delete target;
		target = next;
	}

	End();
}


int Port::IsSending()
{
	//printf("sending?? pending=%d\n", pending);
	int sending = pending;
	OutputTarget *target; //, *next;
	target = targets.GetRoot();

	while (target!=NULL && !sending)
	{
		target->WaitMutex();
		//printf(">>> sending?? %s->sending=%d\n", target->GetLabel().c_str(), target->sending);
		sending |= target->sending;
		target->PostMutex();
		target = target->GetMeshNext();
	}

	return sending;
}


void Port::FinishSend()
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
