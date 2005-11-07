
#include "yarpy.h"

#include <yarp/YARPPort.h>
#include <yarp/YARPTime.h>

void yarpy() {
  printf("Time is %g\n", YARPTime::GetTimeAsSeconds());
}
