///
/// $Id: exec_test6.cpp,v 1.2 2003-04-18 09:25:49 gmetta Exp $
///
///
#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>

#include <stdio.h>

#include "YARPThread.h"
#include "YARPTime.h"

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

int main(int argc, char *argv[])
{
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

	return 0;
}

