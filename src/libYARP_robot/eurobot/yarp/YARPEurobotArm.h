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

//
// $Id: YARPEurobotArm.h,v 1.4 2004-12-29 14:11:42 beltran Exp $
//
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
#include <yarp/YARPGALILOnEurobotArmAdapter.h>
#include <yarp/YARPRobotMath.h>

typedef YARPGenericControlBoard<YARPGALILOnEurobotArmAdapter, YARPEurobotArmParameters> MyGenericControlBoard;

class YARPEurobotArm : public YARPGenericControlBoard<YARPGALILOnEurobotArmAdapter, YARPEurobotArmParameters>
{
public:
	YARPEurobotArm():MyGenericControlBoard()
	{
		_pidSigns = NULL;
	}

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

	int setStiffness(int i, double s);
	
	// set offset to i-th joint
	int setG(int i, double g);
	// set offsets to all joints
	int setGs(const double *g);

	// set offset and stiffness
	int setPIDs(const LowLevelPID *pids);

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
	int initialize(YARPEurobotArmParameters &par);
	/**
	 * Uninitializes the control board and frees memory.
	 * This function does all what the destructor has to do.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int uninitialize();

private:
	inline void _angleToEncoders(const double *ang, double *enc, const YARPEurobotArmParameters &_parameters, const double *zeros);
	inline void _encoderToAngles(const double *enc, double *ang, const YARPEurobotArmParameters &_parameters, const double *zeros);
	int _initialize();

	int *_pidSigns;
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
