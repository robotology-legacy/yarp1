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
/// $Id: YARPRateThread.h,v 1.3 2004-07-09 13:45:59 eshuy Exp $
///
///

//
// 
#ifndef __YARPRateThreadh__
#define __YARPRateThreadh__

//////////////////////////////////////////////////////////////////////////
//
// YARPRateThread.h
//
// 
// Thread class implementation. Inherit from it and overload the three 
// pure virtual methods.
// Based on ACE.
//
// link:
// ace.lib
//
// WIN32: see SystemUtils::set_high_res_scheduling() if you want to do precise 
// scheduling (< 10/15 ms depening on the machine) --> link winmm.lib
//
// THREAD_STATS if you want to test scheduling statistics 
// 
// September 2002 -- by nat
// 
// September 2002 -- by nat -- modified release flag, added timeout
//
// TODO: check thread priorities

// define this if you want scheduling statistics
// #define THREAD_STATS
//	this is compiled out because it is rather time-consuming.

#if defined(__WIN32__) || defined(__QNX6__) || defined(__LINUX__)

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Thread_Manager.h>
#include <ace/Synch.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>

#ifdef THREAD_STATS
#include "IterativeStats.h" //from models lib link models.lib/modelsdb.lib
#endif

#include <yarp/YARPString.h>

#ifdef __QNX6__
#include <sys/neutrino.h>
#endif

const ACE_Time_Value _timeout_value(20,0);	// (20 sec) timeout value for the release (20 sec)

#ifdef THREAD_STATS
typedef struct thr_stts
{
	IterativeStats period;
	IterativeStats time;
} thread_statistics;
#endif

class YARPRateThread 
{
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

#ifdef THREAD_STATS
	thread_statistics thread_stats;
#endif

	ACE_Lock_Adapter<ACE_Thread_Mutex> mutex;

public:
	YARPRateThread(const char *c, int r): name(c)
	{
		isRunningF = false;
		isSuspendedF = false;

		thread_id = -1;
		thread_priority = ACE_DEFAULT_THREAD_PRIORITY;

		period.set(0, r*1000);	//period here is usec
	}

	virtual ~YARPRateThread(void)
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

#ifdef __WIN32__
		// create suspended.
		thread_id = ACE_Thread_Manager::instance ()->spawn((unsigned long (__cdecl *)(void *))real_thread, //thread function
														   this,		//thread parameter
														   THR_NEW_LWP||THR_SUSPENDED,
														   0,
														   0,
														   thread_priority
															);
#else
		// create suspended.
		thread_id = ACE_Thread_Manager::instance ()->spawn(real_thread, //thread function
								   this,		//thread parameter
								   THR_NEW_LWP||THR_SUSPENDED,
								   0,
								   0,
								   thread_priority
							 	  );
#endif
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
#if defined(__WIN32__)
			// win32 doesn't support kill signal, so we have to use the API
			TerminateThread (handle, -1);
#elif defined(__QNX6__)
	                ThreadDestroy (thread_id, -1, (void *)-1);
#elif defined(__LINUX__)
			pthread_cancel (thread_id);
#else
#error "destroy thread: not implemented for the specified architecture"
#endif
			timeout_flag = true;
		}

		// after the syncro event or the kill the thread should end, just wait for it
		ACE_Thread_Manager::instance()->wait_grp(thread_id);		
		
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

#ifdef THREAD_STATS
		thread_stats.period.reset();
		thread_stats.time.reset();
