//
// $Id: YARPConfigQNX6.h,v 1.16 2003-07-06 23:25:45 gmetta Exp $
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
///(128*128*3+100)

///#define YARP_TCP_NO_DELAY 1

/// set the default protocol in port creation.
#define YARP_DEFAULT_PROTOCOL	YARP_QNET

/// set the def protocol number of req ports.
#define YARP_PROTOCOL_REGPORTS  1

/// set the number of ports used by UDP for a single port.
#define YARP_UDP_REGPORTS       11

/// read timeout on socket transport (in seconds).
#define YARP_SOCK_TIMEOUT		5

/// read short timeout on socket transport (in seconds). - used for waiting short latency delay.
#define YARP_SHORT_SOCK_TIMEOUT	1

/// stale connections timeout.
#define YARP_STALE_TIMEOUT		3600

/// alignment bytes required for mmx/ipl instructions.
#define YARP_IMAGE_ALIGN 8

/// default length for general purpose strings.
#define YARP_STRING_LEN 256

#include <ace/config.h>

#endif
