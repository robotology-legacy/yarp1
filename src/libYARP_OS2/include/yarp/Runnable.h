#ifndef _YARP2_RUNNABLE_
#define _YARP2_RUNNABLE_

namespace yarp {
  class Runnable;
}

class yarp::Runnable {
public:
  virtual ~Runnable() {}

  /**
   * Body to run
   */
  virtual void run() {}


  /**
   * User-defined procedure for stopping execution.  There is no
   * general-purpose way to achieve that.
   */
  virtual void close() {}
  
  /**
   * Called from the creator before the new thread exists
   * and before the associcated Thread::start() call returns
   */
  virtual void beforeStart() {}

  virtual void afterStart(bool success) {}


};

#endif

