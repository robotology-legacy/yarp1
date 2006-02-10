
#include <yarp/Port.h>
#include <yarp/PortCore.h>
#include <yarp/Logger.h>
#include <yarp/NameClient.h>

using namespace yarp::os;

Port::Port() {
  implementation = new PortCore();
  YARP_ASSERT(implementation!=NULL);
}


bool Port::open(const char *name) {
  bool success = true;
  try {
    PortCore& core = *((PortCore*)implementation);
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.registerName(name);
    success = address.isValid();
    if (success) {
      success = core.listen(address);
      if (success) {
	success = core.start();
      }
    }
  } catch (IOException e) {
    success = false;
    YARP_DEBUG(Logger::get(),e.toString() + " <<< Port::register failed");
  }
  if (!success) {
    close();
  }
  return success;
}


void Port::close() {
  PortCore& core = *((PortCore*)implementation);
  core.close();
  core.join();
}


Port::~Port() {
  if (implementation!=NULL) {
    close();
    delete ((PortCore*)implementation);
    implementation = NULL;
  }
}

