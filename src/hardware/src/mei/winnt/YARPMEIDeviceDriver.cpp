// $Id: YARPMEIDeviceDriver.cpp,v 1.14 2003-05-30 14:26:18 natta Exp $

#include "YARPMEIDeviceDriver.h"

// MEI
#define MEI_WINNT
#include <sys/pcdsp.h>
#define MEI_MSVC40		
#include <sys/medexp.h>		
#include <sys/idsp.h>
//////

YARPMEIDeviceDriver::YARPMEIDeviceDriver() :
YARPDeviceDriver<YARPNullSemaphore, YARPMEIDeviceDriver>(CBNCmds)
{
	// fill function pointer table
	m_cmds[CMDSetSpeed] = &YARPMEIDeviceDriver::setSpeed;
	m_cmds[CMDSetAcceleration] = &YARPMEIDeviceDriver::setAcceleration;
	m_cmds[CMDSetPosition] = &YARPMEIDeviceDriver::setPosition;
	m_cmds[CMDSetPID] = &YARPMEIDeviceDriver::setPid;
	m_cmds[CMDGetPosition] = &YARPMEIDeviceDriver::getPosition;

	m_cmds[CMDSetOutputPort] = &YARPMEIDeviceDriver::setOutputPort;
	m_cmds[CMDGetOutputPort] = &YARPMEIDeviceDriver::getOutputPort;
	m_cmds[CMDSetOutputBit] = &YARPMEIDeviceDriver::setOutputBit;
	m_cmds[CMDClearOutputBit] = &YARPMEIDeviceDriver::clearOutputBit;

	m_cmds[CMDSetOffset] = &YARPMEIDeviceDriver::setOffset;
	m_cmds[CMDSetOffsets] = &YARPMEIDeviceDriver::setOffsets;
		
	m_cmds[CMDSetSpeeds] = &YARPMEIDeviceDriver::setSpeeds;
	m_cmds[CMDSetAccelerations] = &YARPMEIDeviceDriver::setAccelerations;
	m_cmds[CMDSetPositions] = &YARPMEIDeviceDriver::setPositions;
	m_cmds[CMDGetPositions] = &YARPMEIDeviceDriver::getPositions;

	m_cmds[CMDBeginMotion] = &YARPMEIDeviceDriver::beginMotion;
	m_cmds[CMDBeginMotions] = &YARPMEIDeviceDriver::beginMotions;

	m_cmds[CMDDefinePositions] = &YARPMEIDeviceDriver::definePositions;
	m_cmds[CMDDefinePosition] = &YARPMEIDeviceDriver::definePosition;

	m_cmds[CMDStopAxes] = &YARPMEIDeviceDriver::stopAxes;
	m_cmds[CMDReadSwitches] = &YARPMEIDeviceDriver::readSwitches;

	m_cmds[CMDServoHere] = &YARPMEIDeviceDriver::dummy;

	m_cmds[CMDGetSpeeds] = &YARPMEIDeviceDriver::getSpeeds;

	m_cmds[CMDGetRefSpeeds] = &YARPMEIDeviceDriver::getRefSpeeds;
	m_cmds[CMDGetRefAccelerations] = &YARPMEIDeviceDriver::getRefAccelerations;
	m_cmds[CMDGetRefPositions] = &YARPMEIDeviceDriver::getRefPositions;
	m_cmds[CMDGetPID] = &YARPMEIDeviceDriver::getPid;
	m_cmds[CMDGetTorques] = &YARPMEIDeviceDriver::getTorques;

	m_cmds[CMDSetIntegratorLimits] = &YARPMEIDeviceDriver::setIntLimits;
	m_cmds[CMDSetTorqueLimits] = &YARPMEIDeviceDriver::setTorqueLimits;
	m_cmds[CMDSetTorqueLimit] = &YARPMEIDeviceDriver::setTorqueLimit;
	m_cmds[CMDSetIntegratorLimit] = &YARPMEIDeviceDriver::setIntLimit;
	m_cmds[CMDGetTorqueLimit] = &YARPMEIDeviceDriver::getTorqueLimit;
	m_cmds[CMDGetTorqueLimits] = &YARPMEIDeviceDriver::getTorqueLimits;
	m_cmds[CMDSetStopRate] = &YARPMEIDeviceDriver::setStopRate;
	m_cmds[CMDGetErrors] = &YARPMEIDeviceDriver::getErrors;

	m_cmds[CMDReadInput] = &YARPMEIDeviceDriver::readInput;

	m_cmds[CMDInitPortAsInput] = &YARPMEIDeviceDriver::initPortAsInput;
	m_cmds[CMDInitPortAsOutput] = &YARPMEIDeviceDriver::initPortAsOutput;
	m_cmds[CMDSetAmpEnableLevel] = &YARPMEIDeviceDriver::setAmpEnableLevel;
	m_cmds[CMDSetAmpEnable] = &YARPMEIDeviceDriver::setAmpEnable;

	m_cmds[CMDDisableAmp] = &YARPMEIDeviceDriver::disableAmp; 
	m_cmds[CMDEnableAmp] = &YARPMEIDeviceDriver::enableAmp;

	m_cmds[CMDControllerIdle] = &YARPMEIDeviceDriver::controllerIdle;
	m_cmds[CMDControllerRun] = &YARPMEIDeviceDriver::controllerRun;
	m_cmds[CMDClearStop] = &YARPMEIDeviceDriver::clearStop;

	m_cmds[CMDSetPositiveLevel] = &YARPMEIDeviceDriver::setPositiveLevel;
	m_cmds[CMDSetNegativeLevel] = &YARPMEIDeviceDriver::setNegativeLevel;
	m_cmds[CMDSetPositiveLimit] = &YARPMEIDeviceDriver::setPositiveLimit;
	m_cmds[CMDSetNegativeLimit] = &YARPMEIDeviceDriver::setNegativeLimit;
	m_cmds[CMDVMove] = &YARPMEIDeviceDriver::vMove;
	m_cmds[CMDSetCommands] = &YARPMEIDeviceDriver::setCommands;
	m_cmds[CMDSetCommand] = &YARPMEIDeviceDriver::setCommand;
	m_cmds[CMDCheckMotionDone] = &YARPMEIDeviceDriver::checkMotionDone;
	m_cmds[CMDWaitForMotionDone] = &YARPMEIDeviceDriver::waitForMotionDone;

	m_cmds[CMDSetHomeIndexConfig] = &YARPMEIDeviceDriver::setHomeIndexConfig;
	m_cmds[CMDSetHomeLevel] = &YARPMEIDeviceDriver::setHomeLevel;
	m_cmds[CMDSetHome] = &YARPMEIDeviceDriver::setHome;
	m_cmds[CMDSetStopRate] = &YARPMEIDeviceDriver::setStopRate;
			
	m_cmds[CMDDummy] = &YARPMEIDeviceDriver::dummy;

	_events = new int[CBNEvents];
	 for(int i = 0; i<CBNEvents; i++)
		_events[i] = NO_EVENT;
	
	_events[CBNoEvent] = NO_EVENT;
	_events[CBStopEvent] = STOP_EVENT;
	_events[CBEStopEvent] = E_STOP_EVENT;
	_events[CBAbortEvent] = ABORT_EVENT;
}

