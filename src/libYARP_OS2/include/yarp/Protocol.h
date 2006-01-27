#ifndef _YARP2_PROTOCOL_
#define _YARP2_PROTOCOL_

#include <yarp/Carrier.h>
#include <yarp/Logger.h>
#include <yarp/TwoWayStream.h>
#include <yarp/Carriers.h>
#include <yarp/BufferedBlockWriter.h>
#include <yarp/StreamBlockReader.h>
#include <yarp/ManagedBytes.h>
#include <yarp/NetType.h>
#include <yarp/ShiftStream.h>

namespace yarp {
  class Protocol;
}

class yarp::Protocol {
public:

  // everything could throw IOException

  /**
   * This becomes owner of shiftstream
   */
  Protocol(ShiftStream *shift) : 
    shift(shift), log(Logger::get()), header(8), number(4) {

    route = Route("null","null","tcp");
    delegate = NULL;
    messageLen = 0;
    pendingAck = false;
  }

  virtual ~Protocol() {
    if (delegate!=NULL) {
      delegate->close();
      delete delegate;
      delegate = NULL;
    }
    if (shift!=NULL) {
      shift->close();
      delete shift;
      shift = NULL;
    }
  }

  void setRoute(const Route& route) {
    this->route = route;
  }

  const Route& getRoute() {
    return route;
  }

  void setCarrier(const String& carrierName) {
    setRoute(getRoute().addCarrierName(carrierName));
    YARP_ASSERT(delegate==NULL);
    delegate = Carriers::chooseCarrier(carrierName);
    delegate->prepareSend(*this);
  }

  void defaultExpectSenderSpecifier() {
    int len = 0;
    NetType::readFull(is(),number.bytes());
    len = NetType::netInt(number.bytes());
    if (len>1000) len = 1000;
    if (len<1) len = 1;
    // expect a null-terminated string
    ManagedBytes b(len);
    NetType::readFull(is(),b.bytes());
    String s = b.get();
    setRoute(getRoute().addFromName(s));
  }

  void sendHeader() {
    YARP_ASSERT(delegate!=NULL);
    delegate->sendHeader(*this);
  }

  void defaultSendHeader() {
    sendProtocolSpecifier();
    sendSenderSpecifier();
  }

  void expectHeader() {
    messageLen = 0;
    expectProtocolSpecifier();
    expectSenderSpecifier();
    YARP_ASSERT(delegate!=NULL);
    delegate->expectExtraHeader(*this);
  }

  void expectReplyToHeader() {
    YARP_ASSERT(delegate!=NULL);
    delegate->expectReplyToHeader(*this);
    writer.reset(delegate->isTextMode());
  }

  void respondToHeader() {
    YARP_ASSERT(delegate!=NULL);
    delegate->respondToHeader(*this);
    os().flush();
  }

  int readYarpInt() {
    int port = 0;
    int len = NetType::readFull(is(),header.bytes());
    YARP_ASSERT(len==header.length());
    return interpretYarpNumber(header.bytes());
  }

  void writeYarpInt(int n) {
    createYarpNumber(n,header.bytes());
    os().write(header.bytes());
    os().flush();
  }

  static int interpretYarpNumber(const Bytes& b) {
    if (b.length()==8) {
      char *base = b.get();
      if (base[0]=='Y' && base[1]=='A' &&
	  base[6]=='R' && base[7]=='P') {
	Bytes b2(b.get()+2,4);
	int x = NetType::netInt(b2);
	return x;
      }
    }
    throw new IOException("bad yarp number");
    return -1;
  }

  static void createYarpNumber(int x,const Bytes& header) {
    if (header.length()!=8) {
      throw IOException("wrong header length");
    }
    char *base = header.get();
    base[0] = 'Y';
    base[1] = 'Y';
    base[6] = 'R';
    base[7] = 'P';
    Bytes code(base+2,4);
    NetType::netInt(x,code);
  }


  void sendIndex() {
    YARP_ASSERT(delegate!=NULL);
    delegate->sendIndex(*this);
  }

  void defaultSendIndex() {
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void sendContent() {
    writer.write(os());
    os().flush();
  }

  void expectIndex() {
    pendingAck = true;
    messageLen = 0;
    YARP_ASSERT(delegate!=NULL);
    delegate->expectIndex(*this);
    reader.reset(is(),messageLen,delegate->isTextMode());
  }

  void defaultExpectIndex() {
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void respondToIndex() {
  }


  void expectAck() {
    YARP_ASSERT(delegate!=NULL);
    if (delegate->requireAck()) {
      ACE_DEBUG((LM_ERROR,"not implemented"));
      throw IOException("not implemented");
    }
  }

  void sendAck() {
    pendingAck = false;
    YARP_ASSERT(delegate!=NULL);
    if (delegate->requireAck()) {
      delegate->sendAck(*this);
    }
  }

  void defaultSendAck() {
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void close() {
    if (pendingAck) {
      sendAck();
    }
    if (shift!=NULL) {
      shift->close();
      delete shift;
      shift = NULL;
    }
    if (delegate!=NULL) {
      delegate->close();
      delete delegate;
      delegate = NULL;
    }
  }

  TwoWayStream& getStreams() {
    YARP_ASSERT(shift!=NULL);
    return *shift;
  }

  OutputStream& os() {
    YARP_ASSERT(shift!=NULL);
    return shift->getOutputStream();
  }

  InputStream& is() {
    YARP_ASSERT(shift!=NULL);
    return shift->getInputStream();
  }


private:

  void sendProtocolSpecifier() {
    YARP_ASSERT(delegate!=NULL);
    delegate->getHeader(header.bytes());
    os().write(header.bytes());
    os().flush();
  }

  void expectProtocolSpecifier() {
    int len = NetType::readFull(is(),header.bytes());
    YARP_ASSERT(len==header.length());
    bool already = false;
    if (delegate!=NULL) {
      if (delegate->checkHeader(header.bytes())) {
	already = true;
      }
    }
    if (!already) {
      delegate = Carriers::chooseCarrier(header.bytes());
    }
    if (delegate==NULL) {
      throw IOException("unrecognized protocol");
    }
    delegate->setParameters(header.bytes());
  }


  void sendSenderSpecifier() {
    const String& senderName = getRoute().getFromName();
    NetType::netInt(senderName.length()+1,number.bytes());
    os().write(number.bytes());
    Bytes b((char*)senderName.c_str(),senderName.length()+1);
    os().write(b);
    os().flush();
  }

  void expectSenderSpecifier() {
    YARP_ASSERT(delegate!=NULL);
    delegate->expectSenderSpecifier(*this);
    ACE_DEBUG((LM_INFO,"Sender name is %s",getRoute().getFromName().c_str()));
  }
  

  int messageLen;
  bool pendingAck;
  ManagedBytes header;
  ManagedBytes number;
  ShiftStream *shift;
  Carrier *delegate;
  Route route;
  BufferedBlockWriter writer;
  StreamBlockReader reader;

  Logger& log;
};

#endif
