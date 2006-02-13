
#include <yarp/Address.h>

yarp::os::Contact yarp::Address::toContact() const {
  return yarp::os::Contact::byName
    (getRegName().c_str()).addSocket(getCarrierName().c_str(),
				     getName().c_str(),
				     getPort());
}

