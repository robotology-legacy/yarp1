
#include <yarp/FallbackNameClient.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>
#include <yarp/NameClient.h>
#include <yarp/os/Time.h>
#include <yarp/FallbackNameServer.h>

using namespace yarp;
using namespace yarp::os;

void FallbackNameClient::run() {
  try {
  Address call = FallbackNameServer::getAddress();
  DgramTwoWayStream send;
  send.join(call,true);
  listen.join(call,false);
  String msg = "NAME_SERVER query root";
  send.writeLine(msg);
  send.flush();
  for (int i=0; i<5; i++) {
    String txt = NetType::readLine(listen);
    if (closed) return;
    YARP_DEBUG(Logger::get(),String("Fallback name client got ") + txt);
    if (txt.strstr("registration ")==0) {
      address = NameClient::extractAddress(txt);
      YARP_DEBUG(Logger::get(),String("Extracted address ") + 
		 address.toString());
      return;
    }
  }
  } catch (IOException e) {
    YARP_DEBUG(Logger::get(),e.toString() + "<<< Fallback client shutting down with exception");
  }
}


void FallbackNameClient::close() {
  if (!closed) {
    closed = true;
    listen.interrupt();
    listen.close();
    join();
  }
}


Address FallbackNameClient::getAddress() {
  return address;
}


Address FallbackNameClient::seek() {
  int tries = 3;
  for (int k=0; k<tries; k++) {

    FallbackNameClient seeker;

    YARP_INFO(Logger::get(),
	      String("Broadcast search for name server try ") + 
	      NetType::toString(k+1) + 
	      String(" of max ") + 
	      NetType::toString(tries));

    seeker.start();
    Time::delay(0.25);
    if (seeker.getAddress().isValid()) {
      return seeker.getAddress();
    }
    int len = 20;
    for (int i0=0; i0<len; i0++) {
      ACE_OS::fprintf(stderr,"++");
    }
    ACE_OS::fprintf(stderr,"\n");
	    
    for (int i=0; i<len; i++) {
      Time::delay(0.025);
      ACE_OS::fprintf(stderr,"++");
      if (seeker.getAddress().isValid()) {
	ACE_OS::fprintf(stderr,"\n");
	return seeker.getAddress();
      }
    }
    ACE_OS::fprintf(stderr,"\n");
    YARP_INFO(Logger::get(),"No response to broadcast search");
    try {
      seeker.close();
      seeker.join();
    } catch (IOException e) {
      YARP_DEBUG(Logger::get(), e.toString() + " <<< exception while closing seeker");
    }
  }
  return Address();
}


