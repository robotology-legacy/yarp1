#ifndef __YARPBABYBOTARM__
#define __YARPBABYBOTARM__

// $Id: YARPBabybotArm.h,v 1.9 2003-05-29 13:14:24 natta Exp $

#define YARP_BABYBOT_ARM_VERBOSE

#ifdef YARP_BABYBOT_ARM_VERBOSE
#define YARP_BABYBOT_ARM_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_DEBUG:", string)
#else YARP_BABYBOT_ARM_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <conf/YARPConfig.h>
#include <YarpGenericComponent.h>
#include "YARPMEIOnBabybotArmAdapter.h"

class YARPBabybotArm : public YARPGenericComponent<YARPMEIOnBabybotArmAdapter, YARPBabybotArmParameters>
{
public:
	// override activatePID
	int activatePID()
	{
		int ret;
		_lock();
		ret = _adapter.activatePID();
		
		while (!_adapter.checkPowerOn())
		{
			YARP_BABYBOT_ARM_DEBUG(("Press the power on button!\n"));
			ACE_OS::sleep(ACE_Time_Value(1,0));
		}
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
	
	// set offset to i-th joint
	int setG(int i, double g);
	// set offsets to the first nJ joints
	int setGs(double *g, int nJ);

	// very specific functions... 6 dof only!
	inline void angleToEncoders(const double *ang, double *enc)
	{ _angleToEncoders(ang, enc, _parameters, _parameters._zeros); } 
	inline void encoderToAngles(const double *enc, double *ang)
	{ _encoderToAngles(enc, ang, _parameters, _parameters._zeros); }
	inline void angleVelToEncoders(const double *ang, double *enc, const YARPBabybotArmParameters &_parameters)
	{	
		double zeros[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		_angleToEncoders(ang, enc, _parameters, zeros);
	}
	inline void encoderVelToAngles(const double *enc, double *ang, const YARPBabybotArmParameters &_parameters)
	{ 
		double zeros[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		_angleToEncoders(enc, ang, _parameters, zeros);
	}

private:
	inline void _angleToEncoders(const double *ang, double *enc, const YARPBabybotArmParameters &_parameters, const double *zeros);
	inline void _encoderToAngles(const double *enc, double *ang, const YARPBabybotArmParameters &_parameters, const double *zeros);
};

inline void YARPBabybotArm::_angleToEncoders(const double *ang, double *enc, const YARPBabybotArmParameters &_parameters, const double *zeros)
{
	// 6 joints only.
	for (int i = 0; i < 4; i++)
	{
		enc[i] = (ang[i] * _parameters._encoderToAngles[i] / (2.0 * pi) + zeros[i]);
	}

	enc[4] = (ang[4] * _parameters._encoderToAngles[4] / (2.0 * pi)) +
			 (ang[3] * _parameters._fwdCouple[3] / (2.0 * pi)) +
			  zeros[4];

	enc[5] = (ang[5] * _parameters._encoderToAngles[5] / (2.0 * pi)) +
		     (ang[3] * _parameters._fwdCouple[4] / (2.0 * pi)) +
			 (ang[4] * _parameters._fwdCouple[5] / (2.0 * pi)) +
			  zeros[5];
}

inline void YARPBabybotArm::_encoderToAngles(const double *enc, double *ang, const YARPBabybotArmParameters &_parameters, const double *zeros)
{
	// 6 joints only.
	for (int i = 0; i < 4; i++)
	{
		ang[i] = (enc[i] - zeros[i]) * 2.0 * pi / _parameters._encoderToAngles[i];
	}

	double e3, e4;
	e3 = (enc[3] - zeros[3]) * 2.0 * pi;
	e4 = (enc[4] - zeros[4]) * 2.0 * pi;

	ang[4] =  e4 / _parameters._encoderToAngles[4] +
			    e3 * _parameters._invCouple[3];

	ang[5] = (enc[5] - zeros[5]) * 2.0 * pi / _parameters._encoderToAngles[5] +
			   e3 * _parameters._invCouple[4] + e4 * _parameters._invCouple[5];
}

#endif

