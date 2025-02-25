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
/// $Id: YARPPort.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

/**
 * \file YARPPort.cpp It contains the implementation of the YARPPort object
 * and some additional private helper classes.
 * A note on destructors: make sure you call the Port->End() from the destructor if
 * you inherits from any of the port classes. This terminates the communication threads
 * smoothly which avoids segmentation violations on leaks on exit.
 * ALT: implement and explicit port un-registration method that also terminates the
 * communication threads.
 *
 */

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include "yarp_private/Port.h"
#include "yarp_private/Sendables.h"
#include <yarp/debug.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPAll.h>

#include <yarp/YARPList.h>

#ifdef __WIN32__
// library initialization.
#pragma init_seg(lib)
#endif

using namespace std;

/** A list of ports is maintained by the library. */
typedef YARPList<YARPPort *> PortList;
static PortList port_list;
static YARPSemaphore port_list_mutex(1);

static void AddPort(YARPPort *port)
{
	port_list_mutex.Wait();
	port_list.push_back(port);
	port_list_mutex.Post();
}

// LATER: find out why this is commented out.
static void RemovePort(YARPPort *port)
{
	ACE_UNUSED_ARG(port);

	port_list_mutex.Wait();

//#ifndef __QNX__
	//  port_list.erase(port);
//#endif
	port_list_mutex.Post();
}


/**
 * YARPSendable is the type of Sendable used by YARP ports. This
 * is simply derived by the Sendable class.
 */
class YARPSendable : public Sendable
{
public:
	/** The YARPPortContent object which knows about read and write. */
	YARPPortContent *ypc;

	/** A flag on whether this class owns the ypc object. */
	int owned;

	/**
	 * Default constructor.
	 */ 
	YARPSendable() { ypc = NULL;  owned = 0; }

	/**
	 * Constructor.
	 * @param n_ypc is a pointer to the YARPPortContent object to be
	 * assigned to this instance.
	 * @param n_owned tells whether the @a n_ypc is owned by this
	 * instance from now on.
	 */
	YARPSendable(YARPPortContent *n_ypc, int n_owned = 1)
	{
		ypc = NULL;  owned = 0;
		Attach(n_ypc, n_owned);
	}

	/**
	 * Destructor. It frees memory if needed.
	 */
	virtual ~YARPSendable() 
	{ 
		if (!YARPThread::IsDying())
		{
			if (owned) delete ypc; 
		}
	}

	/**
	 * Attaches a YARPPortContent object to the YARPSendable.
	 * @param n_ypc is a pointer to the YARPPortContent object to be
	 * assigned to this instance.
	 * @param n_owned tells whether the @a n_ypc is owned by this
	 * instance from now on.
	 */
	void Attach(YARPPortContent *n_ypc, int n_owned = 1)
	{
		ACE_ASSERT(ypc==NULL);
		ypc = n_ypc;
		owned = n_owned;
	}

	/**
	 * Gets the content of the sendable.
	 * @return a pointer to the YARPPortContent object.
	 */
	YARPPortContent *Content()
	{
		return ypc;
	}

	/**
	 * Writes the YARPPortContent to the network. Note that
	 * the Sendable class is just a pattern here since pretty much
	 * everything is reimplemented. Also YARPPortContent's know
	 * how to send/receive themselves.
	 * @param sender is a reference to the BlockSender object that
	 * interfaces to the network code.
	 * @return true on success, false otherwise.
	 */
	virtual int Write(BlockSender& sender)
	{
		ACE_ASSERT(ypc!=NULL);
		return ypc->Write(sender);
	}

	/**
	 * Reads the YARPPortContent from the network. Note that
	 * the Sendable class is just a pattern here since pretty much
	 * everything is reimplemented. Also YARPPortContent's know
	 * how to send/receive themselves.
	 * @param sender is a reference to the BlockSender object that
	 * interfaces to the network code.
	 * @return true on success, false otherwise.
	 */
	virtual int Read(BlockReceiver& receiver)
	{
		ACE_ASSERT(ypc!=NULL);
		return ypc->Read(receiver);
	}

