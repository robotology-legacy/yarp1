#ifndef _YARP2_THREAD_
#define _YARP2_THREAD_

#include <yarp/Runnable.h>
#include <yarp/Semaphore.h>

#include <ace/Thread.h>


namespace yarp {
  class Thread;
}

/**
 * An abstraction for a thread of executation.
 */
class yarp::Thread : public Runnable {
public:
  Thread();
  Thread(Runnable *target);

  virtual ~Thread();

  int join(double seconds = -1);
  virtual void run();
  virtual void close();

  // should throw if no success
  virtual bool start();

  virtual void beforeStart();
  virtual void afterStart(bool success);

  // call before start
  void setOptions(int stackSize = 0);

  static int getCount();

  // won't be public for long...
  static void changeCount(int delta);

  // get a unique key
  long int getKey();

private:
  int stackSize;
  ACE_hthread_t hid;
  ACE_thread_t id;
  bool active;
  Runnable *delegate;

  static int threadCount;
  static Semaphore threadMutex;
};

#endif

