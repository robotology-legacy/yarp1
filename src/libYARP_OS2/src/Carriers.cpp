
#include <yarp/Carriers.h>
#include <yarp/Logger.h>
#include <yarp/TcpFace.h>
#include <yarp/FakeFace.h>

using namespace yarp;


static Logger log("Carriers", Logger::get());


Carrier *Carriers::chooseCarrier(const String& name) {
  YARP_FAIL(log,"Dummy called");
  return NULL;
}

Carrier *Carriers::chooseCarrier(const Bytes& bytes) {
  YARP_FAIL(log,"Dummy called");
  return NULL;
}


Face *Carriers::listen(const Address& address) {
  // for now, only TcpFace exists - otherwise would need to manage 
  // multiple possibilities
  YARP_DEBUG(log,"listen called");
  Face *face = NULL;
  if (address.getCarrierName() == String("fake")) {
    face = new FakeFace();
  }
  if (face == NULL) {
    new TcpFace();
  }
  face->open(address);
  return face;
}


OutputProtocol *Carriers::connect(const Address& address) {
  YARP_FAIL(Logger::get(),"Dummy called");
  return NULL;
}


