#ifndef __PORTABILITY_H_INC
#define __PORTABILITY_H_INC

#ifdef UNIX_BUILD
#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif
#endif

#endif
