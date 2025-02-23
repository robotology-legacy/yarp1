// ArmThred.h: interface for the ArmThread class.
//
//
// Arm control thread.
// handle connection to the mei board (com object) and dispatch arm
// status
//////////////////////////////////////////////////////////////////////

#ifndef __armthreadh__
#define __armthreadh__

#include <yarp/YARPConfig.h>

#include <yarp/YARPRobotMath.h>	// useful stuff, like degToRad, pi and radToDeg include also VisDMatrix.h
#include <yarp/YARPRateThread.h>

///#include "debug.h"

#include <yarp/YARPRobotHardware.h>

typedef YARPArm YARPArmClass;

#include <yarp/YARPTrajectoryGen.h>
#include "YARPGravityEstimator.h"
#include <yarp/YARPPort.h>
#include <yarp/YARPVectorPortContent.h>
#include "AState.h"

#include "tirednessControl.h"

#include "armstatus.h"
///#include "YARPNameClient.h"

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

		// printf("--> Motion: trajectory %d\t arm %d\n",  tr_done ? 1:0, pid_done ? 1:0 );

		return pid_done && tr_done;
	}

	void shake(const YVector &delta)
	{
		// start shake sequence
		_shaking = true;
		ASShake *tmp = ASShake ::instance();
		tmp->n = 16;
		YVector pos;
		YVector first;
		YVector second;

		pos.Resize(6);
		first.Resize(6);
		second.Resize(6);
		// first = 0.0;
		second = 0.0;

		first = delta; 
		_trajectory.getCurrent(pos.data());
		tmp->setActualPosition(pos);
		tmp->setTrajectory(first, second);
		changeInitState(tmp);
	}

	// set a new position in the ASDirectCommand state... the command will be
	// executed only if the thread goes or is in this state.
	void directCommand(const YVector &cmd, bool force = false)
	{
		ASDirectCommand *tmp = ASDirectCommand::instance();
		if (force) 
		{
			ARM_THREAD_DEBUG(("Forcing a new direct command\n"));
			tmp->newCmdFlag = FORCED;
		}
		else
		{
			ARM_THREAD_DEBUG(("Asking a new direct command\n"));
			tmp->newCmdFlag = NEW;
		}
		tmp->cmd = cmd;
		changeInitState(tmp);
	}

	void directCommandMode()
	{
		ASDirectCommand *tmp = ASDirectCommand::instance();
		
		if (tmp->newCmdFlag == NEW)
			tmp->newCmdFlag = NONE; // goes to directCommand, but wait for a new cmd

		changeInitState(tmp);
	}

	// zero g mode
	void zeroGMode()
	{ changeInitState(ASZeroGInit::instance()); }

	void forceResting(bool f)
	{ 
		_tirednessControl.inhibit(false);

		if (f)
			ARM_THREAD_DEBUG(("Forced to rest\n"));
		else
			ARM_THREAD_DEBUG(("Free to move again\n"));
		_tirednessControl.forceResting(f);
	}

	void forceResting()
	{ 
		_tirednessControl.inhibit(false);
		if (_tirednessControl.forced() )
		{
			ARM_THREAD_DEBUG(("Free to move again\n"));
			_tirednessControl.forceResting(false);
		}
		else
		{
			ARM_THREAD_DEBUG(("Forced to rest\n"));
			_tirednessControl.forceResting(true);
		}
	}

	//force a particular value of inhibition
	void inhibitResting(bool f)
	{
		_tirednessControl.forceResting(false);
		if (f)
			ARM_THREAD_DEBUG(("Resting inhibited\n"));
		else
			ARM_THREAD_DEBUG(("Resting allowed\n"));
		_tirednessControl.inhibit(f);
	}

	//anable/disable inhibition
	void inhibitResting()
	{
		_tirednessControl.forceResting(false);
		if (_tirednessControl.inhibited())
		{
			ARM_THREAD_DEBUG(("Resting allowed\n"));
			_tirednessControl.inhibit(false);
		}
		else
		{
			ARM_THREAD_DEBUG(("Resting inhibited\n"));
			_tirednessControl.inhibit(true);
		}
	}

	void park(int index);	// park arm
	void resetEncoders(const double *pos);
	int setStiffness(int joint, double k);
	void printPids();

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

	void _initLocalPIDs(const LowLevelPID *newPids)
	{
		for(int i = 0; i < _nj; i++)
		{
			_pids[i] = newPids[i];
			if (_pids[i].KP > 0)
				_pidSigns[i] = 1;
			else if (_pids[i].KP < 0)
				_pidSigns[i] = -1;
			else
				_pidSigns[i] = 0;
		}
	}

	bool _decMaxTorques(double delta, double value, int nj);
	bool _incMaxTorques(double delta, double value, int nj);

	// this is a private function, it can be called only from within the thread
	void _directCommand(const YVector &cmd, int nst = 0);
	inline void send_commands();
	inline void read_status();

	bool _checkLimits(const YVector &inCmd, YVector &outCmd);

	AState *_arm_state;
	AState *_init_state;

	YVector _actual_command;			// actual arm command, joint space
	YVector _wristF;
	YARPInputPortOf<YVector> _wristPort;

	////////
	YARPOutputPortOf<YARPControlBoardNetworkData> _armStatusPort;
	YARPOutputPortOf<YVector> _armPositionPort;
	YARPOutputPortOf<YVector> _torquesPort;
	
	bool _restingInhibited;

	YARPTrajectoryGen _trajectory;
	TirednessControl _tirednessControl;

	bool _shaking;

	double _rate;

	int _nj;
	int _nSteps;
	
	char _iniFile[80];						// config file
	char _path[255];

public: //later: make it private
	YARPControlBoardNetworkData _arm_status;
	YVector _old_arm_pos;
	
	YARPArmClass _arm;

	J1GravityEstimator _gravity1;
	J2GravityEstimator _gravity2;
	J3GravityEstimator _gravity3;
	J5GravityEstimator _gravity5;
	int *_gravityFlags;
	LowLevelPID *_pids;
	int *_pidSigns;
	
	YVector _cmd;	//move it from here !
	YVector _speed;
	YVector _parkSpeed;
	YVector _acc;
	YVector _limitsMax;
	YVector _limitsMin;
	YVector _gravityTerms;

	YVector _shakeCmd; //this is used only by AB

	double	_newGain;
	int		_newGainJoint;

	// FORCE FEEDBACK
	bool forceFeedback;

};

#endif //.h