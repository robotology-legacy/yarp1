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
/// $Id: YARPPort.cpp,v 1.7 2003-04-24 16:54:44 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>

///#include <assert.h>
///#include <stdio.h>

#include "Port.h"
#include "YARPPort.h"
#include "YARPNameService.h"
#include "Sendables.h"
#include "YARPSemaphore.h"
#include "YARPAll.h"
#include "debug.h"

#include <list>

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

using namespace std;

//int __debug_level = 40;
//int __debug_level = 40;

typedef list<YARPPort *> PortList;
static PortList port_list;
static YARPSemaphore port_list_mutex(1);

static void AddPort(YARPPort *port)
{
	port_list_mutex.Wait();
	port_list.push_back(port);
	port_list_mutex.Post();
}

/// LATER: find out why this is commented out.
static void RemovePort(YARPPort *port)
{
	port_list_mutex.Wait();
#ifndef __QNX__
	//  port_list.erase(port);
#endif
	port_list_mutex.Post();
}

///
///
///
class YARPSendable : public Sendable
{
public:
	YARPPortContent *ypc;
	int owned;

	YARPSendable() { ypc = NULL;  owned = 0; }
	YARPSendable(YARPPortContent *n_ypc, int n_owned = 1)
	{
		ypc = NULL;  owned = 0;
		Attach(n_ypc,n_owned);
	}

	virtual ~YARPSendable() 
	{ 
		if (!YARPThread::IsDying())
		{
			if (owned) delete ypc; 
		}
	}

	void Attach(YARPPortContent *n_ypc, int n_owned = 1)
	{
		ACE_ASSERT(ypc==NULL);
		ypc = n_ypc;
		owned = n_owned;
	}

	YARPPortContent *Content()
	{
		return ypc;
	}

	virtual int Write(BlockSender& sender)
	{
		ACE_ASSERT(ypc!=NULL);
		return ypc->Write(sender);
	}

	virtual int Read(BlockReceiver& receiver)
	{
		ACE_ASSERT(ypc!=NULL);
		return ypc->Read(receiver);
	}

	virtual int Destroy()
	{
		ACE_ASSERT(ypc!=NULL);
		ypc->Recycle();

		return Sendable::Destroy();
	}
};

///
///
///
template <class T>
class YARPSendablesOf : public Sendables
{
public:
	YARPPort *port;

	//YARPSendables() { port = NULL; }
	YARPSendablesOf() { port = NULL; }

	void Attach(YARPPort& yp) { port = &yp; }

	void Put(T *s)
	{
		PutSendable(s);
	}

	T *Get()
	{
		//printf("***Get() > 1\n");
		T *t = (T*)GetSendable();
		/*
		if (t!=NULL)
		{
		printf("***Get() > 2a %ld (%d)\n", (long int) t, t->ref_count);
		printf("***Get() > 2a %ld (%d)\n", (long int) t, ((Sendable*)t)->ref_count);
		}
		*/
		if (t==NULL)
		{
			ACE_ASSERT(port!=NULL);
			t = new T(port->CreateContent());
			//printf("***Get() > 3\n");
			ACE_ASSERT(t!=NULL);
			t->ZeroRef();
		}
		//printf("***Get() > 3b\n");
		ACE_ASSERT(t!=NULL);
		//printf("***Get() > 4\n");
		t->owner = this;
		return t;
	}
};

class PortData : public Port
{
public:
	YARPSendablesOf<YARPSendable> sendables;
	CountedPtr<YARPSendable> p_s;
	int service_type;
	YARPInputPort *in_owner;
	YARPOutputPort *out_owner;

	void Attach(YARPPort& yp)
	{
		sendables.Attach(yp);
	}

	virtual void OnRead() { if (in_owner!=NULL) in_owner->OnRead(); }
	virtual void OnSend() { if (out_owner!=NULL) out_owner->OnWrite(); }
};

PortData& CastPortData(void *system_resource)
{
	ACE_ASSERT(system_resource!=NULL);
	return *((PortData *)system_resource);
}

#define PD CastPortData(system_resource)

YARPPort::YARPPort()
{
	system_resource = new PortData;
	ACE_ASSERT(system_resource!=NULL);
	content = NULL;
	PD.in_owner = NULL;
	PD.out_owner = NULL;
	PD.Attach(*this);
	AddPort(this);

	PD.GetProtocolTypeRef() = YARP_NO_SERVICE_AVAILABLE;
}


