/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPBabybotHand.h,v 1.7 2004-01-17 00:15:15 gmetta Exp $
///
///

//
//
// Originally feb 2003 -- by nat
//
// adapted for yarp June 2003 -- by nat

#ifndef __YARPBABYBOTHANDH__
#define __YARPBABYBOTHANDH__

#include <ace/log_msg.h>
#include "YARPGalilOnBabybotHandAdapter.h"
#include "YARPNIDAQOnBabybotHandAdapter.h"
#include <YARPConfigFile.h>
#include <YARPSemaphore.h>

#define YARP_BABYBOT_HAND_VERBOSE

#ifdef YARP_BABYBOT_HAND_VERBOSE
#define YARP_BABYBOT_HAND_DEBUG(string) YARP_DEBUG("YARP_BABYBOT_HAND_DEBUG:", string)
#else  YARP_BABYBOT_HAND_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <math.h>

class YARPTorqueControl
{
public:
	YARPTorqueControl()
	{
		_maxTime = 0;
		_thresholds = NULL;
		_limit = NULL;
		_time = NULL;
	}
	~YARPTorqueControl()
	{
		if (_thresholds != NULL)
			delete [] _thresholds;
		if (_limit != NULL)
			delete [] _limit;
		if (_time != NULL)
			delete [] _time;
	}

	void resize(int nj, unsigned int maxT, const double *thr)
	{
		_nj = nj;
		_maxTime = maxT;
		_time = 0;
		_limit = false;

		_thresholds = new double [_nj];
		_limit = new bool [_nj];
		_time = new unsigned int [_nj];

		for(int i = 0; i < _nj; i++)
		{
			_thresholds[i] = fabs(thr[i]);
			_limit[i] = false;
			_time[i] = 0;
		}
		
	}

	bool update(const double *tr, double *max)
	{
		int i;
		bool ret = false;
		for (i = 0; i < _nj; i++)
		{
			if (_limit[i])
			{
				ret = true;
			}
			else 
			{
				if (fabs(tr[i]) > _thresholds[i])
					_time[i]++;

				if (_time[i]>_maxTime)
				{
					_limit[i] = true;
					ret = true;
					max[i] = _thresholds[i];
				}
			}
		}
		return ret;
	}

	void reset()
	{
		int i;
		for(i = 0; i < _nj; i++)
		{ 
			_time[i] = 0;
			_limit[i] = false;
		}
	}

	unsigned int _maxTime;
	unsigned int *_time;
	bool *_limit;
	double *_thresholds;
	int _nj;
};

template <class ADAPTERHALL, class ADAPTERGALIL, class PARAMETERS>
class YARPBabybotHandTemplate
{
public:
	YARPBabybotHandTemplate()
	{
		_initialized = false;
	}

	~YARPBabybotHandTemplate()
	{
		if (_initialized)
			uninitialize();

	}

	void _alloc()
	{
		_hall_encoders_raw = new double [_parameters._nidaq_ch];
		_optic_encoders = new double [_parameters._naj];
		_optic_encoders_raw = new double [_parameters._naj];
		_optic_encoders_sp_raw = new double [_parameters._naj];
		_reference_positions = new double [_parameters._naj];
		_reference_speeds = new double [_parameters._naj];
		_reference_accs = new double [_parameters._naj];
		_motor_errors = new double [_parameters._naj];
		_switches = new char [_parameters._naj];
		_is_moving = new char [_parameters._naj];

		_optic_encoder_zeros = new double [_parameters._naj];

//		_tmp_command_int = new double [_parameters._naj];
		_tmp_command_double = new double [_parameters._naj];
		_tmp_command_short = new short [_parameters._naj];
		_tmp_command_char = new char [_parameters._naj];
	
		_offsets = new double [_parameters._naj];
		_motor_torques = new double [_parameters._naj];
		_max_torques = new double [_parameters._naj];
	}
	void _falloc()
	{
		delete [] _hall_encoders_raw;
		delete [] _optic_encoders;
		delete [] _optic_encoders_raw;
		delete [] _optic_encoders_sp_raw;
		delete [] _reference_positions;
		delete [] _reference_speeds;
		delete [] _reference_accs;
		delete [] _motor_errors;
		delete [] _switches;
		delete [] _is_moving;

		delete [] _optic_encoder_zeros;

//		delete [] _tmp_command_int;
		delete [] _tmp_command_double;
		delete [] _tmp_command_short;
		delete [] _tmp_command_char;
	
		delete [] _offsets;
		delete [] _motor_torques;
		delete [] _max_torques;
	}

