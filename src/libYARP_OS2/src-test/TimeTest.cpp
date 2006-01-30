#include <yarp/Time.h>

#include "TestList.h"

using namespace yarp;

class TimeTest : public UnitTest {
public:
  virtual String getName() { return "TimeTest"; }

  void testDelay() {
    report(0,"testing delay (there will be a short pause)...");
    double target = 0.5;
    double t1 = Time::now();
    Time::delay(target);
    double t2 = Time::now();
    double dt = t2-t1-target;
    double limit = 0.1; // don't be too picky, there is a lot of undefined slop
    bool inLimits = (-limit<dt)&&(dt<limit);
    checkEqual(true,inLimits,"delay for 0.5 seconds");
  }

  virtual void runTests() {
    testDelay();
  }
};

static TimeTest theTimeTest;

UnitTest& getTimeTest() {
  return theTimeTest;
}

