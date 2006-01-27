#ifndef _YARP2_BLOCKWRITER_
#define _YARP2_BLOCKWRITER_

#include <yarp/Bytes.h>
#include <yarp/String.h>

namespace yarp {
  class BlockWriter;
}

class yarp::BlockWriter {
public:
  virtual ~BlockWriter() {}

  // semantics: you promise that data will
  // persist until the end of writing -
  // managed by Content classes.
  // For max. efficient operation, this is the
  // only append() method you should call.
  virtual void appendBlock(const Bytes& data) = 0;

  // no promise about persistance.
  virtual void appendBlockCopy(const Bytes& data) = 0;

  virtual void appendInt(int data) = 0;
  virtual void appendString(const String& data) = 0;
  virtual void appendLine(const String& data) = 0;

  //virtual bool isTextMode() = 0;
};

#endif
