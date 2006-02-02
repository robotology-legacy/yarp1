#ifndef _YARP2_BLOCKREADER_
#define _YARP2_BLOCKREADER_

#include <yarp/Bytes.h>
#include <yarp/String.h>

namespace yarp {
  class BlockReader;
}

/**
 * Read from an input port.  This is the interface available when
 * reconstructing an object from a description.
 * The underlying protocol is hidden,
 * except that the object may optionally interpret a more
 * human-friendly form during text-mode connections.
 */
class yarp::BlockReader {
public:

  virtual ~BlockReader() {
  }

  virtual void expectBlock(const Bytes& b) = 0;
  virtual int expectInt() = 0;

  virtual String expectString(int len) = 0;
  virtual String expectLine() = 0;

  virtual bool isTextMode() = 0;
  virtual int getSize() = 0;
};

#endif

