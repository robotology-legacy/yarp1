#ifdef __QNX__
//
// ArmControl.cpp implementation of arm control classes.
//

#include "ArmControl.h"
#include "YARPTime.h"

//
// A single instance of the meid handler.
int YARPGenericArmControl::meidx0 = -1;
int YARPGenericArmControl::meidx1 = -1;
int YARPGenericArmControl::meidx2 = -1;
int YARPGenericArmControl::m_instances = 0;
YARPSemaphore YARPGenericArmControl::m_mutex;		// default = 1

YARPGenericArmControl::YARPGenericArmControl ()
{
	ueidx = -1;
}

YARPGenericArmControl::~YARPGenericArmControl ()
{
}

// class YARPGenericArmControl
int YARPGenericArmControl::Initialize (void)
{
	Lock ();
	if (m_instances == 0)
	{
		meidx0 = meid_open(MEI_NAME0);
		if (meidx0 == -1)
		{
			printf("Failed to locate MEI daemon %s!\n", MEI_NAME0);
			Unlock ();
			return -1;
		}

		meidx1 = meid_open(MEI_NAME1);
		if (meidx1 == -1)
		{
			printf("Failed to locate MEI daemon %s!\n", MEI_NAME1);
			Unlock ();
			return -1;
		}

		meidx2 = meid_open(MEI_NAME2);
		if (meidx2 == -1)
		{
			printf("Failed to locate MEI daemon %s!\n", MEI_NAME2);
			Unlock ();
			return -1;
		}

		int stat = meid_dsp_reset(meidx0);
			stat |= meid_dsp_reset(meidx1);
			stat |= meid_dsp_reset(meidx2);

		if (stat != 0)
		{
			printf("MEI Reset Failed... Error code %d!\n", stat);
			Unlock ();
			return -1;
		}
	}

	// init the uei card. where's the error check?
	// this needs to be done for each thread/instance.
	ueid_initialize(UEID_NAME, &ueidx);
	ueid_sw_cl_start(ueidx, BOARD);

	m_instances ++;
	Unlock ();

	return 0;
}

int YARPGenericArmControl::Uninitialize ()
{
	// close every single instance (copy) of meidx.
	Lock ();
	meid_close (meidx0);
	meid_close (meidx1);
	meid_close (meidx2);

	// no close for ueid?

	m_instances--;
	Unlock ();

	return 0;
}

//
//
// position control thread class.
__positioncontrol::__positioncontrol () : YARPRateThread ("poscontrolthread", POSITION_PERIOD)
{
	// actual gains are zero at startup.
	m_zero_gain = true;

	m_gains = new double*[NJOINTS];
	m_2b_gains = new double*[NJOINTS];
	m_gains_hold = new double*[NJOINTS];
	assert (m_gains != NULL);
	assert (m_2b_gains != NULL);
	assert (m_gains_hold != NULL);

	for (int i = 0; i < NJOINTS; i++)
	{
		m_gains[i] = new double[NPARAMS];
		memset (m_gains[i], 0, sizeof(double) * NPARAMS);
		m_2b_gains[i] = new double[NPARAMS];
		memset (m_2b_gains[i], 0, sizeof(double) * NPARAMS);
		m_gains_hold[i] = new double[NPARAMS];
		memset (m_gains_hold[i], 0, sizeof(double) * NPARAMS);
	}

	m_pot_zero = new int[NJOINTS];
	m_torque_zero = new int[NJOINTS];
	assert (m_pot_zero != NULL);
	assert (m_torque_zero != NULL);

	m_enabled = new bool[NJOINTS];
	assert (m_enabled != NULL);
	m_calibrated = new bool[NJOINTS];
	m_t_calibrated = new bool[NJOINTS];
	assert (m_calibrated != NULL);
	assert (m_t_calibrated != NULL);

	m_intrajectory = new bool[NJOINTS];
	assert (m_intrajectory != NULL);

	m_limits_max = new double[NJOINTS];
	m_limits_min = new double[NJOINTS];
	assert (m_limits_max != NULL);
	assert (m_limits_min != NULL);
	memset (m_limits_max, 0, sizeof(double) * NJOINTS);
	memset (m_limits_min, 0, sizeof(double) * NJOINTS);

	m_angle = new double[NJOINTS];
	m_2b_angle = new double[NJOINTS];
	m_command = new double[NJOINTS];
	m_2b_command = new double[NJOINTS];
	m_error = new double[NJOINTS];
	m_2b_error = new double[NJOINTS];
	m_derror = new double[NJOINTS];
	m_ierror = new double[NJOINTS];
	m_old_error = new double[NJOINTS];

	assert (m_angle != NULL);
	assert (m_2b_angle != NULL);
	assert (m_command != NULL);
	assert (m_2b_command != NULL);
	assert (m_error != NULL);
	assert (m_2b_error != NULL);
	assert (m_derror != NULL);
	assert (m_ierror != NULL);
	assert (m_old_error != NULL);

	m_torque = new int16[NJOINTS];
	m_old_torque = new int16[NJOINTS];
	m_2b_torque = new int16[NJOINTS];
	assert (m_torque != NULL && 
			m_2b_torque != NULL &&
			m_old_torque != NULL);

	m_speed = new double[NJOINTS];
	m_2b_speed = new double[NJOINTS];
	assert (m_speed != NULL);
	assert (m_2b_speed != NULL);

	m_alpha = DEFAULTALPHAVALUE;

	m_oldfiltervalues = new double[NJOINTS];
	assert (m_oldfiltervalues != NULL);
	memset (m_oldfiltervalues, 0, sizeof(double) * NJOINTS);

	m_lastcommand = new ArmCommandType[NJOINTS];
	assert (m_lastcommand != NULL);
	memset (m_lastcommand, 0, sizeof(ArmCommandType) * NJOINTS);

	m_speed_setpoint = new double[NJOINTS];
	m_accel_setpoint = new double[NJOINTS];
	
	assert (m_speed_setpoint != NULL);
	assert (m_accel_setpoint != NULL);
}

