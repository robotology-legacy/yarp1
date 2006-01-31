#ifndef _YARP2_NAMECLIENT_
#define _YARP2_NAMECLIENT_

#include <yarp/Address.h>

namespace yarp {
  class NameClient;
  class NameServer;
}

class yarp::NameClient {
public:
  
  static NameClient& getNameClient() {
    if (instance==NULL) {
      instance = new NameClient();
    }
    return *instance;
  }

  // for memory management testing
  static void removeNameClient() {
    if (instance!=NULL) {
      delete instance;
      instance = NULL;
    }
  }

  Address getAddress() {
    return address;
  }

  String getNamePart(const String& name) {
    return name;
  }

  Address queryName(const String& name);
  Address registerName(const String& name);
  Address registerName(const String& name, const Address& address);
  Address unregisterName(const String& name);

  Address probe(const String& cmd) {
    String result = send(cmd);
    return extractAddress(result);
  }

  Address extractAddress(const String& txt);

  String send(const String& cmd);

  void setFakeMode(bool fake = true) {
    this->fake = fake;
  }

  bool isFakeMode() {
    return fake;
  }

  virtual ~NameClient();


private:
  NameClient() : address("localhost",10000) { 
    fake = false;
    fakeServer = NULL;
  }

  NameServer& getServer();


  Address address;
  String host;
  bool fake;
  NameServer *fakeServer;

  static NameClient *instance;
};

#endif
