/*
	paulfitz Mon May 21 14:08:59 EDT 2001
	$Id: YARPScheduler.h,v 1.2 2003-04-10 15:01:25 gmetta Exp $
*/
#ifndef YARPScheduler_INC
#define YARPScheduler_INC

#include <conf/YARPConfig.h>
#include "YARPAll.h"

class YARPScheduler
{
public:
	static void yield ();
	static void setHighResScheduling ();	// WIN32 only?
};

#endif
