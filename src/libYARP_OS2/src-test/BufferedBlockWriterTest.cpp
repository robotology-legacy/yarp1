#include <yarp/BufferedBlockWriter.h>
#include <yarp/StringOutputStream.h>

#include "TestList.h"

using namespace yarp;

class BufferedBlockWriterTest : public UnitTest {
public:
  virtual String getName() { return "BufferedBlockWriterTest"; }

  void testWrite() {
    report(0,"testing writing...");
    StringOutputStream sos;    
    BufferedBlockWriter bbr;
    bbr.reset(true);
    bbr.appendLine("Hello");
    bbr.appendLine("Greetings");
    bbr.write(sos);
    checkEqual(sos.toString(),"Hello\nGreetings\n","two line writes");
  }

  virtual void runTests() {
    testWrite();
  }
};

static BufferedBlockWriterTest theBufferedBlockWriterTest;

UnitTest& getBufferedBlockWriterTest() {
  return theBufferedBlockWriterTest;
}

