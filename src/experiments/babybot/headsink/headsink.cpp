// headsink.cpp : Defines the entry point for the console application.
//
// 2003 -- by nat

#include <yarp/YARPScheduler.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>

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
	SBSimpleInput inhibitSaccadeInput(YBVSinkInhibitSaccade);

	SBSimpleInput enableVorInput(YBVSinkEnableVor);
	SBSimpleInput enableSaccadeInput(YBVSinkEnableSaccade);
	SBSimpleInput enableTrackerInput(YBVSinkEnableTracker);
	SBSimpleInput enableVergenceInput(YBVSinkEnableVergence);

	SBSimpleInput trackingMode(YBVSinkTrackingMode);
	SBSimpleInput saccadeMode(YBVSinkSaccadeMode);

	SBSimpleInput displayStatusInput(YBVSinkDisplayStatus);
	
	SBOutputInhibitAll			inhibitAllCmd;
	SBOutputEnableAll			enableAllCmd;
	SBOutputSuppress			suppressCmd;
	SBOutputRelease				releaseCmd;
	SBOutputInhibitVor			inhibitVorCmd;
	SBOutputInhibitTracker		inhibitTrackerCmd;
	SBOutputInhibitVergence		inhibitVergenceCmd;
	SBOutputInhibitSaccade		inhibitSaccadeCmd;

	SBOutputEnableVor			enableVorCmd;
	SBOutputEnableTracker		enableTrackerCmd;
	SBOutputEnableVergence		enableVergenceCmd;
	SBOutputEnableSaccade		enableSaccadeCmd;

	SBOutputTrackingMode		trackingModeCmd;
	SBOutputSaccadeMode			saccadeModeCmd;

	SBOutputDisplayStatus		displayStatusCmd;

	behavior.setInitialState(&waitIdle);
	behavior.add(&inhibitAllInput, &inhibitAllCmd);
	behavior.add(&enableAllInput, &enableAllCmd);
	behavior.add(&inhibitTrackerInput, &inhibitTrackerCmd);
	behavior.add(&inhibitVorInput, &inhibitVorCmd);
	behavior.add(&inhibitVergenceInput, &inhibitVergenceCmd);
	behavior.add(&inhibitSaccadeInput, &inhibitSaccadeCmd);

	behavior.add(&enableTrackerInput, &enableTrackerCmd);
	behavior.add(&enableVorInput, &enableVorCmd);
	behavior.add(&enableVergenceInput, &enableVergenceCmd);
	behavior.add(&enableSaccadeInput, &enableSaccadeCmd);

	behavior.add(&displayStatusInput, &displayStatusCmd);
	behavior.add(&suppress, &suppressCmd);
	behavior.add(&release, &releaseCmd);
	
	behavior.add(&trackingMode, &trackingModeCmd);
	behavior.add(&saccadeMode, &saccadeModeCmd);
	
	// start sink thread
	headSink.start();

	// start behavior
	behavior.Begin();
	behavior.loop();

	headSink.terminate(false);

	return 0;
}
