// need this to pick up WaitableTimer code
#define _WIN32_WINNT 0x400

#include <windows.h>
#include <stdio.h>

#include <sys/timeb.h>
#include <time.h>

#include "YARPTime.h"

double YARPTime::GetTimeAsSeconds()
{
   struct _timeb timebuffer;

   _ftime( &timebuffer );

   return timebuffer.time + timebuffer.millitm/1000.0;

}

void YARPTime::DelayInSeconds(double delay_in_seconds)
{
    HANDLE hTimer = NULL;
    LARGE_INTEGER liDueTime;

	// units of 100 nanoseconds of all things
    liDueTime.QuadPart = -((__int64)(-0.5+10000000L*delay_in_seconds));


    // Create a waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!hTimer)
    {
        printf("CreateWaitableTimer failed (%d)\n", GetLastError());
		exit(1);
    }

	/// printf("Waiting for 10 seconds...\n");

    // Set a timer to wait for 10 seconds.
    if (!SetWaitableTimer(
        hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        printf("SetWaitableTimer failed (%d)\n", GetLastError());
		exit(2);
    }

    // Wait for the timer.

    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
        printf("WaitForSingleObject failed (%d)\n", GetLastError());
    //else printf("Timer was signaled.\n");

	if (hTimer!=NULL)
	{
		CloseHandle(hTimer);
	}

}
