#include <yarp/Port.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>
#include <yarp/Time.h>
#include <yarp/Bottle.h>

#include "TestList.h"

using namespace yarp::os;
using typename yarp::String;

class PortTest : public yarp::UnitTest {
public:
  virtual yarp::String getName() { return "PortTest"; }

  void testOpen() {
    report(0,"checking opening and closing ports");
    Port out, in;

    in.open("/in");
    out.open(Contact::bySocket("tcp","",9999));

    //report(0,"running companion...");
    //char *argv[] = {"yarp","name","list"};
    //yarp::Companion::main(3,argv);

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

  virtual void runTests() {
    yarp::NameClient& nic = yarp::NameClient::getNameClient();
    nic.setFakeMode(true);
    testOpen();
    nic.setFakeMode(false);
  }
};

static PortTest thePortTest;

yarp::UnitTest& getPortTest() {
  return thePortTest;
}

