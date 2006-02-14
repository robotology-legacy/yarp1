#ifndef _YARP2_PORTWRITER_
#define _YARP2_PORTWRITER_

#include <yarp/ConnectionWriter.h>

namespace yarp {
  namespace os {
    class PortWriter;
  }
}

class yarp::os::PortWriter {
public:
  virtual ~PortWriter() {}

  /**
   * Write data.  Be aware that depending on the nature of the
   * connections a port has, and what protocol they use, and how
   * efficient the YARP implementation is, this method may be called
   * once, twice, or many times, as the result of a single call to
   * Port::write
   */
  virtual bool write(ConnectionWriter& connection) = 0;

  /**
   * This is called when Port::write is complete
   */
  virtual void onCompletion() {}
};

#endif
