///
/// pasa Apr 2003
///

#include "YARPSemaphore.h"

/// ACE inclusion
#include <ace/config.h>
#include <ace/Synch.h>


///
#include <windows.h>


///	ACE_Lock_Adapter<ACE_Thread_Mutex> mutex;

YARPSemaphore::YARPSemaphore (int initial_count)
{
	ACE_Semaphore *sema = new ACE_Semaphore (initial_count);
	system_resource = (void *)sema;

	ACE_ASSERT (system_resource != NULL);
}

///
/// clone the semaphore 
///		polls the copied sema, 
/// 
YARPSemaphore::YARPSemaphore (const YARPSemaphore& yt)
{
	ACE_Semaphore *sema = new ACE_Semaphore (0);
	system_resource = (void *)sema;
	ACE_ASSERT (system_resource != NULL);
	
	if (yt.system_resource != NULL)
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

YARPSemaphore::~YARPSemaphore ()
{
	if (system_resource!=NULL)
	{
		//CloseHandle(system_resource);
		if (system_resource != NULL) delete (ACE_Semaphore *)system_resource; 

	    system_resource = NULL; // not necessary, just coding style
	}
}

void YARPSemaphore::BlockingWait ()
{
	((ACE_Semaphore *)system_resource)->acquire();
}

int YARPSemaphore::PollingWait ()
{
	if (((ACE_Semaphore *)system_resource)->tryacquire() < 0)
	{
		if (errno == EBUSY)
			ACE_DEBUG ((LM_DEBUG, "%p\n", "Semaphore is already acquired, can't decrement"));

		return 0;
	}
	else
	{
		return 1;
	}
}


void YARPSemaphore::Post ()
{
	int ret = ((ACE_Semaphore *)system_resource)->release();
	ACE_ASSERT (ret == 0);   // not interested in previous count
}
