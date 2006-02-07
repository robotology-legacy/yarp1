
#include <yarp/Bottle.h>
#include <yarp/BufferedBlockWriter.h>
#include <yarp/StreamBlockReader.h>
#include <yarp/FakeTwoWayStream.h>
#include <yarp/Logger.h>

#include <yarp/UnitTest.h>

#include "TestList.h"

using namespace yarp;


class BottleTest : public UnitTest {
public:
  void testSize() {
    report(0,"testing sizes...");
    Bottle bot;
    checkEqual(0,bot.size(),"empty bottle");
    bot.addInt(1);
    checkEqual(1,bot.size(),"add int");
    bot.addString("hello");
    checkEqual(2,bot.size(),"add string");
    bot.clear();
    checkEqual(0,bot.size(),"clear");
  }


  void testString() {
    report(0,"testing string representation...");
    String target = "hello \"my\" \\friend";
    Bottle bot;
    bot.addInt(5);
    bot.addString("hello \"my\" \\friend");
    String txt = bot.toString();
	const char *expect = "5 \"hello \\\"my\\\" \\\\friend\"";
    checkEqual(txt,expect,"string rep");
    Bottle bot2;
    bot2.fromString(txt);
    checkEqual(2,bot2.size(),"return from string rep");
  }

  void testBinary() {
    report(0,"testing binary representation...");
    Bottle bot;
    bot.addInt(5);
    bot.addString("hello");
    checkEqual(bot.isInt(0),true,"type check");
    checkEqual(bot.isString(1),true,"type check");
    ManagedBytes store(bot.byteCount());
    bot.toBytes(store.bytes());
    Bottle bot2;
    bot2.fromBytes(store.bytes());
    checkEqual(bot2.size(),2,"recovery binary, length");
    checkEqual(bot2.isInt(0),bot.isInt(0),"recovery binary, integer");
    checkEqual(bot2.isString(1),bot.isString(1),"recovery binary, integer");
  }

  void testStreaming() {
    report(0,"testing streaming (just text mode)...");

    Bottle bot;
    bot.addInt(5);
    bot.addString("hello");

    try {
      BufferedBlockWriter bbw(true);
      bot.writeBlock(bbw);
      
      String s;
      StringInputStream sis;
      StreamBlockReader sbr;
      
      s = bbw.toString();
      sis.add(s);
      sbr.reset(sis,NULL,s.length(),true);
      
      Bottle bot2;
      bot2.readBlock(sbr);
      checkEqual(bot2.toString(),bot.toString(),"to/from stream");
    } catch (IOException e) {
      report(1,e.toString() + " <<< exception thrown");
    }
  }

  virtual void runTests() {
    testSize();
    testString();
    testBinary();
    testStreaming();
  }

  virtual String getName() {
    return "BottleTest";
  }
};


// single instance for testing
static BottleTest theBottleTest;

UnitTest& getBottleTest() {
  return theBottleTest;
}

