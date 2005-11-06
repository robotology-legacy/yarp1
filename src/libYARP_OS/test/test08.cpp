///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test08.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
///
///
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <stdio.h>

#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPNetworkTypes.h>
#include <yarp/YARPScheduler.h>

extern int __debug_level;

NetInt32 foo;
char name[256];

YARPInputPortOf<NetInt32> in(YARPInputPort::NO_BUFFERS, YARP_UDP);
YARPOutputPortOf<NetInt32> out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
#define NET_NAME "local"


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

		while (!IsTerminated())
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