YARPMEIDeviceDriver::~YARPMEIDeviceDriver()
{
	delete [] _events;
}

int YARPMEIDeviceDriver::open(void *d)
{
	// temporarly removed
	int16 rc = 0;

	MEIOpenParameters *p = (MEIOpenParameters *)d;

	_njoints = p->nj;

	rc = dsp_init(p->meiPortAddr);	// init
	rc = dsp_reset();				// reset

	_ref_speeds = new double [_njoints];
	_ref_accs = new double [_njoints];
	_ref_positions = new double [_njoints];

	_all_axes = new int16[_njoints];
	int i;
	for(i = 0; i < _njoints; i++)
		_all_axes[i] = i;

	_filter_coeffs = new int16* [_njoints];
	for(i = 0; i < _njoints; i++)
		_filter_coeffs[i] = new int16 [COEFFICIENTS];

	_dsp_rate = dsp_sample_rate();
	
	return rc;
}

int YARPMEIDeviceDriver::close(void) 
{
	int16 rc = 0;

	delete [] _ref_speeds;
	delete [] _ref_accs;
	delete [] _ref_positions;
	
	for(int i = 0; i < _njoints; i++)
		delete [] _filter_coeffs[i];

	delete [] _filter_coeffs;
	delete [] _all_axes;

	_njoints = 0;
	
	return rc;
}

