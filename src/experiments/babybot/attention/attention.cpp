// attention.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>

#include "att.h"
#include "attbehavior.h"

int main(int argc, char* argv[])
{
	AttSharedData _data;
	AttBehavior _behavior(&_data);

	AttBHand hand;
	AttBTarget target;
	AttBSimpleInput lookHand(YBVAttentionLookHand);
	AttBSimpleInput lookTarget(YBVAttentionLookTarget);
	
	_behavior.setInitialState(&target);
	_behavior.add(&lookHand, &target, &hand);
	_behavior.add(NULL, &target, &target);

	_behavior.add(&lookTarget, &hand, &target);
	_behavior.add(NULL, &hand, &hand);

	// start behavior
	_behavior.Begin();
	_behavior.loop();

	return 0;
}
