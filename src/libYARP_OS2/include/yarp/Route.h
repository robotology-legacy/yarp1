#ifndef _YARP2_ROUTE_
#define _YARP2_ROUTE_

#include <yarp/String.h>

namespace yarp {
  class Route;
}

class yarp::Route {
public:
  Route() {}

  Route(const String& fromKey,
	const String& toKey,
	const String& carrier) {
    this->fromKey = fromKey;
    this->toKey = toKey;
    this->carrier = carrier;
  }

  Route(const Route& alt) {
    fromKey = alt.fromKey;
    toKey = alt.toKey;
    carrier = alt.carrier;
  }

  const String& getFromName() {
    return fromKey;
  }

  const String& getToName() {
    return toKey;
  }

  const String& getCarrierName() {
    return carrier;
  }

  Route addFromName(const String& fromName) {
    return Route(fromName,getToName(),getCarrierName());
  }

  Route addToName(const String& toName) {
    return Route(getFromName(),toName,getCarrierName());
  }

  Route addCarrierName(const String& carrierName) {
    return Route(getFromName(),getToName(),carrierName);
  }

  String toString() {
    return getFromName() + "->" + getCarrierName() + "->" +
      getToName();
  }

private:
  String fromKey, toKey, carrier;
};

#endif

