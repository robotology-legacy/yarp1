/*
paulfitz Mon May 21 13:50:02 EDT 2001
*/
#ifndef YARPTime_INC
#define YARPTime_INC

#include "YARPAll.h"

class YARPTime
{
public:
  static double GetTimeAsSeconds();

  // Assertion fails if insufficient resources
  static void DelayInSeconds(double delay_in_seconds);
};

#endif
