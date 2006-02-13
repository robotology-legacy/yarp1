
#include <yarp/UnitTest.h>

#include <yarp/Logger.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>

#include "TestList.h"

using namespace yarp;


#ifdef __LINUX__
#define CHECK_FOR_LEAKS
#endif

#ifdef CHECK_FOR_LEAKS
// this is just for memory leak checking, and only works in linux
#warning "memory leak detection on"
#include <mcheck.h>
#endif


int main(int argc, char *argv[]) {
  //return yarp_test_main(argc,argv);
#ifdef CHECK_FOR_LEAKS
  mtrace();
#endif

  ACE::init();

  bool done = false;

  if (argc>1) {
    int verbosity = 0;
    while (String(argv[1])==String("verbose")) {
      verbosity++;
      argc--;
      argv++;
    }
    if (verbosity>0) {
      Logger::get().setVerbosity(verbosity);
    }
    
    if (String(argv[1])==String("regression")) {
      done = true;
      UnitTest::startTestSystem();
      TestList::collectTests();  // just in case automation doesn't work
      if (argc>2) {
	UnitTest::getRoot().run(argc-2,argv+2);
      } else {
	UnitTest::getRoot().run();
      }
      UnitTest::stopTestSystem();
      NameClient::removeNameClient();
    }
  } 
  if (!done) {
    Companion::main(argc,argv);
  }
  ACE::fini();

  return 0;
}

