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
	SBSimpleInput enableAllInput(YBVSinkEnableAll);
	SBSimpleInput inhibitVorInput(YBVSinkInhibitVor);
	SBSimpleInput inhibitTrackerInput(YBVSinkInhibitTracker);
	SBSimpleInput inhibitVergenceInput(YBVSinkInhibitVergence);
	SBSimpleInput displayStatusInput(YBVSinkDisplayStatus);
	
	SBOutputInhibitAll			inhibitAllCmd;
	SBOutputEnableAll			enableAllCmd;
	SBOutputInhibitVor			inhibitVorCmd;
	SBOutputInhibitTracker		inhibitTrackerCmd;
	SBOutputInhibitVergence		inhibitVergenceCmd;
	SBOutputDisplayStatus		displayStatusCmd;

	behavior.setInitialState(&waitIdle);
	behavior.add(&inhibitAllInput, &inhibitAllCmd);
	behavior.add(&enableAllInput, &enableAllCmd);
	behavior.add(&inhibitTrackerInput, &inhibitTrackerCmd);
	behavior.add(&inhibitVorInput, &inhibitVorCmd);
	behavior.add(&inhibitVergenceInput, &inhibitVergenceCmd);
	behavior.add(&displayStatusInput, &displayStatusCmd);
	
	// start sink thread
	headSink.start();

	// start behavior
	behavior.Begin();
	behavior.loop();

	headSink.terminate(false);

	return 0;
}