int YARPMEIDeviceDriver::setPosition(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	_ref_positions[tmp->axis] = *((double *)(tmp->parameters));
	
	int axis = tmp->axis;
	rc = start_move(axis, _ref_positions[axis], _ref_speeds[axis], _ref_accs[axis]);
	
	return rc;
}

int YARPMEIDeviceDriver::setSpeed(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	_ref_speeds[tmp->axis] = *((double *)(tmp->parameters));

	return rc;
}

int YARPMEIDeviceDriver::setAcceleration(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	_ref_accs[tmp->axis] = *((double *)(tmp->parameters));

	return rc;
}

int YARPMEIDeviceDriver::definePosition (void *cmd) 
{
	long rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double *pos = (double *)(tmp->parameters);

	rc = set_position(tmp->axis, *pos);

	return rc;
}

int YARPMEIDeviceDriver::getPositions(void *j)
{
	long rc = 0;

	double *out = (double *) j;

	for(int i = 0; i < _njoints; i++)
		rc = get_position(i, &out[i]);

	return rc;
}

int YARPMEIDeviceDriver::getRefPositions(void *j)
{
	long rc = 0;

	double *out = (double *) j;

	for(int i = 0; i < _njoints; i++)
		rc = get_command(i, &out[i]);

	return rc;
}

int YARPMEIDeviceDriver::getErrors(void *errs)
{
	long rc = 0;

	double *out = (double *) errs;

	for(int i = 0; i < _njoints; i++)
		rc = get_error(i, &out[i]);

	return rc;
}

int YARPMEIDeviceDriver::getTorques(void *trqs)
{
	long rc = 0;

	double *out = (double *) trqs;

	for(int i = 0; i < _njoints; i++)
	{	
		int16 tmp;
		get_dac_output(i, &tmp);
		out[i] = (double) (tmp);
	}

	return rc;
}

int YARPMEIDeviceDriver::getSpeeds(void *spds)
{
	long rc = 0;
	double *out = (double *) spds;
	
	DSP_DM vel;
	for(int i = 0; i < _njoints; i++) {
		P_DSP_DM current_v_addr = dspPtr->data_struct + DS_CURRENT_VEL + DS(i);
		pcdsp_transfer_block(dspPtr, TRUE, FALSE, current_v_addr, _njoints, &vel);
		out[i] =  vel*_dsp_rate;
	}

	return rc;
}

int YARPMEIDeviceDriver::getPosition(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	rc = get_position(axis, (double *)tmp->parameters);

	return rc;
}

int YARPMEIDeviceDriver::setPositions (void *param) 
{
	long rc = 0;

	memcpy(_ref_positions, param, sizeof(double)*_njoints);
	
	for(int i = 0; i < _njoints; i++)
		rc = start_move(i, _ref_positions[i], _ref_speeds[i], _ref_accs[i]);
	
	// this call is buggy
	// rc = start_move_all(_njoints, _all_axes, _ref_positions, _ref_speeds, _ref_accs);
	// wait_for_all(_njoints, _all_axes);

	return rc;
}

int YARPMEIDeviceDriver::setSpeeds (void *spds) 
{
	long rc = 0;

	memcpy(_ref_speeds, spds, sizeof(double)*_njoints);
	
	return rc;
}

int YARPMEIDeviceDriver::setAccelerations (void *param) 
{
	long rc = 0;

	memcpy(_ref_accs, param, sizeof(double)*_njoints);

	return rc;
}
	
