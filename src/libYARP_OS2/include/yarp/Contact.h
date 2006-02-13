#ifndef _YARP2_CONTACT_
#define _YARP2_CONTACT_

#include <yarp/ConstString.h>

namespace yarp {
  namespace os {
    class Contact;
  }
}

/**
 * Class for representing how to reach parts of a YARP network.
 * May contain complete or partial information.
 */
class yarp::os::Contact {
public:

  /**
   * Constructor.  Returns a new, blank, unnamed contact.
   */
  static Contact empty();

  /**
   * Constructor.  Returns a named contact, with no information about
   * how to reach that contact.
   */
  static Contact byName(const char *name);
  
  /**
   * Constructor.  Returns an unnamed contact, with information about
   * which carrier/protocol to use to reach that contact (tcp, udp, ...)
   */
  static Contact byCarrier(const char *carrier);

  Contact addCarrier(const char *carrier);

  /**
   * Constructor.  Returns an unnamed contact, with information about
   * how to reach it using socket communication
   */
  static Contact bySocket(const char *carrier, 
			  const char *host,
			  int portNumber);

  Contact addSocket(const char *carrier, 
		    const char *host,
		    int portNumber);

  Contact(const Contact& alt);

  const Contact& operator = (const Contact& alt);

  virtual ~Contact();

  ConstString getName() const;
  ConstString getHost() const;
  ConstString getCarrier() const;
  int getPort() const;

private:

  Contact();


  void *implementation;
};

#endif
