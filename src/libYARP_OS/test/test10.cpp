///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test10.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
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

#define for if(1) for

NetInt32 foo;

///YARPOutputPortOf<NetInt32> out;

extern int __debug_level;

class Thread1 : public YARPThread
{
protected:
	int id;

public:
	void SetID(int n_id) { id = n_id; }
  
	virtual void Body()
	{
		YARPInputPortOf<NetInt32> in;

		char buf[256];
		ACE_OS::sprintf(buf,"/etest/foo%d", id);
		in.Register(buf);
		YARPTime::DelayInSeconds(2);
		while (!IsTerminated())
		{
			ACE_OS::printf("Waiting for input\n");
			in.Read();
			ACE_OS::printf("Read %d\n", (int)in.Content());
		}
	}
};

#define MAX_T 30

int main(int argc, char *argv[])
{
	ACE_UNUSED_ARG(argc);
	ACE_UNUSED_ARG(argv);

	Thread1 *t1[MAX_T];

	__debug_level = 100;

	for (int i=0; i<MAX_T; i++)
	{
		t1[i] = new Thread1;
		ACE_ASSERT (t1[i] != NULL);
		t1[i]->SetID(i);
	}

	for (int i=0; i<MAX_T; i++)
	{
		t1[i]->Begin();
///	    YARPTime::DelayInSeconds(1.0);
	}

	YARPTime::DelayInSeconds(100000);

	for (int i=0; i<MAX_T; i++)
	{
		t1[i]->End();
	}

	return 0;
}

