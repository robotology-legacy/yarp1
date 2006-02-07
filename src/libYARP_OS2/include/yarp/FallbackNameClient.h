#ifndef _YARP2_FALLBACKNAMECLIENT_
#define _YARP2_FALLBACKNAMECLIENT_

#include <yarp/Thread.h>
#include <yarp/Address.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
  class FallbackNameClient;
}

class yarp::FallbackNameClient : public Thread {
public:
  FallbackNameClient() {
    closed = false;
  }

  virtual void run();
  virtual void close();

  Address getAddress();

  static Address seek();

private:
  Address address;
  DgramTwoWayStream listen;
  bool closed;
};

#endif

