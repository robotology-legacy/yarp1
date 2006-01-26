#ifndef _YARP2_NETTYPE_
#define _YARP2_NETTYPE_

#include <yarp/String.h>
#include <yarp/Bytes.h>
#include <yarp/InputStream.h>
#include <yarp/IOException.h>

namespace yarp {
  class NetType;
}

class yarp::NetType {
public:

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
