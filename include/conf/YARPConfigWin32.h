//
// $Id: YARPConfigWin32.h,v 1.30 2003-08-27 16:37:31 babybot Exp $
// YARP config file WIN32.
//
//

#ifndef __YARPConfigWin32h__
#define __YARPConfigWin32h__

//
// included by YARPConfig.h

#ifndef __WIN32__
#	define __WIN32__
#endif

#ifndef __WIN__
#	define __WIN__
#endif

// using Microsoft Visual C++
#ifndef __WIN_MSVC__
#	define __WIN_MSVC__
#endif

///
/// Configuration flags
///
#define SINGLE_MUTEX_FOR_REFCOUNTED 1
#define UPDATED_PORT 1
#define MAX_PACKET (128*128*3+100)		
///8192
///(128*128*3+100)	/// shouldn't be required

/// MSVC has pragma once directive.
#define YARP_HAS_PRAGMA_ONCE	1

/// disable TCP Nagle's algorithm (experimental).
#define YARP_TCP_NO_DELAY		1

/// set the default protocol in port creation.
#define YARP_DEFAULT_PROTOCOL	YARP_TCP

/// set here how many ports are requested by the def protocol.
#define YARP_PROTOCOL_REGPORTS  11

/// set the number of ports used by UDP for a single port.
#define YARP_UDP_REGPORTS       11

/// read timeout on socket transport (in seconds).
#define YARP_SOCK_TIMEOUT       5

/// read short timeout on socket transport (in seconds). - used for waiting short latency delay.
#define YARP_SHORT_SOCK_TIMEOUT	1

/// magic number for handshaking connections.
#define YARP_MAGIC_NUMBER	7777

/// alignment bytes required for mmx/ipl instructions.
#define YARP_IMAGE_ALIGN 8

/// default length for general purpose strings.
#define YARP_STRING_LEN 256

/// default network name (must match your default net description in the name server config).
#define YARP_DEFAULT_NET "default"

/// disables warning for init_seg(lib) very legitimate use of directive.
#pragma warning (disable:4073)

///
/// ACE inclusion
///
#include <ace/config.h>

#endif
