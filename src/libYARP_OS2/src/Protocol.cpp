
#include <yarp/Protocol.h>

using namespace yarp;


void Protocol::defaultSendIndex() {
  YARP_ASSERT(writer!=NULL);
  writeYarpInt(10);
  int len = writer->length();
  char lens[] = { len, 1, 
		  -1, -1, -1, -1,
		  -1, -1, -1, -1 };
  Bytes b(lens,10);
  os().write(b);
  for (int i=0; i<len; i++) {
    NetType::netInt(writer->length(i),number.bytes());
    os().write(number.bytes());
  }
  NetType::netInt(0,number.bytes());
  os().write(number.bytes());
}


void Protocol::defaultExpectIndex() {
  ACE_OS::printf("expecting an index, requireAck is %d\n", delegate->requireAck());
  ACE_DEBUG((LM_DEBUG,"Protocol::expectIndex for %s", getRoute().toString().c_str()));
  // expect index header
  int r = NetType::readFull(is(),header.bytes());
  if (r!=header.length()) {
    throw IOException("broken index");
  }
  int len = interpretYarpNumber(header.bytes());
  if (len<0) {
    throw IOException("broken index - header is not a number");
  }
  if (len!=10) {
    throw IOException("broken index - header is wrong length");
  }
  r = NetType::readFull(is(),indexHeader.bytes());
  if (r!=indexHeader.length()) {
    throw IOException("broken index, secondary header");
  }
  int inLen = (unsigned char)(indexHeader.get()[0]);
  int outLen = (unsigned char)(indexHeader.get()[1]);

  int total = 0;
  for (int i=0; i<inLen; i++) {
    int l = NetType::readFull(is(),number.bytes());
    if (l!=number.length()) {
      throw IOException("bad input block length");
    }
    int x = NetType::netInt(number.bytes());
    total += x;
  }
  for (int i=0; i<outLen; i++) {
    int l = NetType::readFull(is(),number.bytes());
    if (l!=number.length()) {
      throw IOException("bad output block length");
    }
    int x = NetType::netInt(number.bytes());
    total += x;
  }
  messageLen = total;
  ACE_DEBUG((LM_DEBUG,"Total message length: %d\n",messageLen));
  ACE_OS::printf("got an index; messageLen is %d\n", messageLen);
}

void Protocol::defaultSendAck() {
  ACE_OS::printf("sending an ack\n");
  if (delegate->requireAck()) {
    writeYarpInt(0);
  }
}

