#ifndef _YARP2_PORT_
#define _YARP2_PORT_

namespace yarp {
  namespace os {
    class Port;
  }
}

class yarp::os::Port {

public:
  Port();
  virtual ~Port();

  bool open(const char *name);

  void close();

private:
  void *implementation;

};

#endif
