
#include <yarp/Thread.h>
#include <yarp/ThreadImpl.h>
#include <yarp/Logger.h>

using namespace yarp;
using namespace yarp::os;

Thread::Thread() {
  implementation = new ThreadImpl;
  YARP_ASSERT(implementation!=NULL);
}


Thread::~Thread() {
  if (implementation!=NULL) {
    delete ((ThreadImpl*)implementation);
    implementation = NULL;
  }
}


int Thread::join(double seconds) {
  return ((ThreadImpl*)implementation)->join(seconds);
}


void Thread::run() {
  ((ThreadImpl*)implementation)->run();
}


void Thread::close() {
  ((ThreadImpl*)implementation)->close();
}

bool Thread::start() {
  return ((ThreadImpl*)implementation)->start();
}

void Thread::setOptions(int stackSize) {
  return ((ThreadImpl*)implementation)->setOptions(stackSize);
}

int Thread::getCount() {
  return ThreadImpl::getCount();
}

// get a unique key
long int Thread::getKey() {
  return ((ThreadImpl*)implementation)->getKey();
}
