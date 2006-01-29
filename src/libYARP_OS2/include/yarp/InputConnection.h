#ifndef _YARP2_INPUTCONNECTION_
#define _YARP2_INPUTCONNECTION_

#include <yarp/InputProtocol.h>
#include <yarp/PortManager.h>
#include <yarp/Logger.h>

namespace yarp {
  class InputConnection;
}

/**
 * This manages a single connection, on the input side.
 */
class yarp::InputConnection {
public:

  /*
    takes and owns the inputprotocol object, then deals with it
    using the YARP Port protocol.  If you don't want to follow
    the YARP Port protocol, then don't use this class -- stick
    with the InputProtocol object.
    
    owns the ip
    doesn't own the manager
   */
  InputConnection(InputProtocol *ip, PortManager *manager) {
    proto = ip;
    this->manager = manager;
  }

  virtual ~InputConnection() {
    if (proto!=NULL) {
      proto->close();
      delete proto;
      proto = NULL;
    }
  }

  virtual void run();

  String getName() {
    if (hasManager()) {
      return getManager().getName();
    }
    return "null";
  }

protected:
  bool hasManager() {
    return manager!=NULL;
  }

  PortManager& getManager() {
    YARP_ASSERT(manager!=NULL);
    return *manager;
  }

private:
  InputProtocol *proto;
  PortManager *manager;
};

#endif

