#ifndef _YARP2_ADAPTERS_
#define _YARP2_ADAPTERS_

#include <yarp/ConnectionWriter.h>
#include <yarp/ConnectionReader.h>
#include <yarp/BlockWriter.h>
#include <yarp/BlockReader.h>
#include <yarp/ManagedBytes.h>

namespace yarp {
  class ConnectionWriterAdapter;
  class WritableAdapter;
  class BlockWriterAdapter;
  class BlockReaderAdapter;
}

// a set of adapters from implementation to user interface classes


class yarp::ConnectionWriterAdapter : public yarp::os::ConnectionWriter {
private:
  BlockWriter& owner;
public:
  ConnectionWriterAdapter(BlockWriter& owner) : owner(owner) {}


  virtual void appendBlock(const char *data, int len) {
    owner.appendBlockCopy(Bytes((char*)data,len));
  }

  virtual void appendInt(int data) {
    owner.appendInt(data);
  }

  virtual void appendString(const char *str, int terminate = '\n') {
    if (terminate=='\n') {
      owner.appendLine(str);
    } else if (terminate==0) {
      owner.appendString(str);
    } else {
      String s = str;
      str += terminate;
      owner.appendBlockCopy(Bytes((char*)(s.c_str()),s.length()));
    }
  }

  virtual void appendExternalBlock(const char *data, int len) {
    owner.appendBlock(Bytes((char*)data,len));
  }

  virtual bool isTextMode() {
    return owner.isTextMode();
  }

  virtual void declareSizes(int argc, int *argv) {
    // cannot do anything with this yet
  }
};

class yarp::WritableAdapter : public yarp::Writable {
private:
  yarp::os::PortWriter& owner;
public:
  WritableAdapter(yarp::os::PortWriter& owner) : owner(owner) {}

  virtual void writeBlock(BlockWriter& writer) {
    ConnectionWriterAdapter adapter(writer);
    owner.write(adapter);
  }
};


class yarp::BlockWriterAdapter : public yarp::BlockWriter {
private:
  yarp::os::ConnectionWriter& owner;
public:
  BlockWriterAdapter(yarp::os::ConnectionWriter& owner) : owner(owner) {}

  virtual void appendBlock(const Bytes& data) {
    return owner.appendExternalBlock(data.get(),data.length());
  }

  virtual void appendBlock(const String& data) {
    return owner.appendExternalBlock(data.c_str(),data.length());
  }

  virtual void appendBlockCopy(const Bytes& data) {
    return owner.appendBlock(data.get(),data.length());
  }

  virtual void appendInt(int data) {
    return owner.appendInt(data);
  }

  virtual void appendString(const String& data) {
    return owner.appendString(data.c_str(),'\0');
  }

  virtual void appendLine(const String& data) {
    return owner.appendString(data.c_str(),'\n');
  }

  virtual bool isTextMode() {
    return owner.isTextMode();
  }
};

class yarp::BlockReaderAdapter : public yarp::BlockReader {
private:
  yarp::os::ConnectionReader& owner;
public:
  BlockReaderAdapter(yarp::os::ConnectionReader& owner) : owner(owner) {}


  virtual void expectBlock(const Bytes& b) {
    owner.expectBlock(b.get(),b.length());
  }

  virtual int expectInt() {
    return owner.expectInt();
  }

  virtual String expectString(int len) {
    ManagedBytes buf(len);
    expectBlock(buf.get(),len);
    return String(buf.get());
  }

  virtual String expectLine() {
    return String(owner.expectText('\n').c_str());
  }

  virtual bool isTextMode() {
    return owner.isTextMode();
  }

  virtual int getSize() {
    return owner.getSize();
  }

  virtual OutputStream *getReplyStream() {
    return NULL;
  }

  virtual TwoWayStream *getStreams() {
    return NULL;
  }

  virtual void expectBlock(const char *data, int len) {
    return owner.expectBlock(data,len);
  }

  virtual ::yarp::os::ConstString expectText(int terminatingChar) {
    return owner.expectText(terminatingChar);
  }
};

#endif
