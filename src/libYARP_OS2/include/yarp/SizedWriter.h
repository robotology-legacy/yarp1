#ifndef _YARP2_SIZEDWRITER_
#define _YARP2_SIZEDWRITER_

#include <yarp/OutputStream.h>

namespace yarp {
  class SizedWriter;
}

/**
 * Minimal requirements for an efficient Writer.
 * Some protocols require knowing the size of a message up front.
 * In general, that requires generating the message before sending
 * it, but a user could do something more clever. The
 * SizedWriter class is referenced by the library instead of
 * BufferedBlockWriter specifically to leave that possibility open.
 */
class yarp::SizedWriter {
public:
  virtual ~SizedWriter() {}

  virtual void write(OutputStream& os) = 0;

  virtual int length() = 0;

  virtual int length(int index) = 0;

  virtual const char *data(int index) = 0;
};

#endif

