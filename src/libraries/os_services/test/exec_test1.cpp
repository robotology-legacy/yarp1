/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, /// 
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: exec_test1.cpp,v 1.4 2003-04-24 08:49:33 gmetta Exp $
///

#include <conf/YARPConfig.h>
#include <ace/config.h>

#include <iostream>
//#include <mig4nto.h>

#include "YARPSemaphore.h"
#include "YARPThread.h"
#include "YARPTime.h"
#include "YARPScheduler.h"

#include <vector>
#include <list>
using namespace std;

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
      while (1)
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
	  goofy.Wait();
	  print.Wait();
	  cout << "*** reading val is " << val << endl;
	  print.Post();
	  goofy.Post();
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

