#include <yarp/FakeTwoWayStream.h>
#include <yarp/Protocol.h>

#include <ace/OS_NS_stdio.h>

#include "TestList.h"

using namespace yarp;

class ProtocolTest : public UnitTest {
public:
  virtual String getName() { return "ProtocolTest"; }

  String simplify(String x) {
    return humanize(x);
  }

  void show(FakeTwoWayStream *fake1, FakeTwoWayStream *fake2) {
    ACE_OS::printf("fake1  //  in: [%s]  //  out: [%s]\n",
		   simplify(fake1->getInputText()).c_str(),
		   simplify(fake1->getOutputText()).c_str());
    ACE_OS::printf("fake2  //  in: [%s]  //  out: [%s]\n\n",
		   simplify(fake2->getInputText()).c_str(),
		   simplify(fake2->getOutputText()).c_str());
  }


  void testBottle() {
    report(0,"trying to send a bottle across a fake stream");
    try {
      // set up a fake sender/receiver pair
      FakeTwoWayStream *fake1 = new FakeTwoWayStream();
      FakeTwoWayStream *fake2 = new FakeTwoWayStream();
      fake1->setTarget(fake2->getStringInputStream());
      fake2->setTarget(fake1->getStringInputStream());

      // hand streams over to protocol managers
      Protocol p1(fake1);
      Protocol p2(fake2);

      p1.open(Route("/out","/in","text"));
      
      checkEqual(fake1->getOutputText(),"CONNECT /out\n",
		 "text carrier header");

      p2.open("/in");

      checkEqual(fake2->getOutputText(),"Welcome /out\n",
		 "text carrier response");

      BufferedConnectionWriter writer;
      writer.appendLine("d");
      writer.appendLine("0 \"Hello\"");
      p1.write(writer);
      
      const char *expect = "CONNECT /out\nd\n0 \"Hello\"\n";
      checkEqual(fake1->getOutputText(),expect,
		 "added a bottle");

      ConnectionReader& reader = p2.beginRead();
      String str1 = reader.expectText().c_str();
      String str2 = reader.expectText().c_str();
      p2.endRead();

      checkEqual(str1,String("d"),"data tag");
      const char *expect2 = "0 \"Hello\"";
      checkEqual(str2,String(expect2),"bottle representation");
    } catch (IOException e) {
      report(1, e.toString() + " <<< exception thrown");
    }
  }

  /*
  void test2() {
    try {
	
    // now we get funky
    ACE_OS::printf("Structured replies are possible\n");
    p2.write(writer);
    show(fake1,fake2);

    ConnectionReader& reader2 = p1.beginRead();
    String str0 = reader2.expectLine(); // deal with welcome line
    str1 = reader2.expectLine();
    str2 = reader2.expectLine();
    p1.endRead();
    ACE_OS::printf("strings are [%s] [%s]\n", str1.c_str(), str2.c_str());
    assertion(str1,String("d"));
    assertion(str2,String("0 \"Hello\""));

    } catch (IOException e) {
	report(1, e.toString() + " <<< exception thrown");
    }
  }
  */

  virtual void runTests() {
    testBottle();
  }
};

static ProtocolTest theProtocolTest;

UnitTest& getProtocolTest() {
  return theProtocolTest;
}
