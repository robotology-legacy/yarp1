#ifndef _YARP2_COMPANION_
#define _YARP2_COMPANION_

#include <yarp/String.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

namespace yarp {
  class Companion;
}

class yarp::Companion {
public:

  static String version() {
    return String("2.0");
  }

  /**
   * The standard main method for the YARP companion utility.
   * @param argv Command line arguments
   */
  static int main(int argc, char *argv[]);

private:

  Companion();
  
  static Companion& getInstance() {
    return instance;
  }

  static Companion instance;

  int dispatch(const char *name, int argc, char *argv[]);

  int cmdVersion(int argc, char *argv[]) {
    ACE_OS::printf("YARP Companion utility version %s implemented in C++\n", 
		   version().c_str());
    return 0;
  }

  int cmdName(int argc, char *argv[]);

  int cmdWhere(int argc, char *argv[]);

  class Entry {
  public:
    String name;
    int (Companion::*fn)(int argc, char *argv[]);

    Entry(const char *name, int (Companion::*fn)(int argc, char *argv[])) :
      name(name),
      fn(fn)
    {}

    Entry() {
    }
  };

  ACE_Hash_Map_Manager<String,Entry,ACE_Null_Mutex> action;

  void add(const char *name, int (Companion::*fn)(int argc, char *argv[])) {
    Entry e(name,fn);
    action.bind(String(name),e);
  }
};

#endif
