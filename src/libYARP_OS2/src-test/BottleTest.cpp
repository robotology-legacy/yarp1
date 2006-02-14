
#include <yarp/BottleImpl.h>
#include <yarp/BufferedBlockWriter.h>
#include <yarp/StreamBlockReader.h>
#include <yarp/FakeTwoWayStream.h>
#include <yarp/Logger.h>

#include <yarp/UnitTest.h>

#include "TestList.h"

using namespace yarp;


static double myfabs(double x) {
  if (x>=0) return x;
  return -x;
}

class BottleTest : public UnitTest {
public:
  void testSize() {
    report(0,"testing sizes...");
    BottleImpl bot;
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
    BottleImpl bot;
    bot.addInt(5);
    bot.addString("hello \"my\" \\friend");
    String txt = bot.toString();
    const char *expect = "5 \"hello \\\"my\\\" \\\\friend\"";
    checkEqual(txt,expect,"string rep");
    BottleImpl bot2;
    bot2.fromString(txt);
    checkEqual(2,bot2.size(),"return from string rep");
  }

  void testBinary() {
    report(0,"testing binary representation...");
    BottleImpl bot;
    bot.addInt(5);
    bot.addString("hello");
    checkEqual(bot.isInt(0),true,"type check");
    checkEqual(bot.isString(1),true,"type check");
    ManagedBytes store(bot.byteCount());
    bot.toBytes(store.bytes());
    BottleImpl bot2;
    bot2.fromBytes(store.bytes());
    checkEqual(bot2.size(),2,"recovery binary, length");
    checkEqual(bot2.isInt(0),bot.isInt(0),"recovery binary, integer");
    checkEqual(bot2.isString(1),bot.isString(1),"recovery binary, integer");
  }

  void testStreaming() {
    report(0,"testing streaming (just text mode)...");

    BottleImpl bot;
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
      
      BottleImpl bot2;
      bot2.readBlock(sbr);
      checkEqual(bot2.toString(),bot.toString(),"to/from stream");
    } catch (IOException e) {
      report(1,e.toString() + " <<< exception thrown");
    }
  }

  void testTypes() {
    report(0,"testing types...");
    BottleImpl bot[3];
    bot[0].fromString("5 10.2 \"hello\" -15 -15.0");
    bot[1].addInt(5);
    bot[1].addDouble(10.2);
    bot[1].addString("hello");
    bot[1].addInt(-15);
    bot[1].addDouble(-15.0);
    ManagedBytes store(bot[0].byteCount());
    bot[0].toBytes(store.bytes());
    bot[2].fromBytes(store.bytes());

    for (int i=0; i<3; i++) {
      BottleImpl& b = bot[i];
      report(0,String("check for bottle number ") +
	     NetType::toString(i));
      checkTrue(b.isInt(0)&&b.isInt(3),"ints");
      checkTrue(b.isDouble(1)&&b.isDouble(4),"doubles");
      checkTrue(b.isString(2),"strings");
      checkEqual(b.getInt(0),5,"arg 0");
      checkTrue(myfabs(b.getDouble(1)-10.2)<0.01,"arg 1");
      checkEqual(b.getString(2),"hello","arg 2");
      checkEqual(b.getInt(3),-15,"arg 3");
      checkTrue(myfabs(b.getDouble(4)+15.0)<0.01,"arg 4");
    }

  }

  virtual void runTests() {
    testSize();
    testString();
    testBinary();
    testStreaming();
    testTypes();
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

