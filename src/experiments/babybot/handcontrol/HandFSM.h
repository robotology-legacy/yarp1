#ifndef __HANDFSMCLASS__
#define __HANDFSMCLASS__

#include <YARPFSM.h>
#include <YARPRobotMath.h>
#include <YARPBabybotHand.h>

#include "HandSharedData.h"

class HandFSM: public YARPFSM<HandFSM, HandSharedData>
{
public:
	HandFSM(HandSharedData *t): YARPFSM<HandFSM, HandSharedData>(t)
	{}
};

typedef YARPFSMStateBase<HandFSM, HandSharedData> HandFSMStates;
typedef YARPFSMInput<HandSharedData> HandFSMInputs;

#endif

