//

#include <yarp/YARPconfig.h>
#include <yarp/YARPParseParameters.h>
#include "HLBehavior.h"
#include <yarp/YARPBabyBottle.h>
#include "findHand.h"

const unsigned char __defaultThreshold = 20;
double __defaultAlpha = 0.3;

int main(int argc, char *argv[])
{
	FindHand _findHand;

	int thr;
	if (!YARPParseParameters::parse(argc, argv, "-t", &thr))
	{
		thr = __defaultThreshold;
	}
	
	double alpha;
	if (!YARPParseParameters::parse(argc, argv, "-a", &alpha))
	{
		alpha = __defaultAlpha;
	}

	_findHand._threshold = (unsigned char) thr;
	_findHand._alpha = alpha;
	
	HLBehavior _hlbehavior(&_findHand);
	HLDoMotion doMotion;
	HLFind find;
	HLShakeStart startFingers(YBVHandShake);
	HLShakeStop stopFingers(YBVHandDone);
	HLShakeStart startWrist(YBVArmShake);
	HLShakeStop stopWrist(YBVArmDone);

	_hlbehavior.setInitialState(&doMotion);
	// fingers (hand)
	_hlbehavior.add(&startFingers, &doMotion, &find);
	_hlbehavior.add(&stopFingers, &find, &doMotion);
	// wrist (arm)
	_hlbehavior.add(&startWrist, &doMotion, &find);
	_hlbehavior.add(&stopWrist, &find, &doMotion);

	_findHand.Begin();
	_hlbehavior.Begin();
	_hlbehavior.loop();

	_findHand.End();
	
	return 0;
}
