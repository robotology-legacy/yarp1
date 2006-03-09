
#include <yarp/UnitTest.h>
#include <yarp/Logger.h>
#include <yarp/IOException.h>

#include <ace/OS_NS_stdio.h>

using namespace yarp;

UnitTest *UnitTest::theRoot = NULL;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class RootUnitTest : public UnitTest {
public:
  RootUnitTest(UnitTest *parent) : UnitTest(parent) {
    // no parent
  }

  virtual String getName() {
    return "root";
  }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



UnitTest::UnitTest() {
  parent = &UnitTest::getRoot();
  /*
    // turn automation off, so order of test cases can be chosen
    // by user
  if (parent!=NULL) {
    parent->add(*this);
  }
  */
  hasProblem = false;
}

UnitTest::UnitTest(UnitTest *parent) {
  this->parent = parent;
  if (parent!=NULL) {
    parent->add(*this);
  }
  hasProblem = false;
}

void UnitTest::add(UnitTest& unit) {
  for (unsigned int i=0; i<subTests.size(); i++) {
    if (subTests[i]==&unit) {
      return; // already present, no need to add
    }
  }
  subTests.push_back(&unit);
}

void UnitTest::clear() {
  subTests.clear();
}

void UnitTest::report(int severity, const String& problem) {
  if (parent!=NULL) {
    parent->report(severity, getName() + ": " + problem);
  } else {
    ACE_OS::printf("%d | %s\n", severity, problem.c_str());
  }
  count(severity);
}

void UnitTest::count(int severity) {
  if (severity>0) {
    // could do something more sophisticated with the reports than this...
    hasProblem = true;
  }
}


void UnitTest::runSubTests(int argc, char *argv[]) {
  //char buf[256];
  //sprintf(buf,"size is %d", subTests.size());
  //report(0,buf);
  for (unsigned int i=0; i<subTests.size(); i++) {
    try {
      subTests[i]->run(argc,argv);
    } catch (IOException e) {
      report(1,String("exception thrown ") + e.toString());
    }
  }
}


void UnitTest::run() {
  try {
    run(0,NULL);
  } catch (IOException e) {
    report(1,String("exception thrown ") + e.toString());
  }
}


void UnitTest::run(int argc, char *argv[]) {
  //report(0,String("starting tests for " + getName()));
  bool ran = false;
  if (argc==0) {
    runTests();
    ran = true;
  } else {
    String name = getName();
    bool onList = false;
    for (int i=0; i<argc; i++) {
      if (name == String(argv[i])) {
	onList = true;
	break;
      }
    }
    if (onList) {
      runTests();
      ran = true;
    }
  }
  runSubTests(argc,argv);
  //report(0,String("ending tests for " + getName()));
  if (hasProblem) {
    report(0,"A PROBLEM WAS ENCOUNTERED");
  } 
  else {
    if (ran) {
      report(0,"no problems reported");
    }
  }
}



void UnitTest::startTestSystem() {
  if (theRoot==NULL) {
    theRoot = new RootUnitTest(NULL);
  }
}

// system starts on first call, probably from a static object - this
// is to avoid link order dependency problems
UnitTest& UnitTest::getRoot() {
  startTestSystem();
  YARP_ASSERT(theRoot!=NULL);
  return *theRoot;
}

// this is the important one to call
void UnitTest::stopTestSystem() {
  if (theRoot!=NULL) {
    delete theRoot;
    theRoot = NULL;
  }
}


bool UnitTest::checkEqualImpl(int x, int y, 
			      const char *desc,
			      const char *txt1,
			      const char *txt2,
			      const char *fname,
			      int fline) {
  char buf[1000];
  ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%d) == %s (%d)",
		  fname, fline, desc, txt1, x, txt2, y);
  if (x==y) {
    report(0,String("[") + desc + "] passed ok");
  } else {
    report(1,String("FAILURE ") + buf);
  }
  return x==y;
}

bool UnitTest::checkEqualImpl(const String& x, const String& y,
			      const char *desc,
			      const char *txt1,
			      const char *txt2,
			      const char *fname,
			      int fline) {
  char buf[1000];
  ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%s) == %s (%s)",
		  fname, fline, desc, txt1, humanize(x).c_str(), txt2, humanize(y).c_str());
  bool ok = (x==y);
  if (ok) {
    report(0,String("[") + desc + "] passed ok");
  } else {
    report(1,String("FAILURE ") + buf);
  }
  return ok;
}


String UnitTest::humanize(const String& txt) {
  String result("");
  for (unsigned int i=0; i<txt.length(); i++) {
    char ch = txt[i];
    if (ch == '\n') {
      result += "\\n";
    } else if (ch == '\0') {
      result += "\\0";
    } else {
      result += ch;
    }
  }
  return result;
}


 
