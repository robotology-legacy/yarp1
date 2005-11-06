///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: test01.cpp,v 2.0 2005-11-06 22:21:27 gmetta Exp $
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <iostream>
using namespace std;

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPList.h>
#include <yarp/YARPParseParameters.h>

YARPSemaphore product(0);

YARPSemaphore goofy(1);
int val = 0;

YARPSemaphore print(1);

class MyThread1 : public YARPThread
{
public:
	virtual void Body()
	{
		int ct = 0;
		while (!IsTerminated())
		{
			print.Wait();
			cout << "*** thread 1 at count " << ct << endl;
			cout << "*** thread 1 POSTING at " << ct << endl;
			print.Post();
			goofy.Wait();
			val = ct;
			goofy.Post();
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
		while (!IsTerminated())
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
		while (!IsTerminated())
		{
			product.Wait();
			print.Wait();
			cout << "*** thread 3 at count " << ct << endl;
			print.Post();
			goofy.Wait();
			print.Wait();
			cout << "*** reading val is " << val << endl;
			print.Post();
			goofy.Post();
			ct++;
		}
	}
};

//
// use: *.exe --1 --2
//
int main(int argc, char *argv[])
{
	bool a1 = false, a2 = false;

	MyThread1 t1;
	MyThread2 t2;
	MyThread3 t3;

	a1 = YARPParseParameters::parse (argc, argv, "-1");
	a2 = YARPParseParameters::parse (argc, argv, "-2");

	if (a1)
	{
		t3.Begin();
		cout << "Starting t3" << endl;
		YARPTime::DelayInSeconds(0.5);
		YARPScheduler::yield();
		cout << "Starting t1,t2" << endl;
		t1.Begin();
		t2.Begin();

		cout << "About to wait 10 sec" << endl;
		YARPTime::DelayInSeconds(10.0);
		cout << "Ending t3\n";
		t3.End();
		YARPTime::DelayInSeconds(5.0);
		cout << "Ending t2,t1" << endl;
		t2.End();
		t1.End();
	}

	if (a2)
	{
		YARPList<MyThread2 *> v;
		#define sz 100


		int i;
		for (i=0; i<sz; i++)
		{
			MyThread2 *t = new MyThread2();
			v.push_back(t);
		}

		YARPListIterator<MyThread2 *> it(v);
		i = 0;
		for (; !it.done(); it++, i++)
		{
			cout << "Starting thread " << i << endl;
			(**it).Begin();
		}

		YARPTime::DelayInSeconds(2.7);

		it.go_head();
		for (; !it.done(); it++)
		{
			(**it).End();
		}

		YARPTime::DelayInSeconds(2.0);
	}

	return 0;
}

