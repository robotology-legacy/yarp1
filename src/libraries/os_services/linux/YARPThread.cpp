#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "YARPSemaphore.h"

#include "YARPThread.h"

extern "C" {
#include <pthread.h>
}

#define pthread_attr_default 0

static void ExecuteThread(void *args)
{
  YARPThread *thread = ((YARPThread *)args);
  thread->Body();
  pthread_exit(NULL);
}

YARPThread::YARPThread()
{
  system_resource = NULL;
  identifier = -1;
  size = -1;
}


YARPThread::YARPThread(const YARPThread& yt)
{
  assert(1==0);
  system_resource = NULL;
  identifier = -1;
  if (yt.system_resource == NULL)
    {
      if (system_resource!=NULL)
	{
	  delete[] ((char*)system_resource);
	}
      system_resource = NULL;
    }
  else
    {
      if (system_resource==NULL)
	{
	  system_resource = (void*) (new pthread_t);
	}
      assert(system_resource!=NULL);
      memcpy(system_resource, yt.system_resource, sizeof(pthread_t));
    }
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
  if (system_resource==NULL)
    {
      system_resource = (void*) (new pthread_t);
    }
  assert(system_resource!=NULL);

  pthread_create( (pthread_t*) system_resource,
		  pthread_attr_default, 
		  (void * (*)(void*)) (&ExecuteThread),
		  (void *) this);

  identifier = 1;

  assert(identifier!=-1);
}

void YARPThread::End()
{
  if (identifier!=-1 && system_resource!=NULL)
    {
      pthread_cancel(*((pthread_t*) system_resource));
      pthread_join(*((pthread_t*) system_resource), NULL);
      identifier = -1;
    }
  if (system_resource!=NULL)
    {
      delete[] ((char*)system_resource);
    }
  system_resource = NULL;
  identifier = -1;
}


class TSData
{
public:
  pthread_key_t registry_key;
  int len;

  TSData()
    {
      int status = pthread_key_create(&registry_key, destroy_from_key);
      len = -1;
    }
  
  ~TSData()
    {
      char *data = Get();
      if (data!=NULL) delete[] data;
      pthread_setspecific(registry_key, NULL);
      pthread_key_delete(registry_key);
    }

  // don't think this will do anything
  static void destroy_from_key(void *ptr)
    {
      if (ptr!=NULL)
	{
	  delete[] ((char*)ptr);
	}
    }

  void Set(int n_len)
    {
      //printf("Yes, Set has been called for %d\n", len);
      len = n_len;
      char *data = new char[n_len];
      assert(data!=NULL);
      pthread_setspecific(registry_key, (void *)data);
    }

  char *Get()
    {
      char* data = (char*)pthread_getspecific(registry_key);
      if (data==NULL && len!=-1)
	{
	  Set(len);	
	  data = (char*)pthread_getspecific(registry_key);
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



