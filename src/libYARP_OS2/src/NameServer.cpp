
#include <yarp/NameServer.h>
#include <yarp/Logger.h>
#include <yarp/PortCore.h>
#include <yarp/Time.h>
#include <yarp/SplitString.h>
#include <yarp/NetType.h>

using namespace yarp;


NameServer::NameServer() {
  dispatcher.add("register", &NameServer::cmdRegister);
}

String NameServer::cmdRegister(int argc, char *argv[]) {
  if (argc<1) {
    return "need at least one argument";
  }
  String portName = argv[0];
  Address address = registerName(portName);
  return terminate(textify(address));
}


String NameServer::textify(const Address& address) {
  String result = "registration name ";
  result = result + address.getRegName() + 
    " ip " + address.getName() + " port " + 
    NetType::toString(address.getPort()) + " type " + 
    address.getCarrierName() + "\n";
  return result;
}


String NameServer::terminate(const String& str) {
  return str + "*** end of message\n";
}




Address NameServer::registerName(const String& name, 
				 const Address& address) {
  NameRecord& nameRecord = getNameRecord(name);
  static int ct = 10050; // temporary measure;
  Address suggestion = address;
  if (!suggestion.isValid()) {
    suggestion = Address("localhost",ct);
    ct++;
  }
  nameRecord.setAddress(suggestion.addRegName(name));
  return nameRecord.getAddress();
}


Address NameServer::queryName(const String& name) {
  NameRecord *rec = getNameRecord(name,false);
  if (rec!=NULL) {
    return rec->getAddress();
  }
  return Address();
}


NameServer::NameRecord *NameServer::getNameRecord(const String& name, 
						  bool create) {
  ACE_Hash_Map_Entry<String,NameRecord> *entry = NULL;
  int result = nameMap.find(name,entry);
  if (result==-1) {
    if (!create) {
      return NULL;
    }
    nameMap.bind(name,NameRecord());
    result = nameMap.find(name,entry);
  }
  YARP_ASSERT(result!=-1);
  YARP_ASSERT(entry!=NULL);
  return &(entry->int_id_);
}





class MainNameServer : public NameServer, public Readable {
public:
  virtual void readBlock(BlockReader& reader) {
    YARP_DEBUG(Logger::get(),"name server got something");
    String msg = reader.expectLine();
    YARP_DEBUG(Logger::get(),String("name server got message ") + msg);
    int index = msg.find("NAME_SERVER");
    if (index==0) {
      YARP_INFO(Logger::get(),String("name server received message: ") + msg);
      SplitString ss(msg.c_str());
      String result = "no command given";
      if (ss.size()>=2) {
	YARP_DEBUG(Logger::get(),String("dispatching to ") + ss.get(1));
	result = dispatcher.dispatch(this,ss.get(1),ss.size()-2,
				     (char **)(ss.get()+2));
      }
      OutputStream *os = reader.getReplyStream();
      if (os!=NULL) {
	if (result=="") {
	  result = "command not recognized";
	}
	YARP_INFO(Logger::get(),String("name server replying: ") + result);
	os->writeLine(result);
	os->flush();
      }
    } else {
	YARP_INFO(Logger::get(),"Name server ignoring unknown command: "+msg);
    }
  }
};


int NameServer::main(int argc, char *argv[]) {
  PortCore server;  // we use a subset of the PortCore functions
  MainNameServer name;
  server.setReadHandler(name);
  server.setAutoHandshake(false);
  server.listen(Address("localhost",10000,"tcp","root"));
  server.start();
  Time::delay(60);
  server.close();
  return 0;
}


