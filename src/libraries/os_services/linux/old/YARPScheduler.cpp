#include <pthread.h>

#include "YARPScheduler.h"


void YARPScheduler::Yield()
{
  pthread_testcancel();
  sched_yield();
  pthread_testcancel();
}
