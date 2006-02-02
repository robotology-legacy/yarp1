
#ifndef _YARP2_UDPCARRIER_
#define _YARP2_UDPCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/DgramTwoWayStream.h>

namespace yarp {
  class UdpCarrier;
}

class yarp::UdpCarrier : public AbstractCarrier {
public:

  UdpCarrier() {
  }

  virtual Carrier *create() {
    return new UdpCarrier();
  }

  virtual String getName() {
    return "udp";
  }

  int getSpecifierCode() {
    return 0;
  }

  virtual bool checkHeader(const Bytes& header) {
    return getSpecifier(header)%16 == getSpecifierCode();
  }

  virtual void getHeader(const Bytes& header) {
    createStandardHeader(getSpecifierCode(), header);
  }

  virtual void setParameters(const Bytes& header) {
  }

  virtual bool requireAck() {
    return false;
  }

  virtual bool isConnectionless() {
    return true;
  }


  virtual void respondToHeader(Protocol& proto) {
    // I am the receiver

    // issue: need a fresh port number...
    int myPort = proto.getStreams().getLocalAddress().getPort()+101;

    Address local = proto.getStreams().getLocalAddress();
    Address remote = proto.getStreams().getRemoteAddress();

    proto.writeYarpInt(myPort);

    DgramTwoWayStream *stream = new DgramTwoWayStream();
    YARP_ASSERT(stream!=NULL);
    
    YARP_DEBUG(Logger::get(),
	       "booga booga, UdpCarrier not really implemented yet");
    proto.takeStreams(NULL); // free up port from tcp
    try {
      stream->open(Address(local.getName(),myPort),remote);
    } catch (IOException e) {
      delete stream;
      throw e;
    }
    proto.takeStreams(stream);
  }

  virtual void expectReplyToHeader(Protocol& proto) {
    // I am the sender
    int myPort = proto.getStreams().getLocalAddress().getPort();
    String myName = proto.getStreams().getLocalAddress().getName();
    String altName = proto.getStreams().getRemoteAddress().getName();

    YARP_DEBUG(Logger::get(),
	       "booga booga, UdpCarrier not really implemented yet");
    int altPort = proto.readYarpInt();

    DgramTwoWayStream *stream = new DgramTwoWayStream();
    YARP_ASSERT(stream!=NULL);

    proto.takeStreams(NULL); // free up port from tcp
    try {
      stream->open(Address(myName,myPort),Address(altName,altPort));
    } catch (IOException e) {
      delete stream;
      throw e;
    }
    proto.takeStreams(stream);
  }

  virtual void start(const Address& address, ShiftStream& previous) {
    //TwoWayStream *str = previous.giveStream();
    //takeStream(str);
    YARP_ERROR(Logger::get(),
	       "UdpCarrier should start streams, but doesn't yet");
  }


};

#endif

