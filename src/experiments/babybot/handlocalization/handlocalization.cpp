//

#include <conf/YARPconfig.h>
#include <YARPParseParameters.h>
#include "findHand.h"

const unsigned char __defaultThreshold = 20;

int main(int argc, char *argv[])
{
	FindHand _findHand;

	int thr;
	if (!YARPParseParameters::parse(argc, argv, "t", &thr))
	{
		thr = __defaultThreshold;
	}
	_findHand._threshold = (unsigned char) thr;

	while(true)
	{
		_findHand.Body();
	}
	return 0;
}
