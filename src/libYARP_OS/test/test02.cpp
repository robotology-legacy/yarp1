///
/// $Id: test02.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
using namespace std;

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPSocketNameService.h>
#include <yarp/YARPNativeNameService.h>
#include <yarp/YARPParseParameters.h>

#define REG_TEST_NAME "/test/test02"
#define REG_LOCATE_NAME "/test/test02"
#define SRC_NAME "/text/test02:o"
#define NET_NAME "default"

extern int __debug_level;

YARPSemaphore out(1);

class MyThread1 : public YARPThread
{
public:
	virtual void Body()
	{
		int ct = 0;
		char txt[128] = "Hello there";
		char reply[128] = "Not set";

		YARPUniqueNameID* id = NULL;
		cout << "Here" << endl;
		cout.flush();

		do 
		{
			out.Wait();
			cout << "Looking up name" << endl;
			cout.flush();
			out.Post();
			
			id = YARPNameService::LocateName(REG_LOCATE_NAME, NET_NAME);
			if (id->getServiceType () == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, "can't locate name, bailing out\n"));
				return;
			}
			
			YARPEndpointManager::CreateOutputEndpoint (*id);
			YARPEndpointManager::ConnectEndpoints (*id, SRC_NAME);

			if (!id->isValid())
			{
				YARPTime::DelayInSeconds(0.2);
			}
		} 
		while (!id->isValid());

		while (!IsTerminated())
		{
			out.Wait();
			cout << "Preparing to send: " << txt << endl;
			cout.flush();
			out.Post();

			YARPSyncComm::Send(id->getNameID(),txt,sizeof(txt),reply,sizeof(reply));

			out.Wait();
			cout << "Got reply : " << reply << endl;
			cout.flush();
			out.Post();

			YARPTime::DelayInSeconds(2.0);

			ct++;
			sprintf(txt, "And a-%d", ct);
		}
	}
};

class MyThread2 : public YARPThread
{
public:
	virtual void Body()
	{
		char buf[128] = "Not set";
		char reply[128] = "Not set";
		int ct = 0;
		YARPTime::DelayInSeconds(0.01);

		/// connect to name server and get ip and port.
#ifdef __QNX6__
		YARPUniqueNameID* id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_QNET, YARPNativeEndpointManager::CreateQnetChannel());
#else
		YARPUniqueNameID* id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_UDP, 11);
#endif
		if (id->getServiceType() == YARP_NO_SERVICE_AVAILABLE)
		{
			ACE_DEBUG ((LM_DEBUG, "can't register name, bailing out\n"));
			return;
		}

		/// create the input endpoint.
		YARPEndpointManager::CreateInputEndpoint (*id);

		while (!IsTerminated())
		{
			out.Wait();
			cout << "Waiting for input" << endl;
			cout.flush();
			out.Post();
			
			YARPNameID reply_id = YARPSyncComm::BlockingReceive(id->getNameID(), buf, sizeof(buf));

			sprintf(reply,"Got %s", buf);
			out.Wait();
			cout << "Received message: " << buf << endl;
			cout.flush();
			out.Post();
			
			YARPSyncComm::Reply(reply_id, reply, sizeof(reply));
			ct++;
		}
	}
};

///
///
///
int main(int argc, char *argv[])
{
	ACE_UNUSED_ARG(argc);
	ACE_UNUSED_ARG(argv);

//	YARPString protocol = "tcp";
//	YARPParseParameters::parse (argc, argv, "-protocol", protocol);

	__debug_level = 80;

	MyThread1 t1;
	MyThread1 t3;
	MyThread2 t2;
	t2.Begin();
	YARPTime::DelayInSeconds(1.0);
	t1.Begin();
	t3.Begin();

	YARPTime::DelayInSeconds(12.0);
	t2.End();	/// stops the receiver first.

	YARPTime::DelayInSeconds(2.0);
	t1.End();	/// stops the senders.
	t3.End();

	return 0;
}

