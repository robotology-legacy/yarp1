// ArmStatus class
// this class stores the status of the arm and should be used for transmission.
// 
//  
// June 2002 -- by nat
//

#ifndef __armstatush__
#define __armstatush__

// #include "NetworkData.h"
#include <YARPControlBoardNetworkData.h>
#include <YARPRobotMath.h>
// #include "RobotKinematics.h"
#include "YARPConfigFile.h"
#include <assert.h>

#define wrist3		6
#define wrist2		5
#define wrist1		4
#define elbow		3
#define shoulder	2
#define waist		1

namespace _armThread
{
	enum  __state {
			directCommand = 0,
			resting = 1,
			restingInit = 2,
			restingLowerGains = 3,
			restingRaiseGains = 4,
			restingWaitIdle = 5,
			waitForMotion = 6,
			move = 7,
			directCommandMove = 8,
			zeroGInit = 9,
			zeroGEnd = 10,
			waitForHand = 11
	};
};

#endif //.h