// Hand thread
//
// Feb 2003 -- by nat
// Adapted to YARP, July 2003 -- by nat

#ifndef __HAND_THREAD__
#define __HAND_THREAD__

#include <conf/YarpConfig.h>

#include <YARPRateThread.h>
#include "HandFSM.h"
#include "HandFSMStates.h"

#include <YARPPort.h>

#include <YARPBabybotHand.h>

class HandThread : public YARPRateThread
{
public:
	HandThread(int rate, 
			   const char *name,
			   const char *cfcFile);
	virtual ~HandThread();

	void doInit();
	void doLoop();
	void doRelease();
	
	HandSharedData _hand;

	ACE_High_Res_Timer	timer;
	ACE_Time_Value		period;

	std::string _cfgFile;

	HandFSM *_fsm;

	// fsm states
	IdleState _waitState;
	InitMoveState _shake1;
	IdleState _shake2;
	InitMoveState _shake3;
	IdleState _shake4;
	EndMotion _endMotion;
	WaitNSteps _wait1;
	Pulse _startShake;
};

#endif // .h