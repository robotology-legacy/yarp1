#include "Armthread.h"
#include "AState.h"

// initialize static instances...
ASDirectCommand		ASDirectCommand::_instance;
ASRestingInit 		ASRestingInit::_instance;
ASWaitForMotion		ASWaitForMotion::_instance;
ASRestingLowerGains ASRestingLowerGains::_instance;
ASRestingWaitIdle	ASRestingWaitIdle::_instance;
ASRestingRaiseGains ASRestingRaiseGains::_instance;
ASMove				ASMove::_instance;
ASDirectCommandMove ASDirectCommandMove::_instance;
ASZeroGInit			ASZeroGInit::_instance;
ASZeroGWait			ASZeroGWait::_instance;
ASZeroGEnd			ASZeroGEnd::_instance;
ASWaitForHand		ASWaitForHand::_instance;
ASShake				ASShake::_instance;

void AState::changeState (ArmThread *t, AState *s)
{
	t->changeReachingState(s);
}

void ASDirectCommand:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::directCommand;
		
	if (t->checkMotionDone() && newCmdFlag!=NONE)
	{
		if (!t->_arm_status.isPIDHigh())
		{
			changeState(t, ASZeroGEnd::instance());	
		}
		else
		{
			ARM_STATE_DEBUG(("New command !\n"));
			t->_directCommand(cmd);
			ASDirectCommandMove::instance()->setFinalCmd(cmd);
			newCmdFlag = NONE;
			changeState(t, ASDirectCommandMove::instance());
		}
	}
	else
		changeState(t, t->_init_state); //ASDirectCommand::instance());
}

void ASDirectCommandMove:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::directCommandMove;
	if (t->checkMotionDone())
	{
		_steps--;
		// wait other few steps
		if (_steps <= 0)
		{
			// check whether the arm is on the table or not
			if ( (t->_arm_status._current_position(1)*radToDeg > -5.0) &&
				_learn)
			{
				ARM_STATE_DEBUG(("Learnt a new position!\n"));
				_steps = _nSteps;
			
				// estimate gravity
				double torque1 = t->_arm_status._torques(1);
				double torque2 = t->_arm_status._torques(2);
				double torque3 = t->_arm_status._torques(3);
				double torque5 = t->_arm_status._torques(5);
				t->_gravity1.update(t->_arm_status._current_position, torque1);
				t->_gravity2.update(t->_arm_status._current_position, torque2);
				t->_gravity3.update(t->_arm_status._current_position, torque3);
				// t->_gravity5.update(t->_wristF(4), torque5);
				
				// estimate error
				_error = t->_arm_status._current_position - _finalCmd;

				t->_gravity1.getP(_lsparameters[0]);
				t->_gravity2.getP(_lsparameters[1]);
				t->_gravity3.getP(_lsparameters[2]);
				
				// joint 5 is different
				// t->_gravity5.getP(_j5Ls);
				
				/////////////////////////
				_errorFile.dump(t->_arm_status._current_position);
				_errorFile.dump(_finalCmd);
				_errorFile.newLine();

				for(int i = 0; i < 3; i++)
					_lsFile.dump(_lsparameters[i]);
				_lsFile.dump(_j5Ls);	// wrist
				
				_lsFile.newLine();

				_pointsFile.dump(t->_arm_status._current_position);
				_pointsFile.dump(t->_arm_status._torques);
				_pointsFile.dump(t->_wristF);
				_pointsFile.newLine();
				////////////////////
			}
		ARM_THREAD_DEBUG(("Arm performed with error:\n"));
		YVector &myPos = t->_arm_status._current_position;
		YVector myError;
		myError = myPos-_finalCmd;
		for(int j = 1; j <= myPos.Length(); j++)
			ACE_OS::printf("%.1lf\t", myError(j)*radToDeg);
		// signal end of motion
		t->writeAndSend(YBVArmDone);
		changeState(t,t->_init_state);
		}
	}
}

void ASWaitForHand:: handle(ArmThread *t)
{
	/*
	t->_arm_status._state._thread = _armThread::waitForHand;
	
	_nSteps++;
	int msg[2];
	if(t->_behaviorsInPort.Read(0))
	{
		memcpy(msg, t->_behaviorsInPort.Content(), sizeof(msg));
		if (msg[0] == 0)
			if (msg[1] == 2)
			{
				_nSteps = 0;
				changeState(t,t->_init_state);
			}
	}

	if (_nSteps>=_timeout)
	{
		ARM_STATE_DEBUG(("Wait for hand timed out... giving up !\n"));
		_nSteps = 0;
		changeState(t,t->_init_state);
	}*/

}

void ASRestingInit:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::restingInit;

	ARM_STATE_DEBUG(("ASRestingInit\n"));
	t->_restingInhibited = true;
	t->_directCommand(firstPosition);

	// prepare ASMove state...
	ASMove *asmove = ASMove::instance();
	asmove->setCmd(secondPosition);
	asmove->setNext(ASRestingLowerGains::instance());
	
	// prepare wait state
	ASWaitForMotion *waitState = ASWaitForMotion::instance();
	waitState->setNext(asmove);
	
	// chenge to wait state
	changeState(t,waitState);

	// signal end of motion
	t->writeAndSend(YBVArmRest);
}

