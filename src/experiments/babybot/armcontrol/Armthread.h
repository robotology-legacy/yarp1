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
#include "AState.h"

#include "tirednessControl.h"

#include "ArmStatus.h"
#include "YARPNameClient.h"

#define ARM_THREAD_VERBOSE

#ifdef ARM_THREAD_VERBOSE
#define ARM_THREAD_DEBUG(string) YARP_DEBUG("ARM_THREAD_DEBUG:", string)
#else  ARM_THREAD_DEBUG(string) YARP_NULL_DEBUG
#endif

// class YARPRateThread {};

class ArmThread: public YARPRateThread
{
public:
	ArmThread(int rate,
			  const char *name = "arm control thread",
			  const char *ini_file = NULL);
	virtual ~ArmThread();

	void doLoop();
	void doInit();
	void doRelease();

	void start_transmission()
	{
		/*
		ACE_INET_Addr addr;
		NameClient ns(ACE_INET_Addr(__name_server_port, __name_server_address));
			
		ns.check_in("arm2", addr);
		if (p_arm_sender->start(addr) == -1)
			ns.check_out("arm2");
			*/
	}

	void stop_transmission()
	{
		/*
		ACE_INET_Addr addr;
		NameClient ns(ACE_INET_Addr(__name_server_port, __name_server_address));

		if (p_arm_sender->terminate() != -1)
			ns.check_out("arm2");
			*/
	}

	bool checkMotionDone()
	{
		bool tr_done = !_trajectory.checkBusy();
		bool pid_done = _arm.checkMotionDone();

		return pid_done && tr_done;
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
	
	// change reaching state
	void changeReachingState(AState *s)
		{_arm_state = s;}

	// change reaching first state
	void changeInitState(AState *s)
		{_init_state = s;}

	// this is a private function, it can be called only from within the thread
	void _directCommand(const YVector &cmd)
	{
		_trajectory.stop();
		_trajectory.setFinal(cmd.data(), _nSteps);
		_arm.setPositions(cmd.data());
	}

	inline void send_commands();
	inline void read_status();

	AState *_arm_state;
	AState *_init_state;

	YVector _actual_command;			// actual arm command, joint space
		
	// WristSensor *p_wrist;					// wrist force sensor
	// DatagramSender *p_arm_sender;			// arm sender

	bool _restingInhibited;

	YARPTrajectoryGen _trajectory;
	TirednessControl _tirednessControl;

	int _nj;
	int _nSteps;
	
	char _iniFile[80];						// config file


public: //later: make it private
	ArmStatus _arm_status;
	YARPBabybotArm _arm;
	J1GravityEstimator _gravity1;
	J2GravityEstimator _gravity2;
	J3GravityEstimator _gravity3;
};

#endif //.h