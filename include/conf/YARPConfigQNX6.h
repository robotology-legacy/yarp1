//
// $Id: YARPConfigQNX6.h,v 1.3 2003-05-01 22:51:19 gmetta Exp $
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

#include <ace/config.h>

#endif