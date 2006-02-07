
#include <yarp/Logger.h>

// some small yarp 1 compatibility issues

using namespace yarp;

void set_yarp_debug(int x, int y) {
  if (x>10 || y>10) {
    Logger::get().setVerbosity(1);
  }
  YARP_DEBUG(Logger::get(), "got YARP1 call to set_yarp_debug");
}