	/**
	 * Destroys both YARPPortContent and the internal Sendable.
	 * @return 1 if the object has been destroyed, 0 otherwise.
	 */
	virtual int Destroy()
	{
		ACE_ASSERT(ypc!=NULL);
		ypc->Recycle();

		return Sendable::Destroy();
	}
};


/**
 * YARPSendablesOf is a template class for sendables similar to
 * SendablesOf.
 */
template <class T>
class YARPSendablesOf : public Sendables
{
public:
	/** A pointer to the YARPPort that owns this object. */
	YARPPort *port;

	/** Default constructor. */
	YARPSendablesOf() { port = NULL; }

	/**
	 * Attaches a YARPPort to this list of sendables.
	 * @param yp is a reference to the YARPPort to attach to.
	 */
	void Attach(YARPPort& yp) { port = &yp; }

	/**
	 * Adds a generic sendable object to the list.
	 * @param s is a pointer to the sendable object to add.
	 */
	void Put(T *s)
	{
		PutSendable(s);
	}

	/**
	 * Gets the last sendable object from the list.
	 * @return a pointer to the last object in the list and 
	 * removes it from the list.
	 */
	T *Get()
	{
		T *t = (T*)GetSendable();
		if (t == NULL)
		{
			ACE_ASSERT(port!=NULL);
			t = new T(port->CreateContent());
			ACE_ASSERT (t != NULL);
			t->ZeroRef();
		}
		ACE_ASSERT(t != NULL);
		t->owner = this;
		return t;
	}
};

/**
 * PortData is a simple convenient container for the generic Port class.
 */
class PortData : public Port
{
public:
	/** A list of YARPSendable implemented by using YARPSendablesOf template. */
	YARPSendablesOf<YARPSendable> sendables;

	/** A referenced pointer used when reading and/or writing. */
	CountedPtr<YARPSendable> p_s;

	/** The protocol type of the Port. */
	int service_type;

	/** The owner, if input port. */
	YARPInputPort *in_owner;

	/** The owner, if output port. */
	YARPOutputPort *out_owner;

	/** 
	 * Attaches the owner to the list.
	 * @param yp is the YARPPort reference that owns the list of sendables.
	 */
	void Attach(YARPPort& yp)
	{
		sendables.Attach(yp);
	}

	/** The OnRead() callback function. */
	virtual void OnRead() { if (in_owner!=NULL) in_owner->OnRead(); }

	/** The OnWrite() callback function. */
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
	if (system_resource != NULL)
		delete ((PortData*)system_resource);
}


void YARPPort::End()
{
  PD.End();
}

int YARPPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
	return PD.SetName (name, net_name);
}

int YARPPort::Unregister(void)
{
	PD.End ();
	return YARP_OK;
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
	if (ACE_OS::strcmp (name, PD.name.c_str()) == 0)
	{
	  ACE_DEBUG ((LM_ERROR, "Try to be serious please\n"));
	  return YARP_FAIL;
	}

	return PD.Say(name);
}


