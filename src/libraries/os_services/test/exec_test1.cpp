//
// $Id: exec_test1.cpp,v 1.3 2003-04-10 15:01:35 gmetta Exp $
//

#include <conf/YARPConfig.h>
#include <ace/config.h>

#include <iostream.h>
//#include <mig4nto.h>

#include "YARPSemaphore.h"
#include "YARPThread.h"
#include "YARPTime.h"
#include "YARPScheduler.h"

#include <vector>
#include <list>
using namespace std;

YARPSemaphore product(0);

YARPSemaphore access(1);
int val = 0;

YARPSemaphore print(1);

class MyThread1 : public YARPThread
{
public:
  virtual void Body()
    {
      int ct = 0;
      while (1)
	{
	  print.Wait();
	  cout << "*** thread 1 at count " << ct << endl;
	  cout << "*** thread 1 POSTING at " << ct << endl;
	  print.Post();
	  access.Wait();
	  val = ct;
	  access.Post();
	  product.Post();
	  print.Wait();
	  cout << "*** thread 1 POSTED at " << ct << endl;
	  print.Post();
	  print.Wait();
	  cout << "*** thread 1 DELAYING at " << ct << endl;
	  print.Post();
	  YARPTime::DelayInSeconds(3.0);
	  ct++;
	}
    }
};

class MyThread2 : public YARPThread
{
public:
  int operator == (const MyThread2& other)
    {
      return 0;
    }
  int operator != (const MyThread2& other)
    {
      return 0;
    }
  int operator > (const MyThread2& other)
    {
      return 0;
    }
  int operator < (const MyThread2& other)
    {
      return 0;
    }
  int operator >= (const MyThread2& other)
    {
      return 0;
    }
  int operator <= (const MyThread2& other)
    {
      return 0;
    }

  virtual void Body()
    {
      int ct = 0;
      YARPTime::DelayInSeconds(0.01);
      while (1)
	{
	  print.Wait();
	  cout << "thread 2 at count " << ct << endl;
	  print.Post();
	  YARPTime::DelayInSeconds(1.0);
	  ct++;
	}
    }
};

class MyThread3 : public YARPThread
{
public:
  virtual void Body()
    {
      int ct = 0;
      while (1)
	{
	  product.Wait();
	  print.Wait();
	  cout << "*** thread 3 at count " << ct << endl;
	  print.Post();
	  access.Wait();
	  print.Wait();
	  cout << "*** reading val is " << val << endl;
	  print.Post();
	  access.Post();
	  ct++;
	}
    }
};


int main()
{
//  mig4nto_init();
  MyThread1 t1;
  MyThread2 t2;
  MyThread3 t3;

  if (1)
  {
    t3.Begin();
    YARPTime::DelayInSeconds(0.5);
    YARPScheduler::yield();
    t1.Begin();
    t2.Begin();
    YARPTime::DelayInSeconds(10.0);
    cout << "Ending t3\n";
    t3.End();
    YARPTime::DelayInSeconds(5.0);
    t2.End();
    t1.End();
  }

  if (0)
    {
      list<MyThread2> v;
#define sz 100
      //MyThread2 v[sz];

      for (int i=0; i<sz; i++)
	{
	  v.push_back(MyThread2());
	  v.back().Begin();
	}

      YARPTime::DelayInSeconds(2.7);
      
      for (list<MyThread2>::iterator it=v.begin(); it!=v.end(); it++)
	{
	  (*it).End();
	}
      
      YARPTime::DelayInSeconds(2.0);
    }

  return 0;
}

