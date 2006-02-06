#ifndef _YARP2_MCASTCARRIER_
#define _YARP2_MCASTCARRIER_

#include <yarp/AbstractCarrier.h>
#include <yarp/UdpCarrier.h>
#include <yarp/DgramTwoWayStream.h>

#include <yarp/Election.h>

namespace yarp {
  class McastCarrier;
}

class yarp::McastCarrier : public UdpCarrier {
protected:
  Address mcastAddress;

  static ElectionOf<McastCarrier> caster;

public:

  McastCarrier() {
  }

  virtual Carrier *create() {
    return new McastCarrier();
  }

  virtual String getName() {
    return "mcast";
  }

  virtual int getSpecifierCode() {
    return 1;
  }


  virtual void sendHeader(Protocol& proto) {
    // need to do more than the default
    proto.defaultSendHeader();
    YARP_DEBUG(Logger::get(),"Adding extra mcast header");

    // hard-code mcast address for now
    Address addr("224.3.1.1",11000);
    int ip[] = { 224, 3, 1, 1 };
    int port = 11000;

    ManagedBytes block(6);
    for (int i=0; i<4; i++) {
      ((unsigned char*)block.get())[i] = (unsigned char)ip[i];
    }
    block.get()[5] = (char)(port%256);
    block.get()[4] = (char)(port/256);
    proto.os().write(block.bytes());
    mcastAddress = addr;
  }

  virtual void expectExtraHeader(Protocol& proto) {
    YARP_DEBUG(Logger::get(),"Expecting extra mcast header");
    ManagedBytes block(6);
    int len = NetType::readFull(proto.is(),block.bytes());
    if (len!=block.length()) {
      throw new IOException("problem with MCAST header");
    }

    int ip[] = { 0, 0, 0, 0 };
    int port = -1;

    unsigned char *base = (unsigned char *)block.get();
    String add;
    for (int i=0; i<4; i++) {
      ip[i] = base[i];
      if (i!=0) { add += "."; }
      char buf[100];
      sprintf(buf,"%d",ip[i]);
      add += buf;
    }
    port = 256*base[4]+base[5];
    Address addr(add,port,"mcast");
    YARP_DEBUG(Logger::get(),String("got mcast header ") + addr.toString());
    mcastAddress = addr;
  }


  virtual void becomeMcast(Protocol& proto, bool sender) {
    ACE_UNUSED_ARG(sender);
    YARP_ERROR(Logger::get(),"MCAST is very experimental, and should not work yet...");
    DgramTwoWayStream *stream = new DgramTwoWayStream();
    YARP_ASSERT(stream!=NULL);
    Address remote = proto.getStreams().getRemoteAddress();
    proto.takeStreams(NULL); // free up port from tcp
    try {
      stream->join(mcastAddress,sender);
      //ManagedBytes b(100);
      //stream->write(b.bytes());
      //stream->open(remote);
      if (sender) {
	addSender(mcastAddress.toString());
      }
      
    } catch (IOException e) {
      delete stream;
      throw e;
    }
    proto.takeStreams(stream);
  }

  virtual void respondToHeader(Protocol& proto) {
    becomeMcast(proto,false);
  }


  virtual void expectReplyToHeader(Protocol& proto) {
    becomeMcast(proto,true);
  }

  virtual void start(const Address& address, ShiftStream& previous) {
    YARP_ERROR(Logger::get(),
	       "I don't think this method is needed anymore");
  }


  void addSender(const String& key) {
    caster.add(key,this);
  }
  
  void addRemove(const String& key) {
    caster.remove(key,this);
  }
  
  bool isElect() {
    void *elect = caster.getElect(mcastAddress.toString());
    return elect==this || elect==NULL;
  }


  virtual bool isActive() {
    return isElect();
  }

};

#endif


