
#include <yarp/Companion.h>

using namespace yarp;

Companion Companion::instance;

Companion::Companion() {
  add("version",&Companion::cmdVersion);
}

int Companion::dispatch(const char *name, int argc, char *argv[]) {
  ACE_OS::printf("Dispatching %s\n", name);
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

  if (argc<=0) {
    ACE_OS::fprintf(stderr,"Please supply a command\n");
    return -1;
  }

  const char *cmd = argv[0];
  argc--;
  argv++;

  return getInstance().dispatch(cmd,argc,argv);
}


