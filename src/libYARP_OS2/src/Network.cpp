
#include <yarp/os/Network.h>

#include <yarp/Companion.h>
#include <yarp/NameClient.h>

using namespace yarp;
using namespace yarp::os;

bool Network::connect(const char *src, const char *dest) {
  int result = Companion::connect(src,dest);
  return result == 0;
}


bool Network::disconnect(const char *src, const char *dest) {
  int result = Companion::disconnect(src,dest);
  return result == 0;
}


int Network::main(int argc, char *argv[]) {
  return Companion::main(argc,argv);
}


Contact Network::queryName(const char *name) {
   NameClient& nic = NameClient::getNameClient();
   Address address = nic.queryName(name);
   return address.toContact();
}


Contact Network::registerName(const char *name) {
   NameClient& nic = NameClient::getNameClient();
   Address address = nic.registerName(name);
   return address.toContact();
}


Contact Network::registerContact(const Contact& contact) {
   NameClient& nic = NameClient::getNameClient();
   Address address = nic.registerName(contact.getName().c_str(),
				      Address::fromContact(contact));
   return address.toContact();
}

Contact Network::unregisterName(const char *name) {
   NameClient& nic = NameClient::getNameClient();
   Address address = nic.unregisterName(name);
   return address.toContact();
}


Contact Network::unregisterContact(const Contact& contact) {
   NameClient& nic = NameClient::getNameClient();
   Address address = nic.unregisterName(contact.getName().c_str());
   return address.toContact();
}

