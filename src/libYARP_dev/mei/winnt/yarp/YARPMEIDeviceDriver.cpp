/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPMEIDeviceDriver.cpp,v 1.4 2006-02-20 12:45:09 gmetta Exp $
///

#include "YARPMEIDeviceDriver.h"

// MEI
#define MEI_WINNT
#include "../dd_orig/include/pcdsp.h"
#define MEI_MSVC40		
#include "../dd_orig/include/medexp.h"		
#include "../dd_orig/include/idsp.h"
//////

YARPMEIDeviceDriver::YARPMEIDeviceDriver() :
YARPDeviceDriver<YARPNullSemaphore, YARPMEIDeviceDriver>(CBNCmds), implemented(false)
{
	// fill function pointer table
	m_cmds[CMDSetSpeed] = &YARPMEIDeviceDriver::setSpeed;
	m_cmds[CMDSetAcceleration] = &YARPMEIDeviceDriver::setAcceleration;
	m_cmds[CMDSetPosition] = &YARPMEIDeviceDriver::setPosition;
	m_cmds[CMDSetPID] = &YARPMEIDeviceDriver::setPid;
	m_cmds[CMDGetPosition] = &YARPMEIDeviceDriver::getPosition;
	m_cmds[CMDSetPortValue] = &YARPMEIDeviceDriver::setOutputPort;
	m_cmds[CMDGetPortValue] = &YARPMEIDeviceDriver::getOutputPort;
	m_cmds[CMDSetOutputBit] = &YARPMEIDeviceDriver::setOutputBit;
	m_cmds[CMDClearOutputBit] = &YARPMEIDeviceDriver::clearOutputBit;
	m_cmds[CMDSetOffset] = &YARPMEIDeviceDriver::setOffset;
	m_cmds[CMDSetOffsets] = &YARPMEIDeviceDriver::setOffsets;
	m_cmds[CMDSetSpeeds] = &YARPMEIDeviceDriver::setSpeeds;
	m_cmds[CMDSetAccelerations] = &YARPMEIDeviceDriver::setAccelerations;
	m_cmds[CMDSetPositions] = &YARPMEIDeviceDriver::setPositions;
	m_cmds[CMDGetPositions] = &YARPMEIDeviceDriver::getPositions;
	m_cmds[CMDDefinePositions] = &YARPMEIDeviceDriver::definePositions;
	m_cmds[CMDDefinePosition] = &YARPMEIDeviceDriver::definePosition;
	m_cmds[CMDStopAxes] = &YARPMEIDeviceDriver::stopAxes;
	//m_cmds[CMDReadSwitches] = &YARPMEIDeviceDriver::readSwitches;
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
	m_cmds[CMDGetPIDErrors] = &YARPMEIDeviceDriver::getErrors;
	//m_cmds[CMDReadInput] = &YARPMEIDeviceDriver::readInput;
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
	m_cmds[CMDVMove] = &YARPMEIDeviceDriver::safeVMove;
	m_cmds[CMDSetCommands] = &YARPMEIDeviceDriver::setCommands;
	m_cmds[CMDSetCommand] = &YARPMEIDeviceDriver::setCommand;
	m_cmds[CMDCheckMotionDone] = &YARPMEIDeviceDriver::checkMotionDone;
	m_cmds[CMDWaitForMotionDone] = &YARPMEIDeviceDriver::waitForMotionDone;
	m_cmds[CMDSetHomingBehavior] = &YARPMEIDeviceDriver::setHomeProcedure;
	m_cmds[CMDGetAnalogChannel] = &YARPMEIDeviceDriver::readAnalog;
	m_cmds[CMDSetAxisAnalog] = &YARPMEIDeviceDriver::setAxisAnalog;

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
	// temporarily removed
	int16 rc = 0;

	MEIOpenParameters *p = (MEIOpenParameters *)d;

	_njoints = p->nj;

	rc = dsp_init(p->meiPortAddr);	// init
	rc = dsp_reset();				// reset

	_ref_speeds = new double [_njoints];
	ACE_ASSERT (_ref_speeds != NULL);
	_ref_accs = new double [_njoints];
	ACE_ASSERT (_ref_accs != NULL);
	_ref_positions = new double [_njoints];
	ACE_ASSERT (_ref_positions != NULL);
	_all_axes = new int16[_njoints];
	ACE_ASSERT (_all_axes != NULL);

	int i;
	for(i = 0; i < _njoints; i++)
		_all_axes[i] = i;

	_filter_coeffs = new int16* [_njoints];
	ACE_ASSERT (_filter_coeffs != NULL);
	for(i = 0; i < _njoints; i++)
	{
		_filter_coeffs[i] = new int16 [COEFFICIENTS];
		ACE_ASSERT (_filter_coeffs[i] != NULL);
	}

	_dsp_rate = dsp_sample_rate();
	
	_winding = new int16[_njoints];
	ACE_ASSERT (_winding != NULL);
	memset (_winding, 0, sizeof(int16) * _njoints);

	_16bit_oldpos = new double[_njoints];
	ACE_ASSERT (_16bit_oldpos != NULL);
	memset (_16bit_oldpos, 0, sizeof(double) * _njoints);

	_position_zero = new double[_njoints];
	ACE_ASSERT (_position_zero != NULL);
	memset (_position_zero, 0, sizeof(double) * _njoints);

	int mask = p->ioPorts;
	for (i = 0; i < MAX_PORTS; i++)
	{
		if (mask & 0x1)
			init_io (i, IO_OUTPUT);
		else
			init_io (i, IO_INPUT);

		mask >>= 1;
	}

	return rc;
}

