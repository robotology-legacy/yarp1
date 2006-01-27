
#include <yarp/Logger.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

using namespace yarp;

Logger Logger::root("yarp: ");

Logger& Logger::get() {
  return root;
}


void Logger::show(int level, const char *txt) {
  if (verbose>0) {
    level = 10000;
  }
  if (parent == NULL) {
    if (level>=low) {
      ACE_OS::fprintf(stderr,"%s: %s\n",prefix.c_str(),txt);
    }
  } else {
    String more(prefix);
    more += ": ";
    more += txt;
    parent->show(level,more.c_str());
  }
}


void Logger::exit(int level) {
  ACE_OS::exit(level);
}
