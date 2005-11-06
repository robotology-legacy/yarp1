///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test06.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
///
///
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>

#include <stdio.h>

#include <yarp/YARPThread.h>
#include <yarp/YARPRateThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPScheduler.h>

///
///
///
YARPThreadSpecific<int> my_int;
ACE_TSS<ACE_TSS_Type_Adapter <int> > i;

class MyThread : public YARPThread
{
public:
	int tid;
	MyThread(int n_tid) : tid(n_tid) {}
	virtual void Body()
	{
		my_int.Content() = tid;
		///(*i) = tid;
		i->operator int & () = tid;
		while (1)
		{
			//printf("Peep %d\n", tid);
			printf("Thread %d at count %d\n", tid, my_int.Content()); /// *i); //
			//my_int.Content() += (tid*10);
			YARPTime::DelayInSeconds(3);
		}
	}
};

///
class MyThreadRated : public YARPRateThread
{
public:
	virtual void doInit () { _cnt = 0; }
	virtual void doLoop () 
	{
		_cnt += 10;
		printf ("RateThread at count %d\n", _cnt);
	}
	virtual void doRelease () {}

	MyThreadRated () : YARPRateThread ("test thread", 1000) {}

	int _cnt;
};

///
///
///
int main(int argc, char *argv[])
{
	ACE_UNUSED_ARG (argc);
	ACE_UNUSED_ARG (argv);

	YARPScheduler::setHighResScheduling ();
	///	data->PrepareChunk (100);

	MyThread t1(1), t2(2), t3(3);
	t1.Begin();
	YARPTime::DelayInSeconds(1);
	t2.Begin();
	YARPTime::DelayInSeconds(1);
	t3.Begin();
	YARPTime::DelayInSeconds(15);
	t3.End();
	t2.End();
	t1.End();

	MyThreadRated t4;
	t4.start ();
	YARPTime::DelayInSeconds(5.5);
	t4.terminate ();

	return 0;
}