virtual __positioncontrol::~__positioncontrol () 
{
	if (m_gains != NULL)
	{
		for (int i = 1; i < NJOINTS; i++)
			if (m_gains[i] != NULL)
				delete[] m_gains[i];
		delete[] m_gains;
	}

	if (m_gains_hold != NULL)
	{
		for (int i = 1; i < NJOINTS; i++)
			if (m_gains_hold[i] != NULL)
				delete[] m_gains_hold[i];
		delete[] m_gains_hold;
	}

	if (m_2b_gains != NULL)
	{
		for (int i = 1; i < NJOINTS; i++)
			if (m_2b_gains[i] != NULL)
				delete[] m_2b_gains[i];
		delete[] m_2b_gains;
	}

	if (m_pot_zero != NULL)
		delete[] m_pot_zero;
	if (m_torque_zero != NULL)
		delete[] m_torque_zero;

	if (m_enabled != NULL)
		delete[] m_enabled;
	if (m_calibrated != NULL)
		delete[] m_calibrated;
	if (m_t_calibrated != NULL)
		delete[] m_t_calibrated;

	if (m_intrajectory != NULL)
		delete[] m_intrajectory;

	if (m_limits_max != NULL)
		delete[] m_limits_max;
	if (m_limits_min != NULL)
		delete[] m_limits_min;

	if (m_angle != NULL)
		delete[] m_angle;
	if (m_command != NULL)
		delete[] m_command;
	if (m_2b_angle != NULL)
		delete[] m_2b_angle;
	if (m_2b_command != NULL)
		delete[] m_2b_command;

	if (m_speed != NULL)
		delete[] m_speed;
	if (m_2b_speed != NULL)
		delete[] m_2b_speed;

	if (m_error != NULL)
		delete[] m_error;
	if (m_2b_error != NULL)
		delete[] m_2b_error;
	if (m_derror != NULL)
		delete[] m_derror;
	if (m_ierror != NULL)
		delete[] m_ierror;
	if (m_old_error != NULL)
		delete[] m_old_error;

	if (m_torque != NULL)
		delete[] m_torque;
	if (m_2b_torque != NULL)
		delete[] m_2b_torque;
	if (m_old_torque != NULL)
		delete[] m_old_torque;

	if (m_oldfiltervalues != NULL)
		delete[] m_oldfiltervalues;

	if (m_lastcommand != NULL)
		delete[] m_lastcommand;

	if (m_speed_setpoint != NULL)
		delete[] m_speed_setpoint;
	if (m_accel_setpoint != NULL)
		delete[] m_accel_setpoint;
}

void __positioncontrol::DoInit(void *caller)
{
	YARPArmControl& control = (YARPArmControl&)*caller;
	int i;

	// get the ueid handle.
	// init the uei card. where's the error check?
	ueid_initialize(UEID_NAME, &ueidx);

	// initial command is the current position.
	memcpy (m_pot_zero, control.m_pot_zero, sizeof(int) * NJOINTS);

	// assuming joints are calibrated.
	memcpy (m_torque_zero, control.m_torque_zero, sizeof(int) * NJOINTS);

	memset (m_enabled, 0, sizeof(bool) * NJOINTS);
	memset (m_calibrated, 0, sizeof(bool) * NJOINTS);
	memset (m_t_calibrated, 0, sizeof(bool) * NJOINTS);
	
	memset (m_intrajectory, 0, sizeof(bool) * NJOINTS);

	// needed for the encoders.
	for (i = 0; i < 3; i++)
	{
		m_16bit_oldangle[i] = m_pot_zero[i];
		m_winding[i] = 0;
	}

	// read positions here for initializing values.
	ReadPositions (control);
	for (i = 0; i < NJOINTS; i++)
	{
		m_command[i] =
		m_2b_command[i] =
		m_2b_angle[i] = m_angle[i];
	
		m_oldfiltervalues[i] = 0;
	}

	memset (m_speed, 0, sizeof(double) * NJOINTS);
	memset (m_2b_speed, 0, sizeof(double) * NJOINTS);

	// other initialization.
	memset (m_old_error, 0, sizeof(double) * NJOINTS);
	memset (m_error, 0, sizeof(double) * NJOINTS);
	memset (m_2b_error, 0, sizeof(double) * NJOINTS);
	memset (m_derror, 0, sizeof(double) * NJOINTS);
	memset (m_ierror, 0, sizeof(double) * NJOINTS);

	memset (m_torque, 0, sizeof(int16) * NJOINTS);
	memset (m_old_torque, 0, sizeof(int16) * NJOINTS);
	memset (m_2b_torque, 0, sizeof(int16) * NJOINTS);

	// upon startup, read also the gains.
	m_zero_gain = true;
	for (i = 0; i < NJOINTS; i++)
	{
		memcpy (m_gains_hold[i], m_2b_gains[i], sizeof(double) * NPARAMS);
		memset (m_gains[i], 0, sizeof(double) * NPARAMS);
	}

	memset (m_lastcommand, 0, sizeof(ArmCommandType) * NJOINTS);
	memset (m_speed_setpoint, 0, sizeof(double) * NJOINTS);
	memset (m_accel_setpoint, 0, sizeof(double) * NJOINTS);
	// sort of init.
	for (i = 0; i < 3; i++) m_accel_setpoint[i] = TorsoDefaultAccelerationValue;

	// this should be fine. don't need fine control on active joints.
	for (i = 0; i < NJOINTS; i++) m_constvel.ActivateJoint (i);
	for (i = 0; i < NJOINTS; i++) m_trapezoidal.ActivateJoint (i);

	// a test only for the shoulder.
	m_gravity.Init (m_limits_min[3], m_limits_max[3]);
	m_gravity.LoadFromFile (DEFAULTGRAVITYFILENAME);

	m_prev_cycle = clock();
	m_timing = false;		// turn this to true to measure time.
}


