/*
paulfitz Mon May 21 13:35:42 EDT 2001
	$Id: YARPSemaphore.h,v 1.2 2003-04-10 15:01:32 gmetta Exp $
 */

#ifndef YARPSemaphore_INC
#define YARPSemaphore_INC

#include <conf/YARPConfig.h>
#include "YARPAll.h"

/*
Ideally, would use POSIX semaphores, threads etc.
 */

class YARPSemaphore
{
private:
	void *system_resource;

public:
	// Assertion fails if insufficient resources at initialization
	YARPSemaphore(int initial_count=1);
	YARPSemaphore(const YARPSemaphore& yt);

	virtual ~YARPSemaphore();

	void BlockingWait();
	int PollingWait();  // returns true if Wait() occurred

	int Wait(int blocking = 1)
	{
		if (blocking) 
		{ 
			BlockingWait(); 
			return 1; 
		}
		else
		{ 
			return PollingWait(); 
		} 
	}

	void Post();
};

#endif
