//
// $Id: YARPConfigQNX6.h,v 1.2 2003-04-30 13:20:54 beltran Exp $
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

#include <ace/config.h>

#endif