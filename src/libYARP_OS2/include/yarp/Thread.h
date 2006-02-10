#ifndef _YARP2_THREAD_
#define _YARP2_THREAD_


namespace yarp {
  namespace os {
    class Thread;
  }
}

/**
 * An abstraction for a thread of execution.
 */
class yarp::os::Thread {
public:
  Thread();

  virtual ~Thread();

  int join(double seconds = -1);
  virtual void run();
  virtual void close();

  // should throw if no success
  virtual bool start();

  // call before start
  void setOptions(int stackSize = 0);

  static int getCount();

  // won't be public for long...
  static void changeCount(int delta);

  // get a unique key
  long int getKey();

private:
  void *implementation;
};

#endif

