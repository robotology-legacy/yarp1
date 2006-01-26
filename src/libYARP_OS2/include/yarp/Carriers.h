#ifndef _YARP2_CARRIERS_
#define _YARP2_CARRIERS_

#include <yarp/String.h>
#include <yarp/Bytes.h>
#include <yarp/Address.h>
#include <yarp/Face.h>
#include <yarp/OutputProtocol.h>
#include <yarp/Carrier.h>

namespace yarp {
  class Carriers;
}

/**
 *
 * The Carriers class is the starting point for all communication.
 *
 */
class yarp::Carriers {
public:
  static Carrier *chooseCarrier(const String& name);
  static Carrier *chooseCarrier(const Bytes& bytes);

  static Face *listen(const Address& address); // throws IOException
  static OutputProtocol *connect(const Address& address); // throws IOException
};


#endif