void __positioncontrol::DoLoop(void *caller)
{
	YARPArmControl& control = (YARPArmControl&)*caller;

	// compute timing only if flag is on.
	if (m_timing)
	{
		clock_t now = clock();
		cout << ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC) << endl;
		m_prev_cycle = now;
	}

	// read the position.
	// get the angles from the uei card.
	// this might be a good place to compute the speed of the joints.
	ReadPositions (control);

	// WARNING: this is very very slow. Practically unusable.
	if (m_enabletorque)
	{
		for (int i = 0; i < NJOINTS; i++)
		{
			int meidx = control.meidx0;
			int joint = control.get_mei_info (i, &meidx);

			meid_read_axis_analog (meidx, joint, m_torque+i);
		}
	}

	Lock ();


	// do trajectory (trapezoidal), position, velocity.
	ComputeTrajectory();
	ComputeErrors();
	ComputeController (control);


	//
	// copy into in/out buffers.
	//
	// CHECK WHETHER NEED TO TAKE INTO ACCOUNT THE m_lastcommand[i]...
	// shoud this be moved to the beginning of the thread?
	for (int i = 0; i < NJOINTS; i++)
	{
		if (!m_intrajectory[i])
			m_command[i] = m_2b_command[i];		// copy last buffer into command. Execute position.
		else
			m_2b_command[i] = m_command[i];		// copy last command into buffer (for consistency).
	}

	memcpy (m_speed, m_2b_speed, sizeof(double) * NJOINTS);

	for (i = 0; i < NJOINTS; i++)
	{
		memcpy (m_gains_hold[i], m_2b_gains[i], sizeof(double) * NPARAMS);
		if (!m_zero_gain)
		{
			// copy also to the actual gain.
			memcpy (m_gains[i], m_2b_gains[i], sizeof(double) * NPARAMS);
		}
	}

	memcpy (m_2b_angle, m_angle, sizeof(double) * NJOINTS);
	memcpy (m_2b_error, m_error, sizeof(double) * NJOINTS);

	if (m_enabletorque)
		memcpy (m_2b_torque, m_torque, sizeof(int16) * NJOINTS);

	Unlock ();
}

void __positioncontrol::DoRelease(void *caller)
{
	YARPArmControl& control = (YARPArmControl&)*caller;

	// zero torque.
	for (int i = 0; i < NJOINTS; i++)
	{
		int meidx = control.meidx0;
		int joint = control.get_mei_info (i, &meidx);
		meid_set_command(meidx, joint, m_torque_zero[i]);
	}

	// disable the PIDs.
	int16 filter[COEFFICIENTS];
	memset (filter, 0, sizeof(int16)*COEFFICIENTS);
	for (i = 0; i < 8; i++) 
	{
		meid_error_check(meid_set_filter(control.meidx0, i, filter));
		meid_error_check(meid_set_filter(control.meidx1, i, filter));
		meid_error_check(meid_set_filter(control.meidx2, i, filter));
	}
}

// manipulate params.
void __positioncontrol::SetPositionGain (int joint, int param, double value) 
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: not a valid joint %d\n", joint);
		return;
	}	

	if (param < 0 || param >= NPARAMS)
		return;

	Lock();
	m_2b_gains[joint][param] = value;
	Unlock();
}

void __positioncontrol::SetPositionGains (int joint, double *values)
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: not a valid joint %d\n", joint);
		return;
	}

	Lock();
	memcpy (m_2b_gains[joint], values, sizeof(double) * NPARAMS);
	Unlock();
}

void __positioncontrol::GetPositionGain (int joint, int param, double *value) 
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: not a valid joint %d\n", joint);
		return;
	}

	if (param < 0 || param >= NPARAMS)
		return;

	Lock();
	*value = m_2b_gains[joint][param];
	Unlock();
}

void __positioncontrol::GetPositionGains (int joint, double *values)
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: not a valid joint %d\n", joint);
		return;
	}

	Lock();
	memcpy (values, m_2b_gains[joint], sizeof(double) * NPARAMS);
	Unlock();
}

