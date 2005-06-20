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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPBabybotArm.h,v 1.9 2005-06-20 14:01:23 gmetta Exp $
///
///

#ifndef __YARPBABYBOTARM__
#define __YARPBABYBOTARM__

#define YARP_BABYBOT_ARM_VERBOSE

#ifdef YARP_BABYBOT_ARM_VERBOSE
#define YARP_BABYBOT_ARM_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_DEBUG:", string)
#else YARP_BABYBOT_ARM_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <yarp/YARPConfig.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPGenericControlBoard.h>
#include <yarp/YARPMEIOnBabybotArmAdapter.h>

/**
 *
 */
typedef YARPGenericControlBoard<YARPMEIOnBabybotArmAdapter, YARPBabybotArmParameters> MyGenericControlBoard;

/**
 * YARPBabybotArm is the high level interface to the babybot PUMA260 - 6 dof arm.
 *
 * WARNING: certain methods are overridden here to take into account the coupling of the
 * PUMA wrist joints. Not all methods that require decoupling are yet implemented: beware of
 * which method you actually call.
 */
class YARPBabybotArm : public MyGenericControlBoard
{
public:
	/**
	 * Default constructor.
	 */
	YARPBabybotArm() : MyGenericControlBoard()
	{
		_pidSigns = NULL;
	}

	/**
	 * Sets the PID values specified in a second set of parameters 
	 * typically read from the intialization file.
	 * @param reset is unused since the activatePid does not use it.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int activateLowPID(bool reset = true)
	{
		return YARPGenericControlBoard<YARPMEIOnBabybotArmAdapter, YARPBabybotArmParameters>
			::activatePID(reset, _parameters._lowPIDs);
	}

	/**
	 * Overrides the activatePID method. This version checks whether 
	 * the power is on on the Puma amplifier box.
	 * @retunr YARP_OK always.
	 */
	int activatePID()
	{
		int ret;
		_lock();
		ret = YARPGenericControlBoard<YARPMEIOnBabybotArmAdapter, YARPBabybotArmParameters>
			::activatePID(true);
		
		while (!_adapter.checkPowerOn())
		{
			YARP_BABYBOT_ARM_DEBUG(("Press the power on button!\n"));
			ACE_OS::sleep(ACE_Time_Value(1,0));
		}
		_unlock();
		return YARP_OK;
	}

	/**
	 * Moves all axes to a certain position. This method only works for "stiff" joints; the
	 * low stiffness axes are untouched.
	 * @param pos an array of double precision number specifying the position in degrees.
	 * @return YARP_OK on success.
	 */
	int setPositions(const double *pos);

	/**
	 * Moves all axes to a certain position. All axes are affected by this method.
	 * @param pos an array of double precision number specifying the position in degrees.
	 * @return YARP_OK on success.
	 */
	int setPositionsAll(const double *pos);

	/**
	 * Sets the speed for successive position movements.
	 * @param vel the array of joint velocities expressed in rad/s.
	 * @return YARP_OK on success.
	 */
	int setVelocities(const double *vel);

	/**
	 * Sets the acceleration for the subsequent position or velocity move.
	 * @param acc is an array of double precision values specifing the
	 * acceleration value for all joints in rad/s^2.
	 * @return YARP_OK on success.
	 */
	int setAccs(const double *acc);

	/**
	 * Starts a velocity motion of all joints.
	 * @param vel is an array of double precision values representing
	 * the speed of each joint. Speed is in degrees/s.
	 * @return YARP_OK on success.
	 */
	int velocityMove(const double *vel);

	/**
	 * Sets the command value/reference position for all axes. Beware that changing
	 * the commanded position abruptly will generate a possibly fast and high-acceleration
	 * movement of the joint. Make sure you know the value of the
	 * new reference position with respect to the actual position of the axis.
	 * @param pos is the array containing the position values in degrees.
	 * @return YARP_OK on success.
	 */
	int setCommands(const double *pos);

	/**
	 * Gets the position (angle) of all joints.
	 * @param pos is an array to contain the joint angles. Joint angles are measured
	 * in degrees.
	 * @return YARP_OK on success.
	 */
	int getPositions(double *pos);

	/**
	 * Gets the velocities of all joints (encoder velocities).
	 * @param vel is an array that receives the joint velocities.
	 * @return YARP_OK on success.
	 */
	int getVelocities(double *vel);

	/**
	 * Changes the P gain of the axis @a i which behaves as
	 * stiffness parameter for the low-stiffness control mode.
	 * @param i is the axis to change the stiffness for.
	 * @param s is the stiffness value.
	 * @return YARP_OK on success.
	 */
	int setStiffness(int i, double s);

	/** 
	 * Changes the offset of the @a i joint which can be interpreted
	 * as a direct torque control. This method is typically used
	 * for gravity or dynamic compensation.
	 * @param i is the axis to change.
	 * @param g is the offset value.
	 * @return YARP_OK on success.
	 */
	int setG(int i, double g);

	/** 
	 * Changes the offset of all joints: changing the offest can be interpreted
	 * as a direct torque control. This method is typically used
	 * for gravity or dynamic compensation.
	 * @param g is the offset value.
	 * @return YARP_OK on success.
	 */
	int setGs(const double *g);

