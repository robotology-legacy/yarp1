//
// $Id: config.h,v 1.3 2003-04-09 11:10:39 gmetta Exp $
//	ACE Config File
//

#include <conf/YARPConfig.h>

#ifdef __QNX6__

#include "ace/config-qnx-rtp.h"

#else

#ifdef __WIN32__

#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#include "ace/config-win32.h"

#endif

#endif