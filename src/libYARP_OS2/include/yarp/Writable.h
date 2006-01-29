#ifndef _YARP2_WRITABLE_
#define _YARP2_WRITABLE_

#include <yarp/BlockWriter.h>

namespace yarp {
  class Writable;
}

class yarp::Writable {
public:
  virtual void writeBlock(BlockWriter& writer) = 0;
};

#endif
