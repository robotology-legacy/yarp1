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
/// $Id: CThreadImpl.h,v 1.2 2004-07-09 07:34:53 eshuy Exp $
///
///
//////////////////////////////////////////////////////////////////////////
//
// CThreadImpl.h
//
// 
// Thread class implementation. Inherit from it and overload the three pure virtual methods.
// Based on ACE.
//
// link:
// models.lib
// ace.lib
//

#ifndef __CThreadImpl__
#define __CThreadImpl__

#include <yarp/conf/YARPConfig.h>

#include <ace/config.h>
#include <ace/Thread_Manager.h>
#include <ace/Synch.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>

#include <yarp/YARPString.h>

const ACE_Time_Value _timeout_value(20,0);	// (20 sec) timeout value for the release (20 sec)

class CThreadImpl {

protected:
	bool isRunningF;		// running flag.	
	bool isSuspendedF;		// suspended flag
	ACE_Time_Value period;	// period.
	YARPString name;		// thread name.
	int thread_id;			// thread id
	int thread_priority;	// thread priority
	ACE_hthread_t  handle;  // os internal descriptor, it is filled by the thread itself
	bool end;

	ACE_Auto_Event synchro;	// event for init synchro
	
	virtual void doInit() =0;
	virtual void doLoop() =0;
	virtual void doRelease() =0;

	ACE_Lock_Adapter<ACE_Thread_Mutex> mutex;

public:
	CThreadImpl(const char *c, int r): name(c)
	{
		isRunningF = false;
		isSuspendedF = false;

		thread_id = -1;
		thread_priority = ACE_DEFAULT_THREAD_PRIORITY;

		period.set(0, r*1000);	//period here is usec
	}

	virtual ~CThreadImpl(void)
	{
		// close the thread
		if (isRunning()) 
		{
			terminate();
		}

	}

	inline void lock(void)
	{	
		// timeout ?
		mutex.acquire();
	}

	inline void unlock(void)
	{
		mutex.release();
	}

	virtual void start(bool wait = true)
	{
		ACE_UNUSED_ARG (wait);

		lock ();

		// create suspended.
#ifdef __WIN32__
		thread_id = ACE_Thread_Manager::instance ()->spawn((unsigned long (__cdecl *)(void *))real_thread, //thread function
														   this,		//thread parameter
														   THR_NEW_LWP||THR_SUSPENDED,
														   0,
														   0,
														   thread_priority
														  );
#else
		thread_id = ACE_Thread_Manager::instance ()->spawn((void *(*) (void *))real_thread, //thread function
														   this,		//thread parameter
														   THR_NEW_LWP||THR_SUSPENDED,
														   0,
														   0,
														   thread_priority
														  );
#endif

		///
		ACE_Thread_Manager::instance()->resume_grp(thread_id);
		isRunningF = true;

		// wait for synchro
		synchro.wait();
		
		unlock (); //check the position of this lock() (it was before the wait)
	}

	void terminate(bool timeout = true)
	{
		if (!isRunning())
			return;

		if (isSuspended())
			resume();
		
		lock ();

		end = true;
		bool timeout_flag = false;

		if (!timeout)
		{
			synchro.wait();
			timeout_flag = false;
		}
		else if (synchro.wait(&_timeout_value, 0) == -1)
		{
			#ifdef ACE_WIN32
			// win32 doesn't support kill signal, so we have to use the API
			TerminateThread (handle, -1);
			#else
			ACE_Thread_Manager::instance()->kill_grp(thread_id, SIGINT);
			#endif
			
			timeout_flag = true;
		}

		// after the syncro event or the kill the thread should end, just wait for it
		ACE_Thread_Manager::instance()->wait_grp(thread_id); /// troubles!
		
		isRunningF = false;
		isSuspendedF = false;

		#ifdef THREAD_STATS
			if (timeout_flag)
			{
				cout << "WARNING: " << name << " exited with timeout\n";
			}
			else
				cout << name << " exited gracefully.\n";

			cout << "Dumping statistics:\n";
			cout << "Estimated period: "<< thread_stats.period.get_mean();
			cout << " +/-" << thread_stats.period.get_std() << "\n";
			cout << "Estimated time: "<< thread_stats.time.get_mean();
			cout << " +/-" << thread_stats.time.get_std();
			cout << "\n";
		#endif /* THREAD_STATS */

		unlock ();
	}

	void setPriority(int p)
	{
		thread_priority = p;
		if (isRunning())
			ACE_OS::thr_setprio(handle, p);
	}

	int getPriority()
	{
		if (isRunning())
			ACE_OS::thr_getprio(handle, thread_priority);
		
		return thread_priority;
	}

	void setInterval(int i)
	{
		if (i >= 0)
		{
			bool was_running = isRunning();
			if (was_running)
			{
				suspend();
			}

			period.set(0,i*1000);
		

			if (was_running)
				resume();
		}
	}

	void suspend()
	{
		lock();
		ACE_Thread_Manager::instance ()->suspend_grp(thread_id);
		isSuspendedF = true;
		unlock();
	}

	void resume()
	{
		lock();
		isSuspendedF = false;
		ACE_Thread_Manager::instance ()->resume_grp(thread_id);
		unlock();
	}

	bool isRunning(void)
	{
		lock ();
		bool ret = isRunningF;
		unlock ();
		return ret;
	}

	bool isSuspended(void)
	{
		lock();
		bool ret = isSuspendedF;
		unlock();
		return ret;
	}

	const char *getName(void) const { return name.c_str(); }

	const int getThreadID(void) const { return thread_id; }

#ifdef __WIN32__
	static unsigned long * __cdecl real_thread(void *p_arg)
#else
	static unsigned long * real_thread(void *p_arg)
#endif
	{
		CThreadImpl *context = (CThreadImpl *) p_arg;

		ACE_High_Res_Timer	begin_timer;	// timer to estimate period
		ACE_High_Res_Timer	thread_timer;	// timer to estimate thread time
		ACE_Time_Value		est_time;		// thread time
		ACE_Time_Value		sleep_period;	// thread sleep

		context->end = false;		
		ACE_Thread_Manager::instance()->thr_self(context->handle);	//get thread os handle
		
		context->doInit();
		context->synchro.signal();

		// set synchro event
		// this first sleep in principle is not needed; it is to avoid checking the
		// first "est_period" value
		ACE_OS::sleep(context->period);
		while (!context->end)
		{
			// loop
			thread_timer.start();
			context->doLoop ();
			thread_timer.stop();
			thread_timer.elapsed_time(est_time);

			// compute sleep time
			sleep_period = context->period - est_time;
			
			if (sleep_period.usec() > 0 && sleep_period.sec() > 0)
			{
				////ACE_DEBUG ((LM_DEBUG, "waiting %d %d\n", sleep_period.usec(), sleep_period.sec()));
				ACE_OS::sleep(sleep_period);
			}
		}

		context->doRelease ();
		// since we have no control on the doRelease function
		// terminate is waiting on "syncro" with timout; the thread is killed if
		// timout accurs.
		context->synchro.signal();	
		// Release waits on ACE_Thread_Manager::instance()->wait_grp();
		return 0;
	}
};

#endif // CThreadImpl.h
