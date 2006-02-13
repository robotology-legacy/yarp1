#ifndef _YARP2_PORT_
#define _YARP2_PORT_

#include <yarp/Contact.h>

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

  void close();

  /**
   * returns contact information for the port.
   */
  Contact where();

private:
  void *implementation;

};

#endif
