#ifndef YARPNetworkTypes_INC
#define YARPNetworkTypes_INC

#include "YARPAll.h"

#ifdef __LINUX__
#include <stdint.h>
#endif

#ifdef __LINUX__
#define PACKED_FOR_NET __attribute__ ((packed))
#else
#define PACKED_FOR_NET
#endif

/*
  Need to specify number and order of bytes
 */

#ifdef __LINUX__
typedef int32_t NetInt32;
#endif


#ifdef __QNX__
typedef long int NetInt32;
#endif

#ifdef __WIN__
#ifdef __WIN_MSVC__
typedef __int32 NetInt32;
#else
#include <sys/config.h>
typedef __int32_t NetInt32;
#endif
#endif


#endif
