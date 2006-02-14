#ifndef _YARP2_PORTREADER_
#define _YARP2_PORTREADER_

#include <yarp/ConstString.h>
#include <yarp/ConnectionReader.h>

namespace yarp {
  namespace os {
    class PortReader;
  }
}

class yarp::os::PortReader {
public:

  virtual ~PortReader() {}

  /**
   * Read data.
   */
  virtual bool read(ConnectionReader& connection) = 0;

};

#endif
