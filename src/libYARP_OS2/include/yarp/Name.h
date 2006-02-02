#ifndef _YARP2_NAME_
#define _YARP2_NAME_

#include <yarp/String.h>

namespace yarp {
  class Name;
}

class yarp::Name {
public:
  Name(const String& txt);

  bool isRooted();

  // other utilities coming...

private:
  String txt;
};

#endif
