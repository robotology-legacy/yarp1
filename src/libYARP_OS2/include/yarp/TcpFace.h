#ifndef _YARP2_TCPFACE_
#define _YARP2_TCPFACE_

#include  <yarp/Face.h>

#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>


namespace yarp {
  class TcpFace;
}

/**
 * One of the few classes that actually is in direct contact with the network
 */
class yarp::TcpFace : public Face {
public:
  virtual void open(const Address& address); // throws IOException
  virtual void close(); // throws IOException
  virtual InputProtocol *read(); // throws IOException
private:
  Address address;
  ACE_SOCK_Acceptor peerAcceptor;
};


#endif
