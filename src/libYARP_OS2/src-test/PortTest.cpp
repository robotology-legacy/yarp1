#include <yarp/Port.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>

#include "TestList.h"

using namespace yarp::os;

class PortTest : public yarp::UnitTest {
public:
  virtual yarp::String getName() { return "PortTest"; }

  void testOpen() {
    report(0,"checking opening and closing ports");
    Port out, in;

    in.open("/in");
    out.open(Contact::bySocket("tcp","",9900));

    report(0,"running companion...");
    char *argv[] = {"yarp","name","list"};
    yarp::Companion::main(3,argv);

    Contact conIn = in.where();
    Contact conOut = out.where();

    assertEqual(conIn.getName().c_str(),"/in","name is recorded");

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

