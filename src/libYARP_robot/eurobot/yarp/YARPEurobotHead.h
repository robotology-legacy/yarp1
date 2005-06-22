/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran-Gonzalez#                      ///
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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Carlos Beltran-Gonzalez#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPEurobotHead.h,v 1.4 2005-06-22 16:06:15 beltran Exp $
///
///

#ifndef __YARPEUROBOTHEAD__
#define __YARPEUROBOTHEAD__

#include <yarp/YARPConfig.h>
#include <yarp/YARPGenericControlBoard.h>
#include <yarp/YARPGALILOnEurobotHeadAdapter.h>

namespace _limits
{
	const int neckPan     = 0;
	const int neckTilt    = 1;
	const int eyeVersion  = 2;
	const int eyeVergence = 3;
};

namespace _joints
{
	const int neckPan  = 0;
	const int neckTilt = 1;
	const int rightEye = 2;
	const int leftEye  = 3;
};


/**
 * YARPEurobotHead is the interface to the babybot platform robot head (5 dof).
 *
 * NOTE: if calling functions in the ADAPTER you must protect them with the
 * mutex otherwise, if calling those in the base class (GenericControlBoard),
 * then you should not protect them with the mutex. Everything in the Generic
 * is protected already (protecting twice causes a deadlock).
 *
 */
