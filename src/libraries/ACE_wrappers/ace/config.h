//
// $Id: config.h,v 1.4 2003-04-18 13:46:03 gmetta Exp $
//	ACE Config File
//

#include <conf/YARPConfig.h>

#ifdef __QNX6__

#include "ace/config-qnx-rtp.h"

#else

#ifdef __WIN32__

///#ifdef _MSC_VER
///#error "_MSC_VER already defined"
///#endif

// my ACE configuration.
///#define _MSC_VER 1200
#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#include "ace/config-win32.h"

// to ensure the object manager is static
#undef ACE_HAS_NONSTATIC_OBJECT_MANAGER

#endif

#endif