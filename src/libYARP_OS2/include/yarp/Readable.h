#ifndef _YARP2_READABLE_
#define _YARP2_READABLE_

#include <yarp/BlockReader.h>

namespace yarp {
  class Readable;
}

/**
 * Specification of minimal operations an object must support to
 * be readable from a port.
 */
class yarp::Readable {
public:
  virtual ~Readable() { }

  virtual void readBlock(BlockReader& reader) = 0;
};

#endif
