// reaching.cpp : Defines the entry point for the console application.
//
#define TEST_REACHING

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

const double __handOpen[] = {100.0*degToRad, 0.0, 20.0*degToRad, 20.0*degToRad, 20.0*degToRad, 20.0*degToRad};
const double __handClose[] = {100.0*degToRad, 50.0*degToRad, -80.0*degToRad, -80.0*degToRad, -80.0*degToRad, -80.0*degToRad};
const double __handAtRest[] = {0.0*degToRad, 50.0*degToRad, 0.0, 0.0, 0.0, 0.0};

const double __armDrop[] = {30*degToRad, -25*degToRad, 0.0, 0.0, -50*degToRad, -190.0*degToRad};

int main(int argc, char* argv[])
{
	YARPBabyBottle _outputTrainBottle;
	
	YVector _arm;
	YVector _head;
	ABSharedData _data;
		
	ReachingBehavior _behavior(&_data);
	RBWaitIdle waitIdle("Waiting for reaching signal");
	RBWaitIdle reachingSeq1("AS state, opening the hand");
	RBWaitDeltaT reachingSeq1b("Waiting for arm done (prepare reaching)", 0.5);
	RBWaitIdle reachingSeq2("AS state, inhibit head");
	RBWaitIdle reachingSeq2b("AS state issue reaching command");
	
	RBWaitIdle reachingSeq31("Waiting for arm done (actually reaching1)");
	RBWaitDeltaT reachingSeq32("Waiting for arm done (closed loop)",0);
	RBWaitDeltaT  reachingSeq32a("Waiting for arm done (actually reaching 2)",2);
	RBWaitIdle inhibitedState("Reaching inhibited");
	// RBWaitIdle reachingSeq33("Waiting for arm done (actually reaching3)");
	RBWaitDeltaT reachingSeq33("Waiting for arm done (actually reaching3)", 2);

	RBWaitDeltaT finalDropState1("Arm going to rest", 3);
	RBWaitIdle finalDropState2("Enabling saccade");
	
	// NO HAND
	// RBWaitDeltaT waitDeltaT1("Waiting for hand", 0.5);
	RBWaitDeltaT waitDeltaT1("Waiting for hand", 3);
	RBWaitDeltaT reachingSeq4("AS state, issue withdraw arm", 0.5);
	RBWaitIdle reachingSeq5("Waiting for arm done (withdrawing arm)");
	RBWaitIdle reachingSeq6("Open hand");
	RBWaitIdle reachingSeq7("Re-positioning hand");
	RBWaitIdle waitHandDone("Waiting for hand done");
	// RBWaitDeltaT checkReaching("Checking reaching", 1);
	RBWaitIdle checkReaching("Checking reaching");	// success failure
	RBWaitIdle checkACK("Check ack");				// check if reaching is feasible
	
	RBWaitIdle signalReachSuccess("Signaling reaching success");
	RBWaitIdle signalReachFailure("Signaling reaching failure");

	RBWaitIdle reachingDropState1("dropping state 1");
	RBWaitIdle reachingDropState2("dropping state 2");
	RBWaitIdle reachingDropState3("dropping state 3");
	RBWaitIdle reachingDropState4("dropping state 4");
		
	RBOutputCommand			reachingPrepareOutput(YBVReachingAck, YBVHandNewCmd, YVector(6, __handOpen));
	RBOutputReaching1		reachingOutput1;
	RBOutputReaching2		reachingOutput2;
	RBOutputReaching3		reachingOutput3;
	RBOutputReachingCL		reachingOutputCL;
	RBOutputBack			reachingBack;
	RBMotorCommand			reachingDropOut (YBVArmForceNewCmd, YVector(6, __armDrop));
	RBLearnOutputCommand	learnOutput;
	RBInputCommand			learnInput(YBVReachingLearn);
	RBInputCommand			armDone(YBVArmDone);
	RBInputCommand			handDone(YBVHandDone);
	RBInputCommand			reachingInput(YBVReachingReach);
	RBInputCommand			reachingAbort(YBVReachingAbort);
	RBInputCommand			reachingAck(YBVReachingAck);
	RB2Output				inhibitAll(YBVSinkSuppress, YBVGraspRflxInhibit);
	RB2Output				enableHandTracking(YBVSinkRelease, YBVSinkTrackingMode);
	RB4Output				enableAll(YBVSinkRelease, YBVGraspRflxRelease,
										YBVArmForceRestingTrue, 
										YBVHandNewCmd, YVector(6, __handAtRest));
	RBMotorCommand			handOpen(YBVHandNewCmd, YVector(6, __handOpen));
	RBMotorCommand			handClose(YBVHandNewCmd, YVector(6, __handClose));
	RBInputCommand			armRest(YBVArmRest);
	RBInputCommand			armIsBusy(YBVArmIsBusy);
	RBOutputMessage			checkReachingOutput(YBVReachingDone);
	RBOutputMessage			inhibitGraspReflex(YBVGraspRflxInhibit);
	RBOutputMessage			enableGraspReflex(YBVGraspRflxRelease);
	
	RBOutputMessage			reachingSuccess(YBVReachingSuccess);
	RBOutputMessage			reachingFailed(YBVReachingFailure);
	RBOutputMessage			enableSaccade(YBVSinkSaccadeMode);
	RBOutputMessage			armForceRest(YBVArmForceRestingTrue);
	
	RBInputCommand			reachingPrepareInput(YBVReachingPrepare);
	RBInputCommand			reachingInhibit(YBVReachingInhibit);
	RBInputCommand			reachingEnable(YBVReachingEnable);
	RBInputString			graspSuccess("HandKinGraspSuccess");
	RBInputString			graspFailure("HandKinGraspFailure");

	_behavior.setInitialState(&waitIdle);
	_behavior.add(&learnInput, &learnOutput);
	// _behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepare);
	// _behavior.add(NULL, &reachingSeq1, &reachingSeq2, &inhibitHead);
	// _behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &handOpen);
	_behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &inhibitGraspReflex);
	_behavior.add(NULL, &reachingSeq1, &checkACK, &reachingPrepareOutput);
	_behavior.add(&reachingAbort, &checkACK, &waitIdle, &enableGraspReflex);
	
	_behavior.add(&reachingAck, &checkACK, &reachingSeq1b);
	
	_behavior.add(&reachingInhibit, &waitIdle, &inhibitedState);
	_behavior.add(&reachingEnable, &inhibitedState, &waitIdle);

	_behavior.add(&armDone, &reachingSeq1b, &reachingSeq2);
