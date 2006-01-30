#include <yarp/StringInputStream.h>

#include "TestList.h"

using namespace yarp;

class StringInputStreamTest : public UnitTest {
public:
  virtual String getName() { return "StringInputStreamTest"; }

  void testRead() {
    report(0,"test reading...");

    StringInputStream sis;
    sis.add("Hello my friend");
    char buf[256];
    sis.check();
    Bytes b(buf,sizeof(buf));
    int len = sis.read(b,0,5);
    checkEqual(len,5,"len of first read");
    buf[len] = '\0';
    checkEqual("Hello",buf,"first read");
    char ch = sis.read();
    checkEqual(ch,' ',"the space");
    len = sis.read(b,0,2);
    checkEqual(len,2,"len of second read");
    buf[len] = '\0';
    checkEqual("my",buf,"second read");
  /*
  try {
    len = sis.read(b,0,100);
    assertion(String("should have thrown an exception"),String(""));
  } catch (IOException e) {
    ACE_OS::printf("Got an exception, good: %s\n", e.toString().c_str());
  }
  */
  }

  virtual void runTests() {
    testRead();
  }
};

static StringInputStreamTest theStringInputStreamTest;

UnitTest& getStringInputStreamTest() {
  return theStringInputStreamTest;
}

