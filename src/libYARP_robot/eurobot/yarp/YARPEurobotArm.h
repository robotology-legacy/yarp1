///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

//
// $Id: YARPEurobotArm.h,v 1.1 2004-07-28 17:17:35 babybot Exp $
//
#ifndef __YARPBABYBOTARM__
#define __YARPBABYBOTARM__

#define YARP_BABYBOT_ARM_VERBOSE

#ifdef YARP_BABYBOT_ARM_VERBOSE
#define YARP_BABYBOT_ARM_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_DEBUG:", string)
#else YARP_BABYBOT_ARM_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include "YARPGALILOnEurobotArmAdapter.h"

class YARPEurobotArm : public YARPGenericControlBoard<YARPGALILOnEurobotArmAdapter, YARPEurobotArmParameters>
{
public:
	// override activatePID
	int activatePID()
	{
		int ret;
		_lock();
		ret = _adapter.activatePID();
		
		/*********************
		while (!_adapter.checkPowerOn())
		{
			YARP_BABYBOT_ARM_DEBUG(("Press the power on button!\n"));
			ACE_OS::sleep(ACE_Time_Value(1,0));
		}
		***********/
		YARP_BABYBOT_ARM_DEBUG(("Press the power on button! And then any key\n"));
		getchar(); //waiting for the user to press a key

		_unlock();
		return YARP_OK;
	}
	int calibrate()
	{
		int ret;
		_lock();
		ret = _adapter.calibrate();
		_unlock();
		return ret;
	}
	// overrides basic methods -> add coupling
	int setPositions(const double *pos);
	int setPositionsAll(const double *pos);
	int setVelocities(const double *vel);
	int setAccs(const double *acc);
	int velocityMove(const double *vel);
	int setCommands(const double *pos);
	int getPositions(double *pos);
	int getVelocities(double *vel);
	
	// set offset to i-th joint
	int setG(int i, double g);
	// set offsets to the first nJ joints
	int setGs(double *g);

	// very specific functions... 6 dof only!
	inline void angleToEncoders(const double *ang, double *enc)
	{ _angleToEncoders(ang, enc, _parameters, _parameters._zeros); } 
	inline void encoderToAngles(const double *enc, double *ang)
	{ _encoderToAngles(enc, ang, _parameters, _parameters._zeros); }
	inline void angleVelToEncoders(const double *ang, double *enc, const YARPEurobotArmParameters &_parameters)
	{	
		double zeros[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		_angleToEncoders(ang, enc, _parameters, zeros);
	}
	inline void encoderVelToAngles(const double *enc, double *ang, const YARPEurobotArmParameters &_parameters)
	{ 
		double zeros[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		_angleToEncoders(enc, ang, _parameters, zeros);
	}

private:
	inline void _angleToEncoders(const double *ang, double *enc, const YARPEurobotArmParameters &_parameters, const double *zeros);
	inline void _encoderToAngles(const double *enc, double *ang, const YARPEurobotArmParameters &_parameters, const double *zeros);
};

inline void YARPEurobotArm::_angleToEncoders(const double *ang, double *enc, const YARPEurobotArmParameters &_parameters, const double *zeros)
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

inline void YARPEurobotArm::_encoderToAngles(const double *enc, double *ang, const YARPEurobotArmParameters &_parameters, const double *zeros)
{
	// 6 joints only.
	int i;
	int j;
	for (i = 0; i < 4; i++)
	{
		j = _parameters._axis_map[i];
		ang[j] = (enc[i] - zeros[i]) * 2.0 * pi / _parameters._encoderToAngles[i];
	}

	double e3, e4;
	e3 = (enc[3] - zeros[3]) * 2.0 * pi;
	e4 = (enc[4] - zeros[4]) * 2.0 * pi;

	ang[_parameters._axis_map[4]] =  e4 / _parameters._encoderToAngles[4] +
			    e3 * _parameters._invCouple[3];

	ang[_parameters._axis_map[5]] = (enc[5] - zeros[5]) * 2.0 * pi / _parameters._encoderToAngles[5] +
			   e3 * _parameters._invCouple[4] + e4 * _parameters._invCouple[5];
}

#endif
