#ifndef __ARMMAP__
#define __ARMMAP__

#include <YARPBPNNet.h>

const int __nJointsArm = 6;

class ArmMap
{
public:
	ArmMap(const char *nnetFile);
	~ArmMap();

	void loadCenter(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading reaching map from memory:");
		_nnet.load(p);
	}

	const YVector &query(const YVector &arm, const YVector &head)
	{

		// query nnet
		return _arm;
	}

private:
	bool _checkLearnCondition();
	bool _checkReachingCondition();

	YARPBPNNet _nnet;
	YVector _arm;
};

#endif