		// refresh local copies
	int input()
	{
		int rc = 0;
		int i;
		lock();
		
		// read Galil
		rc = _control_board.IOCtl(CMDGetPositions, _tmp_command_double);
		// _convert_and_decouple_output_raw(_tmp_command_int, _optic_encoders_raw);
		_convert_output_raw(_tmp_command_double, _optic_encoders_raw);
		_convert_and_decouple_output(_tmp_command_double, _optic_encoders);
		
		rc = _control_board.IOCtl(CMDGetSpeeds, _tmp_command_double);
		_convert_and_decouple_output_raw(_tmp_command_double, _optic_encoders_sp_raw);
		
		/*
		for(i = 0; i < 6; i++)
			printf("%lf ", _optic_encoders_sp_raw[i]);
		printf("\n");*/
		// _convert_output_raw(_tmp_command_double, _optic_encoders_sp_raw);
		// _convert_output_raw(_tmp_command_int, _optic_encoders_sp_raw);

		rc = _control_board.IOCtl(CMDGetTorques, _tmp_command_double);
		_convert_output_raw(_tmp_command_double, _motor_torques);

		// errors
		rc = _control_board.IOCtl(CMDGetErrors, _tmp_command_double);
		_convert_output_raw(_tmp_command_double, _motor_errors);

		// reference positions
		rc = _control_board.IOCtl(CMDGetRefPositions, _tmp_command_double);
		_convert_output_raw(_tmp_command_double, _reference_positions);

		// switches // LATER: this should go in the adapter
		rc = _control_board.IOCtl(CMDReadSwitches, _tmp_command_char);
		_convert_output_raw(_tmp_command_char, _switches);
		_moving = false;
		for(i = 0; i < _naj; i++)
		{
			_is_moving[i] = ((char) 0x80 & _switches[i]);
			_moving = _moving || _is_moving[i];
		}

		// read NIDAQ
		rc = _hall_sensors.IOCtl(CMDAIVReadScan, _hall_encoders_raw);

		unlock();
		return rc;
	}

	// start motion (all axes)
	int output()
	{
		lock();
		if (_torqueControl.update(_motor_torques, _max_torques))
		{
			_convert_input_raw(_max_torques, _tmp_command_double);
			_control_board.IOCtl(CMDSetTorqueLimits, _tmp_command_double);
		}
		unlock();
		return 0;
	}

	int idleMode()
	{	
		int ret;
		lock();
		ret = _control_board.idleMode();
		unlock();
		return ret;
	}

	int activatePID()
	{
		lock();
		int ret = _control_board.activatePID();
		unlock();
		return ret;
	}

