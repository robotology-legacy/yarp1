
#include <yarp/TcpFace.h>
#include <yarp/Logger.h>
#include <yarp/SocketTwoWayStream.h>
#include <yarp/Protocol.h>

#include <ace/OS_NS_stdio.h>

using namespace yarp;

static Logger log("TcpFace", Logger::get());

TcpFace::TcpFace() {
  closed = true;
}

TcpFace::~TcpFace() {
  if (!closed) {
    close();
  }
}


void TcpFace::open(const Address& address) {
  YARP_DEBUG(log,"TcpFace should throw exceptions");

  this->address = address;
  ACE_INET_Addr	serverAddr(address.getPort());
  peerAcceptor.open(serverAddr,1);
  closed = false;
}

void TcpFace::close() {
  YARP_DEBUG(log,"TcpFace should throw exceptions");
  if (!closed) {
    closed = true;
    try {
      OutputProtocol *op = write(address);
      if (op!=NULL) {
	op->close();
	delete op;
      }
    } catch (IOException e) {
      // no problem
      ACE_OS::printf("exception during write\n");
    }
    peerAcceptor.close();
  }
}


InputProtocol *TcpFace::read() {
  YARP_DEBUG(log,"TcpFace should throw exceptions");

  //ACE_INET_Addr clientAddr;
  //ACE_SOCK_Stream newStream;

  YARP_DEBUG(log,"Waiting for something to happen");
  //peerAcceptor.accept(newStream);
  
  SocketTwoWayStream *stream  = new SocketTwoWayStream();
  try {
    //ACE_OS::printf("waiting to open stream\n");
    stream->open(peerAcceptor);
    //ACE_OS::printf("opened stream\n");
  } catch (IOException e) {
    //ACE_OS::printf("cleaning up stream\n");
    delete stream;
    throw e;
  }
  if (closed) {
    peerAcceptor.close();
    //ACE_OS::printf("cleaning up stream, TcpFace closed\n");
    stream->close();
    delete stream;
    stream = NULL;
    throw IOException("TcpFace closed");
  }
  return new Protocol(stream);
}


OutputProtocol *TcpFace::write(const Address& address) {
  SocketTwoWayStream *stream  = new SocketTwoWayStream();
  stream->open(address);
  return new Protocol(stream);
}

