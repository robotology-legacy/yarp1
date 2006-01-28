#ifndef _YARP2_PORTABLE_
#define _YARP2_PORTABLE_

namespace yarp {
  class Portable;
}

class yarp::Portable {
  virtual void readBlock(BlockReader& reader) = 0;
  virtual void writeBlock(BlockWriter& writer) = 0;
};

#endif

