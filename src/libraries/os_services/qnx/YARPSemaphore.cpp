#include <semaphore.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "YARPSemaphore.h"

YARPSemaphore::YARPSemaphore(int initial_count)
{
  static int creation_count = 0;
  creation_count++;
  //printf("]]]] Creating semaphore number %d\n", creation_count);
  system_resource = NULL; // not necessary, just coding style
  system_resource = new sem_t;
  assert(system_resource!=NULL);
  assert(sem_init(((sem_t*)system_resource),1,initial_count)==0);
}

YARPSemaphore::YARPSemaphore(const YARPSemaphore& yt)
{
  assert(1==0);
  system_resource = NULL; // not necessary, just coding style
  system_resource = new sem_t;
  assert(system_resource!=NULL);
  int initial_count = 0;
  assert(sem_init(((sem_t*)system_resource),1,initial_count)==0);
  if (yt.system_resource!=NULL)
    {
      int ct = 0;
      YARPSemaphore& danger_yt = *((YARPSemaphore *)&yt);
      while (danger_yt.PollingWait())
	{
	  ct++;
	}
      while (ct)
	{
	  danger_yt.Post();
	  Post();
	  ct--;
	}
    }
}

YARPSemaphore::~YARPSemaphore()
{
  sem_destroy((sem_t*)system_resource);
  delete ((sem_t*)system_resource);
  system_resource = NULL; // not necessary, just coding style
}

void YARPSemaphore::BlockingWait()
{
  sem_wait((sem_t*)system_resource);
}

int YARPSemaphore::PollingWait()
{
  return (sem_trywait((sem_t*)system_resource)==0);
}

void YARPSemaphore::Post()
{
  sem_post((sem_t*)system_resource);
}

