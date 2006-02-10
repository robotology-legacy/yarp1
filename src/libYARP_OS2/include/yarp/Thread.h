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

  bool start();

  virtual void beforeStart();
  virtual void afterStart(bool success);

  bool isClosing();

  // call before start
  void setOptions(int stackSize = 0);

  static int getCount();

  // get a unique key
  long int getKey();

private:
  void *implementation;
};

#endif

