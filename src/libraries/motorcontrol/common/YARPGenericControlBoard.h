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
/// $Id: YARPGenericControlBoard.h,v 1.13 2004-07-06 19:41:39 babybot Exp $
///
///

#ifndef __YARPGenericControlBoardhh__ 
#define __YARPGenericControlBoardhh__

#include <conf/YARPConfig.h>
#include <YARPControlBoardUtils.h> // required for LowLevelPID class
#include <YARPSemaphore.h>

#include <math.h>
#include <YARPRobotMath.h>
#include <YARPString.h>

#define YARP_GEN_CB_VERBOSE

#ifdef YARP_GEN_CB_VERBOSE
#define YARP_GEN_CB_DEBUG(string) YARP_DEBUG("GEN_COMPONENT_DEBUG :", string)
#else YARP_GEN_CB_DEBUG(string) YARP_NULL_DEBUG
#endif

/**
 * \file YARPGenericControlBoard.h
 * a generic class that encapsulates the interface to a motor control card.
 */

/**
 * Generic incapsulation of a motor control board.
 * This class is a template with two class arguments: the adapter which contains
 * the actual device driver amd a parameter class which contains certain hardware 
 * specific parameters.
 */

template <class ADAPTER, class PARAMETERS>
class YARPGenericControlBoard
{
public:
	/**
	 * Constructor.
	 */
	YARPGenericControlBoard<ADAPTER, PARAMETERS> ()
	{
		_temp_double = NULL;
		_currentLimits = NULL;
		_newLimits = NULL;
	}

	/**
	 * Destructor.
	 */
	~YARPGenericControlBoard<ADAPTER, PARAMETERS>()
	{
		if (_temp_double != NULL)
			delete [] _temp_double;
		if (_newLimits != NULL)
			delete [] _newLimits;
		if (_currentLimits != NULL)
			delete [] _currentLimits;
	}

