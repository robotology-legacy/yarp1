#ifndef _YARP2_FACEPORTMANAGER_
#define _YARP2_FACEPORTMANAGER_

#include <yarp/PortManager.h>
#include <yarp/Address.h>
#include <yarp/Carriers.h>

namespace yarp {
  class FacePortManager;
}

/**
 * For the first time, we mix comms with threads.
 */
class yarp::FacePortManager : public PortManager {
public:
  FacePortManager(const Address& address) {
    face = Carriers::listen(address);
  }

  virtual ~FacePortManager() {
    close();
  }

  virtual void close() {
    if (face!=NULL) {
      face->close();
      delete face;
      face = NULL;
    }
  }

private:
  Face *face;
};

#endif

