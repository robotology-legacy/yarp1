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
/// $Id: exec_test5.cpp,v 1.5 2003-05-01 22:51:20 gmetta Exp $
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

#define __UDP

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
		
#ifdef __UDP
		id = YARPNameService::RegisterName(REG_TEST_NAME, YARP_UDP, 11);
#else
		id = YARPNameService::RegisterName(REG_TEST_NAME);
#endif
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
		YARPTime::DelayInSeconds(10.0);
	}
	
	YARPTime::DelayInSeconds(20000.0);
	t2.End();
	t1.End();
	return 0;
}