YARPPort::~YARPPort()
{
	RemovePort(this);
	if (system_resource!=NULL && !YARPThread::IsDying())
	{
		delete ((PortData*)system_resource);
	}
}


int YARPPort::Register(const char *name)
{
	return PD.SetName (name);
	///return 0;
}

int YARPPort::IsReceiving()
{
	return PD.CountClients();
}

int YARPPort::IsSending()
{
	return PD.IsSending();
}

void YARPPort::FinishSend()
{
	PD.FinishSend();
}

int YARPPort::Connect(const char *name)
{
	return PD.Say(name);
}


int YARPPort::Connect(const char *src_name, const char *dest_name)
{
	YARPUniqueNameID id = YARPNameService::LocateName (src_name);
	YARPEndpointManager::CreateOutputEndpoint (id);
	YARPEndpointManager::ConnectEndpoints (id);

	if (id.isValid())
	{
		Port p;
		p.SayServer (id.getNameID(), dest_name);
	}

	YARPEndpointManager::Close (id);

	return YARP_OK;
}


YARPPortContent& YARPPort::Content()
{
	if (content == NULL)
	{
		ACE_OS::fprintf(stderr,"Content requested for port %s when it was not available\n", PD.name.c_str());
		ACE_OS::fprintf(stderr,"Please make sure you understand the lifetime of the content associated with\nan input or output port\n");
		ACE_OS::exit (1);
	}

	ACE_ASSERT (content != NULL);
	return *content;
}


void YARPPort::Deactivate()
{
	PD.Deactivate();
}


void YARPPort::DeactivateAll()
{
	port_list_mutex.Wait();
	for (PortList::iterator it=port_list.begin(); it!=port_list.end(); it++)
	{
		(*it)->Deactivate();
	}
	port_list_mutex.Post();
}



YARPInputPort::YARPInputPort(int n_service_type, int n_protocol_type)
{
	PD.service_type = n_service_type;
	PD.protocol_type = n_protocol_type;
	PD.in_owner = this;
}


YARPInputPort::~YARPInputPort()
{
}


int YARPInputPort::Register(const char *name)
{
	int service_type = PD.service_type;
	PD.TakeReceiverIncoming(new YARPSendable(CreateContent()));
	if (service_type == DOUBLE_BUFFERS || service_type == TRIPLE_BUFFERS)
	{
		PD.TakeReceiverLatest(new YARPSendable(CreateContent()));
	}
	if (service_type == TRIPLE_BUFFERS)
	{
		PD.TakeReceiverAccess(new YARPSendable(CreateContent()));
	}
	return YARPPort::Register(name);
}


bool YARPInputPort::Read(bool wait)
{
	PD.Relinquish ();
	content = NULL;
	YARPSendable *ptr = (YARPSendable *)PD.Acquire (wait);
	if (ptr != NULL)
	{
		content = ptr->Content();
	}
	return (content != NULL);
}


YARPOutputPort::YARPOutputPort(int n_service_type, int n_protocol_type)
{
	PD.service_type = n_service_type;
	PD.GetProtocolTypeRef() = n_protocol_type;
	PD.out_owner = this;
}


YARPOutputPort::~YARPOutputPort()
{
}


int YARPOutputPort::Register(const char *name)
{
	return YARPPort::Register(name);
}


YARPPortContent& YARPOutputPort::Content()
{
	//printf("*** Content() > 1\n");
	if (content == NULL)
	{
		//printf("*** Content() > 2\n");
		YARPSendable *p = PD.sendables.Get();
		//printf("*** Content() > 2b\n");
		PD.p_s.Set(p);
		//printf("*** Content() > 3\n");
		YARPSendable *sendable = PD.p_s.Ptr();
		ACE_ASSERT(sendable!=NULL);
		//printf("*** Content() > 4\n");
		if (sendable->Content()==NULL)
		{
			//printf("*** Content() > 5\n");
			sendable->Attach(CreateContent());
		}
		//printf("*** Content() > 6\n");
		content = sendable->Content();
		//printf("*** Content() > 7\n");
	}
	//printf("*** Content() > 8\n");
	return YARPPort::Content();
}



void YARPOutputPort::Write(bool wait)
{
	//  printf("*** Write() > 1\n");
	PD.RequireCompleteSend(wait);
	//  printf("*** Write() > 2\n");
	PD.Share(PD.p_s.Ptr());
	//  printf("*** Write() > 3\n");
	PD.p_s.Reset();
	//  printf("*** Write() > 4\n");
	content = NULL;
}

