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

  Address(const Address& alt) {
    name = alt.name;
    port = alt.port;
    carrier = alt.carrier;
    regName = alt.regName;
  }

  Address() {
    port = -1;
  }

  const yarp::String& getName() const {
    return name;
  }

  int getPort() const {
    return port;
  }

  const yarp::String& getCarrierName() const {
    return carrier;
  }

  const yarp::String& getRegName() const {
    return regName;
  }

  yarp::String toString() const {
    char buf[100];
    ACE_OS::itoa(port,buf,10);
    return carrier + ":/" + name + ":" + buf;
  }
};

#endif

