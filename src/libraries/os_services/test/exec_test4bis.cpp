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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: exec_test4bis.cpp,v 1.2 2003-05-16 00:02:31 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

///#include <stdio.h>

#ifdef __WIN_MSVC__
#	include <iostream>
#else
#	include <iostream.h>
#endif

#include "YARPSemaphore.h"
#include "YARPThread.h"
#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPSocketNameService.h"
#include "YARPNativeNameService.h"

#define REG_TEST_NAME "/pippo1"
#define REG_LOCATE_NAME "/pippo1"
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
		cout << "Here" << endl;
		cout.flush();

		do 
		{
			out.Wait();
			cout << "Looking up name" << endl;
			cout.flush();
			out.Post();
			
			id = YARPNameService::LocateName(REG_LOCATE_NAME);
			if (id.getServiceType () == YARP_NO_SERVICE_AVAILABLE)
			{
				ACE_DEBUG ((LM_DEBUG, "can't locate name, bailing out\n"));
				return;
			}

///			id.allocP2(1);
///			id.getP2Ptr()[0] = 3000;
			YARPEndpointManager::CreateOutputEndpoint (id);
			YARPEndpointManager::ConnectEndpoints (id);

			if (!id.isValid())
			{
				YARPTime::DelayInSeconds(0.2);
			}
		} 
		while (!id.isValid());

		while (1)
		{
			out.Wait();
			cout << "Preparing to send: " << txt << endl;
			cout.flush();
			out.Post();

			YARPSyncComm::Send(id.getNameID(),txt,sizeof(txt),reply,sizeof(reply));

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

#ifdef __QNX6__
		YARPUniqueNameID id = YARPNameService::RegisterName(REG_TEST_NAME, YARP_QNET, YARPNativeEndpointManager::CreateQnetChannel());
#else
		/// connect to name server and get ip and port.
		YARPUniqueNameID id = YARPNameService::RegisterName(REG_TEST_NAME, YARP_UDP, 11);
#endif
		if (id.getServiceType() == YARP_NO_SERVICE_AVAILABLE)
		{
			ACE_DEBUG ((LM_DEBUG, "can't register name, bailing out\n"));
			return;
		}

		/// create the input endpoint.
		YARPEndpointManager::CreateInputEndpoint (id);

		while (1)
		{
			out.Wait();
			cout << "Waiting for input" << endl;
			cout.flush();
			out.Post();
			
			///YARPNameID id = YARPSyncComm::BlockingReceive(YARPNameID(),buf,sizeof(buf));
			YARPNameID reply_id = YARPSyncComm::BlockingReceive(id.getNameID(), buf, sizeof(buf));

			sprintf(reply,"Got %s", buf);
			out.Wait();
			cout << "Received message: " << buf << endl;
			cout.flush();
			out.Post();
			
			YARPSyncComm::Reply(reply_id, reply, sizeof(reply));
			//YARPTime::DelayInSeconds(1.0);
			ct++;
		}
	}
};

int main(int argc, char *argv[])
{
	ACE_UNUSED_ARG(argc);
	ACE_UNUSED_ARG(argv);

	MyThread1 t1;
//	MyThread1 t3;
	MyThread2 t2;
	t2.Begin();
	YARPTime::DelayInSeconds(5.0);
	t1.Begin();
//	t3.Begin();
	YARPTime::DelayInSeconds(10.0);
	///t2.End();
	t1.End();
//	t3.End();

	YARPTime::DelayInSeconds(10.0);
	t2.End();
	return 0;
}

