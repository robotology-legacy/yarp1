#ifndef __HEADFSMCLASS__
#define __HEADFSMCLASS__

#include <YARPFSM.h>
#include <YARPRobotMath.h>
#include <YARPBabybotHead.h>

#include "HeadSharedData.h"

class HeadFSM: public YARPFSM<HeadFSM, HeadSharedData>
{
public:
	HeadFSM(HeadSharedData *t): YARPFSM<HeadFSM, HeadSharedData>(t)
	{}
};

typedef YARPFSMStateBase<HeadFSM, HeadSharedData> HeadFSMStates;
typedef YARPFSMInput<HeadSharedData> HeadFSMInputs;
typedef YARPFSMOutput<HeadSharedData> HeadFSMOutputs;

#endif