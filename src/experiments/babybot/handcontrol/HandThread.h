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
#include <YARPBehavior.h>

#include <YARPVectorPortContent.h>
#include <YARPPort.h>

#include <YARPBabybotHand.h>

#define HAND_THREAD_VERBOSE

#ifdef HAND_THREAD_VERBOSE
#define HAND_THREAD_DEBUG(string) YARP_DEBUG("HAND_THREAD_DEBUG:", string)
#else  HAND_THREAD_DEBUG(string) YARP_NULL_DEBUG
#endif

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

	void singleMove(const YVector &pos, int time);
	void shake();
	
	HandSharedData _hand;

	ACE_High_Res_Timer	timer;
	ACE_Time_Value		period;

	std::string _cfgFile;

	HandFSM *_fsm;

	// fsm states
	IdleState _waitState;
	
	// shake
	InitMoveState *_shakeMove;
	IdleState *_shakeWait;
	EndMotion _endMotion;
	
	// inputs
	WaitNSteps _wait1;
	Pulse _startShake;
	
	// single movement
	InitMoveState _move1State;
	IdleState _move2State;
	WaitNSteps _waitMove;
	Pulse _startMove;
	EndMotion _endMotionState;

	//
	YVector _cmd;
	YARPOutputPortOf<YVector> _handStatusOut;
	YVector _status;
};

#endif // .h