///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test05.cpp,v 1.1 2005-11-04 16:31:38 gmetta Exp $
///
///
#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <iostream>
using namespace std;

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPNameService.h>
#include <yarp/YARPScheduler.h>

#define REG_TEST_NAME "localhost|1111"
#define REG_LOCATE_NAME "localhost|1111"
#define SRC_NAME "source|YYYY"
#define NET_NAME "default"

///#define __UDP
#define __MCAST
#define __PRINT

YARPSemaphore out(1);

extern int __debug_level;

class MyThread1 : public YARPThread
{
public:
	virtual void Body()
	{
		int ct = 0;
		char txt[128] = "Hello there";
		char reply[128] = "Not set";

		YARPUniqueNameID* id;
		YARPUniqueNameID* idc;

		do 
		{
			out.Wait();
			cout << "Looking up name" << endl;
			cout.flush();
			out.Post();
			id = YARPNameService::LocateName(REG_LOCATE_NAME, NET_NAME, YARP_MCAST);
			YARPEndpointManager::CreateOutputEndpoint (*id);

			idc = YARPNameService::LocateName(REG_LOCATE_NAME, NET_NAME, YARP_UDP);
			YARPEndpointManager::ConnectEndpoints (*idc, SRC_NAME);

			if (!id->isValid() || !idc->isValid())
			{
				YARPTime::DelayInSeconds(1);
			}
		} 
		while (!id->isValid() || !idc->isValid());

		int x = 42;
		while (!IsTerminated())
		{
#ifdef __PRINT
			out.Wait();
			cout << "Preparing to send: " << txt << endl;
			cout.flush();
			out.Post();
#endif

			YARPMultipartMessage smsg(2);
			smsg.Set(0,txt,sizeof(txt));
			smsg.Set(1,(char*)(&x),sizeof(x));
			YARPMultipartMessage rmsg(2);
			double y = 999;
			rmsg.Set(0,reply,sizeof(reply));
			rmsg.Set(1,(char*)(&y),sizeof(y));
#ifdef __PRINT
			cout << "*** sending: " << txt << " and number " << x << endl;
			cout.flush();
#endif
			int result = YARPSyncComm::Send(id->getNameID(),smsg,rmsg);
			x++;

			if (result>=0)
			{
#ifdef __PRINT
				out.Wait();
				cout << "(result " << result << ") Got reply : " << reply << " and number " << y << endl;
				cout.flush();
				out.Post();
#else
				if ((x % 10) == 0)
				{
					cout << "sent msg : " << x << endl;
					cout << "(result " << result << ") Got reply : " << reply << " and number " << y << endl;
					cout.flush();
				}
#endif
			}
			else
			{
				cout << "connection is dead" << endl;
			}

			YARPTime::DelayInSeconds(0.01);

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
		YARPUniqueNameID* id;

		char buf[128] = "Not set";
		char reply[128] = "Not set";
		int ct = 0;
		YARPTime::DelayInSeconds(0.01);
		
#if defined(__UDP)
		id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_UDP, 11);
#elif defined(__MCAST)
		id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_MCAST, 11);
#else
		id = YARPNameService::RegisterName(REG_TEST_NAME, NET_NAME, YARP_TCP, 11);
#endif
		YARPEndpointManager::CreateInputEndpoint (*id);

		out.Wait();
		///cout << "*** The assigned port is " << id->getAddressRef().get_port_number() << endl;
		///cout.flush();
		out.Post();

		double start = YARPTime::GetTimeAsSeconds();
		double prevtime = start;
		double cumul = 0;
		int counter = 0;
		bool first = true;

		while (!IsTerminated())
		{
#ifdef __PRINT
			out.Wait();
			cout << "Waiting for input" << endl;
			cout.flush();
			out.Post();
#endif
			YARPMultipartMessage imsg(2);
			int x = 999;
			imsg.Set(0,buf,sizeof(buf));
			imsg.Set(1,(char*)(&x),sizeof(x));

			YARPNameID idd = YARPSyncComm::BlockingReceive(id->getNameID(), imsg);
			if (first)
			{
				start = YARPTime::GetTimeAsSeconds();
				prevtime = start;
				cumul = 0;
				counter = 0;
				first = false;
			}

			sprintf(reply,"<%s,%d>", buf, x);
#ifdef __PRINT
			out.Wait();
			cout << "Received message: " << buf << " and number " << x << endl;
			cout.flush();
			out.Post();
#endif
			double y = 432.1;
			YARPMultipartMessage omsg(2);
			omsg.Set(0,reply,sizeof(reply));
			omsg.Set(1,(char*)(&y),sizeof(y));
			
			YARPSyncComm::Reply(idd,omsg);
			//YARPTime::DelayInSeconds(1.0);
			ct++;

			double now = YARPTime::GetTimeAsSeconds();
			cumul += (now - prevtime);
			counter ++;
			prevtime = now;

#ifndef __PRINT
			if ((x % 10) == 0)
			{
				cout << "Received message: " << buf << " and number " << x << endl;
				cout << "average thread time : " << cumul/counter << endl;
				cout.flush();
			}
#endif
		}
	}
};

int main(int argc, char *argv[])
{
	int s = 1, c = 1;
	YARPScheduler::setHighResScheduling();
 
	__debug_level = 80;

	if (argc>=2)
	{
		s = c = 0;
		for (unsigned int i=0; i<strlen(argv[1]); i++)
		{
			if (argv[1][i] == 's') s = 1;
			if (argv[1][i] == 'c') c = 1;
		}
	}

	MyThread1 t1;
	MyThread2 t2;
	if (s)
	{
		t2.Begin();
		YARPTime::DelayInSeconds(1.0);
	}

	if (c)
	{
		t1.Begin();
	}

	if (!s)
	{
		YARPTime::DelayInSeconds(10.0);
	}
	
	YARPTime::DelayInSeconds(20.0);
	t2.End();
	t1.End();
	return 0;
}

