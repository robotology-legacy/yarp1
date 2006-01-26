#ifndef _YARP2_CARRIER_
#define _YARP2_CARRIER_

#include <yarp/ShiftStream.h>
#include <yarp/Bytes.h>
#include <yarp/Protocol.h>

namespace yarp {
  class Carrier;
}

class yarp::Carrier : public ShiftStream {

  virtual Carrier *create() = 0;

  virtual String getName() = 0;
  virtual bool checkHeader(const Bytes& header) = 0;
  virtual void setParameters(const Bytes& header) = 0;
  virtual void getHeader(const Bytes& header) = 0;


  virtual bool isConnectionless() = 0;
  virtual bool canAccept() = 0;
  virtual bool canOffer() = 0;
  virtual bool isTextMode() = 0;
  virtual bool requireAck() = 0;

  // all remaining may throw IOException

  virtual void start(const Address& address, ShiftStream& previous) = 0;

  //virtual void close() = 0; // from ShiftStream

  // sender
  virtual void prepareSend(Protocol& proto) = 0;
  virtual void sendHeader(Protocol& proto) = 0;
  virtual void expectReplyToHeader(Protocol& proto) = 0;
  virtual void sendIndex(Protocol& proto) = 0;

  // receiver
  virtual void expectExtraHeader(Protocol& proto) = 0;
  virtual void respondToHeader(Protocol& proto) = 0;
  virtual void expectIndex(Protocol& proto) = 0;
  virtual void expectSenderSpecifer(Protocol& proto) = 0;
  virtual void sendAck(Protocol& proto) = 0;

  virtual bool isActive() = 0;

  virtual String toString() = 0;
};

#endif

