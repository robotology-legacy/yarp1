#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#include "YARPTime.h"

double YARPTime::GetTimeAsSeconds()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec + tv.tv_usec/(1000000.0);
}


void YARPTime::DelayInSeconds(double delay_in_seconds)
{
  pthread_testcancel();
  usleep((int)(0.5+delay_in_seconds*1000000.0));
  pthread_testcancel();
}

