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
/// $Id: YARPThread.cpp,v 1.15 2003-08-12 16:50:52 gmetta Exp $
///
///

/// ACE inclusion.
#include <ace/config.h>
#include <ace/Thread.h>
#include <ace/Synch.h>

///
#include "YARPThread.h"
#include "YARPSemaphore.h"

#ifdef __QNX6__
#include <sys/neutrino.h>
#endif

///
/// converted from #define.
const size_t DEFAULT_THREAD_STACK_SIZE = 8000;


#ifdef __WIN32__
static unsigned __stdcall ExecuteThread (void *args)
#else
unsigned ExecuteThread (void *args)
#endif
{
	YARPBareThread *thread = ((YARPBareThread *)args);
	
	//signal(SIGTERM,&HandleSignal);  // argh, no way to send a signal to the thread :(
	
	thread->Body();
	
	return 0;
}

///
///
///
YARPBareThread::YARPBareThread (void) : sema(0)
{
	system_resource = NULL;
	identifier = -1;
	size = -1;
	running = false;
	shutdown_state = YT_None;
	sema.Post();
}

YARPBareThread::YARPBareThread (const YARPBareThread& yt) : sema(0)
{
	/// a bit ambiguous?
	system_resource = NULL;
	identifier = yt.identifier;
	running = yt.running;
	shutdown_state = YT_None;
	sema.Post();
}

YARPBareThread::~YARPBareThread (void)
{
	if (!IsTerminated()) End();
}

void YARPBareThread::Begin (int stack_size)
{
	sema.Wait();
	ACE_ASSERT (running == 0);
	ACE_ASSERT (shutdown_state == YT_None);

	if (stack_size <= 0)
	{
		stack_size = DEFAULT_THREAD_STACK_SIZE;
	}

	ACE_thread_t threadID;

	ACE_ASSERT (system_resource == NULL);

	if (system_resource == NULL)
	{
		if (ACE_Thread::spawn (
							 (ACE_THR_FUNC)ExecuteThread,
							 (void *)this,
							 THR_JOINABLE | THR_NEW_LWP,
							 &threadID,
							 (ACE_hthread_t *)&system_resource,
							 ACE_DEFAULT_THREAD_PRIORITY,
							 0, 
							 (size_t)stack_size // not sure about this.
							 ) == -1)
		{
			ACE_DEBUG((LM_DEBUG, "%p\n", "Error in spawning thread"));
		}
	}

	ACE_ASSERT (system_resource != NULL);
	identifier = threadID;
	running = true;

	sema.Post();
}

int YARPBareThread::Join (int timeout)
{
	sema.Wait();
	if (shutdown_state == YT_AskedEnd)
		shutdown_state = YT_Joining;
	sema.Post();

	ACE_UNUSED_ARG (timeout);
	int r = ACE_Thread::join ((ACE_hthread_t)system_resource);

	/// if joined (otherwise hung!)
	sema.Wait();
	system_resource = NULL;
	identifier = -1;
	shutdown_state = YT_None;
	sema.Post();

	return r;
}

int YARPBareThread::GetPriority (void)
{
	int prio = YARP_FAIL;
	ACE_Thread::getprio ((ACE_hthread_t)system_resource, prio);
	return prio;
}

int YARPBareThread::SetPriority (int prio)
{
	return ACE_Thread::setprio ((ACE_hthread_t)system_resource, prio);
}

///
/// parameter, if == 0 don't wait and kills the thread.
///			   if == -1 joins the thread (risky, no timeout).
///	otherwise it'll wait dontkill milliseconds before termination.
///
void YARPBareThread::End(int dontkill)
{
	sema.Wait();

	if (identifier != -1 && dontkill == 0)
	{
		running = false;
		shutdown_state = YT_End;

		ACE_DEBUG ((LM_DEBUG, "YARPBareThread::End : WARNING --- thread forced a kill\n"));

#if defined(__WIN32__)
		
		TerminateThread ((HANDLE)system_resource, -1);

#elif defined(__QNX6__)
		
		ThreadDestroy (identifier, -1, (void *)-1);

#elif defined(__LINUX__)

		ACE_Thread::kill (identifier, SIGKILL);
#else

#error "YARPBareThread::End - not implemented for the specified architecture"

#endif
	}
	else
	if (identifier != -1 && dontkill == -1)
	{
		ACE_ASSERT (running == true || shutdown_state == YT_None);

		running = false;
		shutdown_state = YT_End;

		sema.Post();
		ACE_Thread::join ((ACE_hthread_t)system_resource);
		sema.Wait();
	}
	else
	if (identifier != -1 && dontkill > 0)
	{
		/// kills it anyway...
		running = false;
		shutdown_state = YT_End;

		sema.Post();

		YARPTime::DelayInSeconds(double(dontkill)/1000.0);
		ACE_DEBUG ((LM_DEBUG, "YARPBareThread::End : WARNING --- thread forced a kill after %d ms\n", dontkill));

#if defined(__WIN32__)
		
		TerminateThread ((HANDLE)system_resource, -1);

#elif defined(__QNX6__)
		
		ThreadDestroy (identifier, -1, (void *)-1);

#elif defined(__LINUX__)

		ACE_Thread::kill (identifier, SIGKILL);
#else

#error "YARPBareThread::End - not implemented for the specified architecture"

#endif
		sema.Wait();
	}

	/// isn't a close handle missing here?
	system_resource = NULL;
	identifier = -1;
	shutdown_state = YT_None;

	sema.Post();
}


int YARPBareThread::IsTerminated (void)
{
	sema.Wait();
	int r = !running;
	sema.Post();
	return r;
}


void YARPBareThread::AskForEnd (void)
{
	sema.Wait();
	if (shutdown_state != YT_None && shutdown_state != YT_AskedEnd)
	{
		sema.Post();
		return;
	}
	running = false;
	shutdown_state = YT_AskedEnd;
	sema.Post();
}

void YARPBareThread::CleanState (void)
{
	sema.Wait();
	shutdown_state = YT_None;
	system_resource = NULL;
	identifier = -1;
	sema.Post();
}