bool __positioncontrol::GetGainStatus (void)
{
	Lock ();
	bool ret = m_zero_gain;
	Unlock ();
	return ret;
}

// LATER: adapt for the torso.
void __positioncontrol::RaiseGains (void) 
{
	if (!m_zero_gain)
	{
		printf ("gains are already up!");
		return;
	}

	const int NSTEPS = 100;
	const int INTERVAL = 30;	// ms.

	// linearly go from 0 to m_gains_hold in a given amount of time.
	// not of much use for the torso. these are not used. set =zero in config.
	double delta[NJOINTS][NPARAMS];
	for (int i = 0; i < NJOINTS; i++)
		for (int j = 0; j < NPARAMS; j++)
		{
			delta[i][j] = m_gains_hold[i][j] / NSTEPS;
		}

	for (int s = 0; s < NSTEPS; s++)
	{
		for (int i = 0; i < NJOINTS; i++)
			for (int j = 0; j < NPARAMS; j++)
			{
				m_gains[i][j] += delta[i][j];
			}

		delay(INTERVAL);
	}

	// final copy to adjust rounding errors.
	for (i = 0; i < NJOINTS; i++)
		memcpy (m_gains[i], m_gains_hold[i], sizeof(double) * NPARAMS);

	m_zero_gain = false;
}

void __positioncontrol::LowerGains (void) 
{
	// linearly go from m_gains_hold to 0 in a given amount of time.
	if (m_zero_gain)
	{
		printf ("gains are already down!");
		return;
	}

	const int NSTEPS = 100;
	const int INTERVAL = 30;	// ms.

	// linearly go from m_gains_hold to 0 in a given amount of time.
	// not of much use for the torso. these are not used. set =zero in config.
	double delta[NJOINTS][NPARAMS];
	for (int i = 0; i < NJOINTS; i++)
		for (int j = 0; j < NPARAMS; j++)
		{
			delta[i][j] = m_gains_hold[i][j] / NSTEPS;
		}

	for (int s = 0; s < NSTEPS; s++)
	{
		for (int i = 0; i < NJOINTS; i++)
			for (int j = 0; j < NPARAMS; j++)
			{
				m_gains[i][j] -= delta[i][j];
			}

		delay(INTERVAL);
	}

	// final copy to adjust rounding errors.
	for (i = 0; i < NJOINTS; i++)
		memset (m_gains[i], 0, sizeof(double) * NPARAMS);

	m_zero_gain = true;
}

void __positioncontrol::SetPosition (int joint, double position) 
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	// LATER: need to check limits.
	// need a trajectory generator.
	Lock();
	m_2b_command[joint] = position;
	m_intrajectory[joint] = false;	// abort trajectory generation on the fly.
	m_lastcommand[joint] = COMMAND_SETPOS;
	Unlock();
}

void __positioncontrol::SetPositions (double *position)
{
	//
	// limits are not checked here!
	Lock();
	memcpy (m_2b_command, position, sizeof(double) * NJOINTS);
	memset (m_intrajectory, 0, sizeof(bool) * NJOINTS);	// abort trajectory.
	memset (m_lastcommand, COMMAND_SETPOS, sizeof(ArmCommandType) * NJOINTS);
	Unlock();
}

void __positioncontrol::GetPosition (int joint, double *position) 
{
	if (joint < 0 || joint >= NJOINTS)
	{
		*position = 0;
		return;
	}

	Lock();
	*position = m_2b_angle[joint];
	Unlock();
}

void __positioncontrol::GetPositions (double *position)
{
	Lock();
	memcpy (position, m_2b_angle, sizeof(double) * NJOINTS);
	Unlock();
}

void __positioncontrol::SetPCalibratedFlag (int joint)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	Lock();
	m_calibrated[joint] = true;
	Unlock();
}

// LATER: extend position control to the torso (joint 0,1,2).
void __positioncontrol::MoveToSimple (int joint, double value)
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: can't apply move to axis %d\n", joint);
		return;
	}

	Lock ();
	m_constvel.Set (joint, m_2b_angle[joint], m_command[joint], value, m_speed_setpoint[joint]);
	m_constvel.InitJoint (joint);
	m_lastcommand[joint] = COMMAND_VEL2;
	Unlock ();
}

void __positioncontrol::MoveTo (int joint, double value)
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: can't apply move to axis %d\n", joint);
		return;
	}

	Lock ();
	m_trapezoidal.Set (joint, m_2b_angle[joint], 0.0, value, m_speed_setpoint[joint], m_accel_setpoint[joint]);
	m_trapezoidal.InitJoint (joint);
	m_lastcommand[joint] = COMMAND_MOVETO;
	Unlock ();
}

void __positioncontrol::MoveTo (int howmany, int *joints, double *value)
{
	// joints is the array of joints to be moved.
	Lock ();
	for (int i = 0; i < howmany; i++)
	{
		int jj = joints[i];
		if (jj >= 3 && jj < NJOINTS)
		{
			m_trapezoidal.Set (jj, m_2b_angle[jj], 0.0, value[i], m_speed_setpoint[jj], m_accel_setpoint[jj]);
			m_trapezoidal.InitJoint (jj);
			m_lastcommand[jj] = COMMAND_MOVETO;
		}
		else
		{
			printf ("__positioncontrol: can't apply move to axis %d\n", jj);
		}
	}
	Unlock ();
}

