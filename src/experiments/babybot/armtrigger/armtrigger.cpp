// armtrigger.cpp : Defines the entry point for the console application.
//

#include "triggerBehavior.h"

int main(int argc, char* argv[])
{
	TBSharedData _data;
	
	TBWaitRead	waitTarget("Target");
	TBWaitRead	waitHand("Hand");
	TBTransition trLookTarget("LookTarget");
	TBTransition trLookHand("LookHand");
	TBTransition trArmRest("ArmRest");
	TBTransition waitReaching("WaitReaching");
	TBTransition waitArmRest("WaitArmRest");
	TBIsTargetCentered checkTarget;
	TBIsHandCentered checkHand;
	TBOutputCommand lookHand(YBVAttentionLookHand);
	TBOutputCommand lookTarget(YBVAttentionLookTarget);
	TBOutputCommand reach(YBVReachingReach);
	TBOutputCommand learn(YBVReachingLearn);
	TBSimpleInput	checkArmDone(YBVArmDone);
	TBSimpleInput	checkArmRest(YBVArmRest);
	TBSimpleInput	checkArmRestDone(YBVArmRestDone);
	TriggerBehavior _behavior(&_data);

	TBOutputCommand armRest(YBVArmForceResting);

	_behavior.setInitialState(&waitTarget);
	_behavior.add(&checkTarget, &waitTarget, &waitReaching, &reach);
	_behavior.add(&checkArmRest, &waitTarget, &waitArmRest, &lookTarget);
	_behavior.add(NULL, &waitTarget, &waitTarget);	// loop here
	_behavior.add(&checkArmDone, &waitReaching, &trLookHand);
	_behavior.add(&checkArmRest, &waitReaching, &waitArmRest, &lookTarget);

	_behavior.add(&checkArmRest, &trLookHand, &waitArmRest, &lookTarget);
	_behavior.add(NULL, &trLookHand, &waitHand, &lookHand);
	// _behavior.add(&checkHand, &waitHand, &trArmRest, &learn);
	_behavior.add(&checkHand, &waitHand, &trLookTarget, &learn);
	_behavior.add(&checkArmRest, &waitHand, &waitArmRest, &lookHand);
	_behavior.add(NULL, &waitHand, &waitHand);	// loop here
	
	_behavior.add(&checkArmRest, &trLookTarget, &waitArmRest, &lookTarget);
	_behavior.add(NULL, &trLookTarget, &waitTarget, &lookTarget);
	
	// _behavior.add(NULL, &trArmRest, &trLookTarget, &armRest);

	_behavior.add(&checkArmRestDone, &waitArmRest, &trLookTarget);

	_behavior.Begin();
	_behavior.loop();

	return 0;
}

