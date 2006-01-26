#ifndef _YARP2_STREAMBLOCKREADER_
#define _YARP2_STREAMBLOCKREADER_

#include <yarp/InputStream.h>
#include <yarp/BlockReader.h>
#include <yarp/IOException.h>
#include <yarp/NetType.h>

namespace yarp {
  class StreamBlockReader;
}

class yarp::StreamBlockReader : public BlockReader {
public:
  StreamBlockReader() {
    in = NULL;
    len = 0;
    textMode = false;
  }

  void set(InputStream& in, int len, bool textMode) {
    this->in = &in;
    this->messageLen = len;
    this->textMode = textMode;
  }

  virtual void expectBlock(const Bytes& b) {
    YARP_ASSERT(in!=NULL);
    int len = b.length();
    if (len==0) return;
    if (len<0) { len = messageLen; }
    if (messageLen>=len && len>0) {
      NetType::readFull(*in,b);
      messageLen -= len;
    }
    throw IOException("expectBlock size conditions failed");
  }


  virtual int expectInt() {
    NetType::NetInt32 x = 0;
    Bytes b((char*)(&x),sizeof(x));
    in.read(b);
    return x;
  }

  virtual String expectString(int len) {
    char *buf = new char[len];
    Bytes b(buf,len);
    in.read(b);
    String s = buf;
    delete[] buf;
    return s;
  }

  virtual String expectLine() {
    return NetType::readLine(in);
  }

  virtual bool isTextMode() {
    return textMode;
  }

  virtual int getSize() {
    return messageLen;
  }

private:

  InputStream *in;
  int messageLen;
  bool textMode;
};

#endif