class YARPEurobotHead : 
	public YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters>
{
public:
	/**
	 * Sets the PID values specified in a second set of parameters 
	 * typically read from the intialization file.
	 * @param reset if true resets the encoders.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int activateLowPID(bool reset = true)
	{
		return activatePID(reset, _parameters._lowPIDs);
	}

	/**
	 * Sets the PID values for a specified axis. Beware that this method doesn't
	 * stop the axis before executing the change. Make sure this is what you intend to
	 * do.
	 * @param axis is the axis to modify.
	 * @param pid is a reference to a LowLevelPID structure containing the parameters.
	 * @param sync is a flag that decides whether the new values is also copied onto the 
	 * internal parameters structure (true by default).
	 * @return YARP_OK on success.
	 */
	int setPID(int axis, const LowLevelPID& pid, bool sync = true)
	{
		int ret = YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters>
			::setPID (axis, pid);
		if (ret == YARP_OK)
		{
			if (sync == true)
			{
				// copies the new values into the highPID structure.
				memcpy (&(_parameters._highPIDs[axis]), &pid, sizeof(LowLevelPID));
			}
			return YARP_OK;
		}
		
		return YARP_FAIL;
	}

	/**
	 * Gets the PID values for a specified axis. 
	 * @param axis is the axis to modify.
	 * @param pid is a reference to a LowLevelPID structure returning the parameters.
	 * @param sync is a flag that decides whether the new values is also copied onto the 
	 * internal parameters structure (true by default).
	 * @return YARP_OK on success.
	 */
	int getPID(int axis, LowLevelPID& pid, bool sync = true)
	{
		int ret = YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters>
			::getPID (axis, pid);

		if (ret == YARP_OK)
		{
			if (sync == true)
			{
				memcpy (&(_parameters._highPIDs[axis]), &pid, sizeof(LowLevelPID));
			}
			return YARP_OK;
		}

		return YARP_FAIL;
	}

	/**
	 * Reads the analog inputs of the card. Many control cards have analog inputs to 
	 * build feedback loops on analog sensors.
	 * @param val is an array of double precision numbers returning the readings from
	 * the A/D converters.
	 * @return YARP_OK on success.
	 */
	int readAnalogs(double *val)
	{
		_lock();
		int ret = _adapter.readAnalogs(val);
		_unlock();
		return ret;
	}

	/**
	 * Reads the analog input of the card.
	 * @param index is the analog input number
 	 * @param val is the return value
	 * @return YARP_OK on success.
	 */
	int readAnalog(int index, double *val)
	{
		_lock();
		int ret = _adapter.readAnalog(index, val);
		_unlock();
		return ret;
	}

	/**
	 * Compute the vergence angle given the current eyes position.
	 * @param pos is the joint angles vector.
	 * @return the vergence angle.
	 */
 	inline double vergence(double *pos)
	{
		return -( pos[_joints::rightEye] - pos[_joints::leftEye] );
	}

	/**
	 * Compute the expected vergence angle given the eyes speed.
	 * @param cmd is a vector containing the joint angle speed.
	 * @return the derivative of the vergence angle.
	 */
	inline double dot_vergence(double *cmd)
	{
		return cmd[_joints::rightEye] - cmd[_joints::leftEye];
	}

	/**
	 * Compute the vesion angle (the eyes common orientation).
	 * @param post is the joint angle vector.
	 * @return the version angle.
	 */
	inline double version(double *pos)
	{
		return 0.5*(-pos[_joints::rightEye]+pos[_joints::leftEye]);
	}

	/**
	 * Compute the expected version angle given the eyes speed.
	 * @param cmd is the joint speed vector.
	 * @return the derivative of the version angle.
	 */
	inline double dot_version(double *cmd)
	{
		return 0.5*(-cmd[_joints::rightEye]+cmd[_joints::leftEye]);

	}

	/**
	 * Check whether any of the joint software limits has been hit.
	 * @param pos is the joint angle vector.
	 * @param cmd is the current command.
	 * @return true if any of the limits has been hit.
	 */
	inline bool checkLimits(double *pos, double *cmd)
	{
		bool ret = false;
		bool tmp = false;
	
		// check eyes, vergence and version
		tmp = checkSingleJoint(vergence(pos), dot_vergence(cmd), _parameters._limitsMax[_limits::eyeVergence], _parameters._limitsMin[_limits::eyeVergence], -1);
		tmp |= checkSingleJoint(version(pos), dot_version(cmd), _parameters._limitsMax[_limits::eyeVersion], _parameters._limitsMin[_limits::eyeVersion], 1);
		if (tmp)
		{
			cmd[_joints::rightEye] = 0.0;
			cmd[_joints::leftEye] = 0.0;
			ret = true;
		}

		tmp = checkSingleJoint(pos[_joints::neckPan], cmd[_joints::neckPan], _parameters._limitsMax[_limits::neckPan], _parameters._limitsMin[_limits::neckPan], 1);
		if (tmp)
		{
			cmd[_joints::neckPan] = 0.0;
			ret = true;
		}

		tmp = checkSingleJoint(pos[_joints::neckTilt], cmd[_joints::neckTilt], _parameters._limitsMax[_limits::neckTilt], _parameters._limitsMin[_limits::neckTilt], 1);
		if (tmp)
		{
			cmd[_joints::neckTilt] = 0.0;
			ret = true;
		}
	
		return ret;
	}

	/**
	 * Check the limits on a given joint.
	 * @param pos is the joint current position.
	 * @param cmd is the joint current command.
	 * @param up is the upper limit.
	 * @param low is the lower limit.
	 * @param sign is the type of check to perform: -1 will check for the command in the same
	 * direction of the limit, this is perfectly ok for certain applications; 1 will check for
	 * the command in the opposite direction wrt the limit since this will move the joint
	 * to a valid zone.
	 * @return true if a limit has been hit.
	 */
	inline bool checkSingleJoint(double pos, double cmd, double up, double low, int sign)
	{
		if (sign > 0)
		{
			if ((pos < low && cmd < 0) || (pos > up && cmd > 0))
				return true;
			else
				return false;
		}
		else
		{
			if ((pos < low && cmd > 0) || (pos > up && cmd < 0))
				return true;
			else
				return false;
		}
	}

	//TODO: to check from here to the end for Eurobot
	/**
	 * Sets the PID gain smoothly (in small increments) to the final value.
	 * @param finalPIDs is an array of LowLevelPID structures used as target value.
	 * @param s is the number of steps.
	 * @return YARP_OK always.
	 */
	int setGainsSmoothly(LowLevelPID *finalPIDs, int s = 150)
	{
		_lock ();

		ACE_OS::printf("Setting gains");

		double steps = (double) s;
		ACE_Time_Value sleep_period (0, 40*1000);
		
		LowLevelPID *actualPIDs;
		LowLevelPID *deltaPIDs;
		actualPIDs = new LowLevelPID [_parameters._nj];
		deltaPIDs = new LowLevelPID [_parameters._nj];
		ACE_ASSERT (actualPIDs != NULL && deltaPIDs != NULL);

		double *shift;
		double *currentPos;
		shift = new double[_parameters._nj];
		currentPos = new double[_parameters._nj];
		ACE_ASSERT (shift != NULL && currentPos != NULL);

		// set command "here"
		// getPositions(currentPos);
		// setCommands(currentPos);

		for(int i = 0; i < _parameters._nj; i++) 
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[i];
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
				cmd.axis = _parameters._axis_map[i];
				cmd.parameters = &actualPIDs[i];
				_adapter.IOCtl(CMDSetPID, &cmd);
			}
			ACE_OS::sleep(sleep_period);
			ACE_OS::printf(".");

			fflush(stdout);
		}
		ACE_OS::printf("done !\n");

		// if !NULL...
		delete [] actualPIDs;
		delete [] deltaPIDs;
		delete [] shift;
		delete [] currentPos; 

		_unlock ();

		return YARP_OK;
	}
	
	/** 
	 * Reduce max torque on axis of delta; returns true if current limit is equal to 
	 * or less than value.
	 * @param axis the axis number to affect.
	 * @param delta is the amount to change the max torque value.
	 * @param value is the target value of a large decrement (e.g. the final max torque is
	 * tested against this number to assert whether a specific target has been reached).
	 * @return true if value has been reached.
	 */
	inline bool decMaxTorque(int axis, double delta, double value)
	{
		_lock ();

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

		_unlock ();

		return ret;
	}

	/** 
	 * Increment the maximum torque on axis of delta; returns true if current limit is equal to 
	 * or less than value.
	 * @param axis the axis number to affect.
	 * @param delta is the amount to change the max torque value.
	 * @param value is the target value of a large increment (e.g. the final max torque is
	 * tested against this number to assert whether a specific target has been reached).
	 * @return true if the target value has been reached.
	 */
	inline bool incMaxTorque(int axis, double delta, double value)
	{
		_lock ();

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

		_unlock ();

		return ret;
	}

	/** 
	 * Reduce max torque on all axes; returns true if current limit is equal to 
	 * or less than value.
	 * @param delta is the amount to change the max torque value.
	 * @param value is the target value of a large decrement (e.g. the final max torque is
	 * tested against this number to assert whether a specific target has been reached).
	 * @param nj is the total number of axes.
	 * @return true if value has been reached.
	 */
	inline bool decMaxTorques(double delta, double value, int nj)
	{
		_lock ();

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

		_unlock ();

		return ret;
	}

	/** 
	 * Increment the maximum torque on all axes; returns true if current limit is equal to 
	 * or less than value.
	 * @param delta is the amount to change the max torque value.
	 * @param value is the target value of a large increment (e.g. the final max torque is
	 * tested against this number to assert whether a specific target has been reached).
	 * @param nj is the total number of axes.
	 * @return true if value has been reached.
	 */
	inline bool	incMaxTorques(double delta, double value, int nj)
	{
		_lock ();

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

		_unlock ();

		return ret;
	}

}; /* YARPEurobotHead */

#endif
