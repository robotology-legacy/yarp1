#ifndef _YARP2_CONNECTIONREADER_
#define _YARP2_CONNECTIONREADER_

#include <yarp/ConstString.h>

namespace yarp {
  namespace os {
    class ConnectionReader;
  }
}

class yarp::os::ConnectionReader {
public:

  virtual ~ConnectionReader() {
  }

  virtual void expectBlock(const char *data, int len) = 0;
  virtual ConstString expectText(int terminatingChar = '\n') = 0;

  virtual int expectInt() = 0;

  virtual bool isTextMode() = 0;
  virtual int getSize() = 0;
};

#endif
