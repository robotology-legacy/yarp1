///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test14.cpp,v 1.1 2005-11-04 16:31:38 gmetta Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>

YARPInputPortOf<int> in_port(YARPInputPort::NO_BUFFERS, YARP_UDP);
YARPOutputPortOf<int> out_port(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

class T1 : public YARPThread
{
public:
	virtual void Body()
	{
		int ct = 0;
		while (1)
		{
			out_port.Content() = ct;
			printf("Writing... %d\n", out_port.Content());
			out_port.Write(1);
			out_port.Content() = -1;
			printf("Writing... %d\n", out_port.Content());
			out_port.Write(1);
			ct++;
			if (ct >= 5)
				return;
		}
	}
};

class T2 : public YARPThread
{
public:
	virtual void Body()
	{
		while (1)
		{
			printf("Reading...\n");
			in_port.Read();
			printf("           Count is %d\n", in_port.Content());
			//YARPTime::DelayInSeconds(1);
			//in_port.Read();
			//printf("Count is %d\n", in_port.Content());
		}
	}
};


int main(int argc, char *argv[])
{
	ACE_UNUSED_ARG(argc);
	ACE_UNUSED_ARG(argv);

	T1 t1;
	T2 t2;

	in_port.Register("/in");
	out_port.Register("/out");

	YARPTime::DelayInSeconds(0.5);
	out_port.Connect("/in");

	YARPTime::DelayInSeconds(1.0);

	t1.Begin();

	t2.Body();

	// it complains on exit...
	return 0;
}

