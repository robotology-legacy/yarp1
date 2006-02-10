#ifndef _YARP2_TIME_
#define _YARP2_TIME_

namespace yarp {
  namespace os {
    class Time;
  }
}

/**
 * Services related to time -- delay, current time.
 */
class yarp::os::Time {
public:
  static void delay(double seconds);
  static double now();
};

#endif
