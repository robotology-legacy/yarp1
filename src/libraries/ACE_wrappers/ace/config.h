//
// $Id: config.h,v 1.5 2003-08-12 15:35:02 gmetta Exp $
//	ACE Config File
//

#include <conf/YARPConfig.h>

#if defined(__QNX6__)

#include "ace/config-qnx-rtp.h"

#elif defined(__WIN32__)

// my ACE configuration.
#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#include "ace/config-win32.h"

// to ensure the object manager is static
#undef ACE_HAS_NONSTATIC_OBJECT_MANAGER

#elif defined(__LINUX__)

// my ACE configuration.
#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#include "ace/config-linux.h"

// to ensure the object manager is static
#undef ACE_HAS_NONSTATIC_OBJECT_MANAGER

#endif
