#ifndef _YARP2_NETTYPE_
#define _YARP2_NETTYPE_

#include <yarp/String.h>
#include <yarp/Bytes.h>
#include <yarp/InputStream.h>
#include <yarp/IOException.h>

#include <ace/OS_NS_stdlib.h>

namespace yarp {
  class NetType;
}

class yarp::NetType {
public:

  static int netInt(const Bytes& code) {
    YARP_ASSERT(code.length()==sizeof(NetType::NetInt32));
    NetType::NetInt32& i = *((NetType::NetInt32*)(code.get()));
    return i;
  }

  static void netInt(int data, const Bytes& code) {
    NetType::NetInt32 i = data;
    Bytes b((char*)(&i),sizeof(i));
    if (code.length()!=sizeof(i)) {
      throw IOException("not enough room for integer");
    }
    ACE_OS::memcpy(code.get(),b.get(),code.length());
  }

  // slow implementation - only relevant for textmode operation
  static String readLine(InputStream& is) {
    String buf("");
    bool done = false;
    while (!done) {
      int v = is.read();
      char ch = (char)v;
      if (v>=32) {
	buf += ch;
      }
      if (ch=='\n') {
	done = true;
      }
      if (ch<0) { 
	throw new IOException("readLine failed");
      }
    }
    return buf;
  }    



  int readFull(InputStream& is, const Bytes& b) {
    int off = 0;
    int fullLen = b.length();
    int remLen = fullLen;
    int result = 1;
    while (result>0&&remLen>0) {
      result = in.read(b,off,remLen);
      if (result>0) {
	remLen -= result;
	off += result;
      }
    }
    return (result<0)?result:fullLen;
  }



  /**
   * Definition of the NetInt32 type
   */
#ifdef __LINUX__
  typedef int32_t NetInt32;
#else
#  ifdef __WIN__
#    ifdef __WIN_MSVC__
  typedef __int32 NetInt32;
#    else
#      include <sys/config.h>
  typedef __int32_t NetInt32;
#    endif
#  else
#    error "need to define NetInt32 for this OS; see yarp/NetType.h"
#  endif
#endif

};

#endif
