// headcontrol.cpp : Defines the entry point for the console application.
//
#include <yarp/YARPScheduler.h>
#include "HeadBehavior.h"

#include <iostream>

// #include "..\..\visualprocessing\parameters.h"
#include "HeadThread.h"

using namespace std;

char __filename[256] = "head.ini";

int main(int argc, char* argv[])
{
	// read ini file
	YARPScheduler::setHighResScheduling();

	cout << "Starting head service\n";

	int _head_thread_rate;

	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];
	ACE_OS::sprintf (path, "%s/%s", root, ConfigFilePath); 

	file.set(path, __filename);
	file.get("[THREAD]", "Rate", &_head_thread_rate);

	ACE_OS::printf("Rate: %d\n", _head_thread_rate);

	// create thread and behavior SM
	HeadThread thread(_head_thread_rate,
						"head thread",
						__filename);
	
	HeadBehavior behavior(&thread);

	// behavior states
	HBWaitIdle				waitIdle("idle");
	HBWaitIdle				hibernated("hibernation");
	HBDirectCommandInput	directIn(YBVHeadNewCmd);
	HBSimpleInput			hibernateIn(YBVHeadHibernate);
	HBSimpleInput			resumeIn(YBVHeadResume);
	HBAccInput				accIn(YBVHeadNewAcc);

	HBHibernateCommand	hibernateOut;
	HBResumeCommand		resumeOut;

	// copnfig behavior SM
	behavior.setInitialState(&waitIdle);
	behavior.add(&directIn, &waitIdle, &waitIdle);
	behavior.add(&accIn, &waitIdle, &waitIdle);
	behavior.add(&hibernateIn, &waitIdle, &hibernated, &hibernateOut);
	behavior.add(&resumeIn, &hibernated, &waitIdle, &resumeOut);

	// start thread and FSM
	thread.start();
	
	behavior.Begin();
	behavior.loop();

	thread.terminate(false);	// no timeout here !
	return 0;
}