	int stopAxes()
	{
		lock();
		int ret = _control_board.stop();
		unlock();
		return ret; 
	}
	int initialize(const YARPString path, const YARPString &init_file)
	{
		int rc = 0;
		if (_initialized)
			return YARP_FAIL;;

		_parameters.load(path, init_file);
		_naj = _parameters._naj;
		_nidaq_ch = _parameters._nidaq_ch;

		_alloc();

		// initialize vectors
		memset(_optic_encoder_zeros, 0, _naj*sizeof(_optic_encoder_zeros[0]));
		memset(_offsets, 0, _naj*sizeof(_offsets[0]));
		memset(_optic_encoders_raw, 0, _naj*sizeof(_optic_encoders_raw[0]));
		memset(_hall_encoders_raw, 0, _naj*sizeof(_hall_encoders_raw[0]));
		memset(_motor_errors, 0, _naj*sizeof(_motor_errors[0]));
		memset(_switches, 0, _naj*sizeof(_switches[0]));
		memset(_is_moving, 9, _naj*sizeof(_is_moving[0]));
		memset(_reference_positions, 9, _naj*sizeof(_reference_positions[0]));

		memcpy(_max_torques, _parameters._torque_limits, _naj*sizeof(double));
	
		_torqueControl.resize(_naj, _parameters._max_time, _parameters._torque_thresholds);
		// open galil
		_control_board.initialize(&_parameters);
		// torque limits
		_control_board.IOCtl(CMDSetTorqueLimits, _max_torques);
		// set idel mode
		rc = idleMode();

		// clear offsets
		_convert_input_raw(_offsets, _tmp_command_double);
		rc = _control_board.IOCtl(CMDSetOffsets, _tmp_command_double);

		// stop axes
		rc = stopAxes();
	
		// reset encoders
		rc = resetEncoders();

		rc = _control_board.activatePID();
	
		// initial speeds and accelerations
		setSpeedsRaw(_parameters._speeds);
		setAccelerationsRaw(_parameters._accelerations);
		setTorqueLimits(_parameters._torque_limits);
		setIntegratorLimits(_parameters._int_limits);

		// open nidaq
		rc = _hall_sensors.initialize(&_parameters);
		
		_initialized = true;

		return rc;
	}

	int uninitialize()
	{	
		int rc = 0;
		if (!_initialized)
			return YARP_FAIL;

		lock();
		
		_control_board.uninitialize();
		_hall_sensors.uninitialize();
				
		_initialized = false;

		_falloc();
		unlock();
		return 0;
	}

	int getPositions(double *pos)
	{
		lock();
		memcpy(pos, _optic_encoders, _naj*sizeof(_optic_encoders[0]));
		unlock();
		return 0;
	}

	int getPositionsRaw(double *pos)
	{
		lock();
		memcpy(pos, _optic_encoders_raw, _naj*sizeof(_optic_encoders_raw[0]));
		unlock();
		return 0;
	}

	int getMotorErrors(double *errs)
	{
		lock();
		memcpy(errs, _motor_errors, _naj*sizeof(_motor_errors[0]));
		unlock();
		return 0;
	}

	int getHallRaw(double *h)
	{
		lock();
		memcpy(h, _hall_encoders_raw, _nidaq_ch*sizeof(_hall_encoders_raw[0]));
		unlock();
		return 0;
	}

	int getInput(unsigned char *i)
	{
		lock();
		*i = _input;
		unlock();
		return 0;
	}

	int getSpeedsRaw(double *spds)
	{
		lock();
		memcpy(spds, _optic_encoders_sp_raw, _naj*sizeof(_optic_encoders_sp_raw[0]));
		unlock();
		return 0;
	}

	int getPID(LowLevelPID &pid, int axis)
	{
		int rc = 0;

		SingleAxisParameters cmd;
		cmd.axis = _axes_map(axis);
		cmd.parameters = &pid;
		rc = _control_board.IOCtl(CMDGetPID, &cmd);
	
		return rc;
	}

	int velocityMoves(const double *sp)
	{
		int rc = 0;
		lock();
		_control_board.stop();
		// reset torque control
		_torqueControl.reset();
		_control_board.IOCtl(CMDSetTorqueLimits, _parameters._torque_limits);
		// apply commands
		_convert_and_couple_input_raw(_reference_accs, _tmp_command_double);
		rc = _control_board.IOCtl(CMDSetAccelerations, _tmp_command_double);
		_convert_and_couple_input_raw(sp, _tmp_command_double);
		rc = _control_board.IOCtl(CMDVMove, _tmp_command_double);
		unlock();
		return rc;
	}

