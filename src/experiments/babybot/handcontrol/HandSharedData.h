#ifndef __HAND_SHARED_DATA
#define __HAND_SHARED_DATA

#include <YARPBabybothand.h>
#include <YARPBehavior.h>

class HandSharedData: public YARPBabybotHand, public YARPBehaviorSharedData
{
public:
	HandSharedData():YARPBehaviorSharedData(YBLabelMotor, "/handcontrol/behavior/o"){};
};


#endif