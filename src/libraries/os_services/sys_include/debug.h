
#ifndef DEBUG_H_INC
#define DEBUG_H_INC

#include <stdio.h>

extern int __debug_level;

#define DEBUG_LEVEL (__debug_level)

#define dbg_printf(x) if(DEBUG_LEVEL>=x) printf
#define dbg_fprintf(x) if(DEBUG_LEVEL>=x) fprintf
#define DBG(x) if(DEBUG_LEVEL>=x) 


#endif
