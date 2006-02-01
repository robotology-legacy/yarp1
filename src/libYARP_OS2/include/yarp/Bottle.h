#ifndef _YARP2_BOTTLE_
#define _YARP2_BOTTLE_

#include <yarp/String.h>
#include <yarp/ManagedBytes.h>
#include <yarp/BlockReader.h>
#include <yarp/BlockWriter.h>
#include <yarp/Portable.h>

#include <ace/Vector_T.h>

namespace yarp {
  class Bottle;
}

class yarp::Bottle : public Portable {
public:
  class Storable {
  public:
    virtual ~Storable() {}
    virtual String toString() = 0;
    virtual void fromString(const String& src) = 0;
    virtual int getCode() = 0;
    virtual void readBlock(BlockReader& reader) = 0;
    virtual void writeBlock(BlockWriter& writer) = 0;
    virtual Storable *create() = 0;

    virtual int asInt() { return 0; }
    virtual String asString() { return ""; }
  };

  class StoreInt : public Storable {
  private:
    int x;
  public:
    StoreInt(int x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode();
    virtual void readBlock(BlockReader& reader);
    virtual void writeBlock(BlockWriter& writer);
    virtual Storable *create() { return new StoreInt(0); }
    virtual int asInt() { return x; }
  };

  class StoreString : public Storable {
  private:
    String x;
  public:
    StoreString(const String& x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode();
    virtual void readBlock(BlockReader& reader);
    virtual void writeBlock(BlockWriter& writer);
    virtual Storable *create() { return new StoreString(String("")); }
    virtual String asString() { return x; }
  };

  /*
  template <class T>
  class Store : public Storable {
  private:
    T x;
  public:
    Store(const T& x) { this->x = x; }
    const T& get() { return x; }

    virtual String toString() = 0;
    virtual void fromString(const String& src) = 0;    
  };
  */

  Bottle();
  virtual ~Bottle();


  bool isInt(int index);
  bool isString(int index);

  int getInt(int index);
  String getString(int index);

  void addInt(int x) {
    add(new StoreInt(x));
  }

  void addFloat(double x) {
    //add(Store<double>(x));
  }

  void addInts(int *x, int ct) {
  }

  void addFloats(double *x, int ct) {
  }

  void addString(const char *text) {
    add(new StoreString(String(text)));
  }

  void clear();

  void fromString(const String& line);
  String toString();
  int size();

  void fromBytes(const Bytes& data);

  void writeBlock(BlockWriter& writer);

  void readBlock(BlockReader& reader);

  const char *getBytes();
  int byteCount();

private:
  ACE_Vector<Storable*> content;
  ACE_Vector<char> data;
  bool dirty;

  void add(Storable *s);
  void smartAdd(const String& str);

  void synch();
};

#endif

