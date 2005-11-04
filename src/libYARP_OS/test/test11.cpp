///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test11.cpp,v 1.1 2005-11-04 16:31:38 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPNetworkTypes.h>

NetInt32 foo;

YARPInputPortOf<NetInt32> in;
YARPOutputPortOf<NetInt32> out;

class Thread1 : public YARPThread
{
public:
	virtual void Body()
	{
		in.Register("/foo/the/rampaging/frog");
		YARPTime::DelayInSeconds(2);
		while (!IsTerminated())
		{
			printf("Waiting for input\n");
			in.Read();
			printf("Read %d\n", (int)in.Content());
		}
	}
};

class Thread2 : public YARPThread
{
public:
	virtual void Body()
    {
		out.Register("/foo/the/rampaging/fly");
		YARPTime::DelayInSeconds(2);
		printf("Step1\n");
		out.Connect("/foo/the/rampaging/frog");
		printf("Step1.5\n");
		YARPTime::DelayInSeconds(2);
		int ct = 1;
		while (!IsTerminated())
		{
			printf("Step2\n");
			out.Content() = ct;
			printf("Step3\n");
			ct++;
			printf("Writing %d\n", (int)out.Content());
			//	  out.Write(true);
			out.Write();
			//	  YARPTime::DelayInSeconds(0.02);
		}
    }
};

int main(int argc, char *argv[])
{
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

	YARPTime::DelayInSeconds(1000000.0);
	return 0;
}

