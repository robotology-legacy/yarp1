// collectpoints.cpp : Defines the entry point for the console application.
//

#include "triggerBehavior.h"

int main(int argc, char* argv[])
{
	TBSharedData _data;
	
	TBWaitRead	waitTarget("HandCentered");
	TBWaitRead	waitArm("ArmNewCmd");
	TBWaitRead	waitArmRest("ArmRest");
	TBWaitRead	waitArmDone("ArmDone");
		
	TBSimpleInput	checkArmDone(YBVArmDone);
	TBSimpleInput	checkArmRest(YBVArmRest);
	TBSimpleInput	checkArmRestDone(YBVArmRestDone);
	TBSimpleInput	checkArmNewCmd(YBVArmIssuedCmd);
	TBSimpleInput	forceDump(YBVCollectPoints);

	TBIsTargetCentered checkTarget;
	TBOutput output("");

	TriggerBehavior _behavior(&_data);

/*	_behavior.setInitialState(&waitTarget);
	_behavior.add(&forceDump, &waitTarget, &waitTarget, &output);
	_behavior.add(NULL, &waitTarget, &waitTarget);
*/

	_behavior.setInitialState(&waitArm);
	_behavior.add(&checkArmRest, &waitArm, &waitArmRest);
	_behavior.add(&checkArmNewCmd, &waitArm, &waitArmDone);
	_behavior.add(&checkArmDone, &waitArmDone, &waitTarget);

	_behavior.add(&checkArmRest, &waitTarget, &waitArmRest);
	_behavior.add(&checkTarget, &waitTarget, &waitArm, &output);
	_behavior.add(NULL, &waitTarget, &waitTarget);

	_behavior.add(&checkArmRestDone, &waitArmRest, &waitArm);
	_behavior.Begin();
	_behavior.loop();

	return 0;
}

