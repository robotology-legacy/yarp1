#ifndef _YARP2_INPUTSTREAM_
#define _YARP2_INPUTSTREAM_

#include <yarp/Bytes.h>

namespace yarp {
  class InputStream;
}

class yarp::InputStream {
public:
  
  virtual ~InputStream() { }

  virtual void check() {}

  virtual int read() { // throws
    unsigned char result;
    int ct = read(Bytes((char*)&result,1));
    if (ct<1) {
      return -1;
    }
    return result;
  }

  virtual int read(const Bytes& b, int offset, int len) { // throws
    return read(Bytes(b.get()+offset,len));
  }

  virtual int read(const Bytes& b) = 0;

  virtual void close() = 0;
};

#endif
