// headcontrol.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <YARPScheduler.h>
#include "HeadBehavior.h"

#include <iostream>

// #include "..\..\visualprocessing\parameters.h"
#include "headthread.h"

using namespace std;

char __filename[256] = "head.ini";

int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	cout << "Starting head service\n";

	int _head_thread_rate;

	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];
	
#if defined(__WIN32__)
	ACE_OS::sprintf (path, "%s\\conf\\babybot\\\0", root); 
#elif defined (__QNX6__)
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
#endif

	file.set(path, __filename);
	file.get("[THREAD]", "Rate", &_head_thread_rate);

	HeadThread thread(_head_thread_rate,
						"head thread",
						__filename);
	thread.start();

	HeadBehavior behavior(&thread, YBLabelMotor, "/headcontrol/behavior/i");

	HBWaitIdle waitIdle("idle");
	HBDirectCommandInput directCmd(YBVHeadNewCmd);
	behavior.setInitialState(&waitIdle);

	behavior.add(&directCmd, &waitIdle, &waitIdle);

	behavior.Begin();
	behavior.loop();

	thread.terminate(false);	// no timeout here !
	return 0;
}
