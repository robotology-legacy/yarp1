///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test03.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <stdio.h>
#include <iostream>
using namespace std;

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPNativeNameService.h>

#define REG_TEST_NAME "localhost|1111"
#define REG_LOCATE_NAME "localhost|1111"
#define SRC_NAME "source|YYYY"
#define NET_NAME "local"

YARPSemaphore out(1);

class MyThread1 : public YARPThread
{
public:
	virtual void Body()
	{
		int ct = 0;
		char txt[128] = "Hello there";
		char reply[128] = "Not set";

		YARPUniqueNameID* id;

		do 
		{
			out.Wait();
			cout << "Looking up name" << endl;
			cout.flush();
			out.Post();

			id = YARPNameService::LocateName(REG_LOCATE_NAME, NET_NAME);
			YARPEndpointManager::CreateOutputEndpoint (*id);
			YARPEndpointManager::ConnectEndpoints (*id, SRC_NAME);

			if (!id->isValid())
			{
				YARPTime::DelayInSeconds(0.2);
			}
		} 
		while (!id->isValid());

		int x = 42;
		while (!IsTerminated())
		{
			out.Wait();
			cout << "Preparing to send: " << txt << endl;
			cout.flush();
			out.Post();

			YARPMultipartMessage smsg(2);
			smsg.Set(0,txt,sizeof(txt));
			smsg.Set(1,(char*)(&x),sizeof(x));
			YARPMultipartMessage rmsg(2);
			double y = 999;
			rmsg.Set(0,reply,sizeof(reply));
			rmsg.Set(1,(char*)(&y),sizeof(y));
			cout << "***sending: " << txt << endl;
			cout.flush();

			YARPSyncComm::Send(id->getNameID(),smsg,rmsg);
			x++;

			out.Wait();
			cout << "Got reply : " << reply << " and number " << y << endl;
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
#ifdef __QNX6__
		YARPUniqueNameID* id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_QNET, YARPNativeEndpointManager::CreateQnetChannel());
#else
		YARPUniqueNameID* id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_UDP, YARP_UDP_REGPORTS);
#endif
		YARPEndpointManager::CreateInputEndpoint (*id);

		while (!IsTerminated())
		{
			out.Wait();
			cout << "Waiting for input" << endl;
			cout.flush();
			out.Post();
			
			YARPMultipartMessage imsg(2);
			int x = 999;
			imsg.Set(0,buf,sizeof(buf));
			imsg.Set(1,(char*)(&x),sizeof(x));

			YARPNameID idd = YARPSyncComm::BlockingReceive (id->getNameID(), imsg);
			///ACE_UNUSED_ARG(idd);

			out.Wait();
			
			sprintf(reply,"<%s,%d>", buf, x);
			cout << "Received message: " << buf << " and number " << x << endl;
			cout.flush();
			out.Post();
			double y = 432.1;
			YARPMultipartMessage omsg(2);
			omsg.Set(0,reply,sizeof(reply));
			omsg.Set(1,(char*)(&y),sizeof(y));
		
	
		///	YARPSyncComm::Reply(id.getNameID(),omsg);
			YARPSyncComm::Reply(idd, omsg);
			
			//YARPTime::DelayInSeconds(1.0);
			ct++;
		}
	}
};

int main(int argc, char *argv[])
{
	ACE_UNUSED_ARG (argc);
	ACE_UNUSED_ARG (argv);

	MyThread1 t1;
	MyThread2 t2;
	t2.Begin();
	YARPTime::DelayInSeconds(1.0);
	t1.Begin();
	YARPTime::DelayInSeconds(10.0);
	t1.End(0);
	cout << "t1 ended" << endl;
	YARPTime::DelayInSeconds(2.0);
	t2.End(0);
	return 0;
}

