#ifndef _YARP2_TCPCARRIER_
#define _YARP2_TCPCARRIER_

#include <yarp/AbstractCarrier.h>

namespace yarp {
  class TcpCarrier;
}

class yarp::TcpCarrier : public AbstractCarrier {
public:

  TcpCarrier() {
    requireAckFlag = true;
  }

  virtual Carrier *create() {
    return new TcpCarrier();
  }

  virtual String getName() {
    return "tcp";
  }

  int getSpecifierCode() {
    return 3;
  }

  virtual bool checkHeader(const Bytes& header) {
    return getSpecifier(header)%16 == getSpecifierCode();
  }

  virtual void getHeader(const Bytes& header) {
    createStandardHeader(getSpecifierCode()+(requireAckFlag?128:0), header);
  }

  virtual void setParameters(const Bytes& header) {
    int specifier = getSpecifier(header);
    requireAckFlag = (specifier&128)!=0;
  }

  virtual bool requireAck() {
    return requireAckFlag;
  }

  virtual bool isConnectionless() {
    return false;
  }

  virtual void respondToHeader(Protocol& proto) {
    int cport = proto.getStreams().getLocalAddress().getPort();
    proto.writeYarpInt(cport);
  }

  virtual void expectReplyToHeader(Protocol& proto) {
    ACE_DEBUG((LM_DEBUG,"TcpCarrier::expectReplyToHeader"));
    ACE_OS::printf("booga booga\n");
    proto.readYarpInt(); // ignore result
  }

  virtual void start(const Address& address, ShiftStream& previous) {
    //TwoWayStream *str = previous.giveStream();
    //takeStream(str);
  }

private:
  bool requireAckFlag;
};

#endif

