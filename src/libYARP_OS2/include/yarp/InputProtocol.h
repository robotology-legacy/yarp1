#ifndef _YARP2_INPUTPROTOCOL_
#define _YARP2_INPUTPROTOCOL_

#include <yarp/Address.h>
#include <yarp/Route.h>
#include <yarp/BlockReader.h>
#include <yarp/InputStream.h>
#include <yarp/OutputStream.h>

namespace yarp {
  class InputProtocol;
}

class yarp::InputProtocol {
public:
  virtual ~InputProtocol() {}

  virtual void initiate(const String& name) = 0;  // throws IOException
  virtual void close() = 0;  // throws IOException

  virtual BlockReader& beginRead() = 0;  // throws IOException
  virtual void endRead() = 0;  // throws IOException

  virtual const Route& getRoute() = 0;

  virtual OutputStream& getOutputStream() = 0; // throws
  virtual InputStream& getInputStream() = 0; // throws
  virtual const Address& getRemoteAddress() = 0; // throws
};

#endif