//	_behavior.add(&armIsBusy, &reachingSeq1b, &waitHandDone);
//	_behavior.add(&armRest, &reachingSeq1b, &waitHandDone);
	
	_behavior.add(NULL, &reachingSeq2, &reachingSeq2b, &inhibitAll);
	_behavior.add(NULL, &reachingSeq2b, &reachingSeq31, &reachingOutput1);
	_behavior.add(&armDone, &reachingSeq31, &reachingSeq32a, &reachingOutputCL);
	_behavior.add(&armIsBusy, &reachingSeq31, &waitIdle, &enableAll);

	_behavior.add(NULL, &reachingSeq32a, &reachingSeq32, &reachingOutput2);
		
#ifndef TEST_REACHING
	_behavior.add(&armDone, &reachingSeq32, &waitDeltaT1, &handClose);
#else
	_behavior.add(&armDone, &reachingSeq32, &waitDeltaT1);
#endif

	_behavior.add(&armIsBusy, &reachingSeq32, &waitIdle, &enableAll);
	// wait
	
#ifndef TEST_REACHING
	_behavior.add(&handDone, &waitDeltaT1, &reachingSeq33, &reachingOutput3);
	_behavior.add(&armDone, &reachingSeq33, &checkReaching, &checkReachingOutput);
	_behavior.add(&armIsBusy, &reachingSeq33, &waitIdle, &enableAll);
	_behavior.add(&armRest, &waitDeltaT1, &waitIdle, &enableAll);
#else 
	_behavior.add(NULL, &waitDeltaT1, &reachingSeq33, &reachingOutput3);
	_behavior.add(NULL, &reachingSeq33, &checkReaching, &checkReachingOutput);
#endif

	// add here instead of NULL message back from SOM
#ifndef TEST_REACHING
	_behavior.add(&graspSuccess, &checkReaching, &signalReachSuccess, &reachingDropOut);
	_behavior.add(&graspFailure, &checkReaching, &signalReachFailure, &reachingBack);
#else
	_behavior.add(NULL, &checkReaching, &reachingSeq5, &reachingBack);
#endif

	_behavior.add(NULL, &signalReachFailure, &reachingSeq5, &reachingFailed);
	_behavior.add(NULL, &signalReachSuccess, &reachingDropState1, &reachingSuccess);
	_behavior.add(NULL, &reachingDropState1, &reachingDropState2, &enableHandTracking);
		
	// AUTO
	// _behavior.add(NULL, &waitDeltaT1, &reachingSeq33, &reachingOutput3); // NO HAND
	//_behavior.add(NULL, &waitDeltaT1, &reachingSeq33);
	// _behavior.add(&armDone, &reachingSeq33, &reachingSeq5, &reachingBack);
	
	

	//_behavior.add(&armIsBusy, &reachingSeq3, &waitIdle, &enableHead);
	//_behavior.add(&armRest, &reachingSeq3, &waitIdle, &enableHead);
	// _behavior.add(NULL, &reachingSeq4, &reachingSeq5, &reachingBack);
#ifndef TEST_REACHING
	_behavior.add(&armDone, &reachingSeq5, &reachingSeq6, &handOpen);
	_behavior.add(&handDone, &reachingSeq6, &waitIdle, &enableAll);
	_behavior.add(&handDone, &waitHandDone, &waitIdle, &enableAll);

	_behavior.add(&armDone, &reachingDropState2, &reachingDropState3, &handOpen);
	_behavior.add(&handDone, &reachingDropState3, &finalDropState1, &armForceRest);
	_behavior.add(NULL, &finalDropState1, &finalDropState2, &enableSaccade);
	_behavior.add(NULL, &finalDropState2, &waitIdle, &enableAll);
	_behavior.add(&armIsBusy, &reachingSeq5, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq31, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq32, &waitIdle, &enableAll);
	_behavior.add(&armRest, &reachingSeq33, &waitIdle, &enableAll);
#else
	_behavior.add(&armDone, &reachingSeq5, &reachingSeq6);
	_behavior.add(NULL, &reachingSeq6, &waitIdle, &enableAll);
#endif

		
	_behavior.Begin();
	_behavior.loop();

	return 0;
}