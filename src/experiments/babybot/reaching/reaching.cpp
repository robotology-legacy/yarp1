// reaching.cpp : Defines the entry point for the console application.
//

#include <YARPPort.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPMatrix.h>
#include <YARPVectorPortContent.h>
#include <YARPControlBoardNetworkData.h>
#include <./conf/YARPVocab.h>
#include "armmap.h"
#include "reachingBehavior.h"

int _headFrames = 0;
int _armFrames = 0;
int _nFrames = 0;

int main(int argc, char* argv[])
{
	YARPBottle _outputTrainBottle;
	
	YVector _arm;
	YVector _head;
	ABSharedData _data;
		
	ReachingBehavior _behavior(&_data);
	RBWaitIdle waitIdle("Waiting for reaching signal");
	RBWaitDeltaT reachingSeq1("Waiting for arm done (prepare reaching)", 0.5);
	RBWaitIdle reachingSeq2("AS state, inhibit head");
	RBWaitIdle reachingSeq2b("AS state issue reaching command");
	RBWaitIdle reachingSeq3("Waiting for arm done (actually reaching)");
	RBWaitDeltaT reachingSeq4("AS state, issue withdraw arm", 3);
	RBWaitIdle reachingSeq5("Waiting for arm done (withdrawing arm)");

	RBOutputCommand			reachingPrepareOutput;
	RBOutputReaching		reachingOutput;
	RBOutputBack			reachingBack;
	RBLearnOutputCommand	learnOutput;
	RBInputCommand			learnInput(YBVReachingLearn);
	RBInputCommand			armDone(YBVArmDone);
	RBInputCommand			reachingInput(YBVReachingReach);
	RBSimpleOutput			inhibitHead(YBVSinkSuppress);
	RBSimpleOutput			enableHead(YBVSinkRelease);
	RBInputCommand			armRest(YBVArmRest);
	RBInputCommand			armIsBusy(YBVArmIsBusy);

	RBInputCommand			reachingPrepareInput(YBVReachingPrepare);

	_behavior.setInitialState(&waitIdle);
	_behavior.add(&learnInput, &learnOutput);
	// _behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepare);
	// _behavior.add(NULL, &reachingSeq1, &reachingSeq2, &inhibitHead);
	_behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepareOutput);
	_behavior.add(&armDone, &reachingSeq1, &reachingSeq2);
	_behavior.add(&armIsBusy, &reachingSeq1, &waitIdle, &enableHead);
	_behavior.add(&armRest, &reachingSeq1, &waitIdle, &enableHead);

	_behavior.add(NULL, &reachingSeq2, &reachingSeq2b, &inhibitHead);
	_behavior.add(NULL, &reachingSeq2b, &reachingSeq3, &reachingOutput);
	_behavior.add(&armDone, &reachingSeq3, &reachingSeq4);
	_behavior.add(&armIsBusy, &reachingSeq3, &waitIdle, &enableHead);
	_behavior.add(&armRest, &reachingSeq3, &waitIdle, &enableHead);
	_behavior.add(NULL, &reachingSeq4, &reachingSeq5, &reachingBack);
	_behavior.add(&armDone, &reachingSeq5, &waitIdle, &enableHead);
	_behavior.add(&armIsBusy, &reachingSeq5, &waitIdle, &enableHead);
	_behavior.add(&armRest, &reachingSeq3, &waitIdle, &enableHead);

	_behavior.Begin();
	_behavior.loop();

	return 0;
}