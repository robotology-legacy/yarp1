#include <yarp/PortCore.h>
#include <yarp/Time.h>
#include <yarp/Carriers.h>

#include "TestList.h"

using namespace yarp;

class PortCoreTest : public UnitTest {
public:
  virtual String getName() { return "PortCoreTest"; }

  void testStartStop() {
    report(0,"checking start/stop works (requires free port 9999)...");
    Address address("localhost",9999,"tcp","/port");
    PortCore core;
    core.listen(address);
    core.start();
    report(0,"there will be a small delay, stress-testing port...");
    int tct = 10;
    int ct = 0;
    for (int i=0; i<tct; i++) {
      Time::delay(0.01*(i%4));
      OutputProtocol *op = Carriers::connect(address);
      if (op!=NULL) {
	op->getOutputStream().write('h');
	op->close();
	delete op;
	ct++; // connect is an event
      } else {
	report(1,"a connection failed");
      }
    }

    Time::delay(0.2);  // close will take precedence over pending connections
    
    report(0,"break for user intervention - port 9999");
    Time::delay(50);
    core.close();
    ct++; // close is an event

    core.join();
    int x = core.getEventCount();
    checkEqual(core.getEventCount(),ct,"Got all events");
  }

  virtual void runTests() {
    testStartStop();
  }
};

static PortCoreTest thePortCoreTest;

UnitTest& getPortCoreTest() {
  return thePortCoreTest;
}
