
#include <yarp/DgramTwoWayStream.h>

#include <yarp/Logger.h>
#include <yarp/Time.h>

#include <ace/SOCK_Dgram_Mcast.h>

using namespace yarp;

void DgramTwoWayStream::open(const Address& remote) {
  Address local;
  ACE_INET_Addr anywhere((u_short)0, INADDR_ANY);
  local = Address(anywhere.get_host_addr(),
		  anywhere.get_port_number());
  open(local,remote);
}

void DgramTwoWayStream::open(const Address& local, const Address& remote) {
  localAddress = local;
  remoteAddress = remote;

  localHandle = ACE_INET_Addr(localAddress.getPort());
  if (remote.isValid()) {
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());
  }
  dgram = new ACE_SOCK_Dgram;
  YARP_ASSERT(dgram!=NULL);
  int result = dgram->open(localHandle);
  if (result!=0) {
    throw IOException("could not open datagram socket");
  }
  dgram->get_local_addr(localHandle);
  localAddress = Address(localHandle.get_host_addr(),
			 localHandle.get_port_number());
  YARP_DEBUG(Logger::get(),String("Update: DGRAM from ") + 
	     localAddress.toString() + 
	     " to " + remote.toString());


  readBuffer.allocate(512);
  writeBuffer.allocate(512);
  readAt = 0;
  readAvail = 0;
  writeAvail = 0;
  happy = true;
}


void DgramTwoWayStream::join(const Address& group, bool sender) {

  if (sender) {
    // just use udp as normal
    open(group);
    return;
  }

  ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast;
  dgram = dmcast;
  YARP_ASSERT(dgram!=NULL);
  YARP_DEBUG(Logger::get(),String("subscribing to mcast address ") + 
	     group.toString());
  ACE_INET_Addr addr(group.getPort(),group.getName().c_str());
  int result = dmcast->subscribe(addr,1);
  if (result!=0) {
    throw IOException("cannot connect to multi-cast address");
  }
  localAddress = group;
  remoteAddress = group;
  localHandle.set(localAddress.getPort(),localAddress.getName().c_str());
  remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());

  readBuffer.allocate(512);
  writeBuffer.allocate(512);
  readAt = 0;
  readAvail = 0;
  writeAvail = 0;
  happy = true;
}

DgramTwoWayStream::~DgramTwoWayStream() {
  close();
}

void DgramTwoWayStream::interrupt() {
  if (!closed) {
    closed = true;
    if (reader) {
      YARP_DEBUG(Logger::get(),"dgram interrupt");
      try {
	DgramTwoWayStream tmp;
	tmp.open(Address(localAddress.getName(),0),localAddress);
	ManagedBytes empty(10);
	for (int i=0; i<empty.length(); i++) {
	  empty.get()[i] = 0;
	}
	tmp.write(empty.bytes());
	tmp.flush();
	tmp.close();
      } catch (IOException e) {
	YARP_DEBUG(Logger::get(),e.toString() + " <<< closer dgram exception");
      }
    YARP_DEBUG(Logger::get(),"finished dgram interrupt");
    }
  }
  happy = false;
}

void DgramTwoWayStream::close() {
  if (dgram!=NULL) {
    interrupt();
    if (dgram!=NULL) {
      dgram->close();
      delete dgram;
      dgram = NULL;
    }
  }
  happy = false;
}

int DgramTwoWayStream::read(const Bytes& b) {
  reader = true;

  if (closed) { 
    return -1; 
  }

  // if nothing is available, try to grab stuff
  if (readAvail==0) {
    readAt = 0;
    ACE_INET_Addr dummy((u_short)0, INADDR_ANY);
    YARP_ASSERT(dgram!=NULL);
    YARP_DEBUG(Logger::get(),"DGRAM Waiting for something!");
    int result =
      dgram->recv(readBuffer.get(),readBuffer.length(),dummy,1);
    YARP_DEBUG(Logger::get(),"DGRAM Got something!");
    if (closed) { return -1; }
    if (result<0) {
      happy = false;
      return result;
    }
    readAvail = result;
  }

  // if stuff is available, take it
  if (readAvail>0) {
    int take = readAvail;
    if (take>b.length()) {
      take = b.length();
    }
    ACE_OS::memcpy(b.get(),readBuffer.get()+readAt,take);
    readAt += take;
    readAvail -= take;
    return take;
  }

  return 0;
}

void DgramTwoWayStream::write(const Bytes& b) {
  //YARP_DEBUG(Logger::get(),"DGRAM prep writing");
  //ACE_OS::printf("DGRAM write %d bytes\n",b.length());

  Bytes local = b;
  while (local.length()>0) {
    //YARP_DEBUG(Logger::get(),"DGRAM prep writing");
    int rem = local.length();
    int space = writeBuffer.length()-writeAvail;
    bool shouldFlush = false;
    if (rem>=space) {
      rem = space;
      shouldFlush = true;
    }
    memcpy(writeBuffer.get()+writeAvail, local.get(), rem);
    writeAvail+=rem;
    local = Bytes(local.get()+rem,local.length()-rem);
    if (shouldFlush) {
      flush();
    }
  }
}


void DgramTwoWayStream::flush() {
  while (writeAvail>0) {
    int writeAt = 0;
    YARP_DEBUG(Logger::get(),"DGRAM writing");
    YARP_ASSERT(dgram!=NULL);
    int len = 0;

    len = dgram->send(writeBuffer.get()+writeAt,writeAvail-writeAt,
		      remoteHandle);

    if (len<0) {
      happy = false;
      throw IOException("DGRAM failed to write");
    }
    writeAt += len;
    writeAvail -= len;
  }
}


bool DgramTwoWayStream::isOk() {
  return happy;
}


void DgramTwoWayStream::reset() {
  readAt = 0;
  readAvail = 0;
  writeAvail = 0;
}



