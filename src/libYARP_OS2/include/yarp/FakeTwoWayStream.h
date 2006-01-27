#ifndef _YARP2_FAKETWOWAYSTREAM_
#define _YARP2_FAKETWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/StringInputStream.h>
#include <yarp/StringOutputStream.h>

namespace yarp {
  class FakeTwoWayStream;
}


/**
 * This class is used for testing.
 * It provides a fake two way stream.
 * Override apply to make stuff happen - default is echo. 
 */
class yarp::FakeTwoWayStream : public TwoWayStream {
public:
  FakeTwoWayStream() : out(this) {
  }

  virtual InputStream& getInputStream() {
    return in;
  }

  virtual OutputStream& getOutputStream() {
    return out;
  }

  virtual const Address& getLocalAddress() {
    return local;
  }

  virtual const Address& getRemoteAddress() {
    return remote;
  }

  virtual void close() {
    in.close();
    out.close();
  }

  virtual void apply(const Bytes& b) {
    in.add(b);
  }

private:

  class ActiveStringOutputStream : public StringOutputStream {
  public:
    ActiveStringOutputStream(FakeTwoWayStream *who) : owner(*who) {
    }

    virtual void write(const Bytes& b) {
      StringOutputStream::write(b);
      owner.apply(b);
    }
  private:
    FakeTwoWayStream& owner;
  };

  StringInputStream in;
  ActiveStringOutputStream out;
  Address local;
  Address remote;
};

#endif
