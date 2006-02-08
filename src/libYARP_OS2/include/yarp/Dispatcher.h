#ifndef _YARP2_DISPATCHER_
#define _YARP2_DISPATCHER_

#include <yarp/String.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/Vector_T.h>
#include <ace/OS_NS_stdio.h>


namespace yarp {
  template <class T, class RET> class Dispatcher;
}

/**
 * Dispatch to named methods based on string input.
 */
template <class T, class RET>
class yarp::Dispatcher {
private:
  class Entry {
  public:
    String name;
    RET (T::*fn)(int argc, char *argv[]);
    
    Entry(const char *name, RET (T::*fn)(int argc, char *argv[])) :
      name(name),
      fn(fn)
    {}

    Entry() {
    }
  };

  ACE_Hash_Map_Manager<String,Entry,ACE_Null_Mutex> action;
  ACE_Vector<String> names;

public:
  void add(const char *name, RET (T::*fn)(int argc, char *argv[])) {
    Entry e(name,fn);
    action.bind(String(name),e);
    // maintain a record of order of keys
    names.push_back(String(name));
  }

  RET dispatch(T *owner, const char *name, int argc, char *argv[]) {
    String sname(name);
    Entry e;
    int result = action.find(sname,e);
    if (result!=-1) {
      return (owner->*(e.fn))(argc,argv);
    } else {
      ACE_DEBUG((LM_ERROR,"Could not find command \"%s\"",name));
    }
    return RET();
  }

  ACE_Vector<String> getNames() {
    return names;
  }
};

#endif

