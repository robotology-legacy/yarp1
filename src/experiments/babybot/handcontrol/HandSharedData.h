#ifndef __HAND_SHARED_DATA
#define __HAND_SHARED_DATA

#include <yarp/YARPBabybothand.h>
#include <yarp/YARPBehavior.h>
#include <yarp/YARPConfigRobot.h>

class HandSharedData: public YARPBabybotHand, public YARPBehaviorSharedData
{
public:
	HandSharedData():YARPBehaviorSharedData("/handcontrol/behavior/o", YBVMotorLabel){};

	bool _shaking;
};


#endif