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
	RBWaitIdle waitIdle;
	RBWaitDeltaT reachingSeq1(0.5);
	RBWaitIdle reachingSeq2;
	RBWaitIdle reachingSeq2b;
	RBWaitIdle reachingSeq3;
	RBWaitDeltaT reachingSeq4(3);
	RBWaitIdle reachingSeq5;

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

	RBInputCommand			reachingPrepareInput(YBVReachingPrepare);

	_behavior.setInitialState(&waitIdle);
	_behavior.add(&learnInput, &learnOutput);
	// _behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepare);
	// _behavior.add(NULL, &reachingSeq1, &reachingSeq2, &inhibitHead);
	_behavior.add(&reachingInput, &waitIdle, &reachingSeq1, &reachingPrepareOutput);
	_behavior.add(&armDone, &reachingSeq1, &reachingSeq2);
	_behavior.add(&armRest, &reachingSeq1, &waitIdle, &enableHead);

	_behavior.add(NULL, &reachingSeq2, &reachingSeq2b, &inhibitHead);
	_behavior.add(NULL, &reachingSeq2b, &reachingSeq3, &reachingOutput);
	_behavior.add(&armDone, &reachingSeq3, &reachingSeq4);
	_behavior.add(&armRest, &reachingSeq3, &waitIdle, &enableHead);
	_behavior.add(NULL, &reachingSeq4, &reachingSeq5, &reachingBack);
	_behavior.add(&armDone, &reachingSeq5, &waitIdle, &enableHead);
	_behavior.add(&armRest, &reachingSeq5, &waitIdle, &enableHead);

	_behavior.Begin();
	_behavior.loop();

	return 0;
}