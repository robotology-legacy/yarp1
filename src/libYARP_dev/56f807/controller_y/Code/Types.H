/*****************************************************************************
*
* types.h - standard header 
*
*****************************************************************************/

#ifndef __TYPES_H
#define __TYPES_H

#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UWord16 clockid_t;
typedef long             time_t;
//typedef unsigned int     ssize_t;

#if 0
typedef struct{
  clockid_t ClockID;
} timer_t;
#else
typedef clockid_t timer_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
