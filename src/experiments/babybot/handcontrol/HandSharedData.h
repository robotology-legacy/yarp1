#ifndef __HAND_SHARED_DATA
#define __HAND_SHARED_DATA

#include <YARPBabybothand.h>
#include <YARPBehavior.h>
#include <./conf/YARPMotorVocab.h>

class HandSharedData: public YARPBabybotHand, public YARPBehaviorSharedData
{
public:
	HandSharedData():YARPBehaviorSharedData("/handcontrol/behavior/o", YBVMotorLabel){};

	bool _shaking;
};


#endif