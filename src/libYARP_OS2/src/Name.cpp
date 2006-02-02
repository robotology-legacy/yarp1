
#include <yarp/Name.h>

using namespace yarp;

Name::Name(const String& txt) {
  this->txt = txt;
}

bool Name::isRooted() {
  if (txt.length()>=1) {
    if (txt[0] == '/') {
      return true;
    }
  }
  return false;
}

