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
/// $Id: YARPGenericControlBoard.h,v 1.24 2005-07-05 19:57:58 natta Exp $
///
///

#ifndef __YARPGenericControlBoardhh__ 
#define __YARPGenericControlBoardhh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <yarp/YARPControlBoardUtils.h> // required for LowLevelPID class
#include <yarp/YARPSemaphore.h>

#include <math.h>
#include <yarp/YARPString.h>

#define YARP_GEN_CB_VERBOSE

#ifdef YARP_GEN_CB_VERBOSE
#define YARP_GEN_CB_DEBUG(string) YARP_DEBUG("GEN_COMPONENT_DEBUG :", string)
#else YARP_GEN_CB_DEBUG(string) YARP_NULL_DEBUG
#endif

/*
 *
 * class hierarchy.
 *
 * YARPGenericControlAdapter <------+ +-> MyDeviceDriver -> YARPDeviceDriver
 *                                  | |
 * YARPGenericControlParameters     | |
 *        ^                         | |
 *        |                         | |
 *   MyParams <......+ + ... > MyAdapter
 *                 . .        
 *                 . .            
 *       YARPGenericControlBoard <----+
 *                                    |
 *                                    |
 *                            MyControlComponent
 *
 */

/**
 * \file YARPGenericControlBoard.h
 * a generic class that encapsulates the interface to a motor control card.
 */

/**
 * A class with the sole purpose of enforcing the interface on 
 * the adapters. Adapter classes MUST be derived from this one.
 * Although the "user" is not forced to use this base class, when
 * used assures compatibility with higher-level code. These methods
 * are declared here since they tend to vary across implementations
 * even with the same control card (e.g. details of the calibration
 * procedure are function of the robot rather than the electronics).
 *
 */
template <class ADAPTER, class PARAMETERS>
class YARPGenericControlAdapter
{
public:
	/**
	 * Single joint calibration.
	 * @param joint is the joint number to calibrate.
	 * @return YARP_OK if successful.
	 */
	virtual int calibrate (int joint) = 0;

	/**
	 * Initialization of the adapter. It takes the parameter
	 * argument and initializes the adapter class.
	 * @param par is the class containing the parameters.
	 * @return YARP_OK if successful.
	 */
	virtual int initialize (PARAMETERS *par) = 0;

	/**
	 * Uninitializes the controller and closes the device driver.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	virtual int uninitialize (void) = 0;

	/**
	 * Disables simultaneously the controller and the amplifier (these are
	 * usually two separate commands on control cards).
	 * @return YARP_OK always.
	 */
	virtual int idleMode (void) = 0;

	/**
	 * Activates the PID controller with given set of gains.
	 * @param reset if true resets the encoder values to zero.
	 * @param pids is an array of PID data structures.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	virtual int activatePID(bool reset, LowLevelPID *pids) = 0;
};

/**
 * another base class used to enforce some structure into the 
 * parameters class. Variables defined here are also used in the generic
 * motor control board.
 *
 * NOTE: this is just a container, it only forces a structure (hopefully).
 */
class YARPGenericControlParameters
{
public:
 	int _nj;

