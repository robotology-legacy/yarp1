#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include "Semaphore.h"

#include "YARPSemaphore.h"



YARPSemaphore::YARPSemaphore(int initial_count)
{
  system_resource = NULL; // not necessary, just coding style
  system_resource = new Semaphore;
  assert(system_resource!=NULL);
  semaphore_init((Semaphore*)system_resource);
  if (initial_count!=1)
    {
      semaphore_decrement((Semaphore*)system_resource);
      for (int i=0; i<initial_count; i++)
	{
	  Post();
	}
    }
}

YARPSemaphore::YARPSemaphore(const YARPSemaphore& yt)
{
  assert(1==0);
  system_resource = NULL; // not necessary, just coding style
  system_resource = new Semaphore;
  assert(system_resource!=NULL);
  semaphore_init((Semaphore*)system_resource);
  if (yt.system_resource!=NULL)
    {
      int initial_count = semaphore_value((Semaphore*)(yt.system_resource));
      if (initial_count!=1)
	{
	  semaphore_decrement((Semaphore*)system_resource);
	  for (int i=0; i<initial_count; i++)
	    {
	      Post();
	    }
	}  
    }
}


YARPSemaphore::~YARPSemaphore()
{
  semaphore_destroy((Semaphore*)system_resource);
  delete ((Semaphore*)system_resource);
  system_resource = NULL; // not necessary, just coding style
}

void YARPSemaphore::BlockingWait()
{
  semaphore_down((Semaphore*)system_resource);
}

int YARPSemaphore::PollingWait()
{
  int v = semaphore_decrement((Semaphore*)system_resource);
  if (v<0)
    {
      /* Not perfect by any means, may need to use a static semaphore
         to make this atomic relative to blocking, posting.
	 Fine for producer/consumer though
      */
      Post();
    }
  return (v>=0);
}

void YARPSemaphore::Post()
{
  semaphore_up((Semaphore*)system_resource);
}

