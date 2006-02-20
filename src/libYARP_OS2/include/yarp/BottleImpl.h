#ifndef _YARP2_BOTTLEIMPL_
#define _YARP2_BOTTLEIMPL_

#include <yarp/String.h>
#include <yarp/ManagedBytes.h>
#include <yarp/BlockReader.h>
#include <yarp/BlockWriter.h>
#include <yarp/Portable.h>

#include <ace/Vector_T.h>

namespace yarp {
  class BottleImpl;
}

/**
 * A flexible data format for holding a bunch of numbers and strings.
 * Handy to use until you work out how to make your own more 
 * efficient formats for transmission.
 */
class yarp::BottleImpl : public Portable {
public:

  BottleImpl();
  virtual ~BottleImpl();


  bool isInt(int index);
  bool isString(int index);
  bool isDouble(int index);

  int getInt(int index);
  String getString(int index);
  double getDouble(int index);

  void addInt(int x) {
    add(new StoreInt(x));
  }

  void addDouble(double x) {
    add(new StoreDouble(x));
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
  void toBytes(const Bytes& data);

  void writeBlock(BlockWriter& writer);

  void readBlock(BlockReader& reader);

  const char *getBytes();
  int byteCount();

private:
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
    virtual double asDouble() { return 0; }
    virtual String asString() { return ""; }
  };

  class StoreInt : public Storable {
  private:
    int x;
  public:
    StoreInt() { x = 0; }
    StoreInt(int x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual void readBlock(BlockReader& reader);
    virtual void writeBlock(BlockWriter& writer);
    virtual Storable *create() { return new StoreInt(0); }
    virtual int asInt() { return x; }
    virtual double asDouble() { return x; }
    static const int code;
  };

  class StoreString : public Storable {
  private:
    String x;
  public:
    StoreString() { x = ""; }
    StoreString(const String& x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual void readBlock(BlockReader& reader);
    virtual void writeBlock(BlockWriter& writer);
    virtual Storable *create() { return new StoreString(String("")); }
    virtual String asString() { return x; }
    static const int code;
  };

  class StoreDouble : public Storable {
  private:
    double x;
  public:
    StoreDouble() { x = 0; }
    StoreDouble(double x) { this->x = x; }
    virtual String toString();
    virtual void fromString(const String& src);
    virtual int getCode() { return code; }
    virtual void readBlock(BlockReader& reader);
    virtual void writeBlock(BlockWriter& writer);
    virtual Storable *create() { return new StoreDouble(0); }
    virtual int asInt() { return (int)x; }
    virtual double asDouble() { return x; }
    static const int code;
  };

  ACE_Vector<Storable*> content;
  ACE_Vector<char> data;
  bool dirty;

  void add(Storable *s);
  void smartAdd(const String& str);

  void synch();
};

#endif

