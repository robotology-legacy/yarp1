///
/// $Id: exec_test5.cpp,v 1.2 2003-04-18 09:25:49 gmetta Exp $
///
///
#include <conf/YARPConfig.h>
#include <ace/config.h>

#include <stdio.h>
#include <iostream>
#include <string.h>

#include "YARPSemaphore.h"
#include "YARPThread.h"
#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPNameService.h"


///#define REG_TEST_NAME "/foo/the/robotic/dentist"
///#define REG_LOCATE_NAME "/foo/the/robotic/dentist"

#define REG_TEST_NAME "localhost|1111"
#define REG_LOCATE_NAME "localhost|1111"
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

		YARPUniqueNameID id;

		do 
		{
			out.Wait();
			cout << "Looking up name" << endl;
			cout.flush();
			out.Post();
			id = YARPNameService::LocateName(REG_LOCATE_NAME);
			YARPEndpointManager::CreateOutputEndpoint (id);
			YARPEndpointManager::ConnectEndpoints (id);

			if (!id.isValid())
			{
				YARPTime::DelayInSeconds(1);
			}
		} 
		while (!id.isValid());

		int x = 42;
		while (1)
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

			int result = YARPSyncComm::Send(id.getNameID(),smsg,rmsg);
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
		YARPUniqueNameID id;

		char buf[128] = "Not set";
		char reply[128] = "Not set";
		int ct = 0;
		YARPTime::DelayInSeconds(0.01);
		
		id = YARPNameService::RegisterName(REG_TEST_NAME);
		YARPEndpointManager::CreateInputEndpoint (id);

		out.Wait();
		cout << "*** The assigned port is " << id.getAddressRef().get_port_number() << endl;
		cout.flush();
		out.Post();

		while (1)
		{
			out.Wait();
			cout << "Waiting for input" << endl;
			cout.flush();
			out.Post();
			YARPMultipartMessage imsg(2);
			int x = 999;
			imsg.Set(0,buf,sizeof(buf));
			imsg.Set(1,(char*)(&x),sizeof(x));

			YARPNameID idd = YARPSyncComm::BlockingReceive(id.getNameID(), imsg);

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
	int s = 1, c = 1;

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
		YARPTime::DelayInSeconds(10000.0);
	}
	
	YARPTime::DelayInSeconds(20.0);
	t2.End();
	t1.End();
	return 0;
}