#endif
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

	static void *real_thread(void *p_arg)
	{
		YARPRateThread *context = (YARPRateThread *) p_arg;

		ACE_High_Res_Timer	begin_timer;	// timer to estimate period
		ACE_High_Res_Timer	thread_timer;	// timer to estimate thread time
		ACE_Time_Value		est_time;		// thread time
#ifdef THREAD_STATS
		ACE_Time_Value		est_period;		// thread period
#endif
		ACE_Time_Value		sleep_period;	// thread sleep

		context->end = false;		
		ACE_Thread_Manager::instance()->thr_self(context->handle);	//get thread os handle
		
		/*
		char buffer[255];
		HANDLE w32timer;
		sprintf (buffer, "Periodic timer%s%u\0", context->name.c_str());
		w32timer = CreateWaitableTimer (NULL, FALSE, buffer);
		LARGE_INTEGER tm;
		tm.LowPart = 0;
		tm.HighPart = 0;
		BOOL ret = SetWaitableTimer(w32timer, &tm, context->period.msec(), NULL, NULL, TRUE);
		*/
		
		context->doInit();
#ifdef THREAD_STATS
				begin_timer.start();
#endif /* THREAD_STATS */
		context->synchro.signal();
		// set synchro event
		// this first sleep in principle is not needed; it is to avoid checking the
		// first "est_period" value
		ACE_OS::sleep(context->period);
		while (!context->end)
		{
#ifdef THREAD_STATS
				// est period
				begin_timer.stop();
				begin_timer.elapsed_time(est_period);
				begin_timer.start();
				// context->thread_stats.period+=est_period.msec();
				context->thread_stats.period+=est_period.usec()/1000.0;
#endif /* THREAD_STATS */

			// loop
			thread_timer.start();
			context->doLoop ();
			thread_timer.stop();
			thread_timer.elapsed_time(est_time);
			
#ifdef THREAD_STATS
				// context->thread_stats.time+=est_time.msec();
				context->thread_stats.time+=est_time.usec()/1000.0;
#endif /* THREAD_STATS */

			// compute sleep time
			sleep_period = context->period-est_time;
			
			if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
				sleep_period.set(0,0);

			ACE_OS::sleep(sleep_period);
			// WaitForSingleObject(w32timer, INFINITE);
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

#elif defined(__QNX4__)

#error "YARPRateThread: can't compile QNX4, this code is no longer maintained"

/// #ifdef __QNX__
/// #ifdef __QNX4__		/// not quite sure it's ok on QNX6

#include <yarp/YARPSemaphore.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/proxy.h>
#include <sys/wait.h>
#include <sys/sched.h>
#include <time.h>
#include <signal.h>
#include <process.h>

#ifndef DEFAULT_THREAD_STACK_SIZE
#define DEFAULT_THREAD_STACK_SIZE 8000
#endif

// How to use YARPRateThread:
//	1) Derive from it.
//	2) Override DoInit, DoLoop, DoRelease pure virtual methods.
//	3) Use Start, Terminate to start/stop the thread.
//	4) The constructor takes as argument the period for the timer.
//	5) DoInit is called once after the thread is started.
//	6) DoLoop is called periodically (it waits on the timer).
//	7) DoRelease is called upon calling Terminate.
//
class YARPRateThread {
private:
	YARPRateThread& operator=(const YARPRateThread&);
	YARPRateThread(const YARPRateThread&);

protected:
	char *stack;
	int pid;			// this was the thread id.

	bool isRunningF;	// running flag.	
	int period;			// period [ms].
	const char *name;	// thread name.

	pid_t proxy_end;
	pid_t proxy_synchro;

	YARPSemaphore   mutex;		// mutex. 
	
	timer_t timer;		// waitable timer (a sort of).
	pid_t proxy_timer;
	struct itimerspec timer_spec;
	struct sigevent event;

	virtual void DoInit(void *) =0;
	virtual void DoLoop(void *) =0;
	virtual void DoRelease(void *) =0;

public:
	inline void Lock(void)
	{	
		mutex.Wait ();
	}

	inline void Unlock(void)
	{
		mutex.Post ();
	}

public:
	YARPRateThread(const char *c, int r, int prio=29) : name(c), mutex(1)
	{
		assert (r > 0);

		pid = 0;
		isRunningF = false;
		period = r;

		// this is needed for synchro.
		proxy_synchro = qnx_proxy_attach (0, 0, 0, -1);
		assert (proxy_synchro >= 0);

		stack = NULL;

		// try to go high priority by default.
		// this is because I couldn't fix a problem on quit.
		// this sends actually everything at high priority.
		int ret = setprio (0, prio);
		if (ret < 0)
		{
			cout << "set priority failed... abort" << endl;
			exit (-1);
		}
	}

	virtual ~YARPRateThread(void)
	{
		// close the thread
		if (isRunningF) 
		{
			isRunningF = false;
			// force a terminate.
			Terminate(1);
		}

		qnx_proxy_detach (proxy_synchro);

		// should it wait for the thread to exit.
	}

	virtual void Start(bool wait = true)
	{
		Lock ();

		if (isRunningF)
		{
			Unlock ();
			return;
		}

		stack = new char[DEFAULT_THREAD_STACK_SIZE];
		assert (stack != NULL);
		//pid = tfork(stack, DEFAULT_THREAD_STACK_SIZE, &RealThread, (void*)this, 0);
		pid = _beginthread(&RealThread, stack, DEFAULT_THREAD_STACK_SIZE, (void*)this);

		assert (pid >= 0);
		isRunningF = true;
		
		Unlock ();

		// aspetta che sia completata la routine di start.
		if (wait)
			WaitSynchro ();
	}

	void Terminate(bool wait = true)
	{
		Lock ();
		if (!isRunningF)
		{
			printf ("growl...\n");
			kill(pid, SIGTERM);
			waitpid(pid, NULL, 0);
			pid = -1;
			if (stack != NULL)
			{
				delete[] stack;
				stack = NULL;
			}
			Unlock ();
			return;
		}

		Trigger (proxy_end);
		Unlock ();

		if (wait)
		{
			Receive (proxy_synchro, 0, 0);
		}

		Lock ();
		isRunningF = false;
		if (stack != NULL)
		{
			delete[] stack;
			stack = NULL;
		}
		Unlock ();
	}

	void WaitSynchro(void)
	{
		Receive (proxy_synchro, 0, 0);
	}

	void SetPriority(int p)
	{
		int ret = setprio (0, p);
		if (ret < 0)
		{
			cout << "set priority failed (must be root?)" << endl;
		}
		return;
	}

	void SetInterval(int i)
	{
		assert (i > 0);

		Lock ();
		period = i;

		timer_spec.it_value.tv_sec = 0;
		timer_spec.it_value.tv_nsec = i * 1000000;
		timer_spec.it_interval.tv_sec = 0;
		timer_spec.it_interval.tv_nsec = i * 1000000;

		// start the timer.
		timer_settime (timer, 0, &timer_spec, NULL);

		Unlock ();
	}

	bool IsRunning(void)
	{
		Lock ();
		bool ret = isRunningF;
		Unlock ();
		return ret;
	}
	
	const char *GetName(void) const { return name; }

	static void RealThread(void *args)
	{
		YARPRateThread *thisThread = (YARPRateThread *)args;

		thisThread->Lock ();

		// create proxies
		thisThread->proxy_timer = qnx_proxy_attach (0, 0, 0, -1);
		assert (thisThread->proxy_timer >= 0);
		thisThread->proxy_end = qnx_proxy_attach (0, 0, 0, -1);
		assert (thisThread->proxy_end >= 0);

		thisThread->event.sigev_signo = -thisThread->proxy_timer;
		thisThread->timer = timer_create (CLOCK_REALTIME, &thisThread->event);
		assert (thisThread->timer >= 0);

		// prepare remaining structs.
		long int nsec = (thisThread->period * 1000000) % 1000000000;
		long int sec = (thisThread->period * 1000000) / 1000000000;
		thisThread->timer_spec.it_value.tv_sec = sec;
		thisThread->timer_spec.it_value.tv_nsec = nsec;
		thisThread->timer_spec.it_interval.tv_sec = sec;
		thisThread->timer_spec.it_interval.tv_nsec = nsec;

		// start the timer.
		timer_settime (thisThread->timer, 0, &thisThread->timer_spec, NULL);

		thisThread->Unlock ();

		Receive (thisThread->proxy_timer, 0, 0);

		// call the init function.
		thisThread->DoInit (args);

		// send event.
		Trigger (thisThread->proxy_synchro);

		bool local_end = false; 

		while (!local_end)
		{
			// ret = WaitForSingleObject (timer, INFINITE);
			// _ASSERT (ret == WAIT_OBJECT_0);
			pid_t recv = Receive (0, 0, 0);

			if (recv == thisThread->proxy_end)
			{
				local_end = true;
			}
			else
			if (recv == thisThread->proxy_timer)
			{
				thisThread->DoLoop (args);
			}
			else
			{
				printf ("Something wrong inside a thread\n");
				//return -1;
				_endthread ();
			}
		}

		thisThread->DoRelease (args);

		thisThread->Lock ();
		// timer must be already stopped.
		thisThread->timer_spec.it_value.tv_sec = 0;
		thisThread->timer_spec.it_value.tv_nsec = 0;
		timer_settime (thisThread->timer, 0, &thisThread->timer_spec, NULL);
		timer_delete (thisThread->timer);

		// detach proxies
		qnx_proxy_detach (thisThread->proxy_timer);
		qnx_proxy_detach (thisThread->proxy_end);
		thisThread->Unlock ();

		Trigger (thisThread->proxy_synchro);

		//exit(1);
		_endthread ();
		//return 1;
		// The previous version (see class aThread) had both a checked unlock 
		// through exceptions handling and a termination control (against deadlocks).
		// TODO: find an alternative to structured exceptions (which are slow).
	}
};

#ifdef DEFAULT_THREAD_STACK_SIZE
#undef DEFAULT_THREAD_STACK_SIZE
#endif

#endif

#endif
