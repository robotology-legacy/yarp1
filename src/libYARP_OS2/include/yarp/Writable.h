#ifndef _YARP2_WRITABLE_
#define _YARP2_WRITABLE_

#include <yarp/BlockWriter.h>

namespace yarp {
  class Writable;
}

/**
 * Specification of minimal operations an object must support to
 * be writable from a port.
 */
class yarp::Writable {
public:
  virtual ~Writable() {}

  virtual void writeBlock(BlockWriter& writer) = 0;
};

#endif
