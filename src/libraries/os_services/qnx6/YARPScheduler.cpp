#include "YARPScheduler.h"
//#include <mig4nto.h>
#include <sched.h>


//#include <sys/kernel.h>

// QNX4 issue:
// sys/kernel.h defines Yield to be __yield, so need to tiptoe around that
#ifdef Yield
#define KERNEL_YIELD __yield
#undef Yield
#else
#define KERNEL_YIELD Yield
#endif



/* MIG4NTO            Yield Use sched_yield() instead. */
void YARPScheduler::Yield()
{
//  ::KERNEL_YIELD();
	sched_yield();
}