void __positioncontrol::VMove (int joint, double value) 
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	Lock();
	m_2b_speed[joint] = value;
	if (value != 0.0)
	{
		m_intrajectory[joint] = true;
		m_lastcommand[joint] = COMMAND_VEL;
	}
	else
	{
		m_intrajectory[joint] = false;
		m_lastcommand[joint] = COMMAND_NONE;
	}
	Unlock();
}

void __positioncontrol::VMove (double *values) 
{
	Lock ();
	memcpy (m_2b_speed, values, sizeof(double) * NJOINTS);
	for (int i = 0; i < NJOINTS; i++)
	{
		m_intrajectory[i] = (values[i] != 0.0) ? true : false;
		m_lastcommand[i] = (values[i] != 0.0) ? COMMAND_VEL : COMMAND_NONE;
	}
	Unlock ();
}

void __positioncontrol::VMove (int joint1, int joint2, double *values)
{
	if (joint1 < 0 || joint1 >= NJOINTS ||
		joint2 < 0 || joint2 >= NJOINTS ||
		joint1 > joint2)
		return;

	Lock ();
	for (int i = joint1; i <= joint2; i++)
	{
		m_2b_speed[i] = values[i];
		m_intrajectory[i] = (values[i] != 0.0) ? true : false;
		m_lastcommand[i] = (values[i] != 0.0) ? COMMAND_VEL : COMMAND_NONE;
	}
	Unlock ();
}

void __positioncontrol::GetError (int joint, double *err)
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: can't get error for joint %d\n", joint);
		return;
	}

	Lock ();
	*err = m_2b_error[joint];
	Unlock ();
}

void __positioncontrol::GetErrors (double *err)
{
	Lock ();
	memcpy (err, m_2b_error, sizeof(double) * NJOINTS);
	Unlock ();
}

void __positioncontrol::SetPositionLimits (int joint, double min, double max)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	Lock ();
	m_limits_min[joint] = min;
	m_limits_max[joint] = max;
	Unlock ();
}

void __positioncontrol::GetPositionLimits (int joint, double *min, double *max)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		*min = 0;
		*max = 0;
		return;
	}

	Lock ();
	*min = m_limits_min[joint];
	*max = m_limits_max[joint];
	Unlock ();
}

void __positioncontrol::SetSpeedSetpoint (int joint, double v)
{
	if (joint < 3 || joint >= NJOINTS)
	{
		printf ("__positioncontrol: can't set speed for joint %d\n", joint);
		return;
	}

	Lock ();
	m_speed_setpoint[joint] = v;
	Unlock ();
}

void __positioncontrol::SetAccelSetpoint (int joint, double v)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	Lock ();
	m_accel_setpoint[joint] = v;
	Unlock ();
}

void __positioncontrol::EnableAxis (int joint)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	// LATER: should check whether it's calibrated before enabling it.
	Lock ();
	m_enabled[joint] = true;
	Unlock ();
}

void __positioncontrol::DisableAxis (int joint)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	Lock ();
	m_enabled[joint] = false;
	Unlock ();
}

void __positioncontrol::SetTCalibratedFlag (int joint, int zero)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return;
	}

	Lock();
	m_t_calibrated[joint] = true;
	m_torque_zero[joint] = zero;
	Unlock();
}

bool __positioncontrol::GetTCalibratedFlag (int joint)
{
	if (joint < 0 || joint >= NJOINTS)
	{
		return false;
	}

	bool ret;
	Lock();
	ret = m_t_calibrated[joint];
	Unlock();
	return ret;
}

void __positioncontrol::SetAlphaFilter (double alpha) 
{
	Lock ();
	m_alpha = alpha;
	Unlock ();
}

void __positioncontrol::GetAlphaFilter (double *alpha)
{
	Lock ();
	*alpha = m_alpha;
	Unlock ();
}

void __positioncontrol::EnableTorque (void)
{
	Lock ();
	m_enabletorque = true;
	Unlock ();
}

void __positioncontrol::DisableTorque (void)
{
	Lock ();
	m_enabletorque = false;
	Unlock ();
}

void __positioncontrol::ToggleTorque (void)
{
	Lock ();
	m_enabletorque = !m_enabletorque;
	Unlock ();
}

void __positioncontrol::GetTorque (int joint, double *torque)
{
	if (!m_enabletorque)
	{
		printf ("__positioncontrol: reading torque not enabled\n");
		return;
	}

	Lock ();
	*torque = m_2b_torque[joint] - m_torque_zero[joint];
	Unlock ();
}

void __positioncontrol::GetTorques (double *torques)
{
	if (!m_enabletorque)
	{
		printf ("__positioncontrol: reading torque not enabled\n");
		return;
	}

	Lock ();
	for (int i = 0; i < NJOINTS; i++)
	{
		torques[i] = m_2b_torque[i] - m_torque_zero[i];
	}
	Unlock ();
}

//
// Gravity compensation methods. Experimental.
void __positioncontrol::GravitySaveToFile (void) 
{ 
	Lock();
	m_gravity.SaveToFile (DEFAULTGRAVITYFILENAME); 
	Unlock();
}

void __positioncontrol::GravityLoadFromFile (void)
{
	Lock();
	m_gravity.LoadFromFile (DEFAULTGRAVITYFILENAME);
	Unlock();
}

