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

  void open(const Address& address) {
    ACE_SOCK_Connector connector;
    ACE_INET_Addr addr(address.getPort(),address.getName().c_str());
    int result = connector.connect(stream,addr);
    if (result==-1) {
      throw IOException("Cannot make socket connection");
    }
  }

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
    return dummy;
  }

  virtual const Address& getRemoteAddress() {
    return dummy;
  }

  virtual void close() {
    stream.close();
  }

  virtual int read(const Bytes& b) {
    return stream.recv_n(b.get(),b.length());
  }

  virtual void write(const Bytes& b) {
    stream.send_n(b.get(),b.length());
  }

private:
  ACE_SOCK_Stream stream;
  Address dummy;
};

#endif

