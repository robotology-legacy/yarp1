
#include <yarp/Carriers.h>
#include <yarp/Logger.h>
#include <yarp/TcpFace.h>
#include <yarp/FakeFace.h>
#include <yarp/TcpCarrier.h>
#include <yarp/TextCarrier.h>
#include <yarp/UdpCarrier.h>
#include <yarp/McastCarrier.h>
#include <yarp/ShmemCarrier.h>
#include <yarp/LocalCarrier.h>

using namespace yarp;


static Logger log("Carriers", Logger::get());

Carriers yarp::Carriers::instance;

Carriers::Carriers() {
  delegates.push_back(new LocalCarrier());
  delegates.push_back(new ShmemCarrier());
  delegates.push_back(new TcpCarrier());
  delegates.push_back(new McastCarrier());
  delegates.push_back(new UdpCarrier());
  delegates.push_back(new TextCarrier());
}

Carriers::~Carriers() {
  ACE_Vector<Carrier *>& lst = delegates;
  for (unsigned int i=0; i<lst.size(); i++) {
    delete lst[i];
  }
  lst.clear();
}

Carrier *Carriers::chooseCarrier(const String *name, const Bytes *header) {
  for (unsigned int i=0; i<delegates.size(); i++) {
    Carrier& c = *delegates[i];
    bool match = false;
    if (name!=NULL) {
      if ((*name) == c.getName()) {
	match = true;
      }
    }
    if (header!=NULL) {
      if (c.checkHeader(*header)) {
	match = true;
      }
    }
    if (match) {
      return c.create();
    }
  }
  ACE_DEBUG((LM_ERROR,"Could not find carrier %s", (name!=NULL)?name->c_str():"[bytes]"));
  throw IOException("Could not find carrier");
  return NULL;
}


Carrier *Carriers::chooseCarrier(const String& name) {
  return getInstance().chooseCarrier(&name,NULL);
}

Carrier *Carriers::chooseCarrier(const Bytes& bytes) {
  return getInstance().chooseCarrier(NULL,&bytes);
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
    face = new TcpFace();
  }
  try {
    face->open(address);
  } catch (IOException e) {
    delete face;
    throw e;
  }
  return face;
}


OutputProtocol *Carriers::connect(const Address& address) {
  TcpFace tcpFace;
  return tcpFace.write(address);
}


