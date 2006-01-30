#ifndef _YARP2_TESTLIST_
#define _YARP2_TESTLIST_

#include <yarp/UnitTest.h>

namespace yarp {
  class TestList;
}

// need to made one function for each new test, and add to collectTests()
// method
extern yarp::UnitTest& getBottleTest();
extern yarp::UnitTest& getStringTest();
extern yarp::UnitTest& getAddressTest();
extern yarp::UnitTest& getStringInputStreamTest();

class yarp::TestList {
public:
  static void collectTests() {
    UnitTest& root = UnitTest::getRoot();
    root.add(getBottleTest());
    root.add(getStringTest());
    root.add(getAddressTest());
    root.add(getStringInputStreamTest());
  }
};

#endif

