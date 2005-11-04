///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test12.cpp,v 1.1 2005-11-04 16:31:38 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <string>
using namespace std;

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPNameService.h>

#define REG_TEST_NAME "/foo/the/robotic/dentist"
#define REG_LOCATE_NAME "/foo/the/robotic/dentist"
#define SRC_NAME "/foo/the/robotic/dentist:o"

//#define REG_TEST_NAME "localhost|1111"
//#define REG_LOCATE_NAME "localhost|1111"
//#define REG_TEST_NAME "/test/exec_test2"
//#define REG_LOCATE_NAME "/test/exec_test2"

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

			id = YARPNameService::LocateName(REG_LOCATE_NAME);
			YARPEndpointManager::CreateOutputEndpoint (*id);
			YARPEndpointManager::ConnectEndpoints (*id, SRC_NAME);

			if (!id->isValid())
			{
				YARPTime::DelayInSeconds(1);
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
			int result = YARPSyncComm::Send(*id,smsg,rmsg);
			x++;

			if (result>=0)
			{
				out.Wait();
				cout << "(result " << result << ") Got reply : " << reply << " and number " << y << endl;
				cout.flush();
				out.Post();
			}
			else
			{
				printf("connection is dead\n");
			}

			//YARPTime::DelayInSeconds(2.0);

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
		
		YARPUniqueNameID* id = YARPNameService::RegisterName(REG_TEST_NAME, "default", YARP_UDP, 11);

		///int result = YARPNameService::RegisterName(REG_TEST_NAME);

		YARPEndpointManager::CreateInputEndpoint (*id);

		out.Wait();
		///cout << "*** The assigned port is " << id->getAddressRef().get_port_number() << endl;
		cout.flush();
		out.Post();

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

			YARPNameID idd = YARPSyncComm::BlockingReceive(id->getNameID(), imsg);
			
			out.Wait();
			sprintf(reply,"<%s,%d>", buf, x);
			cout << "Received message: " << buf << " and number " << x << endl;
			cout.flush();
			out.Post();

			double y = 432.1;
			YARPMultipartMessage omsg(2);
			omsg.Set(0,reply,sizeof(reply));
			omsg.Set(1,(char*)(&y),sizeof(y));

			YARPSyncComm::Reply(idd,omsg);
			//YARPTime::DelayInSeconds(1.0);
			ct++;
		}
    }
};

int main(int argc, char *argv[])
{
///	__debug_level = 100;

	int s = 1, c = 1;
	if (argc>=2)
	{
		s = c = 0;
		for (unsigned int i=0; i<strlen(argv[1]); i++)
		{
			if (argv[1][i] == 's') s=1;
			if (argv[1][i] == 'c') c=1;
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
		YARPTime::DelayInSeconds(10000.0);
	}

	YARPTime::DelayInSeconds(20000.0);
	t2.End();
	t1.End();

	return 0;
}

