///
/// $Id: YARPScheduler.cpp,v 1.2 2003-04-10 15:01:35 gmetta Exp $
///
///

///
#include <ace/config.h>
#include <ace/OS.h>

///
#include <windows.h>
#include <mmsystem.h>

#ifdef Yield
#undef Yield
#endif

#include "YARPScheduler.h"

void YARPScheduler::yield ()
{
	// wonder whether it's portable.
	ACE_Time_Value tv(0);
	ACE_OS::sleep(tv);
}

void YARPScheduler::setHighResScheduling ()
{
#ifdef ACE_WIN32
	TIMECAPS tm;
	timeGetDevCaps(&tm, sizeof(TIMECAPS));
	timeBeginPeriod(1);
#endif
}