#include <sys/kernel.h>
#include <signal.h>
#include <assert.h>
#include <time.h>

#include "YARPTime.h"

/* basic delay() function not threadsafe in QNX4 */

static void SafeDelay(int milliseconds)
{
    timer_t id;
    struct itimerspec timer;
    struct sigevent event;
  
    event.sigev_signo = 0;
    assert((id=timer_create( CLOCK_REALTIME, &event ))!=-1);
  
    timer.it_value.tv_sec     = (milliseconds/1000);
    timer.it_value.tv_nsec    = (milliseconds%1000)*1000L*1000L;
    timer.it_interval.tv_sec  = 0L;
    timer.it_interval.tv_nsec = 0L;
    timer_settime( id, 0, &timer, NULL );

    timer_delete( id );
}


double YARPTime::GetTimeAsSeconds()
{
  struct timespec tspec;
  clock_gettime(CLOCK_REALTIME,&tspec);
  return tspec.tv_nsec/1000000000.0 + tspec.tv_sec;
}


void YARPTime::DelayInSeconds(double delay_in_seconds)
{
  SafeDelay((int)(0.5+delay_in_seconds*1000));
}

