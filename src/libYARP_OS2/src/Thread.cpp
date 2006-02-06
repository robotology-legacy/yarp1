
#include <yarp/Thread.h>
#include <yarp/Semaphore.h>
#include <yarp/Logger.h>

using namespace yarp;

int Thread::threadCount = 0;
Semaphore Thread::threadMutex(1);


#ifdef __WIN32__
static unsigned __stdcall theExecutiveBranch (void *args)
#else
unsigned theExecutiveBranch (void *args)
#endif
{
  // just for now -- rather deal with broken pipes through normal procedures
  ACE_OS::signal(SIGPIPE, SIG_IGN);

  Thread *thread = (Thread *)args;
  thread->run();
  Thread::changeCount(-1);
  YARP_DEBUG(Logger::get(),"Thread shutting down");
  //ACE_Thread::exit();
  return 0;
}


Thread::Thread() {
  delegate = NULL;
  active = false;
  setOptions();
}


Thread::Thread(Runnable *target) {
  delegate = target;
  active = false;
  setOptions();
}


Thread::~Thread() {
  YARP_DEBUG(Logger::get(),"Thread being deleted");
  if (active) {
    join();
  }
}


long int Thread::getKey() {
  // if id doesn't fit in long int, should do local translation
  return (long int)id;
}


void Thread::setOptions(int stackSize) {
  this->stackSize = stackSize;
}

int Thread::join(double seconds) {
  if (active) {
    int result = ACE_Thread::join(hid);
    active = false;
    return result;
  }
  return 0;
}

void Thread::run() {
  if (delegate!=NULL) {
    delegate->run();
  }
}

void Thread::close() {
  if (delegate!=NULL) {
    delegate->close();
  }
}

void Thread::beforeStart() {
  if (delegate!=NULL) {
    delegate->beforeStart();
  }
}

void Thread::afterStart(bool success) {
  if (delegate!=NULL) {
    delegate->afterStart(success);
  }
}

bool Thread::start() {
  beforeStart();
  int result = ACE_Thread::spawn((ACE_THR_FUNC)theExecutiveBranch,
				 (void *)this,
				 THR_JOINABLE | THR_NEW_LWP,
				 &id,
				 &hid,
				 ACE_DEFAULT_THREAD_PRIORITY,
				 0,
				 (size_t)stackSize);
  if (result==0) {
    Thread::changeCount(1);
    active = true;
    afterStart(true);
  } else {
    afterStart(false);
  }
  return result==0;
}



int Thread::getCount() {
  threadMutex.wait();
  int ct = threadCount;
  threadMutex.post();
  return ct;
}


void Thread::changeCount(int delta) {
  threadMutex.wait();
  threadCount+=delta;
  threadMutex.post();
}


