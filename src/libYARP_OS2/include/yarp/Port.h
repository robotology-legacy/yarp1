#ifndef _YARP2_PORT_
#define _YARP2_PORT_

#include <yarp/Contact.h>
#include <yarp/PortReader.h>
#include <yarp/PortWriter.h>

namespace yarp {
  namespace os {
    class Port;
  }
}

class yarp::os::Port {

public:
  Port();
  virtual ~Port();

  /**
   * Start port operation, registering the port using the given name.
   */
  bool open(const char *name);

  /**
   * Start port operation, so that it can be reached
   * using the contact information given.  If the contact
   * information is incomplete, the name server is used
   * to complete it.  Set registerName to false if you don't
   * want name server help.
   */
  bool open(const Contact& contact, bool registerName = true);

  bool addOutput(const char *name) {
    return addOutput(Contact::byName(name));
  }

  bool addOutput(const char *name, const char *carrier) {
    return addOutput(Contact::byName(name).addCarrier(carrier));
  }

  bool addOutput(const Contact& contact);

  void close();

  /**
   * returns contact information for the port.
   */
  Contact where();

  /**
   * write something to the port
   */
  bool write(PortWriter& writer);

  /**
   * read something from the port
   */
  bool read(PortReader& reader);

  /**
   * set an external writer for port data
   */
  void setWriter(PortWriter& writer);

  /**
   * set an external reader for port data
   */
  void setReader(PortReader& reader);

private:
  void *implementation;

};

#endif
