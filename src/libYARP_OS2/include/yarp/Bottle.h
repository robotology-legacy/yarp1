#ifndef _YARP2_BOTTLE_
#define _YARP2_BOTTLE_

#include <yarp/ConstString.h>
#include <yarp/PortReader.h>
#include <yarp/PortWriter.h>

namespace yarp {
  namespace os {
    class Bottle;
  }
}

/**
 * A collection of simple objects that can be safely and
 * simply transported across the network in a portable way.
 * Handy to use until you feel the need to make your own more 
 * efficient formats for transmission.
 */
class yarp::os::Bottle : public PortReader, public PortWriter {
public:
  Bottle();
  virtual ~Bottle();

  void clear();

  void addInt(int x);
  void addDouble(double x);
  void addString(const char *str);

  int getInt(int index);
  ConstString getString(int index);
  double getDouble(int index);

  bool isInt(int index);
  bool isDouble(int index);
  bool isString(int index);

  void fromString(const char *text);
  ConstString toString();

  bool write(ConnectionWriter& writer);
  bool read(ConnectionReader& reader);

private:
  void *implementation;
};

#endif

