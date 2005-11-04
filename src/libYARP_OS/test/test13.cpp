///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test13.cpp,v 1.1 2005-11-04 16:31:38 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <string.h>

#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPNetworkTypes.h>
#include <yarp/YARPScheduler.h>

NetInt32 foo;

#define LEN 600000 ///MAX_PACKET-100

class Msg
{
public:
	char msg[LEN];
};

YARPInputPortOf<Msg> in(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
YARPOutputPortOf<Msg> out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
///YARPInputPortOf<Msg> in;
///YARPOutputPortOf<Msg> out;

class Thread1 : public YARPThread
{
public:
	Thread1 (const char *name) : YARPThread()
	{
		in.Register(name);
	}

	virtual void Body()
	{
		///in.Register("/foo/the/rampaging/frog");
		//YARPTime::DelayInSeconds(2);
		double start = YARPTime::GetTimeAsSeconds();
		double prevtime = start;
		double cumul = 0;
		int counter = 0;
		bool first = true;

		while (!IsTerminated())
		{
			///printf("Waiting for input\n");
			in.Read();
			if (first)
			{
				start = YARPTime::GetTimeAsSeconds();
				prevtime = start;
				cumul = 0;
				counter = 0;
				first = false;
			}

			double now = YARPTime::GetTimeAsSeconds();
			cumul += (now - prevtime);
			counter ++;
			prevtime = now;

			///printf("Read %s\n", in.Content().msg);
			if ((counter % 10) == 0)
			{
				printf("Read at %d, %s\n", counter, in.Content().msg);
				printf("average time : %f\n", cumul / counter);
			}
		}
	}
};

class Thread2 : public YARPThread
{
public:
	virtual void Body()
	{
		out.Register("/foo/send");
		//YARPTime::DelayInSeconds(2);
		///printf("Step1\n");
		///out.Connect("/foo/the/rampaging/frog");
		///printf("Step1.5\n");
		//YARPTime::DelayInSeconds(2);
		int ct = 1;
		while (!IsTerminated())
		{
			///printf("Step2\n");
			sprintf(out.Content().msg,"foo");
			///printf("Step3\n");
			ct++;
			///printf("Writing %s\n", out.Content().msg);
			//	  out.Write(true);
			out.Write(true);
			YARPTime::DelayInSeconds(.025);
		}
	}
};

extern int __debug_level;

int main(int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling();

///	__debug_level = 80;
	Thread1 *t1 = NULL;

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

	if (s && argc == 3 && !c)
	{
		t1 = new Thread1(argv[2]);
		t1->Begin();
	}
	else
	if (s)
	{
		t1 = new Thread1("/foo/recv");
		t1->Begin();
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

