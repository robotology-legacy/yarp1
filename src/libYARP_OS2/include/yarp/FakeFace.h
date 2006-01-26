#ifndef _YARP2_FAKEFACE_
#define _YARP2_FAKEFACE_

#include <yarp/Face.h>

namespace yarp {
  class FakeFace;
}

class yarp::FakeFace : public Face {
public:
  virtual void open(const Address& address); // throws IOException
  virtual void close(); // throws IOException
  virtual InputProtocol *read(); // throws IOException
private:
};

#endif
