/*
	paulfitz Mon May 21 13:50:02 EDT 2001
	$Id: YARPTime.h,v 1.2 2003-04-10 15:01:25 gmetta Exp $
*/

#ifndef YARPTime_INC
#define YARPTime_INC

#include <conf/YARPConfig.h>
#include "YARPAll.h"

class YARPTime
{
public:
  static double GetTimeAsSeconds();

  // Assertion fails if insufficient resources
  static void DelayInSeconds(double delay_in_seconds);
};

#endif