void __positioncontrol::GravityToggleActive (void)
{
	Lock ();
	m_gravity.ToggleActive ();
	Unlock ();
}

void __positioncontrol::GravityToggleLearning (void)
{
	Lock ();
	m_gravity.ToggleLearning ();
	Unlock ();
}


//
// Shared data.
//
__positioncontrol YARPArmControl::m_thread;				// the actual control thread.

int16 ** YARPArmControl::m_control_params = NULL;		// MEI control params.

int *	 YARPArmControl::m_pot_zero = NULL;				// initial reading of the pots.
int *	 YARPArmControl::m_torque_zero = NULL;			// strain gauges zero.

bool YARPArmControl::m_torso_shutdown_flag = true;		// whether gains are zero.

YARPArmControl::YARPArmControl ()
{
	// static vars are already initialized.
	// take care of the locals.
}


YARPArmControl::~YARPArmControl ()
{
	// free instance specific mem.
}

// uses default filename.
int YARPArmControl::Initialize() 
{
	return YARPArmControl::Initialize(DEFAULTARMFILENAME);
}

// if Initialize fails please exit from calling process.
int YARPArmControl::Initialize (const char *filename)
{
	if (YARPGenericArmControl::Initialize () < 0)
	{
		printf ("YARPArmControl: base class init failed\n");
		return -1;
	}

	Lock ();

	if (m_instances == 1)
	{
		// this is the first instance - m_instances correctly incremented 
		// by base class Initialize.
		assert (filename != NULL);

		// alloc mem.
		m_pot_zero = new int[NJOINTS];
		m_torque_zero = new int[NJOINTS];
		assert (m_pot_zero != NULL);
		assert (m_torque_zero != NULL);
		
		memset (m_torque_zero, 0, sizeof(int) * NJOINTS);

		m_control_params = new int16*[NJOINTS];
		assert (m_control_params != NULL);

		for (int i = 0; i < NJOINTS; i++)
		{
			m_control_params[i] = new int16[COEFFICIENTS];
			assert (m_control_params[i] != NULL);
		}

		// config file.
		// WARNING: different control schema (torque, pos)
		//	might require different config files.
		ReadConfig (filename);

		// set the gain to zero.
		// activate the analog loop.
		int16 filter[COEFFICIENTS];
		memset (filter, 0, sizeof(int16)*COEFFICIENTS);
		for (i = 0; i < 8; i++) 
		{
			meid_error_check(meid_set_filter(meidx0, i, filter));
			meid_error_check(meid_set_filter(meidx1, i, filter));
			meid_error_check(meid_set_filter(meidx2, i, filter));
		}

		// this is a more traditional position control.
		for (i = 0; i < 3; i++)
		{
			int meidx = meidx0;
			int joint = get_mei_info (i, &meidx);

			meid_error_check(meid_init_analog(meidx , joint, FALSE, TRUE));  // setup so that can read torque cells
			meid_error_check(meid_set_axis_analog (meidx, joint, TRUE));

			meid_error_check(meid_set_analog_channel (meidx, joint, joint, FALSE, TRUE));

			// feedback encoder.
			meid_error_check(meid_set_feedback(meidx, joint, FB_ENCODER));  
			meid_error_check(meid_set_error_limit(meidx, joint, 32767.0, NO_EVENT));
		}

		for (i = 3; i < NJOINTS; i++)
		{
			// activate the analog in for each joint.
			int meidx = meidx0;
			int joint = get_mei_info (i, &meidx);

			meid_error_check(meid_init_analog(meidx , joint, FALSE, TRUE));  // setup so that can read torque cells
			meid_error_check(meid_set_axis_analog (meidx, joint, TRUE));

			meid_error_check(meid_set_analog_channel (meidx, joint, joint, FALSE, TRUE));
			meid_error_check(meid_set_feedback(meidx, joint, FB_ANALOG));  

			meid_error_check(meid_set_error_limit(meidx, joint, 32767.0, NO_EVENT));
		}

		// read zeros and store in m_pot_zero.
		ReadZeroPositions ();

		m_torso_shutdown_flag = true;	// in fact mei gains are zero.
	}
	else
	{
		// this is not the first instance.
		// we don't need to read the config file.
	}

	Unlock ();

	return 0;
}

// eat all char until a \n is encountered.
void YARPArmControl::EatLine (FILE *fp)
{
	// check if line begins with '//'
	for (;;)
	{
		char c1, c2;
		fread (&c1, 1, 1, fp);
		fread (&c2, 1, 1, fp);

		if (c1 == '/' && c2 == '/')
		{
			do
			{
				fread (&c1, 1, 1, fp);
			}
			while (c1 != '\n' && !feof(fp));
		}
		else
		{
			fseek (fp, -2, SEEK_CUR);
			return;		// exit from here.
		}
	}
}

void YARPArmControl::ReadConfig (const char *filename)
{
	FILE *fp = fopen (filename, "r");
	assert (fp != NULL);

	EatLine (fp);

	for (int i = 0; i < NJOINTS; i++)
	{
		EatLine (fp);

		for (int j = 0; j < COEFFICIENTS; j++)
		{
			fscanf (fp, "%hd ", &m_control_params[i][j]);
		}
//		printf ("set: %d %d %d\n", m_control_params[i][DF_SHIFT], m_control_params[i][DF_P], m_control_params[i][DF_D]);
		fscanf (fp, "\n");

		double tmp = 0;
		for (j = 0; j < NPARAMS; j++)
		{
			fscanf (fp, "%lf ", &tmp);
			m_thread.SetPositionGain (i, j, tmp);
		}
		fscanf (fp, "\n");

		double min, max;
		fscanf (fp, "%lf %lf\n", &min, &max);
		m_thread.SetPositionLimits (i, min, max);
	}

	fclose (fp);
}