	double *_zeros;
	double *_signs;
	int *_axis_map;
	int *_inv_axis_map;
	double *_encoderToAngles;
	double *_limitsMax;
	double *_limitsMin;
};

/**
 * Generic incapsulation of a motor control board.
 * This class is a template with two class arguments: the adapter which contains
 * the actual device driver amd a parameter class which contains certain hardware 
 * specific parameters.
 * This class is part of a robot library that can be used as templates and allows
 * specialization to create useful robot control abstractions. It is worth mentioning
 * that this library is very much biased toward humanoid robots hence the name
 * head, arm, etc. This is not perhaps the right abstraction for wheeled robots.
 * Note that the adapter is the device driver controlling the hardware. The generic
 * control board class expects certain methods/messages to be available as in 
 * YARPControlBoardUtils.h. The parameters class allows adding some further specialization
 * without changing the device driver. This class also allows multithread access.
 *
 * WARNING: YARPGenericControlBoard is NOT consistently checking for bounds in arrays,
 * joint numbers, arguments, etc.
 *
 * WARNING: although you generally derive from the YARPGenericControlBoard, this class
 * does not use virtual functions. Please make sure you understand the implications of
 * this. In practice, it shouldn't be much of an issue as long as you don't start
 * playing with pointers to it.
 *
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
	 * Frees memory but still requires uninitialization.
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

	/**
	 * Blocking wait on motion.
	 * @param time is the polling interval in ms, 0 means busy wait.
	 * @return YARP_OK on success.
	 */
	int waitForMotionDone(int time = 0)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDWaitForMotionDone, &time);
		_unlock();
		return ret;
	}

	/**
	 * Waits for frames (commands) left to be zero.
	 * @param time is the polling interval in ms, 0 means busy wait.
	 * @return YARP_OK on success.
	 */
	int waitForFramesLeft(int time = 0)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDWaitForFramesLeft, &time);
		_unlock();
		return ret;
	}

	/**
	 * Device driver specific calibration routine.
	 * @return YARP_OK on success.
	 */
	int calibrate(int joint = -1)
	{
		int ret;
		_lock();
		ret = _adapter.calibrate(joint);
		_unlock();
		return ret;
	}

	/**
	 * Initializes the control card (with default parameters).
	 * @return YARP_OK on success.
	 */
	int initialize()
	{
		_lock();
		int ret = _initialize();
		_unlock();
		return ret;
	}

	/**
	 * Initializes the control card (reading paramters values from a file).
	 * @param path is the path of the config file.
	 * @param init_file is the initialization file. This file contains many
	 * head specific parameters that are then used to initialize the robot
	 * properly.
	 * @return YARP_OK on success.
	 */
	int initialize(const YARPString &path, const YARPString &init_file)
	{
	  _lock();
	  int ret = _parameters.load(path, init_file);
	  if (ret==YARP_FAIL)
	    {
	      _unlock();
	      return ret;
	    }
	  ret = _initialize();
	  _unlock();
	  return ret;
	}

	/**
	 * Initializes the control card.
	 * @param par is the reference to the parameter type structure (one of
	 * the arguments of the template).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int initialize(PARAMETERS &par)
	{
		_lock();
		_parameters.copy (par);
		int ret = _initialize();
		_unlock();
		return ret;
	}

	/**
	 * Uninitializes the control board and frees memory.
	 * This function does all what the destructor has to do.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
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
	 * Saves current parameters to boot memory (typically a flash memory in the
	 * card).
	 * @param axis specifies the axis. Depending on the board configuration this
	 * command might actually save one or many axes.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int saveToBootMemory (int axis)
	{
		int ret = YARP_FAIL;
		_lock ();
		if (axis >= 0 && axis < _parameters._nj)
		{
			axis = _parameters._axis_map[axis];
			ret = _adapter.IOCtl(CMDSaveBootMemory, &axis);
		}
		_unlock ();
		return ret;
	}

	/**
	 * Reads parameters from boot memory to RAM.
	 * @param axis specifies the axis' memory to read from. Depending on the board
	 * configuration, brand, and random factors :) this method acually reads one
	 * or more axes simultaneously.
	 * @return YARP_OK on success.
	 */
	int loadFromBootMemory (int axis)
	{
		int ret = YARP_FAIL;
		_lock ();
		if (axis >= 0 && axis < _parameters._nj)
		{
			axis = _parameters._axis_map[axis];
			ret = _adapter.IOCtl(CMDLoadBootMemory, &axis);
		}
		_unlock ();
		return ret;
	}

	/**
	 * Sets the software limits for a given axis: many control cards allow for 
	 * checking limits directly inside the PID loop.
	 * @param axis is axis to change/set the limits for.
	 * @param min is the minimum value of the joint position in degrees.
	 * @param max is the maximum value of the joint position in degrees.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int setSoftwareLimits (int axis, double min, double max)
	{
		_lock ();
		if (axis >= 0 && axis < _parameters._nj)
		{
			min = angleToEncoder(min,
								_parameters._encoderToAngles[axis],
								_parameters._zeros[axis],
								(int) _parameters._signs[axis]);

			max = angleToEncoder(max,
								_parameters._encoderToAngles[axis],
								_parameters._zeros[axis],
								(int) _parameters._signs[axis]);

			SingleAxisParameters x;
			x.axis = _parameters._axis_map[axis];
			x.parameters = &min;
			_adapter.IOCtl(CMDSetSWNegativeLimit, (void *)&x);

			x.parameters = &max;
			_adapter.IOCtl(CMDSetSWPositiveLimit, (void *)&x);
			
			_unlock ();
			return YARP_OK;
		}

		_unlock ();
		return YARP_FAIL;
	}

	/**
	 * Gets the software limits for a given axis: many control cards allow for 
	 * checking limits directly inside the PID loop.
	 * @param axis is axis to change/set the limits for.
	 * @param min is the minimum value of the joint position in degrees.
	 * @param max is the maximum value of the joint position in degrees.
	 * @return YARP_OK on success
	 */
	int getSoftwareLimits (int axis, double& min, double& max)
	{
		_lock ();
		if (axis >= 0 && axis < _parameters._nj)
		{
			SingleAxisParameters par;
			par.axis = _parameters._axis_map[axis];

			par.parameters = (void *)&max;
			_adapter.IOCtl(CMDGetSWPositiveLimit, (void *)&par);
			max = encoderToAngle(max, 	
								_parameters._encoderToAngles[axis],
								_parameters._zeros[axis],
								(int) _parameters._signs[axis]);

			par.parameters = (void *)&min;
			_adapter.IOCtl(CMDGetSWNegativeLimit, (void *)&par);
			min = encoderToAngle(min, 	
								_parameters._encoderToAngles[axis],
								_parameters._zeros[axis],
								(int) _parameters._signs[axis]);

			_unlock ();
			return YARP_OK;
		}

		_unlock ();
		return YARP_FAIL;
	}

	/**
	 * Disables all control amplifiers.
	 * @return YARP_OK on success.
	 */
	int idleMode()
	{	
		int ret;
		_lock();
		ret = _adapter.idleMode();
		_unlock();
		return ret;
	}

	/**
	 * Sets the PID values for a specified axis. Beware that this method doesn't
	 * stop the axis before executing the change. Make sure this is what you intend to
	 * do.
	 * @param axis is the axis to set the PID for.
	 * @param pid is a reference to a LowLevelPID structure containing the parameters.
	 * @return YARP_OK on success.
	 */
	int setPID(int axis, const LowLevelPID& pid)
	{
		_lock();
		if (axis >= 0 && axis < _parameters._nj)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[axis];
			cmd.parameters = const_cast<LowLevelPID *>(&pid);
			int ret = _adapter.IOCtl(CMDSetPID, &cmd);
			_unlock();
			return ret;
		}
		_unlock();
		return YARP_FAIL;
	}

	/**
	 * Gets the PID values for a specified axis. 
	 * @param axis is the axis to modify.
	 * @param pid is a reference to a LowLevelPID structure returning the parameters.
	 * @return YARP_OK on success.
	 */
	int getPID(int axis, LowLevelPID& pid)
	{
		_lock();
		if (axis >= 0 && axis < _parameters._nj)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[axis];
			cmd.parameters = &pid;
			int ret = _adapter.IOCtl(CMDGetPID, &cmd);
			_unlock();
			return ret;
		}
		_unlock();
		return YARP_FAIL;
	}

	/**
	 * Activates the PID controller using the values stored in the internal structures.
	 * @param reset tells whether to reset the encoder position.
	 * @param pids is an array of LowLevelPID structures containing the gain of the controller.
	 * @return YARP_OK on success.
	 */
	int activatePID(bool reset = true, LowLevelPID *pids = NULL)
	{
		_lock();
		int ret = _adapter.activatePID(reset, pids);
		_unlock();
		return ret;
	}

	/**
	 * Moves all axes to a certain position.
	 * @param pos an array of double precision number specifying the position in degrees.
	 * @return YARP_OK on success.
	 */
	int setPositions(const double *pos)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++)
			_temp_double[_parameters._axis_map[i]] = angleToEncoder(pos[i],
											_parameters._encoderToAngles[i],
											_parameters._zeros[i],
											(int) _parameters._signs[i]);
		
		_adapter.IOCtl(CMDSetPositions, _temp_double);
		_unlock();
		return YARP_OK;
	}

	/**
	 * Moves a specific joint to a certain position by employing some
	 * predefined trajectory profile (trapezoidal typically).
	 * @param i the axis to be moved.
	 * @param pos the final position in degrees.
	 * @return YARP_OK on success.
	 */
	int setPosition(int i, double pos)
	{
		_lock();
		if (i >= 0 && i < _parameters._nj)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[i];
			pos = angleToEncoder(pos,
		 						 _parameters._encoderToAngles[i],
								 _parameters._zeros[i],
								 (int) _parameters._signs[i]);
			cmd.parameters = &pos;
			
			_adapter.IOCtl(CMDSetPosition, &cmd);
			_unlock();

			return YARP_OK;
		}

		_unlock ();
		return YARP_FAIL;
	}

	/**
	 * Sets the speed for successive position movements.
	 * @param vel the array of joint velocities expressed in rad/s.
	 * @return YARP_OK on success.
	 */
	int setVelocities(const double *vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[_parameters._axis_map[i]] = angleToEncoder(vel[i],
											_parameters._encoderToAngles[i],
											0.0,
											(int) _parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetSpeeds, _temp_double);
		_unlock();
		return YARP_OK;
	}

	/**
	 * Sets the speed for successive position movements: single axis version.
	 * @param i is the axis to be controlled.
	 * @param vel is the joint speed expressed in degrees/s.
	 * @return YARP_OK on success.
	 */
	int setVelocity(int i, double vel)
	{
		_lock();
		if (i >= 0 && i < _parameters._nj)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[i];

			vel = angleToEncoder(vel,
								 _parameters._encoderToAngles[i],
								 0.0,
								 (int) _parameters._signs[i]);

			cmd.parameters = &vel;

			_adapter.IOCtl(CMDSetSpeed, &cmd);
			_unlock();
			return YARP_OK;
		}

		_unlock();
		return YARP_FAIL;
	}

	/**
	 * Sets the acceleration for the subsequent position or velocity move.
	 * @param acc is an array of double precision values specifing the
	 * acceleration value for all joints in rad/s^2.
	 * @return YARP_OK on success.
	 */
	int setAccs(const double *acc)
	{
		_lock();
		int ret;
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[_parameters._axis_map[i]] = angleToEncoder(acc[i],
											_parameters._encoderToAngles[i],
											0.0,
											(int) _parameters._signs[i]);
		}

		ret = _adapter.IOCtl(CMDSetAccelerations, _temp_double);
		_unlock();
		return ret;
	}

	/**
	 * Starts a velocity motion of all joints.
	 * @param vel is an array of double precision values representing
	 * the speed of each joint. Speed is in degrees/s.
	 * @return YARP_OK on success.
	 */
	int velocityMove(const double *vel)
	{
		int ret;
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[_parameters._axis_map[i]] = angleToEncoder(vel[i],
											_parameters._encoderToAngles[i],
											0.0,
											(int) _parameters._signs[i]);
		}

		ret = _adapter.IOCtl(CMDVMove, _temp_double);
		_unlock();
		return ret;
	}


	/**
	 * Starts a velocity motion of all joints and check first whether other
	 * commands might be pending for execution. This is very specific to the
	 * control card although it's safe to be called, it'll revert to a velocityMove()
	 * if this feature is not supported.
	 * @param vel is an array of double precision values representing
	 * the speed of each joint. Speed is in degrees/s.
	 * @return YARP_OK on success.
	 */
	int safeVelocityMove(const double *vel)
	{
		int ret;
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[_parameters._axis_map[i]] = angleToEncoder(vel[i],
											_parameters._encoderToAngles[i],
											0.0,
											(int) _parameters._signs[i]);
		}

		ret = _adapter.IOCtl(CMDSafeVMove, _temp_double);
		_unlock();
		return ret;
	}

	/**
	 * Sets the command value/reference position for all axes. Beware that changing
	 * the commanded position abruptly will generate a possibly fast and high-acceleration
	 * movement of the joint. Make sure you know the value of the
	 * new reference position with respect to the actual position of the axis.
	 * @param pos is the array containing the position values in degrees.
	 * @return YARP_OK on success.
	 */
	int setCommands(const double *pos)
	{
		int ret;
		_lock();
		for (int i = 0; i < _parameters._nj; i++) 
		{
			_temp_double[_parameters._axis_map[i]] = angleToEncoder(pos[i],
											_parameters._encoderToAngles[i],
											0.0,
											(int) _parameters._signs[i]);
		}

		ret = _adapter.IOCtl(CMDSetCommands, _temp_double);
		_unlock();
		return ret;
	}

	/**
	 * Stops the movement of all joints.
	 * @return YARP_OK if successful.
	 */
	int stopMotion()
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDServoHere, NULL);
		_unlock();
		return ret;
	}

	/**
	 * Checks whether the last command has been completed.
	 * Useful if certain positions have to be achieved precisely to wait
	 * before sending other commands.
	 * @return true if the motion is terminated, false otherwise.
	 */
	bool checkMotionDone()
	{
		bool ret = false;
		_lock();
		_adapter.IOCtl(CMDCheckMotionDone, &ret);
		_unlock();
		return ret;
	}

	/**
	 * Checks whether are there commands pending in the motion queue.
	 * Some cards have buffering (queues) between the card and the host computer to 
	 * improve performances. This method is useful to avoid an overflow of the
	 * communication buffers.
	 * @return true if there are commands left to be executed.
	 */
	bool checkFramesLeft()
	{
		bool ret = false;
		_lock();
		_adapter.IOCtl(CMDCheckFramesLeft, &ret);
		_unlock();
		return ret;
	}

	/**
	 * Gets the position (angle) of all joints.
	 * @param pos is an array to contain the joint angles. Joint angles are measured
	 * in degrees.
	 * @return YARP_OK on success.
	 */
	int getPositions(double *pos)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDGetPositions, _temp_double);
		int j;
		for (int i = 0; i < _parameters._nj; i++) 
		{
			j = _parameters._inv_axis_map[i];
			pos[j] = encoderToAngle(_temp_double[i],
									_parameters._encoderToAngles[j],
									_parameters._zeros[j],
									(int) _parameters._signs[j]);
		}
		_unlock();
		return ret;
	}

	/**
	 * Gets the velocities of all joints (encoder velocities).
	 * @param vel is an array that receives the joint velocities.
	 * @return YARP_OK on success.
	 */
	int getVelocities(double *vel)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDGetSpeeds, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			vel[_parameters._inv_axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												0.0,
												(int) _parameters._signs[i]);
		
		}
		_unlock();
		return ret;
	}

	/**
	 * Gets the acceleration set point for all joints.
	 * @param accs is an array to receive the acceleration values (degrees/s^2).
	 * @return YARP_OK on success.
	 */
	int getRefAccs(double *accs)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDGetRefAccelerations, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			accs[_parameters._inv_axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												0.0,
												(int) _parameters._signs[i]);
		}
		_unlock();
		return ret;
	}

	/**
	 * Gets istantaneous acceleration set point for all joints.
	 * @param accs is an array to receive the acceleration values (degrees/s^2).
	 * @return YARP_OK on success.
	 */
	int getAccs(double *accs)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDGetAccelerations, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			accs[_parameters._inv_axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												0.0,
												(int) _parameters._signs[i]);
		
		}
		_unlock();
		return ret;
	}

	/**
	 * Reads the control value from the board that for current control
	 * correseponds (approximately) to the generated torque. In some cases this
	 * is the PWM value, in other just the output of the DAC.
	 * @param t is an array of double precision values that will contain the
	 * torque values.
	 * 
	 */
	int getTorques(double *t)
	{
		int ret;
		_lock();
		ret = _adapter.IOCtl(CMDGetTorques, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) 
		{
			t[_parameters._inv_axis_map[i]] = _temp_double[i];		
		}
		_unlock();
		return ret;
	}

	/**
	 * Sets the encoder positions for all joints. This can either set them to
	 * zero or to some specified values.
	 * @param pos is an array with the encoder values (not angles!), if NULL
	 * (default) it will set all to zero.
	 * @return -1 always.
	 */
	int resetEncoders(const double *pos = NULL)
	{
	    int ret;
		_lock();
		for(int i = 0; i < _parameters._nj; i++)
		{
			if (pos == NULL)
				_temp_double[_parameters._axis_map[i]] = 0.0;
			else
				_temp_double[_parameters._axis_map[i]] = pos[i];
		}
		ret = _adapter.IOCtl(CMDDefinePositions, _temp_double);
		_unlock();
		return ret;
	}

	/**
	 * Gets the current PID error for a specified axis.
	 * @param axis is the axis to request the value for.
	 * @param value contains the return value.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int getPIDError(int axis, double &value)
	{
	    _lock();
	    int ret;
	    if (axis >= 0 && axis < _parameters._nj)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[axis];
			cmd.parameters = &value;
			ret = _adapter.IOCtl(CMDGetPIDError, &cmd);
			value = encoderToAngle(value,
				   _parameters._encoderToAngles[axis],
				   0.0,
				   (int) _parameters._signs[axis]);
			_unlock();
			return ret;
		}
	    _unlock();
	    return YARP_FAIL;
	}

	/**      
 	 * Perform a relative movement.
	 * @param axis is the axis to move.
	 * @param value how much to move the axis for (deg).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int setPositionRelative (int axis, double value)
	{                    
		_lock();
		int ret;
		if (axis>=0 && axis<_parameters._nj)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[axis];
			value = angleToEncoder(value,
								_parameters._encoderToAngles[axis],
								0.0, 
								(int) _parameters._signs[axis]);
			cmd.parameters = &value;
			ret = _adapter.IOCtl(CMDRelativeMotion, &cmd);
			_unlock();
			return ret;
		}
		_unlock();
		return YARP_FAIL;
	}

	/**      
 	 * Perform a relative movement on all axis.
	 * @values a vector which contains how much to move each axis for (deg).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int setPositionsRelative (double *pos)
	{                  
	  _lock();
	  for (int i = 0; i < _parameters._nj; i++)
	    _temp_double[_parameters._axis_map[i]] = angleToEncoder(pos[i],
								    _parameters._encoderToAngles[i],
								    0,
								    (int) _parameters._signs[i]);
	  
	  _adapter.IOCtl(CMDRelativeMotionMultiple, _temp_double);
	  _unlock();
	  return YARP_OK;
	}

	/**
	 * Gets the card's max torque: this value is a function of the specific 
	 * output device of the control card and of resolution and firmware implementation
	 * (i.e. MEI is 32767.0, Galil 9.999).
	 * @param axis is the axis to request the value for.
	 * @return a double precision value representing the maximum output value.
	 */
	double getMaxTorque(int axis) { return _adapter.getMaxTorque(axis); }

	/**
	 * Gets the number of axis controlled by this interface.
	 * @return an integer value (0 or negative if not initialized).
	 */
	inline int nj() { return _parameters._nj; }

	/**
	 * Converts angles in degrees into encoder values in ticks.
	 * @param angle is the angle to be converted.
	 * @param encParam is the multiplication factor (linear).
	 * @param zero is the encoder's zero value (i.e. a value in ticks to
	 * align the encoder's zero with the actual angular zero.
	 * @param sign tells whether the encoder counts on the same direction
	 * as the angle.
	 * @return the converted value in encoder ticks.
	 */
	inline double angleToEncoder(double angle, double encParam, double zero, int sign)
	{
		if (sign == 1)
			return -(angle * encParam) / (360) + zero;
		else
			return angle * encParam / (360) + zero;
	}

	/**
	 * Converts encoder values into angles in degrees.
	 * @param encoder is the angle measured in encoder ticks.
	 * @param encParam is the multiplication factor (linear).
	 * @param zero is the encoder's zero value (i.e. a value in ticks to
	 * align the encoder's zero with the actual angular zero.
	 * @param sign tells whether the encoder counts on the same direction
	 * as the angle.
	 * @return the converted value of the angle in degrees.
	 */
	inline double encoderToAngle(double encoder, double encParam, double zero, int sign)
	{
		if (sign == 1)
			return (zero-encoder) * 360 / encParam;
		else
			return (encoder - zero) * 360 / encParam;
	}

protected:
	/**
	 * Waits on the internal mutex (for multithread access).
	 */
	inline void _lock(void) { _mutex.Wait(); }

	/**
	 * Posts on the internal mutex (for multithread access).
	 */
	inline void _unlock(void) { _mutex.Post(); }

	/**
	 * Helper function to intialize the device driver.
	 */
	int _initialize(void)
	{
		_temp_double = new double [_parameters._nj];
		ACE_ASSERT (_temp_double != NULL);
		_currentLimits = new double [_parameters._nj];
		ACE_ASSERT (_currentLimits != NULL);
		_newLimits = new double [_parameters._nj];
		ACE_ASSERT (_newLimits != NULL);

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

	double *_temp_double;
	double *_currentLimits;
	double *_newLimits;

	// will become protected one day.
public:
	ADAPTER _adapter;
	PARAMETERS _parameters;
};



#endif // h
