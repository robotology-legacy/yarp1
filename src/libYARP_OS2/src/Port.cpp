
#include <yarp/Port.h>
#include <yarp/PortCore.h>
#include <yarp/Logger.h>
#include <yarp/NameClient.h>
#include <yarp/Contact.h>

using namespace yarp;
using namespace yarp::os;

Port::Port() {
  implementation = new PortCore();
  YARP_ASSERT(implementation!=NULL);
}


bool Port::open(const char *name) {
  return open(Contact::byName(name));
}


bool Port::open(const Contact& contact, bool registerName) {
  bool success = true;
  Address caddress(contact.getHost().c_str(),
		   contact.getPort(),
		   contact.getCarrier().c_str(),
		   contact.getName().c_str());
  try {
    PortCore& core = *((PortCore*)implementation);
    NameClient& nic = NameClient::getNameClient();
    Address address = caddress;
    if (registerName) {
      address = nic.registerName(contact.getName().c_str(),caddress);
    }
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


Contact Port::where() {
  PortCore& core = *((PortCore*)implementation);
  Address address = core.getAddress();
  return address.toContact();
}


