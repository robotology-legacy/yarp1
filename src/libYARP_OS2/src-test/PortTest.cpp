#include <yarp/Port.h>
#include <yarp/NameClient.h>

#include "TestList.h"

using namespace yarp::os;

class PortTest : public yarp::UnitTest {
public:
  virtual yarp::String getName() { return "PortTest"; }

  void testOpen() {
    report(0,"checking opening and closing ports");
    Port out, in;

    in.open("/in");
    out.open("/out");

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

