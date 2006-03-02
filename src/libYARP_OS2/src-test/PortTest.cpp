#include <yarp/os/Port.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>
#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>

#include "TestList.h"

using namespace yarp::os;

using yarp::String;
using yarp::NetType;
using yarp::Logger;


class PortTest : public yarp::UnitTest {
public:
  virtual yarp::String getName() { return "PortTest"; }

  void testOpen() {
    report(0,"checking opening and closing ports");
    Port out, in;

    in.open("/in");
    out.open(Contact::bySocket("tcp","",9999));

    Contact conIn = in.where();
    Contact conOut = out.where();

    out.addOutput(Contact::byName("/in").addCarrier("udp"));
    Time::delay(0.2);

    checkEqual(conIn.getName().c_str(),"/in","name is recorded");

    checkTrue(String(conOut.getName().c_str()).strstr("/tmp")==0,
	      "name is created");

    Bottle bot1, bot2;
    bot1.fromString("5 10 \"hello\"");
    out.write(bot1);
    in.read(bot2);
    checkEqual(bot1.getInt(0),5,"check bot[0]");
    checkEqual(bot1.getInt(1),10,"check bot[1]");
    checkEqual(bot1.getString(2).c_str(),"hello","check bot[2]");

    bot1.fromString("18");
    out.write(bot1);
    in.read(bot2);
    checkEqual(bot1.getInt(0),18,"check one more send/receive");

    in.close();
    out.close();
  }


  void testBuffer() {
    report(0,"checking buffering");
    Bottle bot1;
    PortReaderBuffer<Bottle> buf;

    bot1.fromString("1 2 3");
    for (int i=0; i<10000; i++) {
      bot1.addInt(i);
    }

    Port input, output;
    input.open("/in");
    output.open("/out");

    input.setReader(buf);
    //buf.attach(input); // this is an alternative

    output.addOutput(Contact::byName("/in").addCarrier("tcp"));
    Time::delay(0.2);

    report(0,"writing...");
    output.write(bot1);
    output.write(bot1);
    output.write(bot1);
    report(0,"reading...");
    Bottle *result = buf.read();

    for (int j=0; j<3; j++) {
      checkTrue(result!=NULL,"got something check");
      if (result!=NULL) {
	checkEqual(bot1.size(),result->size(),"size check");
	YARP_INFO(Logger::get(),String("size is in fact ") + 
		  NetType::toString(result->size()));
      }
    }

    output.close();
    input.close();
  }

  virtual void runTests() {
    yarp::NameClient& nic = yarp::NameClient::getNameClient();
    nic.setFakeMode(true);
    testOpen();
    testBuffer();
    nic.setFakeMode(false);
  }
};

static PortTest thePortTest;

yarp::UnitTest& getPortTest() {
  return thePortTest;
}

