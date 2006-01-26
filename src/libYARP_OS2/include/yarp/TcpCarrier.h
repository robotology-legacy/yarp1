#ifndef _YARP2_TCPCARRIER_
#define _YARP2_TCPCARRIER_

#include <yarp/AbstractCarrier.h>

namespace yarp {
  class TcpCarrier;
}

class yarp::TcpCarrier : public AbstractCarrier {

  TcpCarrier() {
    requireAck = true;
  }

  virtual Carrier *create() {
    return new TcpCarrier();
  }

  virtual String getName() {
    return "tcp";
  }

  int getSpecifier() {
    return 3;
  }

  virtual bool checkHeader(const Bytes& header) {
    return getSpecifier(header)%16 == getSpecifier();
  }

  virtual void getHeader(const Bytes& header) {
    createStandardHeader(getSpecifier()+(requireAck?128:0), header);
  }

  virtual void setParameters(const Bytes& header) {
    int specifier = getSpecifier(header);
    requireAck = (specifier&128)!=0;
  }

  virtual bool requireAck() {
    return requireAck;
  }

  virtual bool isConnectionless() {
    return false;
  }

  virtual void expectReplyToHeader(Protocol& proto) {
    int cport = proto.getStreams().getLocalAddress().getPort();
    proto.writeYarpInt(cport);
  }

  virtual void start(const Address& address, ShiftStream& previous) {
    YARP_DEBUG(LM_ERROR,"not implemented");
    throw IOException("not implemented");
  }

private:
  bool requireAck;
};

#endif