int YARPMEIDeviceDriver::definePositions (void *param) 
{
	int16 rc = 0;

	double *cmds = (double *) param;
	for(int i = 0; i < _njoints; i++)
		rc = set_position(i, cmds[i]);

	return rc;
}

int YARPMEIDeviceDriver::setOutputPort(void *cmd)
{
	int16 rc = 0;
	IOParameters *par = (IOParameters *) cmd;
	rc = set_io(par->port, (short) par->value);
	
	return rc;
}

int YARPMEIDeviceDriver::getOutputPort(void *cmd)
{
	int16 rc = 0;
	IOParameters *par = (IOParameters *) cmd;
	rc = get_io(par->port, (short *) &par->value);
	
	return rc;
}

int YARPMEIDeviceDriver::setAmpEnableLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_amp_enable_level(axis, value);
	
	return rc;
}

int YARPMEIDeviceDriver::setAmpEnable(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_amp_enable(axis, value);

	return rc;
}

int YARPMEIDeviceDriver::setPositiveLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	ControlBoardEvents *event = (ControlBoardEvents *) tmp->parameters;
	rc = set_positive_limit(axis, _events[*event]);

	return rc;
}

int YARPMEIDeviceDriver::setNegativeLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	ControlBoardEvents *event = (ControlBoardEvents *) tmp->parameters;
	rc = set_negative_limit(axis, _events[*event]);
	
	return rc;
}

int YARPMEIDeviceDriver::setPositiveLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_positive_level(axis, value);

	return rc;
}

int YARPMEIDeviceDriver::setNegativeLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_negative_level(axis, value);
	
	return rc;
}

int YARPMEIDeviceDriver::enableAmp(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = enable_amplifier(*ax);

	return rc;
}

int YARPMEIDeviceDriver::disableAmp(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = disable_amplifier(*ax);

	return rc;
}

int YARPMEIDeviceDriver::controllerRun(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	while(!motion_done(*ax));	// required otherwise controller_run may fail
	rc = controller_run(*ax);
	while(!motion_done(*ax));	//LATER: check this
	
	return rc;
}

int YARPMEIDeviceDriver::clearStop(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = clear_status(*ax);
	// while(!motion_done(*ax));	//LATER: check this
	
	return rc;
}

int YARPMEIDeviceDriver::setStopRate(void *cmd) 
{
	int16 rc = 0;
	
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	double rate = *((double *)(tmp->parameters));
	rc = set_stop_rate(tmp->axis, rate);

	return rc;
}

int YARPMEIDeviceDriver::controllerIdle(void *axis)
{
	int16 rc = 0;
	int16 *ax = (int16 *) axis;
	rc = controller_idle(*ax);
	while(!motion_done(*ax));	//LATER: check this

	return rc;
}

int YARPMEIDeviceDriver::setOutputBit(void *n)
{
	// LATER
	return 0;
}

int YARPMEIDeviceDriver::clearOutputBit(void *n)
{
	// LATER
	return 0;
}

int YARPMEIDeviceDriver::setOffset(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double val = *((double *) tmp->parameters);

	_filter_coeffs[axis][DF_OFFSET] = (int16) round(val);
	rc = set_filter(axis, _filter_coeffs[axis]);
	
	return rc;
}

int YARPMEIDeviceDriver::setOffsets(void *offs)
{
	int16 rc = 0;

	double *cmd = (double *) offs;

	for(int i = 0; i<_njoints; i++)
	{
		_filter_coeffs[i][DF_OFFSET] = (int16) round(cmd[i]);
		rc = set_filter(i, _filter_coeffs[i]);
	}
	
	return rc;
}

int YARPMEIDeviceDriver::setTorqueLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double val = *((double *) tmp->parameters);

	_filter_coeffs[axis][DF_DAC_LIMIT] = (int16) round(val);
	rc = set_filter(axis, _filter_coeffs[axis]);
	
	return rc;
}