	int setPositionsRaw(const double *pos)
	{
		int rc = 0;
		lock();

		double tmp_speeds[6];
		double tmp_accs[6];
		double tmp_pos[6];
		double tmp_actual_pos[6];
		
		_convert_input_raw(_optic_encoders_raw,tmp_actual_pos);
		_convert_and_couple_input_raw(pos, tmp_pos);
		_convert_and_couple_input_raw(_reference_speeds, tmp_speeds);
		_convert_and_couple_input_raw(_reference_accs, tmp_accs);
		
		// max t
		double tmax = 0;
		for(int i = 0; i < _parameters._naj; i++) {
			double t = 0;
			t = fabs((double) (tmp_actual_pos[i]-tmp_pos[i])/tmp_speeds[i]);
			if (t>tmax)
				tmax = t;
		}
		// compute new speeds and accs
		for(i = 0; i < _parameters._naj; i++) {
			tmp_speeds[i] = fabs((double) (tmp_actual_pos[i]-tmp_pos[i])/tmax);
			if (tmp_speeds[i] == 0)
				tmp_speeds[i] = 200;
			tmp_accs[i] = tmp_speeds[i]*10;
		}

		// reset torque control
		_torqueControl.reset();
		_control_board.IOCtl(CMDSetTorqueLimits, _parameters._torque_limits);
		_control_board.stop();
		// apply commands
		rc = _control_board.IOCtl(CMDSetSpeeds, tmp_speeds);
		rc = _control_board.IOCtl(CMDSetAccelerations, tmp_accs);
		rc = _control_board.IOCtl(CMDSetPositions, tmp_pos);

		unlock();
		return rc;
	}

	int setPositions(const double *pos)
	{
		int rc = 0;
		lock();

		double tmp_speeds[6];
		double tmp_accs[6];
		double tmp_pos[6];
		double tmp_actual_pos[6];
		
		_convert_input_raw(_optic_encoders_raw, tmp_actual_pos);
		_convert_and_couple_input(pos, tmp_pos);
		_convert_and_couple_input_raw(_reference_speeds, tmp_speeds);
		_convert_and_couple_input_raw(_reference_accs, tmp_accs);
		
		// max t
		double tmax = 0;
		for(int i = 0; i < _parameters._naj; i++) {
			double t = 0;
			t = fabs((double) (tmp_actual_pos[i]-tmp_pos[i])/tmp_speeds[i]);
			if (t>tmax)
				tmax = t;
		}
		// compute new speeds and accs
		for(i = 0; i < _parameters._naj; i++) {
			tmp_speeds[i] = fabs((double) (tmp_actual_pos[i]-tmp_pos[i])/tmax);
			if (tmp_speeds[i] == 0)
				tmp_speeds[i] = 200;
			tmp_accs[i] = tmp_speeds[i]*10;
		}

		// reset torque control
		_torqueControl.reset();
		_control_board.IOCtl(CMDSetTorqueLimits, _parameters._torque_limits);
		_control_board.stop();
		// apply commands
		rc = _control_board.IOCtl(CMDSetSpeeds, tmp_speeds);
		rc = _control_board.IOCtl(CMDSetAccelerations, tmp_accs);
		rc = _control_board.IOCtl(CMDSetPositions, tmp_pos);

		unlock();
		return rc;
	}

	int setPositionRaw(int axis, const double *pos)
	{
		int rc = 0;
		lock();
		SingleAxisParameters cmd;
		cmd.axis = _axes_map(axis);
		cmd.parameters = _optic_encoder_zeros;
		rc = _control_board.IOCtl(CMDSetPosition, pos);
		unlock();
		return rc;
	}

	int setSpeedRaw(int axis, const double *sp)
	{
		int rc = 0;
		lock();
		_reference_speeds[axis-1] = sp[axis-1];
		SingleAxisParameters cmd;
		cmd.axis = _axes_map(axis);
		cmd.parameters = sp;
		rc = _control_board.IOCtl(CMDSetSpeed, &cmd);
		unlock();
		return rc;
	}

	int setSpeedsRaw(const double *spds)
	{
		int rc = 0;
		lock();
		memcpy(_reference_speeds, spds, _naj*sizeof(_reference_speeds[0]));
		_convert_input_raw(spds, _tmp_command_double);
		rc = _control_board.IOCtl(CMDSetSpeeds, _tmp_command_double);
		unlock();
		return rc;
	}

