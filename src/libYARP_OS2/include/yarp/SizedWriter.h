#ifndef _YARP2_SIZEDWRITER_
#define _YARP2_SIZEDWRITER_

#include <yarp/OutputStream.h>

namespace yarp {
  class SizedWriter;
}

class yarp::SizedWriter {
public:
  virtual ~SizedWriter() {}

  virtual void write(OutputStream& os) = 0;

  // also way to extract block sizes
};

#endif

