#include <windows.h>

#ifdef Yield
#undef Yield
#endif

#include "YARPScheduler.h"


void YARPScheduler::Yield()
{
	Sleep(0);
}
