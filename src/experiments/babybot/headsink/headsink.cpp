// headsink.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>

#include "sink.h"
#include "sinkbehavior.h"

const int __rate = 20;
const char *__name = "headsink";
const char *__inifile = "headcontrol.ini";

int main(int argc, char* argv[])
{
	Sink headSink(__rate, __name, __inifile);
	SinkBehavior behavior(&headSink);

	SBWaitIdle waitIdle("idle state");
	SBSimpleInput inhibitAllInput(YBVSinkInhibitAll);
	SBOutputInhibitAll inhibitAllCmd;

	behavior.setInitialState(&waitIdle);
	behavior.add(&inhibitAllInput, &waitIdle, &waitIdle, &inhibitAllCmd);
	
	// start sink thread
	headSink.start();

	// start behavior
	behavior.Begin();
	behavior.loop();

	headSink.terminate(false);

	return 0;
}
