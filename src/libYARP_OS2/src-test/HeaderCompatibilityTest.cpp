
/**
 *
 * This is a special set of tests, to see how well we support
 * the "Classic" YARP interface
 *
 */

//#include <yarp/HeaderCompatibility.h>

#include <yarp/YARPPort.h>
#include <yarp/NameClient.h>
#include <yarp/Time.h>

#include "TestList.h"

using namespace yarp;

class HeaderCompatibilityTest : public UnitTest {
public:
  virtual String getName() { return "HeaderCompatibilityTest"; }

  void testPorts() {
    report(0,"checking ports...");
    NameClient& nic = NameClient::getNameClient();
    nic.setFakeMode(true);

    YARPInputPortOf<int> in;
    YARPOutputPortOf<int> out;

    checkEqual(nic.queryName("/hct/read").isValid(),false,"YARPPort pre reg");
    checkEqual(nic.queryName("/hct/write").isValid(),false,"YARPPort pre reg");
    in.Register("/hct/read");
    out.Register("/hct/write");
    YARPPort::Connect("/hct/write","/hct/read");
    checkEqual(nic.queryName("/hct/read").isValid(),true,"YARPPort post reg");
    checkEqual(nic.queryName("/hct/write").isValid(),true,"YARPPort post reg");
    checkEqual((&(out.Content())==NULL),false,"output content exists...");
    out.Content() = 15;
    checkEqual((int)(out.Content()),15,"output content settable");
    out.Write();
    checkEqual(in.Read()!=0,true,"read something");
    checkEqual((int)(in.Content()),15,"got right value at input port");
    //checkEqual(out.Content(),15,"output content");

    nic.setFakeMode(false);    
  }

  virtual void runTests() {
    testPorts();
  }
};

static HeaderCompatibilityTest theHeaderCompatibilityTest;

UnitTest& getHeaderCompatibilityTest() {
  return theHeaderCompatibilityTest;
}

