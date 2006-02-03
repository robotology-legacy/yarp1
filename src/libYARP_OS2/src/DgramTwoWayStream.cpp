
#include <yarp/DgramTwoWayStream.h>

#include <yarp/Logger.h>

using namespace yarp;

void DgramTwoWayStream::open(const Address& remote) {
  Address local;
  ACE_INET_Addr anywhere((u_short)0, INADDR_ANY);
  local = Address(anywhere.get_host_addr(),
		  anywhere.get_port_number());
  open(local,remote);
}

void DgramTwoWayStream::open(const Address& local, const Address& remote) {
  YARP_DEBUG(Logger::get(),"DGRAM is *very* experimental");

  localAddress = local;
  remoteAddress = remote;

  //YARP_ERROR(Logger::get(),String("DGRAM from ") + local.toString() + 
  //     " to " + remote.toString());


  //localHandle.set(localAddress.getPort(),localAddress.getName().c_str());

  localHandle = ACE_INET_Addr(localAddress.getPort());
  //remoteHandle = ACE_INET_Addr(remoteAddress.getPort());
  if (remote.isValid()) {
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getName().c_str());
  }
  int result = dgram.open(localHandle);
  if (result!=0) {
    throw IOException("could not open dgram socket");
  }
  dgram.get_local_addr(localHandle);
  localAddress = Address(localHandle.get_host_addr(),
			 localHandle.get_port_number());
  YARP_DEBUG(Logger::get(),String("Update: DGRAM from ") + 
	     localAddress.toString() + 
	     " to " + remote.toString());


  readBuffer.allocate(512);
  writeBuffer.allocate(512);
  readAt = 0;
  readAvail = 0;
  writeAt = 0;
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
	ManagedBytes empty(10000);
	tmp.write(empty.bytes());
	tmp.close();
      } catch (IOException e) {
	YARP_DEBUG(Logger::get(),e.toString() + " <<< closer dgram exception");
      }
    YARP_DEBUG(Logger::get(),"finished dgram interrupt");
    }
  }
}

void DgramTwoWayStream::close() {
  interrupt();
  dgram.close();
}

int DgramTwoWayStream::read(const Bytes& b) {
  reader = true;

  if (closed) { return -1; }

  // if nothing is available, try to grab stuff
  if (readAvail==0) {
    readAt = 0;
    ACE_INET_Addr dummy;
    int result = 
      dgram.recv(readBuffer.get(),readBuffer.length(),dummy,1);
    YARP_DEBUG(Logger::get(),"DGRAM Got something!");
    //dgram.recv(readBuffer.get(),readBuffer.length(),remoteHandle,1);
    if (closed) { return -1; }
    if (result<0) return result;
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
    //ACE_OS::printf("dgram read ends %d\n", take);
    return take;
  }

  //ACE_OS::printf("dgram read ends\n");
  return 0;
}

void DgramTwoWayStream::write(const Bytes& b) {
  //int packetLen = buffer.length();
  //int len = b.length();

  // should buffer, but no buffering right now
  dgram.send(b.get(),b.length(),remoteHandle);
}

void DgramTwoWayStream::flush() {
  /*
  if (at>0) {
    dgram.send(buffer.get(),at,target);
    at = 0;
  }
  */
}




