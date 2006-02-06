#ifndef _YARP2_STREAMBLOCKREADER_
#define _YARP2_STREAMBLOCKREADER_

#include <yarp/InputStream.h>
#include <yarp/BlockReader.h>
#include <yarp/IOException.h>
#include <yarp/NetType.h>
#include <yarp/Bytes.h>
#include <yarp/Logger.h>

namespace yarp {
  class StreamBlockReader;
}

class yarp::StreamBlockReader : public BlockReader {
public:
  StreamBlockReader() {
    in = NULL;
    out = NULL;
    messageLen = 0;
    textMode = false;
  }

  void reset(InputStream& in, OutputStream& out, int len, bool textMode) {
    this->in = &in;
    this->out = &out;
    this->messageLen = len;
    this->textMode = textMode;
  }

  virtual void expectBlock(const Bytes& b) {
    YARP_ASSERT(in!=NULL);
    int len = b.length();
    if (len==0) return;
    if (len<0) { len = messageLen; }
    if (len>0) {
      NetType::readFull(*in,b);
      messageLen -= len;
      return;
    }
    throw IOException("expectBlock size conditions failed");
  }


  virtual int expectInt() {
    NetType::NetInt32 x = 0;
    Bytes b((char*)(&x),sizeof(x));
    YARP_ASSERT(in!=NULL);
    in->read(b);
    return x;
  }

  virtual String expectString(int len) {
    char *buf = new char[len];
    Bytes b(buf,len);
    YARP_ASSERT(in!=NULL);
    in->read(b);
    String s = buf;
    delete[] buf;
    return s;
  }

  virtual String expectLine() {
    YARP_ASSERT(in!=NULL);
    return NetType::readLine(*in);
  }

  virtual bool isTextMode() {
    return textMode;
  }

  virtual int getSize() {
    return messageLen;
  }

  virtual OutputStream *getReplyStream() {
    YARP_ASSERT(out!=NULL);
    return out;
  }

private:

  InputStream *in;
  OutputStream *out;
  int messageLen;
  bool textMode;
};

#endif
