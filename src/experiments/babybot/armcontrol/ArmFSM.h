#ifndef __ARMFSMCLASS__
#define __ARMFSMCLASS__

#include <YARPFSM.h>
#include <YARPRobotMath.h>
#include <YARPBabybotArm.h>

#include "Armthread.h"

typedef ArmThread ArmSharedData;

class ArmFSM: public YARPFSM<ArmFSM, ArmSharedData>
{
public:
	ArmFSM(ArmSharedData *t): YARPFSM<ArmFSM, ArmSharedData>(t)
	{}
};

typedef YARPFSMStateBase<ArmFSM, ArmSharedData> ArmFSMStates;
typedef YARPFSMInput<ArmSharedData> ArmFSMInputs;
typedef YARPFSMOutput<ArmSharedData> ArmFSMOutputs;

#endif