	int setAccelerationRaw(int axis, const double *acc)
	{
		int rc = 0;
		lock();
		_reference_accs[axis-1]] = acc[axis-1];
		SingleAxisParameters cmd;
		cmd.axis = _axes_map(axis);
		cmd.parameters = acc;
		rc = _control_board.IOCtl(CMDSetAcceleration, &cmd);
		unlock();
		return rc;
	}

	int setAccelerationsRaw(const double *accs)
	{
		int rc = 0;
		lock();
		memcpy(_reference_accs, accs, _naj*sizeof(_reference_accs[0]));
		_convert_input_raw(accs, _tmp_command_double);
		rc = _control_board.IOCtl(CMDSetAccelerations, _tmp_command_double);
		unlock();
		return rc;
	}

	int getRefAccelerationsRaw(double *accs)
	{
		int rc = 0;
		lock();
		rc = _control_board.IOCtl(CMDGetRefAccelerations, _tmp_command_double);
		_convert_output_raw(_tmp_command_double, accs);
		unlock();
		return rc;
	}

	int getRefSpeedsRaw(double *sp)
	{
		int rc = 0;
		lock();
		rc = _control_board.IOCtl(CMDGetRefSpeeds, _tmp_command_double);
		_convert_output_raw(_tmp_command_double, sp);
		unlock();
		return rc;
	}

	int getRefPositions(double *pos)
	{
		int rc = 0;
		lock();
		memcpy(pos, _reference_positions, _naj*sizeof(_reference_positions[0]));
		unlock();
		return rc;
	}

	int setIntegratorLimits(const double *lmts)
	{
		int rc = 0;
		lock();
		_convert_input_raw(lmts, _tmp_command_double);
		rc = _control_board.IOCtl(CMDSetIntegratorLimits, _tmp_command_double);
		unlock();
		return rc;
	}

	int setTorqueLimits(const double *lmts)
	{
		int rc = 0;
		lock();
		_convert_input_raw(lmts, _tmp_command_double);
		rc = _control_board.IOCtl(CMDSetTorqueLimits, _tmp_command_double);
		unlock();
		return rc;
	}

	int resetEncoder(int axis)
	{
		int rc = 0;
		lock();
		SingleAxisParameters cmd;
		cmd.axis = _axes_map(axis);
		cmd.parameters = _optic_encoder_zeros;
		rc = _control_board.IOCtl(CMDDefinePosition, &cmd);
		unlock();
		return rc;
	}

	int resetEncoders()
	{	
		int rc = 0;
		lock();
		_convert_input_raw(_optic_encoder_zeros, _tmp_command_double);
		rc = _control_board.IOCtl(CMDDefinePositions, _tmp_command_double);
		unlock();
		return rc;
	}

	int setPid(int axis, LowLevelPID &pid)
	{
		int rc = 0;
		lock();
		SingleAxisParameters cmd;
		cmd.axis = _axes_map(axis);
		cmd.parameters = &pid;
		rc = _control_board.IOCtl(CMDSetPID, &cmd);
		unlock();
		return rc;
	}

	int isMoving(char *moving)
	{
		lock();
		memcpy(moving, _is_moving, _naj*sizeof(_is_moving[0]));
		unlock();
		return 0;
	}

	bool isMoving()
	{
		lock();
		bool ret = _moving;
		unlock();
		return ret;
	}

	int getMotorTorques(double *trqs)
	{
		lock();
		memcpy(trqs, _motor_torques, _naj*sizeof(_motor_torques[0]));
		unlock();
		return 0;
	}

