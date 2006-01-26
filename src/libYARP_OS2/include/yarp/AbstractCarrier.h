#ifndef _YARP2_ABSTRACTCARRIER_
#define _YARP2_ABSTRACTCARRIER_

namespace yarp {
  class AbstractCarrier;
}

class yarp::AbstractCarrier : public Carrier {

  virtual Carrier *create() = 0;

  virtual String getName() = 0;

  virtual bool checkHeader(const Bytes& header) = 0;

  virtual void setParameters(const Bytes& header) {
    // default - no parameters
  }

  virtual void getHeader(const Bytes& header) = 0;


  virtual bool isConnectionless() {
    // conservative choice - shortcuts are taken for connection
    return true;
  }


  virtual bool canAccept() {
    return true;
  }

  virtual bool canOffer() {
    return true;
  }

  virtual bool isTextMode() {
    return false;
  }

  virtual bool requireAck() {
    return false;
  }

  virtual String toString() {
    return getName();
  }

  // all remaining may throw IOException

  virtual void open(const Address& address, ShiftStream& previous) = 0;
  virtual void close() = 0;

  // sender
  virtual void prepareSend(Protocol& proto) {
  }

  virtual void sendHeader(Protocol& proto) {
    proto.defaultSendHeader();
  }

  virtual void expectReplyToHeader(Protocol& proto) {
  }

  virtual void sendIndex(Protocol& proto) {
    proto.defaultSendIndex();
  }

  // receiver
  virtual void expectExtraHeader(Protocol& proto) {
  }

  // left abstract, no good default
  virtual void respondToHeader(Protocol& proto) = 0;

  virtual void expectIndex(Protocol& proto) {
    proto.defaultExpectIndex();
  }

  virtual void expectSenderSpecifer(Protocol& proto) {
    proto.defaultExpectSenderSpecifier();
  }

  virtual void sendAck(Protocol& proto) {
    proto.defaultSendAck();
  }

  virtual bool isActive() {
    return true;
  }


protected:

  int getSpecifier(const Bytes& b) {
    int x = Protocol::interpretYarpNumber(b);
    if (x>=0) {
      return x-7777;
    }
    return x;
  }

  void createStandardHeader(int specifier,const Bytes& header) {
    Protocol::createYarpNumber(7777+specifier,header);
  }

};

#endif
