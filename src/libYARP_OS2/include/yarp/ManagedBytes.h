#ifndef _YARP2_MANAGEDBYTES_
#define _YARP2_MANAGEDBYTES_

namespace yarp {
  class ManagedBytes;
}

class yarp::ManagedBytes {
public:
  ManagedBytes(const Bytes& ext, bool owned = false) {
    b = ext;
    this->owned = owned;
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
    if (owned) {
      if (get()!=0) {
	delete[] get();
      }
    }
  }

  const Bytes& bytes() {
    return b;
  }

private:
  Bytes b;
  bool owned;
};

#endif
