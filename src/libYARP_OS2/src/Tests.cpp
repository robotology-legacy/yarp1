
#include <yarp/Address.h>

#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_stdlib.h>

/*
 * Just the start of a test harness.
 * Should find some portable C++ equivalent of junit.
 */

using namespace yarp;

template <class T>
static void assertion(const T& x, const T& y) {
  if (x!=y) {
    ACE_OS::printf("Failure!\n");
    ACE_OS::exit(1);
  }
}

static int checkAddress() {
  Address address("localhost",10000,"tcp");
  String txt = address.toString();
  assertion(txt,String("tcp:/localhost:10000"));
  return 0;
}

int yarp_test_main(int argc, char *argv[]) {
  ACE_OS::printf("yarp testing underway\n");
  checkAddress();
  ACE_OS::printf("yarp testing done\n");
  return 0;
}

