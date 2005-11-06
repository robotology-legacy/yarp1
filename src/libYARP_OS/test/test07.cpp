///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test07.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
///
///

#include <stdio.h>
#include <string.h>

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <yarp/YARPTime.h>
#include <yarp/YARPSyncComm.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPNetworkTypes.h>
#include <yarp/YARPThread.h>


class MyC : public YARPPortContent
{
public:
	NetInt32 datum;
	MyC() { datum = 0; }
  
	virtual int Read(YARPPortReader& reader)
	{
		return reader.Read((char*)(&datum),sizeof(datum));
	}
  
	virtual int Write(YARPPortWriter& writer)
	{
		return writer.Write((char*)(&datum),sizeof(datum));
	}

	virtual int Recycle() { return 0; }
};

class MyInputPort : public YARPInputPort
{
public:
	virtual YARPPortContent *CreateContent() { return new MyC; }
};

class MyOutputPort : public YARPOutputPort
{
public:
	virtual YARPPortContent *CreateContent() { return new MyC; }
};

MyInputPort in;
MyOutputPort out;

class Thread1 : public YARPThread
{
public:
	virtual void Body()
	{
		in.Register("/foo/the/rampaging/frog");
		YARPTime::DelayInSeconds(2);
		while (1)
		{
			printf("Waiting for input\n");
			in.Read();
			printf("Read %d\n", (int)((MyC*)(&in.Content()))->datum);
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
		while (1)
		{
			printf("Step2\n");
			((MyC*)(&out.Content()))->datum = ct;
			printf("Step3\n");
			ct++;
			printf("Writing %d\n", (int)((MyC*)(&out.Content()))->datum);
			out.Write();
			YARPTime::DelayInSeconds(4);
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
		for (int i=0; i<(int)strlen(argv[1]); i++)
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