	// blocking wait on motion; poll on sleep(time)
	// time is ms, 0 = busy wait
	int waitForMotionDone(int time = 0)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDWaitForMotionDone, &time);
		_unlock();
		return ret;
	}

	int waitForFramesLeft(int time = 0)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDWaitForFramesLeft, &time);
		_unlock();
		return ret;
	}

	int calibrate()
	{
		int ret;
		_lock();
		ret = _adapter.calibrate();
		_unlock();
		return ret;
	}

	int initialize()
	{
		_lock();
		int ret = _initialize();
		_unlock();
		return ret;
	}

	/**
	 * initialize the control card.
	 */
	int initialize(const YARPString &path, const YARPString &init_file)
	{
		_lock();
		_parameters.load(path, init_file);
		int ret = _initialize();
		_unlock();
		return ret;
	}

	int idleMode()
	{	
		int ret;
		_lock();
		ret = _adapter.idleMode();
		_unlock();
		return ret;
	}

	int activatePID(bool reset = true)
	{
		_lock();
		int ret = _adapter.activatePID(reset);
		_unlock();
		return ret;
	}

	int activateLowPID(bool reset = true)
	{
		_lock();
		int ret = _adapter.activateLowPID(reset);
		_unlock();
		return ret;
	}

	int readAnalogs(double *val)
	{
		_lock();
			int ret = _adapter.readAnalogs(val);
		_unlock();
		return ret;
	}

	int uninitialize()
	{
		_lock();
		if (_temp_double != NULL)
			delete [] _temp_double;
		if (_newLimits != NULL)
			delete [] _newLimits;
		if (_currentLimits != NULL)
			delete [] _currentLimits;

		_temp_double = NULL;
		_currentLimits = NULL;
		_newLimits = NULL;

		if (_adapter.uninitialize() == YARP_FAIL) 
		{
			YARP_GEN_CB_DEBUG(("Error un-initializing control board!\n"));
			_unlock();
			return YARP_FAIL;
		}
		YARP_GEN_CB_DEBUG(("Control board uninitialized!\n"));
		_unlock();
		return YARP_OK;
	}

	/**
	 * move all axes to a certain position.
	 * @param pos an array of double precision number specifying the position in radians.
	 * @return YARP_OK on success.
	 */
	int setPositions(const double *pos)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++)
			_temp_double[i] = angleToEncoder(pos[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											_parameters._zeros[i],
											_parameters._signs[i]);

		_adapter.IOCtl(CMDSetPositions, _temp_double);
		_unlock();
		return YARP_OK;
	}

	/**
	 * move a specific joint to a certain position.
	 * @param i the axis to be moved.
	 * @param pos the final position in radians.
	 * @return YARP_OK on success.
	 */
	int setPosition(int i, double pos)
	{
		ACE_ASSERT (i >= 0 && i < _parameters._nj);
		_lock();

		SingleAxisParameters cmd;
		cmd.axis = i;
		pos = angleToEncoder(pos,
		 					 _parameters._encoderToAngles[i],
							 _parameters._zeros[i],
							 _parameters._signs[i]);
		cmd.parameters = &pos;

		_adapter.IOCtl(CMDSetPosition, &cmd);
		_unlock();

		return YARP_OK;
	}

	/**
	 * set the speed for successive position movements.
	 * @param vel the array of joint velocities expressed in rad/s.
	 * @return YARP_OK on success.
	 */
	int setVelocities(const double *vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[i] = angleToEncoder(vel[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetSpeeds, _temp_double);
		_unlock();
		return YARP_OK;
	}

	/**
	 * set the speed for successive position movements.
	 * @param vel the array of joint velocities expressed in rad/s.
	 * @return YARP_OK on success.
	 */
	int setVelocity(int i, double vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[i] = angleToEncoder(vel[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetSpeeds, _temp_double);
		_unlock();
		return YARP_OK;
	}

	int setAccs(const double *acc)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[i] = angleToEncoder(acc[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetAccelerations, _temp_double);
		_unlock();
		return -1;
	}

	int velocityMove(const double *vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[i] = angleToEncoder(vel[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDVMove, _temp_double);
		_unlock();
		return -1;
	}

	int safeVelocityMove(const double *vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[i] = angleToEncoder(vel[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSafeVMove, _temp_double);
		_unlock();
		return -1;
	}

	int setCommands(const double *pos)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[i] = angleToEncoder(pos[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetCommands, _temp_double);
		_unlock();
		return -1;
	}

	bool checkMotionDone()
	{
		bool ret;
		_lock();
		_adapter.IOCtl(CMDCheckMotionDone, &ret);
		_unlock();
		return ret;
	}

	bool checkFramesLeft()
	{
		bool ret;
		_lock();
		_adapter.IOCtl(CMDCheckFramesLeft, &ret);
		_unlock();
		return ret;
	}

	int getPositions(double *pos)
	{
		_lock();
		_adapter.IOCtl(CMDGetPositions, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			pos[_parameters._axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												_parameters._zeros[i],
												_parameters._signs[i]);
		
		}
		_unlock();
		return -1;
	}

	int getVelocities(double *vel)
	{
		_lock();
		_adapter.IOCtl(CMDGetSpeeds, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			vel[_parameters._axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												0.0,
												_parameters._signs[i]);
		
		}
		_unlock();
		return -1;
	}

	int getAccs(double *accs)
	{
		_lock();
		_adapter.IOCtl(CMDGetRefAccelerations, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			accs[_parameters._axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												0.0,
												_parameters._signs[i]);
		
		}
		_unlock();
		return -1;
	}

	// read dac output
	int getTorques(double *t)
	{
		_lock();
		_adapter.IOCtl(CMDGetTorques, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			t[_parameters._axis_map[i]] = _temp_double[i];		
		}
		_unlock();
		return -1;
	}

	// reset encoders to a specified value; NULL (default) means reset to 0.0s
	int resetEncoders(double *pos = NULL)
	{
		_lock();
		for(int i = 0; i < _parameters._nj; i++)
		{
			if (pos == NULL)
				_temp_double[i] = 0.0;
			else
				_temp_double[i] = pos[i];
		}
		_adapter.IOCtl(CMDDefinePositions, _temp_double);
		_unlock();
		return -1;
	}

	// return max torque for the control board (i.e. MEI is 32767.0, Galil 9.999)
	double getMaxTorque(int axis) { return _adapter.getMaxTorque(axis); }
	inline int nj() { return _parameters._nj; }

	int setGainsSmoothly(LowLevelPID *finalPIDs, int s = 150);
	// reduce max torque on axis of delta; returns true if current limit is equal to or less than value
	bool decMaxTorque(int axis, double delta, double value);
	// same as above, act on multiple joints (up to nj)
	bool decMaxTorques(double delta, double value, int nj);
	// increase max torque on axis of delta; returns true if current limit is equal to or more than value
	bool incMaxTorque(int axis, double delta, double value);
	// same as above, act on multiple joints (up to nj)
	bool incMaxTorques(double delta, double value, int nj);

	inline double angleToEncoder(double angle, double encParam, int zero, int sign);
	inline double encoderToAngle(double encoder, double encParam, int zero, int sign);

public:
	PARAMETERS _parameters;

protected:
	inline void _lock(void) { _mutex.Wait(); }	// add timeout ?
	inline void _unlock(void) { _mutex.Post(); }

	int _initialize()
	{
		_temp_double = new double [_parameters._nj];
		_currentLimits = new double [_parameters._nj];
		_newLimits = new double [_parameters._nj];

		if (_adapter.initialize(&_parameters) == YARP_FAIL) 
		{
			YARP_GEN_CB_DEBUG(("Error initializing Control board!\n"));
			_unlock();
			return YARP_FAIL;
		}
		YARP_GEN_CB_DEBUG(("Control board initialized!\n"));
		return YARP_OK;
	}

	YARPSemaphore _mutex;

	int *_tmp_int;
	ADAPTER _adapter;
	double *_temp_double;

	double *_currentLimits;
	double *_newLimits;
};



// This procedure set new gains smoothly in 's' steps.
// Care must be taken in dealing with the SHIFT (scale) parameter
// Here we take the maximum value between the actual and the new one.
// "finalPIDs" are scaled accordingly.
template <class ADAPTER, class PARAMETERS>
int YARPGenericControlBoard<ADAPTER, PARAMETERS>::setGainsSmoothly(LowLevelPID *finalPIDs, int s)
{
	ACE_OS::printf("Setting gains");

	double steps = (double) s;
	ACE_Time_Value sleep_period (0, 40*1000);
	
	LowLevelPID *actualPIDs;
	LowLevelPID *deltaPIDs;
	actualPIDs = new LowLevelPID [_parameters._nj];
	deltaPIDs = new LowLevelPID [_parameters._nj];

	double *shift;
	double *currentPos;
	shift = new double[_parameters._nj];
	currentPos = new double[_parameters._nj];
	
	// set command "here"
	// getPositions(currentPos);
	// setCommands(currentPos);

	for(int i = 0; i < _parameters._nj; i++) {
		SingleAxisParameters cmd;
		cmd.axis = i;
		cmd.parameters = &actualPIDs[i];
		_adapter.IOCtl(CMDGetPID, &cmd);

		// handle shift (scale)
		double actualShift = actualPIDs[i].SHIFT;
		double finalShift = finalPIDs[i].SHIFT;
		if (actualShift > finalShift)
		{
			shift[i] = actualShift;
			finalPIDs[i] = finalPIDs[i]*(pow(2,(finalShift+actualShift)));
		}
		else
		{
			shift[i] = finalShift;
			actualPIDs[i] = actualPIDs[i]*(pow(2,(actualShift+finalShift)));
		}
			
		deltaPIDs[i] = (finalPIDs[i] - actualPIDs[i])/steps;
	}
	
	for(int t = 0; t < (int) steps; t++)
	{
		for(int i = 0; i < _parameters._nj; i++)
		{
			actualPIDs[i] = actualPIDs[i] + deltaPIDs[i];
			actualPIDs[i].SHIFT = shift[i];
		
			SingleAxisParameters cmd;
			cmd.axis = i;
			cmd.parameters = &actualPIDs[i];
			_adapter.IOCtl(CMDSetPID, &cmd);
		}
		ACE_OS::sleep(sleep_period);
		ACE_OS::printf(".");

		fflush(stdout);
	}
	ACE_OS::printf("done !\n");

	delete [] actualPIDs;
	delete [] deltaPIDs;
	delete [] shift;
	delete [] currentPos; 
	return -1;
}

template <class ADAPTER, class PARAMETERS>
inline double YARPGenericControlBoard<ADAPTER, PARAMETERS>::
angleToEncoder(double angle, double encParam, int zero, int sign)
{
	if (sign == 1)
		return -(angle * encParam) / (2.0 * pi) + zero;
	else
		return angle * encParam / (2.0 * pi) + zero;
}

template <class ADAPTER, class PARAMETERS>
inline double YARPGenericControlBoard<ADAPTER, PARAMETERS>::
encoderToAngle(double encoder, double encParam, int zero, int sign)
{
	if (sign == 1)
		return (-encoder - zero) * 2.0 * pi / encParam;
	else
		return (encoder - zero) * 2.0 * pi / encParam;
}

template <class ADAPTER, class PARAMETERS>
inline bool YARPGenericControlBoard<ADAPTER, PARAMETERS>::
decMaxTorque(int axis, double delta, double value)
{
	bool ret = false;
	double currentLimit, newLimit;
	SingleAxisParameters cmd;
	cmd.axis = _parameters._axis_map[axis];
	cmd.parameters = &currentLimit;
	_adapter.IOCtl(CMDGetTorqueLimit, &cmd);

	newLimit = currentLimit - (fabs(delta));
	
	// check newLimit to see it we are done
	if (newLimit <= value)
	{
		newLimit = value;
		ret = true;
	}
	// be sure we are not going below zero
	if (newLimit < 0.0)
	{
		newLimit = 0.0;
		ret = true;
	}

	cmd.parameters = &newLimit;
	_adapter.IOCtl(CMDSetTorqueLimit, &cmd);

	return ret;
}

template <class ADAPTER, class PARAMETERS>
inline bool YARPGenericControlBoard<ADAPTER, PARAMETERS>::
incMaxTorque(int axis, double delta, double value)
{
	const double maxTorque = _parameters._maxDAC[_parameters._axis_map[axis]];

	bool ret = false;
	double currentLimit, newLimit;
	SingleAxisParameters cmd;
	cmd.axis = _parameters._axis_map[axis];
	cmd.parameters = &currentLimit;
	_adapter.IOCtl(CMDGetTorqueLimit, &cmd);

	newLimit = currentLimit + (fabs(delta));
	
	// check newLimit to see it we are done
	if (newLimit >= value)
	{
		newLimit = value;
		ret = true;
	}
	// be sure we are not going above max torque...
	if (newLimit >= maxTorque)
	{
		newLimit = maxTorque;
		ret = true;
	}

	cmd.parameters = &newLimit;
	_adapter.IOCtl(CMDSetTorqueLimit, &cmd);

	return ret;
}

template <class ADAPTER, class PARAMETERS>
inline bool YARPGenericControlBoard<ADAPTER, PARAMETERS>::
decMaxTorques(double delta, double value, int nj)
{
	bool ret = true;
	
	_adapter.IOCtl(CMDGetTorqueLimits, _currentLimits);

	int i;
	// set new limits to "_currentLimits" for all joints
	for(i = 0; i < _parameters._nj; i++)
		_newLimits[i] = _currentLimits[i];
	
	// only reduce limits for specified joints
	for(i = 0; i < nj; i++)
	{
		int j = _parameters._axis_map[i];
		_newLimits[j] = _currentLimits[j] - (fabs(delta));
	
		// check newLimit to see it we are done
		// be sure we are not going below zero
		if (_newLimits[j] < 0.0)
		{
			_newLimits[j] = 0.0;
			ret = ret && true;
		}
		else if (_newLimits[j] <= value)
		{
			_newLimits[j] = value;
			ret = ret && true;
		}
		else
			ret = false;
	}

	_adapter.IOCtl(CMDSetTorqueLimits, _newLimits);

	return ret;
}

template <class ADAPTER, class PARAMETERS>
inline bool YARPGenericControlBoard<ADAPTER, PARAMETERS>::
incMaxTorques(double delta, double value, int nj)
{
	bool ret = true;
		
	_adapter.IOCtl(CMDGetTorqueLimits, _currentLimits);

	int i;
	// set new limits to "_currentLimits" for all joints
	for(i = 0; i < _parameters._nj; i++)
		_newLimits[i] = _currentLimits[i];
	
	// increase limit only for the specified joints
	for(i = 0; i < nj; i++)
	{
		int j = _parameters._axis_map[i];
		_newLimits[j] = _currentLimits[j] + (fabs(delta));
	
		// be sure we are not going above max torque
		if (_newLimits[j] >=  _parameters._maxDAC[j])
		{
			_newLimits[j] = _parameters._maxDAC[j];
			ret = ret && true;
		}
		else if (_newLimits[j] >= value)
		{
			_newLimits[j] = value;
			ret = ret && true;
		}
		else
			ret = false;
	}

	_adapter.IOCtl(CMDSetTorqueLimits, _newLimits);

	return ret;
}

#endif // h
