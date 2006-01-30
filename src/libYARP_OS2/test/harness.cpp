
#include <yarp/UnitTest.h>

#include <yarp/Logger.h>

#include "TestList.h"

using namespace yarp;


extern int yarp_test_main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  //return yarp_test_main(argc,argv);

  UnitTest::startTestSystem();
  TestList::collectTests();  // just in case automation doesn't work
  UnitTest::getRoot().run();
  UnitTest::stopTestSystem();

  return 0;
}

