#include "YARPRnd.h"

YARPRnd::YARPRnd(long seed)
{
	theSeed = seed;
	aa = 16807L;
	mm = 2147483647L;
	qq = 127773L;
	rr = 2836L;
}

double YARPRnd::getNumber()
{
	hh = theSeed/qq;
	lo = theSeed-hh*qq;
	test = aa*lo-rr*hh;
	if (test > 0)
	{
		theSeed = test;
	}
	else
	{
		theSeed = test+mm;
	}
		
	return (double)theSeed/(double)mm;
}
