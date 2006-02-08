#ifndef _YARP2_ROUTE_
#define _YARP2_ROUTE_

#include <yarp/String.h>

namespace yarp {
  class Route;
}

/**
 * Information about a connection between two ports.
 * Contains the names of the endpoints, and the name of 
 * the carrier in use between them.
 */
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

  const String& getFromName() const {
    return fromKey;
  }

  const String& getToName() const {
    return toKey;
  }

  const String& getCarrierName() const {
    return carrier;
  }

  Route addFromName(const String& fromName) const {
    return Route(fromName,getToName(),getCarrierName());
  }

  Route addToName(const String& toName) const {
    return Route(getFromName(),toName,getCarrierName());
  }

  Route addCarrierName(const String& carrierName) const {
    return Route(getFromName(),getToName(),carrierName);
  }

  String toString() const {
    return getFromName() + "->" + getCarrierName() + "->" +
      getToName();
  }

private:
  String fromKey, toKey, carrier;
};

#endif

