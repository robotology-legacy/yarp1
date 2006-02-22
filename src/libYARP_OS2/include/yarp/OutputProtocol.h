#ifndef _YARP2_OUTPUTPROTOCOL_
#define _YARP2_OUTPUTPROTOCOL_

#include <yarp/Route.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/SizedWriter.h>

namespace yarp {
  class OutputProtocol;
  class InputProtocol;
}

/**
 * The output side of an active connection between two ports.
 */
class yarp::OutputProtocol {
public:
  // all can throw IOException

  virtual ~OutputProtocol() {}

  virtual void open(const Route& route) = 0;
  virtual void close() = 0;

  virtual const Route& getRoute() = 0;
  virtual bool isActive() = 0;
  virtual bool isTextMode() = 0;
  virtual bool isConnectionless() = 0;

  virtual bool checkStreams() = 0;

  virtual void write(SizedWriter& writer) = 0;

  // some connections are capable of ping-ponging
  virtual InputProtocol& getInput() = 0;
  
  // direct access
  virtual OutputStream& getOutputStream() = 0;
  virtual InputStream& getInputStream() = 0;
};

#endif
