// attention.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPScheduler.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>

#include "attbehavior.h"

int main(int argc, char* argv[])
{
	AttSharedData _data;
	AttBehavior _behavior(&_data);

	AttBHand hand;
	AttBTarget target;
	AttBEgoMap egoMap;
	AttBHandPrediction prediction;
	AttBSimpleInput lookHand(YBVAttentionLookHand);
	AttBSimpleInput lookTarget(YBVAttentionLookTarget);
	AttBSimpleInput lookHPrediction(YBVAttentionLookPrediction);
	AttBSimpleInput lookEgoMap(YBVAttentionLookEgoMap);
	
	_behavior.setInitialState(&target);
	_behavior.add(&lookHand, &target, &hand);
	_behavior.add(&lookHPrediction, &target, &prediction);
	_behavior.add(&lookEgoMap, &target, &egoMap);
	_behavior.add(NULL, &target, &target);

	_behavior.add(&lookTarget, &hand, &target);
	_behavior.add(&lookHPrediction, &hand, &prediction);
	_behavior.add(&lookEgoMap, &hand, &egoMap);
	_behavior.add(NULL, &hand, &hand);

	_behavior.add(&lookHand, &prediction, &hand);
	_behavior.add(&lookTarget, &prediction, &target);
	_behavior.add(&lookEgoMap, &prediction, &egoMap);
	_behavior.add(NULL, &prediction, &prediction);

	_behavior.add(&lookHand, &egoMap, &hand);
	_behavior.add(&lookTarget, &egoMap, &target);
	_behavior.add(&lookHPrediction, &egoMap, &prediction);
	_behavior.add(NULL, &egoMap, &egoMap);

	// start behavior
	_behavior.Begin();
	_behavior.loop();

	return 0;
}
