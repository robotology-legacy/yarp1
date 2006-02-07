
#include <yarp/NameServer.h>
#include <yarp/Logger.h>
#include <yarp/PortCore.h>
#include <yarp/Time.h>
#include <yarp/SplitString.h>
#include <yarp/NetType.h>
#include <yarp/ManagedBytes.h>
#include <yarp/NameConfig.h>
#include <yarp/FallbackNameServer.h>

using namespace yarp;



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Basic functionality
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


Address NameServer::unregisterName(const String& name) {
  Address prev = queryName(name);
  if (prev.isValid()) {
    if (prev.getPort()!=-1) {
      HostRecord& host = getHostRecord(prev.getName());
      host.release(prev.getPort());
      NameRecord& rec = getNameRecord(name);
      rec.clear();
      tmpNames.release(name);
    }
  }
  
  return queryName(name);
}



Address NameServer::registerName(const String& name, 
				 const Address& address) {
  YARP_DEBUG(Logger::get(),"in registerName...");

  if (name!="...") {
    unregisterName(name);
  }

  Address suggestion = address;

  if (!suggestion.isValid()) {
    suggestion = Address("...",0,"...",name);
  }

  String portName = name;
  if (portName == "...") {
    portName = tmpNames.get();
  }

  String carrier = suggestion.getCarrierName();
  if (carrier == "...") {
    carrier = "tcp";
  }

  String machine = suggestion.getName();
  if (machine == "...") {
    if (carrier!="mcast") {
      YARP_DEBUG(Logger::get(),"FIXME: machine is assumed to be localhost");
      machine = "localhost"; 
    } else {
      machine = mcastRecord.get();
    }
  }

  int port = suggestion.getPort();
  if (port == 0) {
    port = getHostRecord(machine).get();
  }

  suggestion = Address(machine,port,carrier,portName);

  YARP_DEBUG(Logger::get(),String("Thinking about registering ") +
	     suggestion.toString());
  
  NameRecord& nameRecord = getNameRecord(suggestion.getRegName());
  nameRecord.setAddress(suggestion);

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


NameServer::HostRecord *NameServer::getHostRecord(const String& name,
						  bool create) {
  ACE_Hash_Map_Entry<String,HostRecord> *entry = NULL;
  int result = hostMap.find(name,entry);
  if (result==-1) {
    if (!create) {
      return NULL;
    }
    hostMap.bind(name,HostRecord());
    result = hostMap.find(name,entry);
  }
  YARP_ASSERT(result!=-1);
  YARP_ASSERT(entry!=NULL);
  return &(entry->int_id_);
}









////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Remote interface
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



void NameServer::setup() {
  dispatcher.add("register", &NameServer::cmdRegister);
  dispatcher.add("unregister", &NameServer::cmdUnregister);
  dispatcher.add("query", &NameServer::cmdQuery);
  dispatcher.add("help", &NameServer::cmdHelp);
}

String NameServer::cmdRegister(int argc, char *argv[]) {
  if (argc<1) {
    return "need at least one argument";
  }
  String portName = argv[0];

  String machine = "...";
  String carrier = "...";
  int port = 0;
  if (argc>=2) {
    carrier = argv[1];
  }
  if (argc>=3) {
    machine = argv[2];
  }
  if (argc>=4) {
    if (String("...") == argv[3]) {
      port = 0;
    } else {
      port = NetType::toInt(argv[3]);
    }
  }

  Address address = registerName(portName,
				 Address(machine,port,carrier,portName));
  return terminate(textify(address));
}


String NameServer::cmdQuery(int argc, char *argv[]) {
  if (argc<1) {
    return "need at least one argument";
  }
  String portName = argv[0];
  Address address = queryName(portName);
  return terminate(textify(address));
}

String NameServer::cmdUnregister(int argc, char *argv[]) {
  if (argc<1) {
    return "need at least one argument";
  }
  String portName = argv[0];
  Address address = unregisterName(portName);
  return terminate(textify(address));
}


String NameServer::cmdHelp(int argc, char *argv[]) {
  String result = "Here are some ways to use the name server:\n";
  ACE_Vector<String> names = dispatcher.getNames();
  for (unsigned i=0; i<names.size(); i++) {
    const String& name = names[i];
    result += String("  NAME_SERVER ") + name + " ...\n";
  }
  return terminate(result);
}

String NameServer::textify(const Address& address) {
  String result = "";
  if (address.isValid()) {
    result = "registration name ";
    result = result + address.getRegName() + 
      " ip " + address.getName() + " port " + 
      NetType::toString(address.getPort()) + " type " + 
      address.getCarrierName() + "\n";
  }
  return result;
}


String NameServer::terminate(const String& str) {
  return str + "*** end of message";
}


String NameServer::apply(const String& txt) {
  SplitString ss(txt.c_str());
  String result = "no command given";
  if (ss.size()>=2) {
    YARP_DEBUG(Logger::get(),String("dispatching to ") + ss.get(1));
    result = dispatcher.dispatch(this,ss.get(1),ss.size()-2,
				 (char **)(ss.get()+2));
  }
  return result;
}





class MainNameServer : public NameServer, public Readable {
public:
  virtual void readBlock(BlockReader& reader) {
    YARP_DEBUG(Logger::get(),"name server got something");
    ManagedBytes header(12);
    reader.expectBlock(header.bytes());
    String ref = "NAME_SERVER ";
    bool ok = true;
    for (int i=0; i<header.length(); i++) {
      if (header.get()[i] != ref[i]) {
	ok = false; 
	break;
      }
    }
    String msg = "?";
    if (ok) {
      msg = ref + reader.expectLine();
    }
    YARP_DEBUG(Logger::get(),String("name server got message ") + msg);
    int index = msg.find("NAME_SERVER");
    if (index==0) {
      YARP_INFO(Logger::get(),String("name server received message: ") + msg);
      String result = apply(msg);
      OutputStream *os = reader.getReplyStream();
      if (os!=NULL) {
	if (result=="") {
	  result = terminate("unknown command\n");
	}
	YARP_INFO(Logger::get(),String("name server replying: ") + result);
	os->writeLine(result);
	os->flush();
	os->close();
      }
    } else {
	YARP_INFO(Logger::get(),"Name server ignoring unknown command: "+msg);
	OutputStream *os = reader.getReplyStream();
	if (os!=NULL) {
	  // this result is necessary for YARP1 support
	  os->writeLine("???????????????????????????????????????");
	  os->flush();
	  os->close();
	}
    }
  }
};


int NameServer::main(int argc, char *argv[]) {

  // pick an address
  Address suggest("...",0); // suggestion is initially empty
  if (argc>=1) {
    if (argc>=2) {
      suggest = Address(argv[0],NetType::toInt(argv[1]));
    } else {
      suggest = Address("...",NetType::toInt(argv[0]));
    }
  }

  // see what address is lying around
  Address prev;
  NameConfig conf;
  if (conf.fromFile()) {
    prev = conf.getAddress();
  }

  // merge
  if (prev.isValid()) {
    if (suggest.getName()=="...") {
      suggest = Address(prev.getName(),suggest.getPort());
    }
    if (suggest.getPort()==0) {
      suggest = Address(suggest.getName(),prev.getPort());
    }
  }

  // still something not set?
  if (suggest.getPort()==0) {
    suggest = Address(suggest.getName(),10000);
  }
  if (suggest.getName()=="...") {
    // should get my IP
    suggest = Address(conf.getHostName(),suggest.getPort());
  }

  // finally, should make sure IP is local, and if not, correct it
  if (!conf.isLocalName(suggest.getName())) {
    YARP_INFO(Logger::get(),"Overriding non-local address for name server");
    suggest = Address(conf.getHostName(),suggest.getPort());
  }
  
  // and save
  conf.setAddress(suggest);
  if (!conf.toFile()) {
    YARP_ERROR(Logger::get(), String("Could not save configuration file ") +
	       conf.getConfigFileName());
  }

  PortCore server;  // we use a subset of the PortCore functions
  MainNameServer name;
  name.registerName("root",suggest);
  server.setReadHandler(name);
  server.setAutoHandshake(false);
  server.listen(Address(suggest.addRegName("root")));
  YARP_INFO(Logger::get(), String("Name server listening at ") + 
	    suggest.toString());
  server.start();

  FallbackNameServer fallback(name);
  fallback.start();

  while (true) {
    YARP_DEBUG(Logger::get(),"name server running happily");
    Time::delay(60);
  }
  server.close();
  fallback.close();
  fallback.join();
  return 0;
}


