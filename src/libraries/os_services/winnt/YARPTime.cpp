//
// $Id: YARPTime.cpp,v 1.2 2003-04-10 15:01:36 gmetta Exp $
//
//

// need this to pick up WaitableTimer code
// #define _WIN32_WINNT 0x400

#include <ace/config.h>
#include <ace/OS.h>

#include <windows.h>
//#include <stdio.h>

//#include <sys/timeb.h>
//#include <time.h>

#include "YARPTime.h"

double YARPTime::GetTimeAsSeconds()
{
	//static ACE_Time_Value gettimeofday (void);

	ACE_Time_Value timev = ACE_OS::gettimeofday ();
	return double(timev.sec()) + timev.usec() * 1e-6; 
}

///
///
/// WARNING: actual precision under WIN32 depends on setting scheduler by means of MM 
///		functions.
///
void YARPTime::DelayInSeconds(double delay_in_seconds)
{
	//static int sleep (const ACE_Time_Value &tv);
	ACE_Time_Value tv;
	tv.sec (int(delay_in_seconds));
	tv.usec ((delay_in_seconds-int(delay_in_seconds)) * 1e6);

	ACE_OS::sleep(tv);
}
