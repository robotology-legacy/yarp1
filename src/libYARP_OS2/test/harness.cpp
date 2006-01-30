
#include <yarp/UnitTest.h>

#include <yarp/Logger.h>

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

extern int yarp_test_main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  //return yarp_test_main(argc,argv);
#ifdef CHECK_FOR_LEAKS
  mtrace();
#endif

  if (argc<=1) {
    UnitTest::startTestSystem();
    TestList::collectTests();  // just in case automation doesn't work
    UnitTest::getRoot().run();
    UnitTest::stopTestSystem();
  } else {
    yarp_test_main(argc,argv);
  }

  return 0;
}

