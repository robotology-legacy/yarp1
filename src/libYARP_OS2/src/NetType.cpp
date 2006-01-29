#include <yarp/NetType.h>

using namespace yarp;

// slow implementation - only relevant for textmode operation

String NetType::readLine(InputStream& is) {
  String buf("");
  bool done = false;
  while (!done) {
    //ACE_OS::printf("preget\n");
    int v = is.read();
    //ACE_OS::printf("got [%d]\n",v);
    char ch = (char)v;
    if (v>=32) {
      buf += ch;
    }
    if (ch=='\n') {
      done = true;
    }
    if (ch<0) { 
      throw IOException("readLine failed");
    }
  }
  return buf;
}    

int NetType::readFull(InputStream& is, const Bytes& b) {
  int off = 0;
  int fullLen = b.length();
  int remLen = fullLen;
  int result = 1;
  while (result>0&&remLen>0) {
    result = is.read(b,off,remLen);
    if (result>0) {
      remLen -= result;
      off += result;
    }
  }
  return (result<0)?result:fullLen;
}
