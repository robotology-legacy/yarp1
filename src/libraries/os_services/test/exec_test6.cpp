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
/// $Id: exec_test6.cpp,v 1.4 2003-04-24 08:49:34 gmetta Exp $
///
///
#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>

#include <stdio.h>

#include "YARPThread.h"
#include "YARPRateThread.h"
#include "YARPTime.h"
#include "YARPScheduler.h"

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

