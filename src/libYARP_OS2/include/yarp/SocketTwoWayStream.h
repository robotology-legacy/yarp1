#ifndef _YARP2_SOCKETTWOWAYSTREAM_
#define _YARP2_SOCKETTWOWAYSTREAM_

#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>

#include <ace/config.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

namespace yarp {
  class SocketTwoWayStream;
}

class yarp::SocketTwoWayStream : public TwoWayStream, 
	    InputStream, OutputStream {
public:
  SocketTwoWayStream() {
  }

  int open(const Address& address);

  void open(ACE_SOCK_Acceptor& acceptor);

  virtual ~SocketTwoWayStream() {
    close();
  }

  virtual InputStream& getInputStream() {
    return *this;
  }

  virtual OutputStream& getOutputStream() {
    return *this;
  }

  virtual const Address& getLocalAddress() {
    return localAddress;
  }

  virtual const Address& getRemoteAddress() {
    return remoteAddress;
  }

  virtual void interrupt() {
    stream.close_reader();
    stream.close_writer();
  }

  virtual void close() {
    stream.close();
  }

  virtual int read(const Bytes& b) {
    //ACE_OS::printf("STWS::read pre \n");
    int result = stream.recv_n(b.get(),b.length());
    //ACE_OS::printf("STWS::read post \n");
    return result;
  }

  virtual void write(const Bytes& b) {
    stream.send_n(b.get(),b.length());
  }

private:
  ACE_SOCK_Stream stream;
  Address localAddress, remoteAddress;
  void updateAddresses();
};

#endif

