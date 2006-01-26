#ifndef _YARP2_STRINGOUTPUTSTREAM_
#define _YARP2_STRINGOUTPUTSTREAM_

namespace yarp {
  class StringOutputStream;
}

class yarp::StringOutputStream : public OutputStream {
public:
  using OutputStream::write;

  StringOutputStream() { }


  String toString() {
    return data;
  }


  virtual void write(const Bytes& b) { // throws
    for (int i=0; i<b.length(); i++) {
      data += b.get()[i];
    }
  }

  virtual void close() {
  }

private:
  String data;
};

#endif

