#ifndef _YARP2_NAMESERVER_
#define _YARP2_NAMESERVER_

#include <yarp/String.h>
#include <yarp/Address.h>
#include <yarp/Logger.h>
#include <yarp/Dispatcher.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

namespace yarp {
  class NameServer;
}

/*
  Implementation of a YARP2-conforming name server.
 */
class yarp::NameServer {
public:

  NameServer();

  virtual ~NameServer() {}

  // address may be partial - partial information gets filled in
  // (not YARP2 compliant yet, won't do fill-in)
  Address registerName(const String& name, 
		       const Address& address);

  Address registerName(const String& name) {
    return registerName(name,Address());
  }

  Address queryName(const String& name);

  Address unregisterName(const String& name) {
    ACE_OS::printf("NameServer::unregisterName not implemented\n");
    return Address();
  }

  static int main(int argc, char *argv[]);

protected:

  String cmdRegister(int argc, char *argv[]);

  String textify(const Address& addr);
  String terminate(const String& str);


  class NameRecord {
  private:
    Address address;
  public:
    void setAddress(const Address& address) {
      this->address = address;
    }

    Address getAddress() {
      return address;
    }
  };

  ACE_Hash_Map_Manager<String,NameRecord,ACE_Null_Mutex> nameMap;
  
  NameRecord *getNameRecord(const String& name, bool create);

  NameRecord& getNameRecord(const String& name) {
    NameRecord *result = getNameRecord(name,true);
    YARP_ASSERT(result!=NULL);
    return *result;
  }

  Dispatcher<NameServer,String> dispatcher;

};

#endif