int YARPPort::Connect(const char *src_name, const char *dest_name)
{
	if (ACE_OS::strcmp (src_name, dest_name) == 0)
	{
		ACE_DEBUG ((LM_ERROR, "Silly you, you tried it, didn't you?\n"));
		return YARP_FAIL;
	}

	// checking syntax.
	if (src_name[0] != '*' && src_name[0] != '/')
	{
		ACE_DEBUG ((LM_ERROR, "The syntax of port connect is wrong\n"));
		ACE_DEBUG ((LM_ERROR, "Please remember that valid names must start with\n"));
		ACE_DEBUG ((LM_ERROR, "/ [forward slash]\n"));
		ACE_DEBUG ((LM_ERROR, "The special symbol '*' is allowed when requesting\n"));
		ACE_DEBUG ((LM_ERROR, "the active port connections\n"));
		return YARP_FAIL;
	}

	// request port connections.
	if (src_name[0] == '*')
	{
		YARPUniqueNameID* id = YARPNameService::LocateName (src_name+1, NULL);
		if (id->getServiceType() != YARP_QNET) id->setServiceType (YARP_TCP);
		id->setRequireAck(1);

		YARPEndpointManager::CreateOutputEndpoint (*id);
		int ret = YARPEndpointManager::ConnectEndpoints (*id, "explicit_connect");

		if (ret == YARP_OK)
		{
			if (id->isValid())
			{
				Port p;
				p.SayServer (id->getNameID(), src_name);
			}

			YARPEndpointManager::Close (*id);
			YARPNameService::DeleteName (id);

			return YARP_OK;
		}

		YARPEndpointManager::Close (*id);
		YARPNameService::DeleteName (id);

		return YARP_FAIL;
	}

	// assuming a valid name starting with /
	if (src_name[0] == '/')
	{
		// NULL 2nd param means no net specified, only IP addr.
		YARPUniqueNameID* id = YARPNameService::LocateName (src_name, NULL);
		if (id->getServiceType() != YARP_QNET) id->setServiceType (YARP_TCP);
		id->setRequireAck(1);

		YARPEndpointManager::CreateOutputEndpoint (*id);
		int ret = YARPEndpointManager::ConnectEndpoints (*id, "explicit_connect");

		if (ret == YARP_OK)
		{
			if (id->isValid())
			{
				Port p;
				p.SayServer (id->getNameID(), dest_name);
			}

			YARPEndpointManager::Close (*id);
			YARPNameService::DeleteName (id);

			return YARP_OK;
		}

		YARPEndpointManager::Close (*id);
		YARPNameService::DeleteName (id);

		// continues to next 'if'.
	}

	// can't connect, try a different strategy.
	if (dest_name[0] == '!')
	{
		ACE_DEBUG ((LM_INFO, "trying again with direct connection\n"));

		// tries again...
		YARPUniqueNameID* id = YARPNameService::LocateName (dest_name+1, NULL);
		if (id->getServiceType() != YARP_QNET) id->setServiceType (YARP_TCP);
		id->setRequireAck(1);

		YARPEndpointManager::CreateOutputEndpoint (*id);
		int ret = YARPEndpointManager::ConnectEndpoints (*id, "explicit_connect");
		if (ret == YARP_OK)
		{
			if (id->isValid())
			{
				Port p;
				const int len = ACE_OS::strlen(src_name);
				char *tmp_name = new char[len+2];
				ACE_ASSERT (tmp_name != NULL);
				
				tmp_name[len+1] = 0;
				ACE_OS::strcpy (tmp_name+1, src_name);
				tmp_name[0] = '~';
				p.SayServer (id->getNameID(), tmp_name);

				delete[] tmp_name;
			}

			YARPEndpointManager::Close (*id);
			YARPNameService::DeleteName (id);
			return YARP_OK;
		}

		YARPEndpointManager::Close (*id);
		YARPNameService::DeleteName (id);

		return YARP_FAIL;
	}

	return YARP_FAIL;
}


YARPPortContent& YARPPort::Content()
{
	if (content == NULL)
	{
		ACE_OS::fprintf(stderr,"Content requested for port %s when it was not available\n", PD.name.c_str());
		ACE_OS::fprintf(stderr,"Please make sure you understand the lifetime of the content associated with\nan input or output port\n");
		fflush(stderr); YARPTime::DelayInSeconds(1);
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

	PortList::iterator it(port_list);
	it.go_head();		// might not be required.

	while (!it.done())
	{
		(*it)->Deactivate();
		it++;
	}

	port_list_mutex.Post();
}


void YARPPort::SetRequireAck(int require_ack) 
{
  PD.SetRequireAck(require_ack);
}


int YARPPort::GetRequireAck()
{
  return PD.GetRequireAck();
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


int YARPInputPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
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
	return YARPPort::Register(name, net_name);
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


int YARPOutputPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
	return YARPPort::Register(name, net_name);
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


void YARPOutputPort::SetAllowShmem(int flag)
{
	PD.SetAllowShmem(flag);
}


int YARPOutputPort::GetAllowShmem(void)
{
	return PD.GetAllowShmem();
}
