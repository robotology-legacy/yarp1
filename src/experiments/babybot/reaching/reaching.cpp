// reaching.cpp : Defines the entry point for the console application.
//
// #define __INHIBITGRASPING__

#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPMatrix.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPConfigRobot.h>

#include "armmap.h"
#include "reachingBehavior.h"

int _headFrames = 0;
int _armFrames = 0;
int _nFrames = 0;

int main(int argc, char* argv[])
{
	YARPBabyBottle _outputTrainBottle;
	
	YVector _arm;
	YVector _head;
	ABSharedData _data;
		
	ReachingBehavior _behavior(&_data);
	RBWaitIdle waitIdle("Waiting for reaching signal");
	RBWaitDeltaT reachingSeq1("Waiting for arm done (prepare reaching)", 0.5);
	RBWaitIdle reachingSeq2("AS state, inhibit head");
	RBWaitIdle reachingSeq2b("AS state issue reaching command");
	
	RBWaitIdle reachingSeq31("Waiting for arm done (actually reaching1)");
	RBWaitIdle reachingSeq32("Waiting for arm done (actually reaching2)");
	RBWaitIdle reachingSeq33("Waiting for arm done (actually reaching3)");
	
	RBWaitDeltaT waitDeltaT1("Waiting for hand", 0.5);
	RBWaitDeltaT reachingSeq4("AS state, issue withdraw arm", 0.5);
	RBWaitIdle reachingSeq5("Waiting for arm done (withdrawing arm)");
	RBWaitIdle reachingSeq6("Open hand");

	RBOutputCommand			reachingPrepareOutput;
	RBHandClose				handClose;
	RBOutputReaching1		reachingOutput1;
	RBOutputReaching2		reachingOutput2;
	RBOutputReaching3		reachingOutput3;
	RBOutputBack			reachingBack;
	RBLearnOutputCommand	learnOutput;
	RBInputCommand			learnInput(YBVReachingLearn);
	RBInputCommand			armDone(YBVArmDone);
	RBInputCommand			handDone(YBVHandDone);
	RBInputCommand			reachingInput(YBVReachingReach);
	RB2Output				inhibitAll(YBVSinkSuppress, YBVGraspRflxInhibit);
	RB2Output				enableAll(YBVSinkRelease, YBVGraspRflxRelease);
	RBHandOpen				openHand;
	RBInputCommand			armRest(YBVArmRest);
	RBInputCommand			armIsBusy(YBVArmIsBusy);

	RBInputCommand			reachingPrepareInput(YBVReachingPrepare);

	_behavior.setInitialState(&waitIdle);
	_behavior.add(&learnInput, &learnOutput);
	// _behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepare);
	// _behavior.add(NULL, &reachingSeq1, &reachingSeq2, &inhibitHead);
	_behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepareOutput);
	_behavior.add(&armDone, &reachingSeq1, &reachingSeq2);
	_behavior.add(&armIsBusy, &reachingSeq1, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq1, &waitIdle, &enableAll);
	
	_behavior.add(NULL, &reachingSeq2, &reachingSeq2b, &inhibitAll);
	_behavior.add(NULL, &reachingSeq2b, &reachingSeq31, &reachingOutput1);
	_behavior.add(&armDone, &reachingSeq31, &reachingSeq32, &reachingOutput2);
	_behavior.add(&armIsBusy, &reachingSeq31, &waitIdle, &enableAll);
	
	_behavior.add(&armDone, &reachingSeq32, &waitDeltaT1, &handClose);
	_behavior.add(&armIsBusy, &reachingSeq32, &waitIdle, &enableAll);
	// wait
	_behavior.add(&handDone, &waitDeltaT1, &reachingSeq33, &reachingOutput3);
	_behavior.add(&armRest, &waitDeltaT1, &waitIdle, &enableAll);
	_behavior.add(&armDone, &reachingSeq33, &reachingSeq5, &reachingBack);
	_behavior.add(&armIsBusy, &reachingSeq33, &waitIdle, &enableAll);


	// _behavior.add(&armIsBusy, &reachingSeq3, &waitIdle, &enableHead);
	// _behavior.add(&armRest, &reachingSeq3, &waitIdle, &enableHead);
	// _behavior.add(NULL, &reachingSeq4, &reachingSeq5, &reachingBack);
	
	_behavior.add(&armDone, &reachingSeq5, &reachingSeq6, &openHand);
	_behavior.add(NULL, &reachingSeq6, &waitIdle, &enableAll);

	_behavior.add(&armIsBusy, &reachingSeq5, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq31, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq32, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq33, &waitIdle, &enableAll);
	
	_behavior.Begin();
	_behavior.loop();

	return 0;
}