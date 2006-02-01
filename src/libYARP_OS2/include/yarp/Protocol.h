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

class yarp::Protocol : public OutputProtocol, public InputProtocol {
public:

  // everything could throw IOException

  /**
   * This becomes owner of shiftstream
   */
  Protocol(TwoWayStream *stream) : 
    log(Logger::get()), header(8), number(4) {
    shift.takeStream(stream);
    route = Route("null","null","tcp");
    delegate = NULL;
    messageLen = 0;
    pendingAck = false;
    writer = NULL;
  }

  virtual ~Protocol() {
    /*
    if (delegate!=NULL) {
      delegate->close();
      delete delegate;
      delegate = NULL;
    }
    shift.close();
    */
    closeHelper();
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
    //writer.reset(delegate->isTextMode());
  }

  void respondToHeader() {
    YARP_ASSERT(delegate!=NULL);
    delegate->respondToHeader(*this);
    os().flush();
  }

  int readYarpInt() {
    int len = NetType::readFull(is(),header.bytes());
    ACE_UNUSED_ARG(len);
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
    // throw IOException("bad yarp number");
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
    ACE_DEBUG((LM_DEBUG,"Protocol::sendIndex for %s", getRoute().toString().c_str()));
    YARP_ASSERT(delegate!=NULL);
    delegate->sendIndex(*this);
  }

  void defaultSendIndex() {
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void sendContent() {
    YARP_ASSERT(writer!=NULL);
    writer->write(os());
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
    ACE_DEBUG((LM_DEBUG,"Protocol::expectIndex for %s", getRoute().toString().c_str()));
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void respondToIndex() {
  }


  void expectAck() {
    YARP_ASSERT(delegate!=NULL);
    if (delegate->requireAck()) {
      ACE_DEBUG((LM_DEBUG,"Protocol::expectAck for %s", getRoute().toString().c_str()));
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
    ACE_DEBUG((LM_DEBUG,"Protocol::sendAck for %s", getRoute().toString().c_str()));
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void interrupt() {
    if (pendingAck) {
      sendAck();
    }
    shift.getInputStream().interrupt();
  }

  void close() {
    closeHelper();
  }

  void closeHelper() {
    try {
      if (pendingAck) {
	sendAck();
      }
    } catch (IOException e) {
      // ok, comms shutting down
    }
    shift.close();
    if (delegate!=NULL) {
      delegate->close();
      delete delegate;
      delegate = NULL;
    }
  }

  TwoWayStream& getStreams() {
    return shift;
  }

  OutputStream& os() {
    return shift.getOutputStream();
  }

  InputStream& is() {
    return shift.getInputStream();
  }

  OutputStream& getOutputStream() {
    return os();
  }

  InputStream& getInputStream() {
    return is();
  }

  const Address& getRemoteAddress() {
    ACE_DEBUG((LM_ERROR,"Protocol::getRemoteAddress not yet implemented"));
    throw IOException("getRemoteAddress failed");
  }


  ///////////////////////////////////////////////////////////////////////
  // OutputProtocol view

  virtual void open(const Route& route) {
    setRoute(route);
    setCarrier(route.getCarrierName());
    sendHeader();
    expectReplyToHeader();
  }

  virtual void open(const String& name) {
    expectHeader();
    respondToHeader();
    setRoute(getRoute().addToName(name));
  }

  virtual bool isActive() {
    YARP_ASSERT(delegate!=NULL);
    return delegate->isActive();
  }

  virtual bool isTextMode() {
    YARP_ASSERT(delegate!=NULL);
    return delegate->isActive();
  }

  virtual void write(SizedWriter& writer) {
    this->writer = &writer;
    if (isActive()) {
      sendIndex();
      sendContent();
      expectAck();
    }
    this->writer = NULL;
  }

  /*
  virtual BlockWriter& beginWrite() {
    writer.reset(delegate->isTextMode());
    return writer;
  }

  virtual void endWrite() {
    if (isActive()) {
      sendIndex();
      sendContent();
      expectAck();
    }
  }
  */

  virtual OutputProtocol& getOutput() {
    return *this;
  }

  virtual InputProtocol& getInput() {
    return *this;
  }


  virtual BlockReader& beginRead() {
    expectIndex();
    respondToIndex();
    return reader;
  }

  virtual void endRead() {
    sendAck();
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
    ACE_UNUSED_ARG(len);
    //ACE_OS::printf("len is %d but header is %d\n", len, header.length());
    if (len==-1) {
      throw IOException("no connection");
    }
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
  Logger& log;
  ManagedBytes header;
  ManagedBytes number;
  ShiftStream shift;
  Carrier *delegate;
  Route route;
  //BufferedBlockWriter writer;
  SizedWriter *writer;
  StreamBlockReader reader;

};

#endif
