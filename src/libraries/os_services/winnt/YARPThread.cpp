#include <windows.h>
#include <process.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>

#include "YARPSemaphore.h"

#include "YARPThread.h"

#define DEFAULT_THREAD_STACK_SIZE 8000

/*
static void HandleSignal(int sig)
{
  _endthreadex( 0 );
}
*/

static unsigned __stdcall ExecuteThread(void *args)
{
  YARPThread *thread = ((YARPThread *)args);
  //signal(SIGTERM,&HandleSignal);  // argh, no way to send a signal to the thread :(
  thread->Body();
  _endthreadex( 0 );
  return 0;
}

YARPThread::YARPThread()
{
  system_resource = NULL;
  identifier = -1;
  size = -1;
}

YARPThread::YARPThread(const YARPThread& yt)
{
  system_resource = NULL;
  identifier = yt.identifier;
}

YARPThread::~YARPThread()
{
  End();
}

int YARPThread::IsTerminated()
{
	return 0;
}


void YARPThread::Begin(int stack_size)
{
  if (stack_size <= 0)
    {
      stack_size = DEFAULT_THREAD_STACK_SIZE;
    }
	unsigned id;
  if (system_resource==NULL)
  {
	system_resource = (void*)_beginthreadex(
		NULL, // SD
		stack_size,                        // initial stack size
	    &ExecuteThread,    // thread function
		(void*)this,                       // thread argument
        0,                    // creation option
		&id                        // thread identifier
	);
  }

  assert(system_resource!=NULL);

  identifier = id;
}

void YARPThread::End()
{
  if (identifier!=-1)
    {
	  TerminateThread(system_resource,0);  // and bang goes the thread's memory and resources:(
      identifier = -1;
    }
  if (system_resource!=NULL)
    {
      CloseHandle(system_resource);
    }
  system_resource = NULL;
  identifier = -1;
}


class TSData
{
public:
	DWORD registry_key;
	int len;

  TSData()
    {
      registry_key = TlsAlloc();
	  assert(registry_key!=0);
      len = -1;
    }
  
  ~TSData()
    {
      char *data = Get();
      if (data!=NULL) delete[] data;
      TlsFree(registry_key);
    }

  void Set(int n_len)
    {
      //printf("Yes, Set has been called for %d\n", len);
      len = n_len;
      char *data = new char[n_len];
      assert(data!=NULL);
      TlsSetValue(registry_key, (void *)data);
    }

  char *Get()
    {
      char* data = (char*)TlsGetValue(registry_key);
      if (data==NULL && len!=-1)
		{
			Set(len);
			data = (char*)TlsGetValue(registry_key);
		}
      assert(data!=NULL);
      return data;
    }
};

YARPThreadSpecificBase::YARPThreadSpecificBase()
{
  system_resource = new TSData;
  assert(system_resource!=NULL);
}


YARPThreadSpecificBase::~YARPThreadSpecificBase()
{
  assert(system_resource!=NULL);
  delete (TSData*)system_resource;
}

void YARPThreadSpecificBase::Set(int len)
{
  assert(system_resource!=NULL);
  ((TSData*)system_resource)->Set(len);
}

char *YARPThreadSpecificBase::Get()
{
  assert(system_resource!=NULL);
  return ((TSData*)system_resource)->Get();
}
