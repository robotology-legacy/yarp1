// handtrackerswitch.cpp : Defines the entry point for the console application.
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

	AttBWait lookPredState("Prediction");
	AttBWait lookCurrState("Current");
	
	AttBWait autolookCurrState("Auto current");
	AttBWait autolookPredState("Auto prediction");
	
	AttBSimpleInput lookPredInput(YBVHTSPrediction);
	AttBSimpleInput lookCurrInput(YBVHTSCurrent);
	AttBSimpleInput lookAutoInput(YBVHTSAuto);
	AttBSimpleInput armDone(YBVArmDone);
	AttBSimpleInput armNewCmd(YBVArmIssuedCmd);

	AttBSimpleOutput attLookPred(YBVAttentionLookPrediction);
	AttBSimpleOutput attLookCurr(YBVAttentionLookHand);
	
	_behavior.setInitialState(&lookCurrState);

	_behavior.add(&lookCurrInput, &lookCurrState, &lookCurrState);
	_behavior.add(&lookPredInput, &lookCurrState, &lookPredState, &attLookPred);
	_behavior.add(&lookAutoInput, &lookCurrState, &autolookPredState, &attLookPred);
			
	_behavior.add(&lookPredInput, &lookPredState, &lookPredState);
	_behavior.add(&lookCurrInput, &lookPredState, &lookCurrState, &attLookCurr);
	_behavior.add(&lookAutoInput, &lookPredState, &autolookPredState, &attLookPred);
	
	_behavior.add(&armDone, &autolookPredState, &autolookCurrState, &attLookCurr);
	_behavior.add(&lookPredInput, &autolookPredState, &lookPredState, &attLookPred);
	_behavior.add(&lookCurrInput, &autolookPredState, &lookCurrState, &attLookCurr);
	
	_behavior.add(&armNewCmd, &autolookCurrState, &autolookPredState, &attLookPred);
	_behavior.add(&lookPredInput, &autolookCurrState, &lookPredState, &attLookPred);
	_behavior.add(&lookCurrInput, &autolookCurrState, &lookCurrState, &attLookCurr);

	// start behavior
	_behavior.Begin();
	_behavior.loop();

	return 0;
}
