// grasprflx.cpp : Defines the entry point for the console application.
//

#include "reflexstates.h"
#include <YARPConfigFile.h>

const int N = 20;

YVector _closePos(6);
YVector _openPos(6);
double _threshold = 0.0;

void readConfigFile()
{
	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];

#if defined(__WIN32__)
	ACE_OS::sprintf (path, "%s\\conf\\babybot\\\0", root); 
#elif defined (__QNX6__)
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
#endif

	file.set(path, "grasprflx.ini");
	file.get("[GRASPREFLEX]", "Threshold", &_threshold, 1);
	file.get("[GRASPREFLEX]", "ClosePosition", _closePos.data(), 6);
	file.get("[GRASPREFLEX]", "OpenPosition", _openPos.data(), 6);
}

int main(int argc, char* argv[])
{
	ReflexShared shared;
	GRBehavior behavior(&shared);
	GRBLoopTouch loopTouch;
	GRBWaitIdle waitClose;
	GRBWaitIdle waitOpen;

	GRBWaitDeltaT waitT[] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2,
							 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 };
	GRBGrasp grasp;
	GRBOutputCommand closeCmd;
	GRBOutputCommand openCmd;
	GRBInit init;
	GRBSimpleInput motionDone(YBVHandDone);

	readConfigFile();

	closeCmd.cmd = _closePos;
	openCmd.cmd = _openPos;

	behavior.setInitialState(&loopTouch);
	behavior.add(&init, &loopTouch, &grasp, &closeCmd); 
	behavior.add(NULL, &loopTouch, &loopTouch);
	behavior.add(NULL, &grasp, &waitClose);
	behavior.add(&motionDone, &waitClose, &waitT[0]);
	for(int i = 0; i<(N-1); i++)
		behavior.add(NULL, &waitT[i], &waitT[i+1]);

	behavior.add(NULL, &waitT[N-1], &waitOpen, &openCmd);
	behavior.add(&motionDone, &waitOpen, &loopTouch);

	behavior.Begin();
	behavior.loop();

	return 0;
}
