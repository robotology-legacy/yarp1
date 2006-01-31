#ifndef _YARP2_COMPANION_
#define _YARP2_COMPANION_

#include <yarp/String.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/Vector_T.h>
#include <ace/OS_NS_stdio.h>

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

  static int connect(const char *target, const char *src);

  static int disconnect(const char *target, const char *src);

private:

  Companion();
  
  static Companion& getInstance() {
    return instance;
  }

  static Companion instance;

  int dispatch(const char *name, int argc, char *argv[]);

  int cmdVersion(int argc, char *argv[]);

  int cmdName(int argc, char *argv[]);

  int cmdWhere(int argc, char *argv[]);

  int cmdHelp(int argc, char *argv[]);

  int cmdConnect(int argc, char *argv[]);

  int cmdDisconnect(int argc, char *argv[]);

  static int sendMessage(const String& port, const String& msg);

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
  ACE_Vector<String> names;

  void add(const char *name, int (Companion::*fn)(int argc, char *argv[])) {
    Entry e(name,fn);
    action.bind(String(name),e);
    // maintain a record of order of keys
    names.push_back(String(name));
  }
};

#endif
