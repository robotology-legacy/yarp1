#include <yarp/StreamBlockReader.h>
#include <yarp/StringInputStream.h>
#include <yarp/StringOutputStream.h>

#include "TestList.h"

using namespace yarp;

class StreamBlockReaderTest : public UnitTest {
public:
  virtual String getName() { return "StreamBlockReaderTest"; }

  void testRead() {
    report(0,"testing reading...");
    StringInputStream sis;
    StringOutputStream sos;
    sis.add("Hello\ngood evening and welcome");
    StreamBlockReader sbr;
    sbr.reset(sis,sos,10,true);
    String line = sbr.expectLine();
    checkEqual(line,"Hello","one line");
  }

  virtual void runTests() {
    testRead();
  }
};

static StreamBlockReaderTest theStreamBlockReaderTest;

UnitTest& getStreamBlockReaderTest() {
  return theStreamBlockReaderTest;
}

