
#include <yarp/TcpFace.h>
#include <yarp/Logger.h>

using namespace yarp;

static Logger log("TcpFace", Logger::get());

void TcpFace::open(const Address& address) {
  YARP_DEBUG(log,"TcpFace should throw exceptions");

  this->address = address;
  ACE_INET_Addr	serverAddr(address.getPort());
  peerAcceptor.open(serverAddr,1);
}

void TcpFace::close() {
  YARP_DEBUG(log,"TcpFace should throw exceptions");
  peerAcceptor.close();
}


InputProtocol *TcpFace::read() {
  YARP_DEBUG(log,"TcpFace should throw exceptions");

  //ACE_INET_Addr clientAddr;
  ACE_SOCK_Stream newStream;

  YARP_DEBUG(log,"Waiting for something to happen");
  peerAcceptor.accept(newStream);
  YARP_DEBUG(log,"Got something");

  return NULL;
}