	/** 
	 * Sets all PID gains simultaneously.
	 * @param pids is an array of LowLevelPID structures.
	 * @return YARP_OK on success.
	 */
	int setPIDs(const LowLevelPID *pids);

	/**
	 * Converts angles in degrees into encoder values in ticks.
	 * @param ang is the vector of angles to be converted.
	 * @param enc is the vector of encoder ticks (returned).
	 */
	inline void angleToEncoders(const double *ang, double *enc)
	{ _angleToEncoders(ang, enc, _parameters, _parameters._zeros); } 

	/**
	 * Converts encoder values into angles in degrees.
	 * @param enc is the vector of encoder ticks.
	 * @param ang is the vector of angles to be converted (returned).
	 */
	inline void encoderToAngles(const double *enc, double *ang)
	{ _encoderToAngles(enc, ang, _parameters, _parameters._zeros); }

	/**
	 * Converts angular speed in degrees/s into encoder values in ticks/s.
	 * @param ang is the vector of angles to be converted.
	 * @param enc is the vector of encoder ticks (returned).
	 * @param _parameters is the struct with the array of parameters.
	 */
	inline void angleVelToEncoders(const double *ang, double *enc, const YARPBabybotArmParameters &_parameters)
	{	
		double zeros[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		_angleToEncoders(ang, enc, _parameters, zeros);
	}

	/**
	 * Converts encoder speed values into angular speed in degrees/s.
	 * @param ang is the vector of angles to be converted.
	 * @param enc is the vector of encoder ticks (returned).
	 * @param _parameters is the struct with the array of parameters.
	 */
	inline void encoderVelToAngles(const double *enc, double *ang, const YARPBabybotArmParameters &_parameters)
	{ 
		double zeros[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		_angleToEncoders(enc, ang, _parameters, zeros);
	}
	
	/**
	 * Overloaded method: initializes the control card (with default parameters).
	 * @return YARP_OK on success.
	 */
	int initialize();
	
	/**
	 * Overloaded method: initializes the control card (reading paramters values from a file).
	 * @param path is the path of the config file.
	 * @param init_file is the initialization file. This file contains many
	 * head specific parameters that are then used to initialize the robot
	 * properly.
	 * @return YARP_OK on success.
	 */
	int initialize(const YARPString &path, const YARPString &init_file);
	
	/**
	 * Overloaded method: Initializes the control card.
	 * @param par is the reference to the parameter type structure (one of
	 * the arguments of the template).
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int initialize(YARPBabybotArmParameters &par);
	
	/**
	 * Uninitializes the control board and frees memory.
	 * This function does all what the destructor has to do.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int uninitialize();

private:
	inline void _angleToEncoders(const double *ang, double *enc, const YARPBabybotArmParameters &_parameters, const double *zeros);
	inline void _encoderToAngles(const double *enc, double *ang, const YARPBabybotArmParameters &_parameters, const double *zeros);
	int _initialize();

	int *_pidSigns;
};

/* implementations */

inline void YARPBabybotArm::_angleToEncoders(const double *ang, double *enc, const YARPBabybotArmParameters &_parameters, const double *zeros)
{
	// 6 joints only.
	int i;
	int j;
	for (i = 0; i < 4; i++)
	{
		j = _parameters._axis_map[i];
		enc[i] = (ang[j] * _parameters._encoderToAngles[i] / (2.0 * pi) + zeros[i]);
	}

	enc[4] = (ang[_parameters._axis_map[4]] * _parameters._encoderToAngles[4] / (2.0 * pi)) +
			 (ang[_parameters._axis_map[3]] * _parameters._fwdCouple[3] / (2.0 * pi)) +
			  zeros[4];
	
	enc[5] = (ang[_parameters._axis_map[5]] * _parameters._encoderToAngles[5] / (2.0 * pi)) +
		     (ang[_parameters._axis_map[3]] * _parameters._fwdCouple[4] / (2.0 * pi)) +
			 (ang[_parameters._axis_map[4]] * _parameters._fwdCouple[5] / (2.0 * pi)) +
			  zeros[5];
}

inline void YARPBabybotArm::_encoderToAngles(const double *enc, double *ang, const YARPBabybotArmParameters &_parameters, const double *zeros)
{
	// 6 joints only.
	int i;
	int j;
	for (i = 0; i < 4; i++)
	{
		j = _parameters._axis_map[i];
		ang[j] = (enc[i] - zeros[i]) * 2.0 * pi / _parameters._encoderToAngles[i];
	}

	double e3, e4, e5;
	e3 = (enc[3] - zeros[3]) * 2.0 * pi;
	e4 = (enc[4] - zeros[4]) * 2.0 * pi;
	e5 = (enc[5] - zeros[5]) * 2.0 * pi;

	ang[_parameters._axis_map[4]] =  e4 / _parameters._encoderToAngles[4] +
			    e3 * _parameters._invCouple[3];

	ang[_parameters._axis_map[5]] = e5 / _parameters._encoderToAngles[5] +
			   e3 * _parameters._invCouple[4] + e4 * _parameters._invCouple[5];
			   
}

#endif

