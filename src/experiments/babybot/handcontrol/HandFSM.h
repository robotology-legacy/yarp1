#ifndef __HANDFSMCLASS__
#define __HANDFSMCLASS__

#include <yarp/YARPFSM.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPBabybotHand.h>

#include "HandSharedData.h"

class HandFSM: public YARPFSM<HandFSM, HandSharedData>
{
public:
	HandFSM(HandSharedData *t): YARPFSM<HandFSM, HandSharedData>(t)
	{}
};

typedef YARPFSMStateBase<HandFSM, HandSharedData> HandFSMStates;
typedef YARPFSMInput<HandSharedData> HandFSMInputs;
typedef YARPFSMOutput<HandSharedData> HandFSMOutputs;

#endif