protected:
	inline int _axes_map(int axis)
	{
		int index = (axis -1);

		if ( (index<0) || (index >= _naj))
		{
			// error, axis is out of range !
			return 0;
		}
		
		return _parameters._axes_lut[index];
	}

	// convert input vector functions and various overloads, raw versions
	inline void _convert_input_raw(const int *input, int *output)
	{
		for (int i = 0; i<_naj; i++)
			output[_parameters._vector_lut[i]] = input[i];
	}

	inline void _convert_input_raw(const int *input, short *output)
	{
		for (int i = 0; i<_naj; i++)
			output[_parameters._vector_lut[i]] = (short) input[i];
	}

	inline void _convert_input_raw(const double *input, double *output)
	{
		for (int i = 0; i<_naj; i++)
			output[_parameters._vector_lut[i]] = input[i];
	}

	// convert input into motor joint (take coupling into account)
	inline void _convert_and_couple_input_raw(const double *input, double *output)
	{
		for (int i = 0; i<_naj; i++)
		{
			output[_parameters._vector_lut[i]] = 0;
			for(int j = 0; j < _naj; j++)
				output[_parameters._vector_lut[i]] += (input[j]*_parameters._coupling[i][j]);
		}
	}

	// convert input into motor joint (take coupling into account)
	inline void _convert_and_decouple_output_raw(const double *input, double *output)
	{
		for (int i = 0; i<_naj; i++)
		{
			output[i] = 0;
			for(int j = 0; j < _naj; j++)
				output[i] += (input[_parameters._vector_lut[j]]*_parameters._de_coupling[i][j]);
		}
	}

	// convert output vector functions, various overloads
	inline void _convert_output_raw(const int *input, int *output)
	{
		for (int i = 0; i<_naj; i++)
			output[i] = input[_parameters._vector_lut[i]];

	}

	inline void _convert_output_raw(const char *input, char *output)
	{
		for (int i = 0; i<_naj; i++)
			output[i] = input[_parameters._vector_lut[i]];

	}
	inline void _convert_output_raw(const double *input, double *output)
	{
		for (int i = 0; i<_naj; i++)
			output[i] = input[_parameters._vector_lut[i]];

	}

	// convert input into motor joint (take coupling into account)
	inline void _convert_and_couple_input(const double *input, double *output)
	{
		for (int i = 0; i<_naj; i++)
		{
			output[_parameters._vector_lut[i]] = 0;
			for(int j = 0; j < _naj; j++)
				output[_parameters._vector_lut[i]] += input[j]*_parameters._coupling[i][j]/_parameters._encoderToAngles[j];
		}
	}

	// convert output from joint (take coupling into account)
	inline void _convert_and_decouple_output(const double *input, double *output)
	{
		for (int i = 0; i<_naj; i++)
		{
			output[i] = 0;
			for(int j = 0; j < _naj; j++)
			{
				output[i] += (_parameters._encoderToAngles[j] * input[_parameters._vector_lut[j]]*_parameters._de_coupling[i][j]);
			}
		}
	}

	inline void lock()
	{ _mutex.Wait(); }

	inline void unlock()
	{ _mutex.Post(); }

	ADAPTERHALL _hall_sensors;
	ADAPTERGALIL _control_board;
	PARAMETERS _parameters;
	
	double *_hall_encoders_raw;
	
	double *_optic_encoders;			// store actual position, radiants, remapped
	double *_optic_encoders_raw;		// store actual position, raw data, remapped
	double *_optic_encoders_sp_raw;		// store actual speed, raw data, remapped
	double *_reference_positions;		
	double *_reference_speeds;
	double *_reference_accs;
	double *_motor_errors;
	char *_switches;
	char _input;
	char *_is_moving;
	bool _moving;
		
	// encoders are reset to these values (should be 0s)
	double *_optic_encoder_zeros;

	// these are used to convert high level axis map into galil axis map
//	int *_tmp_command_int;
	double *_tmp_command_double;
	short *_tmp_command_short;
	char *_tmp_command_char;
	
	// offsets and torques
	double *_offsets;
	double *_motor_torques;
	double *_max_torques;

	bool _initialized;
	int _naj;
	int _nidaq_ch;

	YARPSemaphore _mutex;
	YARPTorqueControl _torqueControl;

};

typedef YARPBabybotHandTemplate<YARPNIDAQOnBabybotHandAdapter, YARPGalilOnBabybotHandAdapter, YARPBabybotHandParameters> YARPBabybotHand;

#endif

