#ifndef _YARP2_PORTCOREOUTPUTUNIT_
#define _YARP2_PORTCOREOUTPUTUNIT_

#include <yarp/PortCore.h>
#include <yarp/PortCoreUnit.h>
#include <yarp/Logger.h>
#include <yarp/OutputProtocol.h>

namespace yarp {
  class PortCoreOutputUnit;
}

class yarp::PortCoreOutputUnit : public PortCoreUnit {
public:
  // specifically for managing input connections

  PortCoreOutputUnit(PortCore& owner, OutputProtocol *op) : 
    PortCoreUnit(owner), op(op), phase(1) {

    YARP_ASSERT(op!=NULL);
    closing = false;
    finished = false;
    running = false;
    name = owner.getName();
  }

  virtual ~PortCoreOutputUnit() {
    closeMain();
  }

  virtual bool start();

  virtual void run();

  virtual bool isOutput() {
    return true;
  }

  // just for testing
  virtual void runSimulation();

  virtual void close() {
    closeMain();
  }

  virtual bool isFinished() {
    return finished;
  }

  const String& getName() {
    return name;
  }

  virtual Route getRoute();


private:
  OutputProtocol *op;
  bool closing, finished, running;
  String name;
  Semaphore phase;

  void closeMain();
};

#endif

