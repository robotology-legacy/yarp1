//

#include <conf/YARPconfig.h>
#include <YARPParseParameters.h>
#include "HLBehavior.h"
#include <YARPBottle.h>
#include "findHand.h"

const unsigned char __defaultThreshold = 20;
double __defaultAlpha = 0.3;

int main(int argc, char *argv[])
{
	FindHand _findHand;

	int thr;
	if (!YARPParseParameters::parse(argc, argv, "t", &thr))
	{
		thr = __defaultThreshold;
	}
	
	double alpha;
	if (!YARPParseParameters::parse(argc, argv, "a", &alpha))
	{
		alpha = __defaultAlpha;
	}

	_findHand._threshold = (unsigned char) thr;
	_findHand._alpha = alpha;
	
	HLBehavior _hlbehavior(&_findHand, YBLabelMotor, "/handlocalization/behavior/i");
	HLDoMotion doMotion;
	HLFind find;
	HLShakeStart start;
	HLShakeStop stop;
	_hlbehavior.setInitialState(&doMotion);
	_hlbehavior.add(&start, &doMotion, &find);
	_hlbehavior.add(&stop, &find, &doMotion);

	_findHand.Begin();
	_hlbehavior.Begin();
	_hlbehavior.loop();
	_findHand.End(-1);

	return 0;
}
