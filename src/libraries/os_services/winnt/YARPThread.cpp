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
/// $Id: YARPThread.cpp,v 1.4 2003-04-19 21:04:52 gmetta Exp $
///
///

/// ACE inclusion.
#include <ace/config.h>
#include <ace/Thread.h>
#include <ace/Synch.h>

///
#include <windows.h>
#include <process.h>
#include <signal.h>

#include "YARPSemaphore.h"
#include "YARPThread.h"

///
/// converted from #define.
const size_t DEFAULT_THREAD_STACK_SIZE = 8000;

/*
static void HandleSignal(int sig)
{
  _endthreadex( 0 );
}
*/

/// very simple thread wrapper.
///
static unsigned __stdcall ExecuteThread (void *args)
{
	YARPThread *thread = ((YARPThread *)args);
	
	//signal(SIGTERM,&HandleSignal);  // argh, no way to send a signal to the thread :(
	
	thread->Body();
	
	//_endthreadex( 0 );
	thread->system_resource = NULL;
	thread->identifier = -1;

	return 0;
}

///
/// WARNING: this requires a mutex for start/end and system_resources variable.
///
YARPThread::YARPThread ()
{
	system_resource = NULL;
	identifier = -1;
	size = -1;
}

YARPThread::YARPThread (const YARPThread& yt)
{
	system_resource = NULL;
	identifier = yt.identifier;
}

YARPThread::~YARPThread ()
{
	End();
}

int YARPThread::IsTerminated ()
{
	return 0;
}

void YARPThread::Begin (int stack_size)
{
	if (stack_size <= 0)
	{
		stack_size = DEFAULT_THREAD_STACK_SIZE;
	}

	ACE_thread_t threadID;
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
}

void YARPThread::End()
{
	if (identifier != -1)
	{
		//TerminateThread(system_resource,0);  // and bang goes the thread's memory and resources:(

#ifdef ACE_WIN32
		// win32 doesn't support kill signal, so we have to use the API
		TerminateThread ((HANDLE)system_resource, -1);
#else
		// need to store the ID.
		kill (identifier, SIGKILL);
#endif

		identifier = -1;
	}

	if (system_resource != NULL)
	{
		// CloseHandle(system_resource);
		/// Should I join the thread?
		ACE_Thread::join((ACE_hthread_t)system_resource);
	}

	system_resource = NULL;
	identifier = -1;
}


