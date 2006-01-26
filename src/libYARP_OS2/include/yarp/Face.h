#ifndef _YARP2_FACE_
#define _YARP2_FACE_

#include <yarp/Address.h>
#include <yarp/InputProtocol.h>

namespace yarp {
  class Face;
}

class yarp::Face {
public:
  virtual ~Face() {} // destructors must be virtual

  virtual void open(const Address& address) = 0; // throws IOException
  virtual void close() = 0; // throws IOException
  virtual InputProtocol *read() = 0; // throws IOException
};

#endif