int YARPMEIDeviceDriver::getTorqueLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double *val = ((double *) tmp->parameters);

	*val = _filter_coeffs[axis][DF_DAC_LIMIT];
		
	return rc;
}

int YARPMEIDeviceDriver::getTorqueLimits(void *trqs)
{
	int16 rc = 0;
	double *out = (double *) trqs;

	for (int i = 0; i<_njoints; i++)
		out[i] = _filter_coeffs[i][DF_DAC_LIMIT];
	
	return rc;
}

int YARPMEIDeviceDriver::setTorqueLimits(void *trqs)
{
	int16 rc = 0;

	double *cmd = (double *) trqs;

	for(int i = 0; i<_njoints; i++)
	{
		_filter_coeffs[i][DF_DAC_LIMIT] = (int16) round(cmd[i]);
		rc = set_filter(i, _filter_coeffs[i]);
	}
	
	return rc;
}

int YARPMEIDeviceDriver::setIntLimits(void *lmts)
{
	int16 rc = 0;

	double *cmd = (double *) lmts;

	for(int i = 0; i<_njoints; i++)
	{
		_filter_coeffs[i][DF_I_LIMIT] = (int16) round(cmd[i]);
		rc = set_filter(i, _filter_coeffs[i]);
	}
	
	return rc;
}

int YARPMEIDeviceDriver::setIntLimit(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	double val = *((double *) tmp->parameters);

	_filter_coeffs[axis][DF_I_LIMIT] = (int16) round(val);
	rc = set_filter(axis, _filter_coeffs[axis]);
	
	return rc;
}

YARPMEIDeviceDriver::readInput(void *input)
{
	int16 rc = 0;	
	// LATER
	return rc;
}

int YARPMEIDeviceDriver::dummy(void *d)
{
	int16 rc = 0;
	
	return rc;
}

int YARPMEIDeviceDriver::beginMotion(void *cmd)
{
	int16 rc = 0;
	// OBSOLETE
	return rc;
}

int YARPMEIDeviceDriver::beginMotions(void *cmd)
{
	int16 rc = 0;
	// OBSOLETE
	return rc;
}

int YARPMEIDeviceDriver::stopAxes(void *par)
{
	int16 rc = 0;
	// LATER
	return rc;
}

int YARPMEIDeviceDriver::readSwitches(void *switches)
{
	int16 rc = 0;
	// LATER
	return rc;
}

int YARPMEIDeviceDriver::getRefSpeeds(void *spds)
{
	int16 rc = 0;

	memcpy(spds, _ref_speeds, sizeof(double) * _njoints);

	return rc;
}

int YARPMEIDeviceDriver::getRefAccelerations(void *accs)
{
	int16 rc = 0;

	memcpy(accs, _ref_accs, sizeof(double) * _njoints);

	return rc;
}

int YARPMEIDeviceDriver::initPortAsInput(void *p)
{
	int16 rc = 0;
	int *port = (int *) p;
	rc = init_io(*port, IO_INPUT);
	return rc;
}

int YARPMEIDeviceDriver::initPortAsOutput(void *p)
{
	int16 rc = 0;
	int *port = (int *) p;
	rc = init_io(*port, IO_OUTPUT);
	return rc;
}

int YARPMEIDeviceDriver::vMove(void *spds)
{
	int16 rc = 0;
	double *cmds = (double *) spds;
	for(int i = 0; i < _njoints; i++)
		rc = v_move(i, cmds[i], _ref_accs[i]);

	return rc;
}

int YARPMEIDeviceDriver::setCommands(void *pos)
{
	int16 rc = 0;
	double *tmpPos = (double *) pos;
	for(int i = 0; i < _njoints; i++)
		rc = set_command(i, tmpPos[i]);

	return rc;
}

int YARPMEIDeviceDriver::setCommand(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	double *tmpPos = (double *) tmp->parameters;
	
	rc = set_command(axis, *tmpPos);

	return rc;
}

