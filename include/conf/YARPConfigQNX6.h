//
// $Id: YARPConfigQNX6.h,v 1.4 2003-05-13 22:38:04 gmetta Exp $
// YARP config file QNX6.
//
//

#ifndef __YARPConfigQNX6h__
#define __YARPConfigQNX6h__

//
// included by YARPConfig.h

#ifndef __QNX6__

#	define __QNX6__

#endif

#define SINGLE_MUTEX_FOR_REFCOUNTED 1
#define UPDATED_PORT 1
#define MAX_PACKET (128*128*3+100)

///#define YARP_TCP_NO_DELAY 1

/// set the default protocol in port creation.
#define YARP_DEFAULT_PROTOCOL	YARP_QNET

/// set the def protocol number of req ports.
#define YARP_PROTOCOL_REGPORTS  1

/// set the number of ports used by UDP for a single port.
#define YARP_UDP_REGPORTS       11

#include <ace/config.h>

#endif
