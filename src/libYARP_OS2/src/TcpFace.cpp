
#include <yarp/TcpFace.h>
#include <yarp/Logger.h>
#include <yarp/SocketTwoWayStream.h>
#include <yarp/Protocol.h>

#include <ace/OS_NS_stdio.h>

using namespace yarp;

static Logger log("TcpFace", Logger::get());


TcpFace::~TcpFace() {
  closeFace();
}


void TcpFace::open(const Address& address) {
  YARP_DEBUG(log,"TcpFace should throw exceptions");

  this->address = address;
  ACE_INET_Addr	serverAddr(address.getPort());
  peerAcceptor.open(serverAddr,1);
}

void TcpFace::close() {
  closeFace();
}

void TcpFace::closeFace() {
  peerAcceptor.close();

  /*
  if (!closed) {
    closed = true;
    OutputProtocol *op = NULL;
    try {
      op = write(address);
      //ACE_OS::printf("write done, gave %ld\n", (long int)op);
      if (op!=NULL) {
	op->close();
      }
    } catch (IOException e) {
      // no problem
      ACE_OS::printf("exception during write\n");
    }
    if (op!=NULL) {
      delete op;
      op = NULL;
    }
    peerAcceptor.close();
  }
  */
}


/**
 * return NULL on failure.  No exceptions thrown.
 */
InputProtocol *TcpFace::read() {

  SocketTwoWayStream *stream  = new SocketTwoWayStream();

  try {
    stream->open(peerAcceptor);
  } catch (IOException e) {
    stream->close();
    delete stream;
    stream = NULL;
  }

  if (stream!=NULL) {
    return new Protocol(stream);
  }
  return NULL;
}


OutputProtocol *TcpFace::write(const Address& address) {
  SocketTwoWayStream *stream  = new SocketTwoWayStream();
  int result = stream->open(address);
  if (result==-1) {
    stream->close();
    delete stream;
    return NULL;
  }
  return new Protocol(stream);
}

