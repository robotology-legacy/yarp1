#ifndef _YARP2_TCPFACE_
#define _YARP2_TCPFACE_

#include  <yarp/Face.h>
#include  <yarp/Semaphore.h>

#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>


namespace yarp {
  class TcpFace;
}

/**
 * Communicating with a port via TCP.
 */
class yarp::TcpFace : public Face {
public:
  TcpFace() { }

  virtual ~TcpFace();

  virtual void open(const Address& address); // throws IOException
  virtual void close(); // throws IOException
  virtual InputProtocol *read(); // throws IOException
  virtual OutputProtocol *write(const Address& address); // throws IOException

  /**
   * This class like all classes except the port objects does
   * not have any notion of running in a multi-threaded environment.
   *
   */

private:
  void closeFace();
  Address address;
  ACE_SOCK_Acceptor peerAcceptor;
};


#endif
