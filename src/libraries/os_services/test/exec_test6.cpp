#include <stdio.h>

#include "YARPThread.h"
#include "YARPTime.h"

YARPThreadSpecific<int> my_int;

class MyThread : public YARPThread
{
public:
  int tid;
  MyThread(int n_tid) : tid(n_tid) {}
  virtual void Body()
    {
      my_int.Content() = 0;
      while (1)
	{
	  //printf("Peep %d\n", tid);
	  printf("Thread %d at count %d\n", tid, my_int.Content());
	  my_int.Content()++;
	  YARPTime::DelayInSeconds(3);
	}
    }
};

void main()
{
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
}

