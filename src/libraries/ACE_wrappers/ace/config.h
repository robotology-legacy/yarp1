#ifdef __QNX6__

#include "ace/config-qnx-rtp.h"

#else

#ifdef __WIN32__

#define ACE_HAS_STANDARD_CPP_LIBRARY 1
#include "ace/config-win32.h"

#endif

#endif