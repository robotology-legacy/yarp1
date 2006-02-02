#ifndef _YARP2_MANAGEDBYTES_
#define _YARP2_MANAGEDBYTES_

#include <yarp/Bytes.h>

namespace yarp {
  class ManagedBytes;
}

class yarp::ManagedBytes {
public:
  ManagedBytes() {
    b = Bytes(NULL,0);
    owned = 0;
  }

  ManagedBytes(const Bytes& ext, bool owned = false) {
    b = ext;
    this->owned = owned;
  }

  ManagedBytes(int len) {
    char *buf = new char[len];
    b = Bytes(buf,len);
    owned = true;
  }

  void allocate(int len) {
    clear();
    char *buf = new char[len];
    b = Bytes(buf,len);
    owned = true;
  }

  void copy() {
    if (!owned) {
      int len = length();
      char *buf = new char[len];
      ACE_OS::memcpy(buf,get(),len);
      b = Bytes(buf,len);
      owned = true;
    }
  }

  int length() const {
    return b.length();
  }

  char *get() const {
    return b.get();
  }

  virtual ~ManagedBytes() {
    clear();
  }

  void clear() {
    if (owned) {
      if (get()!=0) {
	delete[] get();
      }
      owned = 0;
    }
    b = Bytes(NULL,0);
  }

  const Bytes& bytes() {
    return b;
  }

private:
  Bytes b;
  bool owned;
};

#endif
