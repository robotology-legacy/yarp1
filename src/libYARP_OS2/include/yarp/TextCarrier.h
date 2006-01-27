#ifndef _YARP2_TEXTCARRIER_
#define _YARP2_TEXTCARRIER_

#include <yarp/TcpCarrier.h>

namespace yarp {
  class TextCarrier;
}

class yarp::TextCarrier : public TcpCarrier {
public:

  virtual String getName() {
    return "txt";
  }

  virtual String getSpecifierName() {
    return "CONNECT ";
  }

  virtual bool checkHeader(const Bytes& header) {
    if (header.length()==8) {
      String target = getSpecifierName();
      for (int i=0; i<8; i++) {
	if (!(target[i]==header.get()[i])) {
	  return false;
	}
      }
      return true;
    }
    return false;
  }

  virtual void getHeader(const Bytes& header) {
    if (header.length()==8) {
      String target = getSpecifierName();
      for (int i=0; i<8; i++) {
	header.get()[i] = target[i];
      }
    }   
  }

  TextCarrier() {
  }

  virtual Carrier *create() {
    return new TextCarrier();
  }

  virtual bool requireAck() {
    return false;
  }

  virtual bool isTextMode() {
    return true;
  }

  virtual void sendHeader(Protocol& proto) {
    String target = getSpecifierName();
    Bytes b((char*)target.c_str(),8);
    proto.os().write(b);
    String from = proto.getRoute().getFromName();
    Bytes b2((char*)from.c_str(),from.length()+1);
    proto.os().write(b2);
    proto.os().write('\n');
    proto.os().flush();
  }

  void expectReplyToHeader(Protocol& proto) {
    ACE_DEBUG((LM_INFO,"don't know what to do about BECOME"));
  }

  void expectSenderSpecifier(Protocol& proto) {
    ACE_DEBUG((LM_ERROR,"not implemented"));
    throw IOException("not implemented");
  }

  void sendIndex(Protocol& proto) {
  }

  void expectIndex(Protocol& proto) {
  }

  void sendAck(Protocol& proto) {
  }

  void respondToHeader(Protocol& proto) {
    String from = "Welcome ";
    from += proto.getRoute().getFromName();
    from += '\n';
    Bytes b2((char*)from.c_str(),from.length()+1);
    proto.os().write(b2);
    proto.os().flush();
    ACE_DEBUG((LM_INFO,"don't know what to do about BECOME"));
  }
};

#endif
