///
/// $Id: Port.h,v 1.2 2003-04-10 15:01:34 gmetta Exp $
///
///

#ifndef __PORT_H_INC
#define __PORT_H_INC

#include <conf/YARPConfig.h>

//#include "debug-new.h"

#include "YARPString.h"

#include <assert.h>
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
	MSG_ID_DETACH_ALL   = 'k',
	MSG_ID_ERROR        = -1
};


class OutputTarget : public BasedLink<OutputTarget>, public Thread
{
public:
	YARPNameID target_pid;
	double check_tick;
	int ticking;
	int active;
	int sending;
	int add_header;
	int deactivate;
	int deactivated;
	Sema something_to_send;
	Sema mutex;
#ifdef UPDATED_PORT
	Sema space_available;
#endif
	CountedPtr<Sendable> p_sendable;

	OutputTarget() : something_to_send(0), 
#ifdef UPDATED_PORT
                   space_available(1),
#endif
                   mutex(1)
    { 
		target_pid.Invalidate(); add_header = 1;  active = 1; sending = 0; 
		deactivate = 0;  deactivated = 0;
		check_tick = 0;  ticking = 0;
    }

	virtual ~OutputTarget() { End(); }
	virtual void Body();

	void WaitMutex() { mutex.Wait(); }
	void PostMutex() { mutex.Post(); }

	virtual void OnSend() {}

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

	void Deactivate()
	{
		WaitMutex();
		deactivate = 1;
		PostMutex();
	}
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


class Port : public Thread
{
public:
	int name_set;
	int add_header;
	int expect_header;
	MeshOf<OutputTarget> targets;
	virtual void Body();
	Sema something_to_send;
	Sema something_to_read;
	Sema okay_to_send;
	Sema wakeup;
	string name;
	HeaderedBlockSender<NewFragmentHeader> sender;
	YARPNameID self_id;

#ifdef UPDATED_PORT
	int require_complete_send;
	void RequireCompleteSend(int flag = 1)
    {
		require_complete_send = flag;
    }
  
	int CountClients()
    {
		int ct = 0;
		MeshLink *ptr;
		ptr = targets.GetRoot();
		while(ptr!=NULL)
		{
			ct++;
			ptr = ptr->BaseGetMeshNext();
		}
		return ct;
    }
#endif
  
	//Sema in_mutex;
	Sema out_mutex;

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

	Port (const char *nname, int autostart = 1) : 
		something_to_send(0), 
		something_to_read(0),
		wakeup(0),
		okay_to_send(1),
	  //in_mutex(1),
		out_mutex(1),
		name(nname)
    { 
		skip=1; has_input = 0; asleep=0; name_set=1; accessing = 0; receiving=0;  
		add_header = 1;
		expect_header = 1;
#ifdef UPDATED_PORT
		require_complete_send = 0;
#endif
		if (autostart) Begin(); 
	}

	virtual ~Port();

	Port() : 
		something_to_send(0), 
		something_to_read(0),
		wakeup(0),
		okay_to_send(1),
		//in_mutex(1),
		out_mutex(1)
	{
		skip=1; has_input = 0; asleep=0; name_set=1; accessing = 0; receiving=0;  
		add_header = 1;
		expect_header = 1;
	}

	void SetName(const char *nname)
	{
		//assert(name.c_str() == NULL);
		name = nname;
		asleep = 1;
		okay_to_send.Wait();
		Begin();
		okay_to_send.Wait();
		okay_to_send.Post();
	}
  
	int SendHelper(YARPNameID pid, const char *buf, int len, int tag=MSG_ID_NULL);
	int SayServer(YARPNameID pid, const char *buf);

	int MakeServer(const char *name);
	YARPNameID GetServer(const char *name);


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

	//maddog  void Say(const char *buf)
	int Say(const char *buf)
    {
		int result = -1;
		if (!self_id.IsValid())
		{
			self_id = GetServer(name.c_str());
		}
		if (self_id.IsValid())
		{
			result = SayServer(self_id,buf);
		}
		return result;
    }

	void Fire()
    {
		char buf[2] = {MSG_ID_GO, 0};
		okay_to_send.Wait(); 
		out_mutex.Wait();
		pending = 1;
		out_mutex.Post();
		Say(buf);
    }

	void Deactivate()
    {
		char buf[2] = {MSG_ID_DETACH_ALL, 0};
		okay_to_send.Wait(); 
		Say(buf);
    }

	void Share(Sendable *nsendable);

	void TakeReceiverAccess(Receivable *nreceiver);
	void TakeReceiverLatest(Receivable *nreceiver);
	void TakeReceiverIncoming(Receivable *nreceiver);


	Sendable *Acquire(int wait=1);
	void Relinquish();

	void WaitInput()
    {
		something_to_read.Wait();
    }

	int IsSending();
	void FinishSend();
};


#define InputPort Port
#define OutputPort Port


#endif

