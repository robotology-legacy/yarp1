#ifndef _YARP2_NAMECLIENT_
#define _YARP2_NAMECLIENT_

#include <yarp/Address.h>

namespace yarp {
  class NameClient;
}

class yarp::NameClient {
public:
  
  static NameClient& getNameClient() {
    return instance;
  }

  Address getAddress() {
    return address;
  }

  String getNamePart(const String& name) {
    return name;
  }

  Address queryName(const String& name) {
    String q("NAME_SERVER query ");
    q += getNamePart(name);
    return probe(q);
  }

  Address registerName(const String& name) {
    String q("NAME_SERVER register ");
    q += getNamePart(name);
    return probe(q);
  }

  Address unregisterName(const String& name) {
    String q("NAME_SERVER unregister ");
    q += getNamePart(name);
    return probe(q);
  }

  Address probe(const String& cmd) {
    String result = send(cmd);
    return extractAddress(result);
  }

  Address extractAddress(const String& txt);

  String send(const String& cmd);

private:
  NameClient() : address("localhost",10000) { 
  }

  Address address;
  String host;

  static NameClient instance;
};

#endif
