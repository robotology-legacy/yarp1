//#include <sys/kernel.h>
#include <signal.h>
#include <assert.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

//#include <mig4nto.h>

#include "YARPTime.h"

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
        struct _pulse   pulse;
        /* your other message structures would go 
           here too */
} my_message_t;


/* basic delay() function not threadsafe in QNX4 */

/*
static void SafeDelay(int milliseconds)
{
    timer_t id;
    struct itimerspec timer;
    struct sigevent event;
  
    event.sigev_signo = 0;

    ///assert((id=timer_create( CLOCK_REALTIME, &event ))!=-1);
    timer_create(CLOCK_REALTIME, &event, &id); //+QNX6+
	assert(id !=-1);
  
    timer.it_value.tv_sec     = (milliseconds/1000);
    timer.it_value.tv_nsec    = (milliseconds%1000)*1000L*1000L;
    timer.it_interval.tv_sec  = 0L;
    timer.it_interval.tv_nsec = 0L;
    timer_settime( id, 0, &timer, NULL );

    timer_delete( id );
}
*/
//SafeDelay in QNX6 version.

static void SafeDelay(int milliseconds)
{
   struct sigevent         event;
   struct itimerspec       itime;
   timer_t                 timer_id;
   int                     chid;
   int                     rcvid;
   my_message_t            msg;

   chid = ChannelCreate(0);

   event.sigev_notify = SIGEV_PULSE;
   event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, 
                                    chid, 
                                    _NTO_SIDE_CHANNEL, 0);
   event.sigev_priority = getprio(0);
   event.sigev_code = MY_PULSE_CODE;
   timer_create(CLOCK_REALTIME, &event, &timer_id);

   itime.it_value.tv_sec     = (milliseconds/1000);
   itime.it_value.tv_nsec    = (milliseconds%1000)*1000L*1000L;
   itime.it_interval.tv_sec  = 0L;
   itime.it_interval.tv_nsec = 0L;

   timer_settime(timer_id, 0, &itime, NULL);

   rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
   if (rcvid == 0) 
   { /* we got a pulse */
        if (msg.pulse.code == MY_PULSE_CODE) 
		{
			timer_delete( timer_id );
			ChannelDestroy(chid);
        } /* else other pulses ... */
   } /* else other messages ... */
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

