
#include "YARPSemaphore.h"

#include <windows.h>
#include <assert.h>



YARPSemaphore::YARPSemaphore(int initial_count)
{
	HANDLE hSemaphore;
	
	// Create a semaphore with initial and max. counts of 10.
	
	hSemaphore = CreateSemaphore( 
		NULL,   // no security attributes
		initial_count,   // initial count
		10000000,        // maximum count
		NULL);  // unnamed semaphore
	
	system_resource = hSemaphore;
	assert(system_resource!=NULL);
}

YARPSemaphore::YARPSemaphore(const YARPSemaphore& yt)
{
	HANDLE hSemaphore;
	LONG initial_count = -1;
	ReleaseSemaphore(yt.system_resource,1,&initial_count);
	DWORD dwWaitResult = WaitForSingleObject( 
        yt.system_resource,   // handle to semaphore
        0L);                  // zero-second time-out interval

	// Create a semaphore with initial and max. counts of 10.
	
	hSemaphore = CreateSemaphore( 
		NULL,   // no security attributes
		initial_count,   // initial count
		10000000,        // maximum count
		NULL);  // unnamed semaphore
	
	system_resource = hSemaphore;
	assert(system_resource!=NULL);
}

YARPSemaphore::~YARPSemaphore()
{
	if (system_resource!=NULL)
	{
		CloseHandle(system_resource);
	    system_resource = NULL; // not necessary, just coding style
	}
}

void YARPSemaphore::BlockingWait()
{
	DWORD dwWaitResult = WaitForSingleObject( 
        system_resource,   // handle to semaphore
        INFINITE);               // infinite time-out interval
	
}

int YARPSemaphore::PollingWait()
{
	DWORD dwWaitResult = WaitForSingleObject( 
        system_resource,   // handle to semaphore
        0L);               // zero-second time-out interval
	
	return (dwWaitResult==WAIT_OBJECT_0);
}


void YARPSemaphore::Post()
{
	HANDLE hSemaphore = system_resource;
	DWORD ret =
		ReleaseSemaphore( 
		hSemaphore,  // handle to semaphore
		1,           // increase count by one
		NULL);
	
	assert (ret != 0);   // not interested in previous count
}
