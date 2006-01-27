#ifndef _YARP2_BOTTLE_
#define _YARP2_BOTTLE_

namespace yarp {
  class Bottle;
}

class yarp::Bottle {
public:
  void addInt(int x);
  void addString(const String& str);
  void clear();

  void fromString(const String& line);
  String toString();

  void fromBytes(const Bytes& data);
  // ...

private:
};

#endif