void ASMove:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::move;

	ARM_STATE_DEBUG(("ASMove\n"));
	t->_directCommand(cmd);
	ASWaitForMotion *waitState = ASWaitForMotion::instance();
	waitState->setNext(_next);
	changeState(t,waitState);
}

void ASWaitForMotion:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::waitForMotion;
		
	if (t->checkMotionDone()) 
	{
		// change state
		changeState(t,_next);
	}
}

void ASRestingLowerGains:: handle(ArmThread *t)
{
/*	printf("DEBUG: ASRestingLowerGains ");
	
	t->_arm_status._state = _armThread::restingLowerGains;
		
	double max = t->_arm.getMaxTorque(0);
	double delta = max/30.0;

	bool done[4];
	done[0] = t->_arm.decMaxTorques(delta, 0.0, t->_nj);
	done[1] = t->_gravity1.reduce(30);
	done[2] = t->_gravity2.reduce(30);
	done[3] = t->_gravity3.reduce(30);

	printf("decMaxTorque: %d\n", done[0]);
	// reduce max torques to 0.0
	if (done[0]&&done[1]&&done[2]&&done[3])
	{
		// lower gains smoothly
		changeState(t, ASRestingWaitIdle::instance());
		t->_arm_status._pidStatus = 0;
		printf("DEBUG: ASRestingLowerGains: DONE\n");
	}*/

	double max = t->_arm.getMaxTorque(0);
	double delta = max/30.0;

	bool done[4];
	done[0] = t->_decMaxTorques(delta, 0.0, t->_nj);
	done[1] = t->_gravity1.reduce(30);
	done[2] = t->_gravity2.reduce(30);
	done[3] = t->_gravity3.reduce(30);

	if (done[0]&&done[1]&&done[2]&&done[3])
	{
		// lower gains smoothly
		changeState(t, ASRestingWaitIdle::instance());
		t->_arm_status._pidStatus = 0;
	}
}

void ASRestingWaitIdle:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::restingWaitIdle;
		
	// stay here untill rested
	if (t->_tirednessControl.low())
	{
		ARM_STATE_DEBUG(("Arm is now fully restored !\n"));
		changeState(t, ASRestingRaiseGains::instance());
	}
}

void ASRestingRaiseGains:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::restingRaiseGains;
		
	double max = t->_arm.getMaxTorque(0);
	double delta = max/30.0;

	bool done[4];
	done[0] = t->_incMaxTorques(delta, max, t->_nj);
	done[1] = t->_gravity1.increase(30);
	done[2] = t->_gravity2.increase(30);
	done[3] = t->_gravity3.increase(30);

	// increase max torques to max torques
	if (done[0]&&done[1]&&done[2]&&done[3])
	{
		// raise gains smoothly
		ARM_STATE_DEBUG(("PID are active, g is activated !\n"));
		t->_restingInhibited = false;
		t->directCommandMode();
		changeState(t, t->_init_state);
		t->_arm_status._pidStatus = 1;
		t->writeAndSend(YBVArmRestDone);
	}
}

/// ZERO G states
void ASZeroGInit:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::zeroGInit;
		
	double max = t->_arm.getMaxTorque(0);
	double delta = max/60.0;

	if (t->_arm_status.isPIDHigh()) 
	{
		t->_restingInhibited = true;
		if (t->_arm.decMaxTorques(delta, 0.0,3))
		{
			ARM_STATE_DEBUG(("Zero G!\n"));
			t->_restingInhibited = false;
			t->_arm_status._pidStatus = 0;
		}
	}
	else
		changeState(t, t->_init_state);
}

void ASZeroGWait:: handle(ArmThread *t)
{
	// stay here forever...
	
}

void ASZeroGEnd:: handle(ArmThread *t)
{
	t->_arm_status._state = _armThread::zeroGEnd;
		
	double max = t->_arm.getMaxTorque(0);
	double delta = max/60.0;

	if (!t->_arm_status.isPIDHigh()) 
	{
		t->_restingInhibited = true;
		if ( t->_arm.incMaxTorques(delta, max, 3))
		{
			// raise gains smoothly
			ARM_STATE_DEBUG(("PIDs active again !\n"));
			t->_restingInhibited = false;
			t->_arm_status._pidStatus = 1;
			changeState(t, t->_init_state);
		}
	}
}

void ASShake:: handle(ArmThread *t)
{
	// t->_arm_status._state._thread = _armThread::shake;

	n--;

	ARM_STATE_DEBUG(("ASShake\n"));
	t->_restingInhibited = true;

	// check n
	if (n<=0) 
	{
		// end shaking sequence
		t->changeInitState(ASDirectCommand::instance());
		changeState(t,t->_init_state);
		t->_shaking = false;
		t->_restingInhibited = false;
		// signal end of motion

		ACE_OS::printf("---> Sending ArmDone...");
		t->writeAndSend(YBVArmDone);
		ACE_OS::printf("done! \n");
		return;
	}
	else if ( (n%2) == 0)
	{
		// first
		cmd = currentPosition+firstPosition;
	}
	else
	{
		cmd = currentPosition-secondPosition;
	}
	
	t->_directCommand(cmd, 10);
	// prepare wait state
	ASWaitForMotion *waitState = ASWaitForMotion::instance();
	waitState->setNext(this);
	// chenge to wait state
	changeState(t,waitState);
}
