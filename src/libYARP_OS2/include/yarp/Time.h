#ifndef _YARP2_TIME_
#define _YARP2_TIME_

namespace yarp {
  class Time;
}

class yarp::Time {
public:
  static void delay(double seconds);
  static double now();
};

#endif