int YARPArmControl::Uninitialize (void)
{
	Lock ();

	if (m_instances == 1)
	{
		if (m_pot_zero != NULL)
			delete[] m_pot_zero;
		m_pot_zero = NULL;

		if (m_torque_zero != NULL)
			delete[] m_torque_zero;
		m_torque_zero = NULL;

		if (m_control_params != NULL)
		{
			for (int i = 0; i < NJOINTS; i++)
			{
				if (m_control_params[i] != NULL)
					delete[] m_control_params[i];
			}

			delete[] m_control_params;
			m_control_params = NULL;
		}
	}

	Unlock ();

	YARPGenericArmControl::Uninitialize ();

	return 0;
}

// Do calibration before enabling joints!
void YARPArmControl::TorqueCalibration (int joint)
{
	Lock ();

	const int NREADINGS = 300;

	int meidx = meidx0;
	int jnt = get_mei_info (joint, &meidx);
	int16 value = 0;
	int avereading = 0;
		
	for (int i = 0; i < NREADINGS; i++)
	{
		meid_read_axis_analog (meidx, jnt, &value);
		avereading += value;
		delay (20);
	}

	// setting the zero.
	// this gets copied at thread startup.
	m_torque_zero[joint] = avereading / NREADINGS;
	m_thread.SetTCalibratedFlag (joint, m_torque_zero[joint]);

	//
	printf ("torque calibration joint %d: %d\n", joint, m_torque_zero[joint]);

	// exclude the torso.
	if (joint >= 3 && joint < NJOINTS)
	{
		// set also MEI gains.
		// set the command to the actual zero.
		meid_error_check(meid_set_command (meidx, jnt, m_torque_zero[joint]));
		meid_error_check(meid_set_filter (meidx, jnt, m_control_params[joint]));
	}
	else
	{
		// Calibration for the torso means that the position gain is already up!
		meid_error_check(meid_set_position (meidx, jnt, 0.0));
		meid_error_check(meid_set_command (meidx, jnt, 0.0));
		meid_error_check(meid_set_filter (meidx, jnt, m_control_params[joint]));
				
		m_torso_shutdown_flag = false;
	}

	Unlock ();
}

void YARPArmControl::TorqueCalibration (void)
{
	Lock ();

	const int NREADINGS = 300;

	int meidx = meidx0;
	int16 value = 0;
	int avereading[NJOINTS];

	memset (avereading, 0, sizeof(int) * NJOINTS);

	for (int i = 0; i < NREADINGS; i++)
	{
		for (int joint = 0; joint < NJOINTS; joint++)
		{
			int jnt = get_mei_info (joint, &meidx);

			meid_read_axis_analog (meidx, jnt, &value);
			avereading[joint] += value;
		}
		delay(10);
	}

	// setting the zero.
	// this gets copied at thread startup.
	for (int joint = 0; joint < NJOINTS; joint++)
	{
		m_torque_zero[joint] = avereading[joint] / NREADINGS;
		m_thread.SetTCalibratedFlag (joint, m_torque_zero[joint]);

		//
		printf ("torque calibration joint %d: %d\n", joint, m_torque_zero[joint]);

		// set also MEI gains.
		// set the command to the actual zero.
		int jnt = get_mei_info (joint, &meidx);

		if (joint >=3 && joint < NJOINTS)
		{
			meid_error_check(meid_set_command (meidx, jnt, m_torque_zero[joint]));
			meid_error_check(meid_set_filter (meidx, jnt, m_control_params[joint]));
		}
		else
		{
			meid_error_check(meid_set_position (meidx, jnt, 0.0));
			meid_error_check(meid_set_command (meidx, jnt, 0.0));

			meid_error_check(meid_set_filter (meidx, jnt, m_control_params[joint]));
		}
	}

	m_torso_shutdown_flag = false;

	Unlock ();
}

// Beware: this sets directly the mei parameters.
void YARPArmControl::SetMEIGain (int joint, int param, int16 value)
{
	Lock ();
	if (m_thread.GetTCalibratedFlag(joint))
	{
		int meidx = meidx0;
		int jnt = get_mei_info (joint, &meidx);

		meid_get_filter (meidx, jnt, m_control_params[joint]);
		m_control_params[joint][param] = value;
		meid_set_filter (meidx, jnt, m_control_params[joint]);
	}
	else
	{
		m_control_params[joint][param] = value;
	}
	Unlock ();
}

void YARPArmControl::GetMEIGain (int joint, int param, int16 *value)
{
	Lock ();

	int meidx = meidx0;
	int jnt = get_mei_info (joint, &meidx);

	if (m_thread.GetTCalibratedFlag (joint))
	{
		meid_get_filter (meidx, jnt, m_control_params[joint]);
		*value = m_control_params[joint][param];
	}
	else
	{
		*value = m_control_params[joint][param];
	}

	Unlock ();
}

