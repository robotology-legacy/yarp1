#ifndef _YARP2_RUNNABLE_
#define _YARP2_RUNNABLE_

namespace yarp {
  class Runnable;
}

class yarp::Runnable {
public:

  /**
   * Body to run
   */
  virtual void run() {}


  /**
   * User-defined procedure for stopping execution.  There is no
   * general-purpose way to achieve that.
   */
  virtual void close() {}
};

#endif

