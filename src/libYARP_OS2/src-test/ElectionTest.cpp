#include <yarp/Election.h>

#include "TestList.h"

using namespace yarp;

class ElectionTest : public UnitTest {
public:
  virtual String getName() { return "ElectionTest"; }

  void testBasics() {
    report(0,"testing the basics of elections");
    String c1 = "Magnifico";
    String c2 = "Grasso";
    String c3 = "Bozo";
    ElectionOf<String> elector;
    elector.add("italy",&c1);
    elector.add("italy",&c2);
    elector.add("france",&c3);
    String *e1 = elector.getElect("italy");
    String *e2 = elector.getElect("france");
    checkTrue(e1!=NULL,"elected entity exists (1)");
    checkTrue(e2!=NULL,"elected entity exists (2)");
    if (e1!=NULL && e2!=NULL) {
      checkTrue((*e1==c1 || *e1==c2),"elected entity is accurate (1)");
      checkTrue(*e2==c3,"elected entity is accurate (2)");
    }
    elector.remove("italy",&c2);
    elector.remove("france",&c3);
    e1 = elector.getElect("italy");
    e2 = elector.getElect("france");
    checkTrue(e1!=NULL,"elected entity exists (1)");
    checkTrue(e2==NULL,"elected entity does not exist (2)");
    if (e1!=NULL) {
      checkTrue(*e1==c1,"elected entity is accurate (1)");
    }
  }


  virtual void runTests() {
    testBasics();
  }
};

static ElectionTest theElectionTest;

UnitTest& getElectionTest() {
  return theElectionTest;
}

