#ifndef _YARP2_PORTABLE_
#define _YARP2_PORTABLE_

#include <yarp/Readable.h>
#include <yarp/Writable.h>

namespace yarp {
  class Portable;
}

/**
 * Readable and Writable.
 */
class yarp::Portable : public yarp::os::PortReader, public yarp::os::PortWriter {
public:
  //  virtual void readBlock(ConnectionReader& reader) = 0;
  //  virtual void writeBlock(ConnectionWriter& writer) = 0;
};

#endif

