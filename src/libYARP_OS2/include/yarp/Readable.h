#ifndef _YARP2_READABLE_
#define _YARP2_READABLE_

#include <yarp/BlockReader.h>

namespace yarp {
  class Readable;
}

class yarp::Readable {
public:
  virtual void readBlock(BlockReader& reader) = 0;
};

#endif
