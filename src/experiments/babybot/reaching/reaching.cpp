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
	RBOutputCommand			reachingOutput;
	RBLearnOutputCommand	learnOutput;
	RBInputCommand			learnInput(YBVReachingLearn);
	RBInputCommand			reachingInput(YBVReachingReach);

	_behavior.setInitialState(&waitIdle);
	_behavior.add(&learnInput, &learnOutput);
	_behavior.add(&reachingInput, &reachingOutput);

	_behavior.Begin();
	_behavior.loop();

	return 0;
}