// set all COEFFICIENTS.
void YARPArmControl::SetMEIGains (int joint, int16 * values) 
{
	Lock ();

	if (m_thread.GetTCalibratedFlag (joint))
	{
		int meidx = meidx0;
		int jnt = get_mei_info (joint, &meidx);

		memcpy (m_control_params[joint], values, sizeof(int16) * COEFFICIENTS);
		meid_set_filter (meidx, jnt, m_control_params[joint]);
	}
	else
	{
		memcpy (m_control_params[joint], values, sizeof(int16) * COEFFICIENTS);
	}

	Unlock ();
}

void YARPArmControl::GetMEIGains (int joint, int16 * values) 
{
	Lock ();

	if (m_thread.GetTCalibratedFlag (joint))
	{
		int meidx = meidx0;
		int jnt = get_mei_info (joint, &meidx);

		meid_get_filter (meidx, jnt, m_control_params[joint]);
		memcpy (values, m_control_params[joint], sizeof(int16) * COEFFICIENTS);
	}
	else
	{
		memcpy (values, m_control_params[joint], sizeof(int16) * COEFFICIENTS);
	}

	Unlock ();
}

// used for the torso only.
void YARPArmControl::RaiseMEIGains (void)
{
	const int NUMAXES = 3;
	Lock ();

	int16 i;
	int psum, pgoal;
	int16 allgains[NUMAXES][COEFFICIENTS];
	const double SHUTDOWN_VEL = 5000.0;
  
	if (m_torso_shutdown_flag == false)
	{
		Unlock ();
		printf ("YARPArmControl: MEI gains are already up\n");
	    return;
	}

	printf ("Raising gains...\n");

	m_torso_shutdown_flag = false;
  
	psum = 0;
	pgoal = 0;
	for(i = 0; i < NUMAXES; i++)
    {
		int meidx = meidx0;
		int jnt = get_mei_info (i, &meidx);

		meid_get_filter(meidx, jnt, &allgains[i][0]);
		psum += allgains[i][DF_P];
		// uses only 0,1,2 of m_control_params.
		allgains[i][DF_D] = m_control_params[i][DF_D];
		meid_set_filter(meidx, jnt, &allgains[i][0]);
		pgoal += m_control_params[i][DF_P];
    }
  
	int count = 0;
	while (pgoal - psum > 0)
	{
		psum = 0;
		for(i = 0; i < NUMAXES; i++)
		{
			int meidx = meidx0;
			int jnt = get_mei_info (i, &meidx);

			if (allgains[i][DF_P] < m_control_params[i][DF_P])
			{
				allgains[i][DF_P]++;
				meid_set_filter(meidx, jnt, &allgains[i][0]);
			}
			psum += allgains[i][DF_P];
		}

		count++;
		if (count < 100)
			YARPTime::DelayInSeconds (0.040);
	}

	for(i = 0; i < NUMAXES; i++)
	{
		int meidx = meidx0;
		int jnt = get_mei_info (i, &meidx);

		meid_get_filter(meidx, jnt, &allgains[i][0]);
		allgains[i][DF_I] = m_control_params[i][DF_I];
		meid_set_filter(meidx, jnt, &allgains[i][0]);
	}

	printf ("Gains are up\n");

	Unlock ();
	return;
}

void YARPArmControl::LowerMEIGains (void)
{
	Lock ();
	const int NUMAXES = 3;

	int16 i;
	int psum;
	int16 allgains[NUMAXES][COEFFICIENTS];
	
	if (m_torso_shutdown_flag == true)
	{
		printf ("YARPArmControl: gains are already down\n");
		Unlock ();
		return;
	}

	printf ("Decreasing gains...\n");

	m_torso_shutdown_flag = true;
  
	// start the gain decrement 
	psum = 0;
	for(i = 0; i < NUMAXES; i++)
	{
		int meidx = meidx0;
		int jnt = get_mei_info (i, &meidx);

		meid_get_filter(meidx, jnt, &allgains[i][0]);
		allgains[i][DF_I] = 0;
		psum += allgains[i][DF_P];
	}

	while(psum > 0)
	{
		psum = 0;
		for(i = 0; i < NUMAXES; i++)
		{
			int meidx = meidx0;
			int jnt = get_mei_info (i, &meidx);

			if (allgains[i][DF_P] > 0)
			{
				allgains[i][DF_P]--; // = allgains[i][DF_P] * 999 / 1000;
				meid_set_filter(meidx, jnt, &allgains[i][0]);
			}
			psum += allgains[i][DF_P];
		}

 		YARPTime::DelayInSeconds(0.020);
	}

	for(i = 0; i < NUMAXES; i++)
	{
		int meidx = meidx0;
		int jnt = get_mei_info (i, &meidx);

		meid_get_filter(meidx, jnt, &allgains[i][0]);
		allgains[i][DF_D] = 0;
		meid_set_filter(meidx, jnt, &allgains[i][0]);
	}

	printf ("Gains are zero now\n");
	Unlock ();

	return;
}

bool YARPArmControl::GetMEIGainStatus (void)
{
	bool ret;

	Lock ();
	ret = m_torso_shutdown_flag;
	Unlock ();

	return ret;
}

void YARPArmControl::PrintAxis (int joint)
{
	int meidx = meidx0;
	int jnt = get_mei_info (joint, &meidx);

	meid_print_axis (meidx, jnt);
}

#endif

