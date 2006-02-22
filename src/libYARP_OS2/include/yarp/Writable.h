#ifndef _YARP2_WRITABLE_
#define _YARP2_WRITABLE_

#include <yarp/ConnectionWriter.h>

#include <yarp/PortWriter.h>

// this typedef will be removed soon - it used be a separate class

namespace yarp {
  typedef yarp::os::PortWriter Writable;
}


#endif
