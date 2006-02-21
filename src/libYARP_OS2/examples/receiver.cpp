// source for receiver.cpp
#include <yarp/Port.h>
#include <yarp/Bottle.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
  Bottle bot2;
  Port input;
  input.open("/in");
  input.read(bot2);
  printf("Got message: %s\n", bot2.toString().c_str());
  input.close();
  return 0;
}