int YARPMEIDeviceDriver::close(void) 
{
	int16 rc = 0;

	if (_ref_speeds != NULL) delete [] _ref_speeds;
	if (_ref_accs != NULL) delete [] _ref_accs;
	if (_ref_positions != NULL) delete [] _ref_positions;
	
	for(int i = 0; i < _njoints; i++)
		if (_filter_coeffs[i] != NULL) delete [] _filter_coeffs[i];

	if (_filter_coeffs != NULL) delete [] _filter_coeffs;
	if (_all_axes != NULL) delete [] _all_axes;

	if (_winding != NULL) delete[] _winding;
	if (_16bit_oldpos != NULL) delete[] _16bit_oldpos;

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

	/// this is to reset the encoder ref value.
	/// LATER: need to verify whether summing pos is the right thing to do.
	_position_zero[tmp->axis] = double(dsp_encoder (tmp->axis)) + *pos;
	_winding[tmp->axis] = 0;

	return rc;
}

int YARPMEIDeviceDriver::getPositions(void *j)
{
	///long rc = 0;
	double *out = (double *) j;

	for(int i = 0; i < _njoints; i++)
	{
		/// direct reading from the DSP (fast!).
		out[i] = double(dsp_encoder (i));

		/// computing the actual encoder value.
		if (_sgn(out[i]) < 0 && _16bit_oldpos[i] > 16384.0 && _winding[i] == 0)
		{
			_winding[i] = 1;
		}
		else
		if (_sgn(out[i]) > 0 && _16bit_oldpos[i] < -16384.0 && _winding[i] == 1)
		{
			_winding[i] = 0;
		}
		else
		if (_sgn(out[i]) > 0 && _16bit_oldpos[i] < -16384.0 && _winding[i] == 0)
		{
			_winding[i] = -1;
		}
		else
		if (_sgn(out[i]) < 0 && _16bit_oldpos[i] > 16384.0 && _winding[i] == -1)
		{
			_winding[i] = 0;
		}

		_16bit_oldpos[i] = out[i];

		switch (_winding[i])
		{
			case 1:
				out[i] = 65535.0 + out[i] - _position_zero[i];
				break;

			case -1:
				out[i] = -65536.0 + out[i] - _position_zero[i];
				break;

			case 0:
				out[i] -= _position_zero[i];
				break;
		}
	}

	return 0;
}

int YARPMEIDeviceDriver::getRefPositions(void *j)
{
	long rc = 0;

	double *out = (double *) j;

	for(int i = 0; i < _njoints; i++)
		rc = get_command(i, &out[i]);

	return rc;
}

// returns the position error.
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
		pcdsp_transfer_block(dspPtr, TRUE, FALSE, current_v_addr, 1, &vel);
		out[i] =  vel*_dsp_rate;
	}

	return rc;
}

