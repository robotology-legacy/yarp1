#include <stdio.h>
#include <semaphore.h>
#include <assert.h>
///#include <sys/kernel.h>
#include <process.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
//#include <mig4nto.h>//+QNX6+
#include <pthread.h>//+QNX6+

#include "YARPThread.h"
#include "YARPSemaphore.h"

#include <set.h>

// Need to hack around times conflict
#define times ignore_times
///#include <sys/psinfo.h>
#include <unistd.h>

#define DEFAULT_THREAD_STACK_SIZE 8000

static YARPSemaphore mutex(1), killer(1);
typedef set<int,less<int> > PidSet;
static PidSet proc;
static int killing = 0;
static int owner = -1;

// Bad thread cleanup method #1
static void ForceKillAll()
{
  ///struct _psinfo2 info_me;
/* MIG4NTO       qnx_getids See the section in the migration guide on getting */
/* MIG4NTO                  process information. */
  ///qnx_getids(0,&info_me);
  mutex.Wait();
  if (!killing)
    {
      for (PidSet::iterator it=proc.begin(); it!=proc.end(); it++)
	{
	  int id = *it;
	 /// struct _psinfo2 info;
/* MIG4NTO       qnx_getids See the section in the migration guide on getting */
/* MIG4NTO                  process information. */
	  ///qnx_getids(id,&info);
	  ///if (info.pid_group==info_me.pid_group)
	  if (getpgid(id)==getpgid(0))
	    {
	      kill(id,SIGKILL);
	    }
	}
    }
  killing = 1;
  mutex.Post();
}

// Bad thread cleanup method #2
static void KillAll(int exception)
{
  int go = 0;
  killer.Wait();
  if (killing == 0)
    {
      killing = 1;
      go = 1;
    }
  killer.Post();
  if (go)
    {
      mutex.Wait();
      proc.erase(exception);
      for (PidSet::iterator it=proc.begin(); it!=proc.end(); it++)
	{
	  printf("Trying to kill process %d\n", *it);
	  //proc.erase(*it);
	  kill(*it,SIGKILL);
	}
      if (owner!=-1)
	{
	  kill(owner,SIGKILL);
	}
      mutex.Post();
    }
}

// Keep list of threads for removal later
static void AddProcess(int pid)
{
  mutex.Wait();
  if (owner==-1)
    {
      owner = pid;
    }
  else if (pid!=owner)
    {
      proc.insert(pid);
    }
  mutex.Post();
}

static void RemoveProcess(int pid)
{
  mutex.Wait();
  proc.erase(pid);
  mutex.Post();
}


static void SigHandler(int sig)
{
  int pid = getpid();
  RemoveProcess(pid);
  KillAll(pid);
  //kill(0,SIGKILL);
}


//static void ExecuteThread(void *args)
void * ExecuteThread(void *args) //+QNX6+
{
  long bits, old_bits;

  //  qnx_pflags( ~0, _PPF_NOZOMBIE, 0, 0 );

  //signal(SIGTERM,SigHandler);
  
  YARPThread *thread = ((YARPThread *)args);
  AddProcess(thread->GetIdentifier());
  thread->Body();
  //thread->identifier = -1;
  //RemoveProcess(thread->GetIdentifier());
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
	  system_resource = new char[yt.size];
	  size = yt.size;
	}
      assert(system_resource!=NULL);

      memcpy(system_resource, yt.system_resource, size);
    }
  identifier = yt.identifier;
}


YARPThread::~YARPThread()
{
  End();
  //if (getpid()==owner)
    //{
  //ForceKillAll();
  //}
}

int YARPThread::IsTerminated()
{
  return 0;
}


void YARPThread::Begin(int stack_size)
{
  static int creation_count = 0;
 
  //  signal(SIGTERM,SigHandler);
  AddProcess(getpid());
  if (stack_size <= 0)
    {
      stack_size = DEFAULT_THREAD_STACK_SIZE;
    }
  system_resource = (void*) (new char[stack_size]);
  size = stack_size;
  assert(system_resource!=NULL);
  int repeatable = 0;
  do {
/* MIG4NTO     _beginthread Use pthread_create(). Note that a thread in QNX 4 */
/* MIG4NTO                  is really just a separate process that shares the */
/* MIG4NTO                  data segment of its parent whereas a thread in */
/* MIG4NTO                  Neutrino is really within the same process as its */
/* MIG4NTO                  parent and shares a great deal more. */
    ///identifier = _beginthread(&ExecuteThread, (char*)system_resource, 
	///		      stack_size, (void*)this);

	///+QNX6+
	int error;
	pthread_t tid;
	pthread_attr_t attr;

    pthread_attr_init( &attr );
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );
    error = pthread_create( &tid, &attr, &ExecuteThread, (void *)this );
	identifier = tid;
	//+QNX6+

    if (error== -1) //+QNX6+	
      {
	int err = 23; //???+QNX6+
	printf("Thread allocation failed, error is %d\n", err);
	repeatable = (err==11);
      }
    creation_count++;
  } while (repeatable);
  assert(identifier!=-1);
}

void YARPThread::End()
{
  assert(identifier!=0);
  /*
  if (identifier!=-1)
    {
      kill(identifier, SIGKILL);
      waitpid(identifier,NULL,0);
      identifier = -1;
    }
  */
  if (identifier!=-1)
    {
	  pthread_t tid = identifier;
      pthread_kill(identifier, SIGKILL);
      //waitpid(identifier,NULL,0);
      identifier = -1;
    }
  if (system_resource!=NULL)
    {
      delete[] ((char*)system_resource);
    }
  system_resource = NULL;
}

void YARPThread::TerminateAll()
{
  ForceKillAll();
}


#include <map>
typedef map<int, char *, less<int> > TSMap;


class TSData
{
public:
  TSMap ts_map;
  YARPSemaphore mutex;
  int len;

  TSData()
    {
      len = -1;
    }
  
  ~TSData()
    {
      mutex.Wait();
      for (TSMap::iterator it = ts_map.begin(); it!=ts_map.end(); it++)
	{
	  if ((*it).second!=NULL)
	    {
	      delete[] (*it).second;
	    }
	}
      mutex.Post();
    }

  void Set(int n_len)
    {
      len = n_len;
      char *data = new char[n_len];
      assert(data!=NULL);
      int id = getpid();
      mutex.Wait();
      ts_map[id] = data;
      mutex.Post();
    }

  char *Get()
    {
      char *data = NULL;
      int id = getpid();
      mutex.Wait();
      if (ts_map.find(id)!=ts_map.end())
	{
	  data = ts_map[id];
	}
      if (data==NULL && len!=-1)
	{
	  mutex.Post();
	  Set(len);
	  mutex.Wait();
	  if (ts_map.find(id)!=ts_map.end())
	    {
	      data = ts_map[id];
	    }
	}
      mutex.Post();
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

