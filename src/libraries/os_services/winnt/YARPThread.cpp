//
//
// $Id: YARPThread.cpp,v 1.2 2003-04-10 15:01:35 gmetta Exp $
//
//

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

	return 0;
}


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


/// internal stuff, never exported.
///
class MemChunk
{
public:
	int _size;
	char * _array;

	MemChunk (void)
	{
		_size = -1;
		_array = NULL;
	}

	MemChunk (int n_len) 
	{
		_array = new char[n_len];
		ACE_ASSERT (_array != NULL);
		_size = n_len;
	}

	~MemChunk ()
	{
		if (_array != NULL) delete[] _array;
	}
};

class TSData
{
public:
	ACE_TSS <MemChunk> *_datum;

	TSData()
	{
		_datum = NULL;
	}

	~TSData()
	{
		delete _datum;
	}

	void Set(int n_len)
	{
		//printf("Yes, Set has been called for %d\n", len);
		if (_datum == NULL)
		{
			_datum = new ACE_TSS<MemChunk>;
			MemChunk *mem_chunk = new MemChunk (n_len);
			_datum->ts_object (mem_chunk);
		}
		else
			ACE_DEBUG ((LM_DEBUG, "Set: mem has been already allocated\n"));
	}

	char *Get()
	{
		//TYPE *ts_object (void) const;

		MemChunk *t = _datum->ts_object ();
		ACE_ASSERT (t != 0);
		char *data = (char *) t->_array;
		ACE_ASSERT (data != NULL);
		return data;
	}
};


YARPThreadSpecificBase::YARPThreadSpecificBase()
{
	system_resource = new TSData;
	ACE_ASSERT (system_resource != NULL);
}


YARPThreadSpecificBase::~YARPThreadSpecificBase()
{
	ACE_ASSERT (system_resource != NULL);
	delete (TSData*)system_resource;
}

void YARPThreadSpecificBase::Set(int len)
{
	ACE_ASSERT (system_resource != NULL);
	((TSData*)system_resource)->Set(len);
}

char *YARPThreadSpecificBase::Get()
{
	ACE_ASSERT (system_resource != NULL);
	return ((TSData*)system_resource)->Get();
}