int YARPMEIDeviceDriver::getPosition(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;

	///rc = get_position(axis, (double *)tmp->parameters);
	double tmpd;
	tmpd = double(dsp_encoder (axis));

	/// computing the actual encoder value.
	if (_sgn(tmpd) < 0 && _16bit_oldpos[axis] > 16384.0 && _winding[axis] == 0)
	{
		_winding[axis] = 1;
	}
	else
	if (_sgn(tmpd) > 0 && _16bit_oldpos[axis] < -16384.0 && _winding[axis] == 1)
	{
		_winding[axis] = 0;
	}
	else
	if (_sgn(tmpd) > 0 && _16bit_oldpos[axis] < -16384.0 && _winding[axis] == 0)
	{
		_winding[axis] = -1;
	}
	else
	if (_sgn(tmpd) < 0 && _16bit_oldpos[axis] > 16384.0 && _winding[axis] == -1)
	{
		_winding[axis] = 0;
	}

	_16bit_oldpos[axis] = tmpd;

	switch (_winding[axis])
	{
		case 1:
			tmpd = 65535.0 + tmpd - _position_zero[axis];
			break;

		case -1:
			tmpd = -65536.0 + tmpd - _position_zero[axis];
			break;

		case 0:
			tmpd -= _position_zero[axis];
			break;
	}

	*((double *)tmp->parameters) = tmpd;
	return rc;
}

int YARPMEIDeviceDriver::readAnalog(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;

	rc = read_axis_analog(axis, (int16 *)tmp->parameters);

	return rc;
}

int YARPMEIDeviceDriver::setAxisAnalog(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int axis = tmp->axis;
	int16 value = *( (int16*) tmp->parameters);

	rc = set_axis_analog(axis, value);

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
	{
		rc = set_position(i, cmds[i]);

		/// this is to reset the encoder ref value.
		/// LATER: need to verify whether summing cmds[i] is the right thing to do.
		_position_zero[i] = double(dsp_encoder (i)) + cmds[i];
		_winding[i] = 0;
	}

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
	ACE_ASSERT (!implemented);
	return 0;
}

int YARPMEIDeviceDriver::clearOutputBit(void *n)
{
	ACE_ASSERT (!implemented);
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
	ACE_ASSERT (!implemented);
	return rc;
}

int YARPMEIDeviceDriver::dummy(void *d)
{
	int16 rc = 0;
	return rc;
}

int YARPMEIDeviceDriver::stopAxes(void *par)
{
	int16 rc = 0;
	ACE_ASSERT (!implemented);
	return rc;
}

int YARPMEIDeviceDriver::readSwitches(void *switches)
{
	int16 rc = 0;
	ACE_ASSERT (!implemented);
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

int YARPMEIDeviceDriver::safeVMove(void *spds)
{
	int16 rc = 0;
	double *cmds = (double *) spds;
	for(int i = 0; i < _njoints; i++)
	{
		if (!frames_left(i))
			rc = v_move(i, cmds[i], _ref_accs[i]);
	}

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

int YARPMEIDeviceDriver::waitForMotionDone(void *cmd)
{
	int16 rc = 0;

	int time = *((int *) cmd);

	if (time != 0) {
		for(int i = 0; i < _njoints; i++)
			while(!motion_done(i)) ACE_OS::sleep(ACE_Time_Value(0,time));
	}
	else {
		for(int i = 0; i < _njoints; i++)
		while(!motion_done(i));	// busy waiting
	}

	return rc;
}


int YARPMEIDeviceDriver::waitForFramesLeft(void *cmd)
{
	int16 rc = 0;

	int time = *((int *) cmd);

	if (time != 0) {
		for(int i = 0; i < _njoints; i++)
			while(frames_left(i)) ACE_OS::sleep(ACE_Time_Value(0,time));
	}
	else {
		for(int i = 0; i < _njoints; i++)
		while(frames_left(i));	// busy waiting
	}

	return rc;
}

int YARPMEIDeviceDriver::checkFramesLeft(void *flag)
{
	int16 rc = 0;
	
	bool *tmp = (bool *) flag;
	*tmp = true;

	for(int i = 0; i < _njoints; i++)
		 *tmp = *tmp && frames_left(i);
	
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

int YARPMEIDeviceDriver::setHomeProcedure(void *cmd)
{
	int16 rc = 0;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	int16 ev = _events[*(ControlBoardEvents *)tmp->parameters];

	SingleAxisParameters x;
	int ipar;
	double dpar;

	x.axis = tmp->axis;
	x.parameters = &ipar;
	ipar = CBIndexOnly;			// index_only
	setHomeIndexConfig(&cmd);
	ipar = 0;					// (active low)
	setHomeLevel(&cmd);

	rc = set_home(tmp->axis, ev);

	x.parameters = &dpar;
	dpar = 50000.0;				// stop rate (acc)
	setStopRate(&cmd);

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