int YARPMEIDeviceDriver::checkMotionDone(void *flag)
{
	int16 rc = 0;
	
	bool *tmp = (bool *) flag;
	*tmp = true;

	for(int i = 0; i < _njoints; i++)
		 *tmp = *tmp && motion_done(i);
	
	return rc;
}

int YARPMEIDeviceDriver::waitForMotionDone(void *flag)
{
	int16 rc = 0;

	for(int i = 0; i < _njoints; i++)
		while(!motion_done(i)) ACE_OS::sleep(ACE_Time_Value(0,50000));
	
	return rc;
}

int YARPMEIDeviceDriver::setHomeIndexConfig(void *cmd)
{
	int16 rc;
	SingleAxisParameters *par = (SingleAxisParameters *) cmd;
	int conf = *(int *)(par->parameters);
	int16 config;
	switch (conf)
	{
	case CBHomeOnly:
		config = HOME_ONLY;
		break;
	case CBIndexOnly:
		config = INDEX_ONLY;
		break;
	case CBHighHomeAndIndex:
		config = HIGH_HOME_AND_INDEX;
		break;
	case CBLowHomeAndIndex:
	default:
		config = LOW_HOME_AND_INDEX;
	}
	rc = set_home_index_config(par->axis, config);
	return rc;
}

int YARPMEIDeviceDriver::setHomeLevel(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int16 value = *((int16 *)tmp->parameters);
	rc = set_home_level(tmp->axis, value);
	return rc;
}

int YARPMEIDeviceDriver::setHome(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	ControlBoardEvents *event = (ControlBoardEvents *) tmp->parameters;
	int16 ev = _events[*event];
	rc = set_home(axis, ev);
	return rc;
}

int YARPMEIDeviceDriver::setPid(void *cmd)
{
	int16 rc = 0;

	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;

	// these are stored to be used later in the setOffsets/setOffset functions
	_filter_coeffs[tmp->axis][DF_P] = (int16) round(pid->KP);
	_filter_coeffs[tmp->axis][DF_I] = (int16) round(pid->KI);
	_filter_coeffs[tmp->axis][DF_D] = (int16) round(pid->KD);
	_filter_coeffs[tmp->axis][DF_ACCEL_FF] = (int16) round (pid->AC_FF);
	_filter_coeffs[tmp->axis][DF_VEL_FF] = (int16) round(pid->VEL_FF);
	_filter_coeffs[tmp->axis][DF_I_LIMIT] = (int16) round(pid->I_LIMIT);
	_filter_coeffs[tmp->axis][DF_OFFSET] = (int16) round(pid->OFFSET);
	_filter_coeffs[tmp->axis][DF_DAC_LIMIT] = (int16) round(pid->T_LIMIT);
	_filter_coeffs[tmp->axis][DF_SHIFT] = (int16) round(pid->SHIFT);
	_filter_coeffs[tmp->axis][DF_FRICT_FF] = (int16) round(pid->FRICT_FF);
	
	rc = set_filter(tmp->axis, _filter_coeffs[tmp->axis]);

	return rc;
}

int YARPMEIDeviceDriver::getPid(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	LowLevelPID *pid = (LowLevelPID *) tmp->parameters;
	int axis = tmp->axis;
	
	get_filter(axis, _filter_coeffs[axis]);

	pid->KP = _filter_coeffs[axis][DF_P];
	pid->KI = _filter_coeffs[axis][DF_I];
	pid->KD = _filter_coeffs[axis][DF_D];
	pid->AC_FF = _filter_coeffs[axis][DF_ACCEL_FF];
	pid->VEL_FF = _filter_coeffs[axis][DF_VEL_FF];
	pid->I_LIMIT = _filter_coeffs[axis][DF_I_LIMIT];
	pid->OFFSET = _filter_coeffs[axis][DF_OFFSET];
	pid->T_LIMIT = _filter_coeffs[axis][DF_DAC_LIMIT];
	pid->SHIFT = _filter_coeffs[axis][DF_SHIFT];
	pid->FRICT_FF = _filter_coeffs[axis][DF_FRICT_FF];
	
	return rc;
}