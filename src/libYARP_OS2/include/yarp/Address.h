#ifndef _YARP2_ADDRESS_
#define _YARP2_ADDRESS_

#include <yarp/String.h>

#include <ace/OS_NS_stdlib.h>

namespace yarp {
  class Address;
}

class yarp::Address {
private:
  yarp::String name, carrier, regName;
  int port;
public:
  Address(yarp::String name,
	  int port) {
    this->name = name;
    this->port = port;
    this->carrier = "tcp";
    this->regName = "";
  }

  Address(yarp::String name,
	  int port,
	  yarp::String carrier) {
    this->name = name;
    this->port = port;
    this->carrier = carrier;
    this->regName = "";
  }

  Address(yarp::String name,
	  int port,
	  yarp::String carrier,
	  yarp::String regName) {
    this->name = name;
    this->port = port;
    this->carrier = carrier;
    this->regName = regName;
  }

  const yarp::String& getName() {
    return name;
  }

  int getPort() {
    return port;
  }

  const yarp::String& getCarrier() {
    return carrier;
  }

  const yarp::String& getRegame() {
    return regName;
  }

  yarp::String toString() {
    char buf[100];
    ACE_OS::itoa(port,buf,10);
    return carrier + ":/" + name + ":" + buf;
  }
};

#endif

