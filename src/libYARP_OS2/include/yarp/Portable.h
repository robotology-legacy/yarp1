#ifndef _YARP2_PORTABLE_
#define _YARP2_PORTABLE_

#include <yarp/Readable.h>
#include <yarp/Writable.h>

namespace yarp {
  class Portable;
}

class yarp::Portable : public Readable, public Writable {
public:
  virtual void readBlock(BlockReader& reader) = 0;
  virtual void writeBlock(BlockWriter& writer) = 0;
};

#endif

