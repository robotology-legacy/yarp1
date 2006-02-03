#ifndef _YARP2_SEMAPHORE_
#define _YARP2_SEMAPHORE_

#include <ace/Synch.h>

namespace yarp {
  class Semaphore;
}

class yarp::Semaphore {
public:
  Semaphore(int initialCount = 1) : sema(initialCount) {
  }

  virtual ~Semaphore() {}

  // blocking wait
  void wait() {
    sema.acquire();
  }

  // polling wait
  bool check() {
    return (sema.tryacquire()<0)?0:1;
  }

  // increment
  void post() {
    sema.release();
  }

private:
  ACE_Semaphore sema;
};

#endif

