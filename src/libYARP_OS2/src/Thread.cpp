
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
  //ACE_OS::printf("Thread shutting down\n");
  //ACE_Thread::exit();
  return 0;
}


Thread::Thread() {
  delegate = NULL;
  setOptions();
}


Thread::Thread(Runnable *target) {
  delegate = target;
  setOptions();
}


Thread::~Thread() {
}


long int Thread::getKey() {
  // if id doesn't fit in long int, should do local translation
  return (long int)id;
}


void Thread::setOptions(int stackSize) {
  this->stackSize = stackSize;
}

int Thread::join(double seconds) {
  int result = ACE_Thread::join(hid);
  return result;
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

void Thread::start() {
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
  }
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


