// headsink.cpp : Defines the entry point for the console application.
//
// 2003 -- by nat

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
	SBSimpleInput suppress(YBVSinkSuppress);
	SBSimpleInput release(YBVSinkRelease);

	SBSimpleInput inhibitVorInput(YBVSinkInhibitVor);
	SBSimpleInput inhibitTrackerInput(YBVSinkInhibitTracker);
	SBSimpleInput inhibitVergenceInput(YBVSinkInhibitVergence);

	SBSimpleInput enableVorInput(YBVSinkEnableVor);
	SBSimpleInput enableTrackerInput(YBVSinkEnableTracker);
	SBSimpleInput enableVergenceInput(YBVSinkEnableVergence);

	SBSimpleInput displayStatusInput(YBVSinkDisplayStatus);
	
	SBOutputInhibitAll			inhibitAllCmd;
	SBOutputEnableAll			enableAllCmd;
	SBOutputSuppress			suppressCmd;
	SBOutputRelease				releaseCmd;
	SBOutputInhibitVor			inhibitVorCmd;
	SBOutputInhibitTracker		inhibitTrackerCmd;
	SBOutputInhibitVergence		inhibitVergenceCmd;

	SBOutputEnableVor			enableVorCmd;
	SBOutputEnableTracker		enableTrackerCmd;
	SBOutputEnableVergence		enableVergenceCmd;


	SBOutputDisplayStatus		displayStatusCmd;

	behavior.setInitialState(&waitIdle);
	behavior.add(&inhibitAllInput, &inhibitAllCmd);
	behavior.add(&enableAllInput, &enableAllCmd);
	behavior.add(&inhibitTrackerInput, &inhibitTrackerCmd);
	behavior.add(&inhibitVorInput, &inhibitVorCmd);
	behavior.add(&inhibitVergenceInput, &inhibitVergenceCmd);

	behavior.add(&enableTrackerInput, &enableTrackerCmd);
	behavior.add(&enableVorInput, &enableVorCmd);
	behavior.add(&enableVergenceInput, &enableVergenceCmd);

	behavior.add(&displayStatusInput, &displayStatusCmd);
	behavior.add(&suppress, &suppressCmd);
	behavior.add(&release, &releaseCmd);
	
	// start sink thread
	headSink.start();

	// start behavior
	behavior.Begin();
	behavior.loop();

	headSink.terminate(false);

	return 0;
}
