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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: main.cpp,v 1.1 2003-04-22 09:06:38 gmetta Exp $
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
#include "YARPSocketDgram.h"

#define T1_SO_PORT 1111
#define T2_SI_PORT 2222
#define T2_SI_POOLSIZE 10
#define _LOCALNAME "localhost"
#define _REMOTENAME "localhost"

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

		YARPUniqueNameID name (YARP_UDP, T2_SI_PORT, _REMOTENAME);
		YARPOutputSocketDgram so;

		int ret = YARP_FAIL;
		do 
		{
			out.Wait();
			cout << "Starting out socket" << endl;
			cout.flush();
			out.Post();
			
			so.Prepare (T1_SO_PORT, name);
			ret = so.Connect ();
		} 
		while (ret != YARP_OK);

		while (1)
		{
			cout << "tick.\n" << endl;
			YARPTime::DelayInSeconds(2.0);
		}

#if 0
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
#endif
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
		YARPUniqueNameID id (YARP_UDP, T2_SI_PORT, _LOCALNAME);

		YARPInputSocketDgram si;
		si.Prepare (id, T2_SI_PORT, T2_SI_POOLSIZE);
		
		while (1)
		{
			out.Wait();
			cout << "Waiting for input" << endl;
			cout.flush();
			out.Post();

			YARPTime::DelayInSeconds(2.0);

#if 0
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
#endif
		}
	}
};

int main(int argc, char *argv[])
{
	MyThread1 t1;
//	MyThread1 t3;
	MyThread2 t2;
	t2.Begin();
	YARPTime::DelayInSeconds(20.0);
	t1.Begin();
//	t3.Begin();
	YARPTime::DelayInSeconds(1000.0);
	///t2.End();
	t1.End();
//	t3.End();

	YARPTime::DelayInSeconds(10.0);
	t2.End();
	return 0;
}

