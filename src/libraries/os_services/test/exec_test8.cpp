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
/// $Id: exec_test8.cpp,v 1.45 2004-05-15 22:09:57 gmetta Exp $
///
///
#include <conf/YARPConfig.h>
#include <YARPAll.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <stdio.h>

#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPNetworkTypes.h"
#include "YARPScheduler.h"

extern int __debug_level;

NetInt32 foo;
char name[256];

YARPInputPortOf<NetInt32> in(YARPInputPort::NO_BUFFERS, YARP_UDP);
YARPOutputPortOf<NetInt32> out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
#define NET_NAME "local"

///YARPInputPortOf<NetInt32> in;
///YARPOutputPortOf<NetInt32> out;

class Thread1 : public YARPThread
{
public:
//	YARPInputPortOf<NetInt32> in;
	virtual void Body()
	{
		///int er = in.Register("/foo/the/rampaging/frog");
		int er = in.Register(name, NET_NAME);
		if (er != YARP_OK)
		{
			ACE_DEBUG ((LM_DEBUG, "Thread1: can't register port name, bailing out\n"));
			return;
		}

		YARPTime::DelayInSeconds(2);

		while (1)
		///for (int i = 0; i < 2; i++)
		{
			printf("Waiting for input\n");
			in.Read();
			ACE_OS::printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$Read %d\n", (int)in.Content());
			YARPTime::DelayInSeconds(5);
		}
	}
};

class Thread2 : public YARPThread
{
public:
//	YARPOutputPortOf<NetInt32> out;
	virtual void Body()
	{
		///int er = out.Register("/foo/the/rampaging/fly");
		int er = out.Register(name, NET_NAME);
		if (er != YARP_OK)
		{
			ACE_DEBUG ((LM_DEBUG, "Thread2: can't register port name, bailing out\n"));
			return;
		}

		YARPTime::DelayInSeconds(2);
		printf("Step1\n");
///		out.Connect("/foo/the/rampaging/frog");
		printf("Step1.5\n");
		YARPTime::DelayInSeconds(2);
		int ct = 1;

		for (int i = 0; i < 100; i++)
///		while (1)
		{
			printf("Step2\n");
			out.Content() = ct;
			printf("Step3\n");
			ct++;
			printf("$$$$$$$$$$$$$$$$$$$$$$$$$Writing %d\n", (int)out.Content());
			out.Write(1);
			YARPTime::DelayInSeconds(1);
		}
	}
};

int main(int argc, char *argv[])
{
//	__debug_level = 80;


	Thread1 t1;
	Thread2 t2;
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

	if (argc == 3 && s)
	{
		ACE_OS::sprintf (name, "%s:s", argv[2]);
		ACE_OS::printf ("name is %s\n", name);
	}
	else 
	if (argc == 3 && c)
	{
		ACE_OS::sprintf (name, "%s:c", argv[2]);
		ACE_OS::printf ("name is %s\n", name);
	}
	else
	{
		ACE_OS::printf ("pls, provide a channel name\n");
		return -1;
	}

	if (s)
	{
		t1.Begin();
	}

	if (c)
	{
		if (s)
		{
			YARPTime::DelayInSeconds(1);
		}
		t2.Begin();
	}

	if (s && c)
	{
		t1.End(0);
		t2.End(0);
		ACE_OS::printf ("can't be server and client at the same time\n");
		return -1;
	}

	if (s)
	{
		t1.Join ();
///		YARPTime::DelayInSeconds(6000.0);
	}

	if (c)
	{
		t2.Join ();
	}

	///YARPTime::DelayInSeconds(6000.0);

	return 0;
}

