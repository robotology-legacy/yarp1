// ArmThred.h: interface for the ArmThread class.
//
//
// Arm control thread.
// handle connection to the mei board (com object) and dispatch arm
// status
//////////////////////////////////////////////////////////////////////

#ifndef __armthreadh__
#define __armthreadh__

#include "conf/YARPConfig.h"

#include <YARPRobotMath.h>	// useful stuff, like degToRad, pi and radToDeg include also VisDMatrix.h
#include <YARPRateThread.h>

#include "debug.h"
#include "YARPBabybotArm.h"
#include "YARPTrajectoryGen.h"
#include "YARPGravityEstimator.h"
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include "AState.h"

#include "tirednessControl.h"

#include "ArmStatus.h"
#include "YARPNameClient.h"

#define ARM_THREAD_VERBOSE

#ifdef ARM_THREAD_VERBOSE
#define ARM_THREAD_DEBUG(string) YARP_DEBUG("ARM_THREAD_DEBUG:", string)
#else  ARM_THREAD_DEBUG(string) YARP_NULL_DEBUG
#endif

#include "ArmBehavior.h"

class ArmThread: public YARPRateThread, public YARPBehaviorSharedData
{
public:
	ArmThread(int rate,
			  const char *name = "arm control thread",
			  const char *ini_file = NULL);
	virtual ~ArmThread();

	void doLoop();
	void doInit();
	void doRelease();

	bool checkMotionDone()
	{
		bool tr_done = !_trajectory.checkBusy();
		bool pid_done = _arm.checkMotionDone();

		return pid_done && tr_done;
	}

	void shake()
	{
		// start shake sequence
		_shaking = true;
		ASShake *tmp = ASShake ::instance();
		tmp->n = 16;
		YVector pos;
		pos.Resize(6);
		_trajectory.getCurrent(pos.data());
		tmp->setActualPosition(pos);
		// tmp->setActualPosition(_arm_status._current_position);
		changeInitState(tmp);
	}

	// set a new position in the ASDirectCommand state... the command will be
	// executed only if the thread goes or is in this state.
	void directCommand(const YVector &cmd)
	{
		ASDirectCommand *tmp = ASDirectCommand::instance();
		changeInitState(tmp);
		tmp->newCmdFlag = true;
		tmp->cmd = cmd;
	}

	void directCommandMode()
	{
		ASDirectCommand *tmp = ASDirectCommand::instance();
		tmp->newCmdFlag = false; // goes to directCommand, but wait for a new cmd
		changeInitState(tmp);
	}

	// zero g mode
	void zeroGMode()
	{
		changeInitState(ASZeroGInit::instance());
	}

	void forceResting(bool flag)
	{
		_tirednessControl.forceResting(flag);
	}

	void park(int index);	// park arm

private:
	friend class	AState;
	friend class	ASRestingInit;
	friend class	ASDirectCommand;
	friend class	ASWaitForMotion;
	friend class	ASRestingWaitIdle;
	friend class	ASRestingLowerGains;
	friend class	ASRestingRaiseGains;
	friend class	ASDirectCommandMove;
	friend class	ASZeroGInit;
	friend class	ASZeroGWait;
	friend class	ASZeroGEnd;
	friend class	ASMove;
	friend class	ASWaitForHand;
	friend class	ASShake;

	// change reaching state
	void changeReachingState(AState *s)
		{_arm_state = s;}

	// change reaching first state
	void changeInitState(AState *s)
		{_init_state = s;}

	// this is a private function, it can be called only from within the thread
	void _directCommand(const YVector &cmd, int nst = 0)
	{
		_trajectory.stop();
		if (nst == 0)
			_trajectory.setFinal(cmd.data(), _nSteps);	// use default
		else 
			_trajectory.setFinal(cmd.data(), nst);
		
		_arm.setPositions(cmd.data());
	}

	inline void send_commands();
	inline void read_status();

	AState *_arm_state;
	AState *_init_state;

	YVector _actual_command;			// actual arm command, joint space
	YVector _wristF;
	YARPInputPortOf<YVector> _wristPort;

	////////
	YARPOutputPortOf<YVector> _armStatusPort;
	
	bool _restingInhibited;

	YARPTrajectoryGen _trajectory;
	TirednessControl _tirednessControl;

	bool _shaking;

	int _nj;
	int _nSteps;
	
	char _iniFile[80];						// config file

public: //later: make it private
	ArmStatus _arm_status;
	YARPBabybotArm _arm;
	J1GravityEstimator _gravity1;
	J2GravityEstimator _gravity2;
	J3GravityEstimator _gravity3;
	J5GravityEstimator _gravity5;

	YVector _cmd;	//move it from here !
	YVector _speed;
	YVector _acc;

};

#endif //.h