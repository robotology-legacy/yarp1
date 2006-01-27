#ifndef _YARP2_OUTPUTPROTOCOL_
#define _YARP2_OUTPUTPROTOCOL_

#include <yarp/Route.h>
#include <yarp/BlockWriter.h>

namespace yarp {
  class OutputProtocol;
}

class yarp::OutputProtocol {
public:
  // all can throw IOException

  virtual ~OutputProtocol() {}

  virtual void open(const Route& route) = 0;
  virtual void close() = 0;

  virtual bool isActive() = 0;
  virtual BlockWriter& beginWrite() = 0;
  virtual void endWrite() = 0;

  virtual const Route& getRoute() = 0;
};

#endif
