#ifndef _YARP2_SEMAPHORE_
#define _YARP2_SEMAPHORE_

namespace yarp {
  namespace os {
    class Semaphore;
  }
}

/**
 * A semaphore abstraction for mutual exclusion and resource management.
 */
class yarp::os::Semaphore {
public:
  Semaphore(int initialCount = 1);

  virtual ~Semaphore();

  // blocking wait
  void wait();

  // polling wait
  bool check();

  // increment
  void post();

private:
  void *implementation;
};

#endif

