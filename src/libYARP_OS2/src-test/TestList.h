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
extern yarp::UnitTest& getTimeTest();
extern yarp::UnitTest& getThreadTest();
extern yarp::UnitTest& getPortCommandTest();
extern yarp::UnitTest& getStringOutputStreamTest();
extern yarp::UnitTest& getStreamBlockReaderTest();
extern yarp::UnitTest& getBufferedBlockWriterTest();
extern yarp::UnitTest& getProtocolTest();
extern yarp::UnitTest& getNameServerTest();
extern yarp::UnitTest& getPortCoreTest();
extern yarp::UnitTest& getHeaderCompatibilityTest();
extern yarp::UnitTest& getElectionTest();
extern yarp::UnitTest& getNameConfigTest();

class yarp::TestList {
public:
  static void collectTests() {
    UnitTest& root = UnitTest::getRoot();
    root.add(getBottleTest());
    root.add(getStringTest());
    root.add(getAddressTest());
    root.add(getStringInputStreamTest());
    root.add(getTimeTest());
    root.add(getPortCommandTest());
    root.add(getStringOutputStreamTest());
    root.add(getStreamBlockReaderTest());
    root.add(getBufferedBlockWriterTest());
    root.add(getThreadTest());
    root.add(getProtocolTest());
    root.add(getNameServerTest());
    root.add(getPortCoreTest());
    root.add(getHeaderCompatibilityTest());
    root.add(getElectionTest());
    root.add(getNameConfigTest());
  }
};

#endif

