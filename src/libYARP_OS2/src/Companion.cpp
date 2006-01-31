
#include <yarp/Companion.h>
#include <yarp/NameClient.h>
#include <yarp/Logger.h>

#include <yarp/Carriers.h>
#include <yarp/BufferedBlockWriter.h>

using namespace yarp;

Companion Companion::instance;

Companion::Companion() {
  add("help",       &Companion::cmdHelp);
  add("version",    &Companion::cmdVersion);
  add("where",      &Companion::cmdWhere);
  add("name",       &Companion::cmdName);
  add("connect",    &Companion::cmdConnect);
  add("disconnect", &Companion::cmdDisconnect);
}

int Companion::dispatch(const char *name, int argc, char *argv[]) {
  //ACE_OS::printf("Dispatching %s\n", name);
  String sname(name);
  Entry e;
  int result = action.find(sname,e);
  if (result!=-1) {
    return (this->*(e.fn))(argc,argv);
  } else {
    ACE_DEBUG((LM_ERROR,"Could not find command \"%s\"",name));
  }
  return -1;
}


int Companion::main(int argc, char *argv[]) {

  // eliminate 0th arg, the program name
  argc--;
  argv++;

  if (argc<=0) {
    ACE_OS::printf("This is the YARP network companion\n");
    return 0;
  }

  int verbose = 0;
  bool more = true;
  while (more && argc>0) {
    more = false;
    if (String(argv[0]) == String("verbose")) {
      verbose++;
      argc--;
      argv++;
      more = true;
    }
  }
  Logger::get().setVerbosity(verbose);

  if (argc<=0) {
    ACE_OS::fprintf(stderr,"Please supply a command\n");
    return -1;
  }

  const char *cmd = argv[0];
  argc--;
  argv++;

  return getInstance().dispatch(cmd,argc,argv);
}



int Companion::cmdName(int argc, char *argv[]) {
  String cmd = "NAME_SERVER";
  for (int i=0; i<argc; i++) {
    cmd += " ";
    cmd += argv[i];
  }
  NameClient& nic = NameClient::getNameClient();
  String result = nic.send(cmd);
  ACE_OS::printf("%s",result.c_str());
  return 0;
}

int Companion::cmdWhere(int argc, char *argv[]) {
  NameClient& nic = NameClient::getNameClient();
  Address address = nic.getAddress();
  ACE_OS::printf("Name server is available at ip %s port %d\n",
		 address.getName().c_str(), address.getPort());
  return 0;
}

int Companion::cmdHelp(int argc, char *argv[]) {
  ACE_OS::printf("Here are ways to use this program:\n");
  for (unsigned i=0; i<names.size(); i++) {
    const String& name = names[i];
    ACE_OS::printf("  <this program> %s\n", name.c_str());
  }
  return 0;
}


int Companion::cmdVersion(int argc, char *argv[]) {
  ACE_OS::printf("YARP Companion utility version %s implemented in C++\n", 
		 version().c_str());
  return 0;
}


int Companion::sendMessage(const String& port, const String& msg) {
  NameClient& nic = NameClient::getNameClient();
  Address srcAddress = nic.queryName(port);
  //Address srcAddress("localhost",9999,"tcp");
  if (!srcAddress.isValid()) {
    ACE_OS::fprintf(stderr, "Cannot find port named %s\n", port.c_str());
    return 1;
  } 
  OutputProtocol *out = Carriers::connect(srcAddress);
  if (out==NULL) {
    ACE_OS::fprintf(stderr, "Cannot connect to port named %s at %s\n", 
		    port.c_str(),
		    srcAddress.toString().c_str());
    return 1;
  }
  Route route("external",port,"text");
  out->open(route);
  BufferedBlockWriter bw;
  bw.appendLine(msg);
  out->write(bw);
  out->close();
  delete out;
  out = NULL;
  
  return 0;
}


int Companion::cmdConnect(int argc, char *argv[]) {
  if (argc!=2) {
    ACE_OS::fprintf(stderr, "Currently must have two arguments, a sender port and receiver port\n");
    return 1;
  }

  const char *src = argv[0];
  const char *dest = argv[1];
  return connect(src,dest);
}


int Companion::cmdDisconnect(int argc, char *argv[]) {
  if (argc!=2) {
    ACE_OS::fprintf(stderr, "Currently must have two arguments, a sender port and receiver port\n");
    return 1;
  }

  const char *src = argv[0];
  const char *dest = argv[1];
  return disconnect(src,dest);
}


int Companion::connect(const char *src, const char *dest) {
  return sendMessage(src,dest);
}

int Companion::disconnect(const char *src, const char *dest) {
  return sendMessage(src,String("!")+dest);
}


