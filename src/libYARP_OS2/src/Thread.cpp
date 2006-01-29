
#include <yarp/Thread.h>
#include <yarp/Logger.h>

using namespace yarp;

#ifdef __WIN32__
static unsigned __stdcall theExecutiveBranch (void *args)
#else
unsigned theExecutiveBranch (void *args)
#endif
{
  Thread *thread = (Thread *)args;
  thread->run();
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
  setOptions();
}


void Thread::setOptions(int stackSize) {
  this->stackSize = stackSize;
}

int Thread::join(double seconds) {
  int result = ACE_Thread::join(id);
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
}

