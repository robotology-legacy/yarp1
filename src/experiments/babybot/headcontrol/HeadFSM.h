

#ifndef __HEADFSMCLASS__
#define __HEADFSMCLASS__

#include <yarp/YARPFSM.h>
#include <yarp/YARPRobotMath.h>



#if defined(__QNXEurobot__)

	#include <yarp/YARPEurobotHead.h>

#else

	#include <yarp/YARPBabybotHead.h>

#endif




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