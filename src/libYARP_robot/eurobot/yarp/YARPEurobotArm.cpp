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
///                    #carlos#
///
///     "Licensed under the Academic Free License Version 1.0"
///

//
// $Id: YARPEurobotArm.cpp,v 1.3 2004-12-29 14:11:42 beltran Exp $
//

#include <yarp/YARPEurobotArm.h>

int YARPEurobotArm::setPositions(const double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);
	for(int i = 0; i < _parameters._nj; i++)
	{
		SingleAxisParameters cmd;
		cmd.axis = i;
		cmd.parameters = &_temp_double[i];
	
		if (_parameters._stiffPID[i] == 1)
			_adapter.IOCtl(CMDSetPosition, &cmd);
	}
	// _adapter.IOCtl(CMDSetPositions, _temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::setPositionsAll(const double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);
	 _adapter.IOCtl(CMDSetPositions, _temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::setVelocities(const double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDSetSpeeds, _temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::setAccs(const double *acc)
{
	_lock();
	angleToEncoders(acc, _temp_double);
	_adapter.IOCtl(CMDSetAccelerations, _temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::velocityMove(const double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDVMove, _temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::setCommands(const double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);

	_adapter.IOCtl(CMDSetCommands,_temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::getPositions(double *pos)
{
	_lock();
	_adapter.IOCtl(CMDGetPositions, _temp_double);
	encoderToAngles(_temp_double, pos);
	_unlock();
	return -1;
}

int YARPEurobotArm::getVelocities(double *vel)
{
	_lock();
	_adapter.IOCtl(CMDGetSpeeds, _temp_double);
	encoderVelToAngles(_temp_double, vel, _parameters);
	_unlock();
	return -1;
}

int YARPEurobotArm::setG(int i, double g)
{
	_lock();
	SingleAxisParameters cmd;
	cmd.axis = _parameters._axis_map[i];
	cmd.parameters = &g;
	_adapter.IOCtl(CMDSetOffset, &cmd);
	_unlock();
	return -1;
}

int YARPEurobotArm::setGs(const double *g)
{
	_lock();
	
	int i, j;
	for(i = 0; i<_parameters._nj; i++)
	{
		j = _parameters._axis_map[i];
		_temp_double[i] = g[j];
	}
			
	_adapter.IOCtl(CMDSetOffsets, _temp_double);
	_unlock();
	return -1;
}

int YARPEurobotArm::setStiffness(int j, double stiff)
{
	int axes = _parameters._axis_map[j];
	LowLevelPID pid = _parameters._lowPIDs[axes];
	pid.KD = 0.0;
	pid.KI = 0.0;
	pid.AC_FF = 0.0;
	pid.VEL_FF = 0.0;
	pid.FRICT_FF = 0.0;
	// don't touch I_LIMIT, OFFSET, T_LIMIT, SHIFT
	pid.KP = fabs(stiff)*_pidSigns[axes];
	return MyGenericControlBoard::setPID(j, pid, false);		//setPID internally lock and unlock
}

int YARPEurobotArm::_initialize()
{
	_pidSigns = new int [_parameters._nj];
	int j = 0;
	for(j = 0; j<_parameters._nj; j++)
	{
		if (_parameters._highPIDs[j].KP > 0)
			_pidSigns[j] = 1;
		else if(_parameters._highPIDs[j].KP < 0)
			_pidSigns[j] = -1;
		else
			_pidSigns[j] = 0;
	}

	return MyGenericControlBoard::_initialize();
}

int YARPEurobotArm::initialize()
{
	_lock();
	int ret = _initialize();
	_unlock();
	return ret;
}

int YARPEurobotArm::initialize(const YARPString &path, const YARPString &init_file)
{
	_lock();
	_parameters.load(path, init_file);
	int ret = _initialize();
	_unlock();
	return ret;
}

int YARPEurobotArm::initialize(YARPEurobotArmParameters &par)
{
	_lock();
	_parameters.copy (par);
	int ret = _initialize();
	_unlock();
	return ret;
}

int YARPEurobotArm::uninitialize()
{
	_lock();
	if (_pidSigns != NULL)
		delete [] _pidSigns;

	_pidSigns = NULL;

	_unlock();
	return MyGenericControlBoard::uninitialize();
}

int YARPEurobotArm::setPIDs(const LowLevelPID *pids)
{
	int j = 0;
	if (pids == NULL)
		return YARP_FAIL;

	for(j = 0; j<_parameters._nj; j++)
	{
		LowLevelPID *tmp = const_cast<LowLevelPID *>(&pids[j]);
		MyGenericControlBoard::setPID(j, *tmp, false);
	}

	/*
	int axes = _parameters._axis_map[j];
	LowLevelPID pid = _parameters._lowPIDs[axes];
	pid.KD = 0.0;
	pid.KI = 0.0;
	pid.AC_FF = 0.0;
	pid.VEL_FF = 0.0;
	pid.FRICT_FF = 0.0;
	pid.OFFSET = g;
	// don't touch I_LIMIT, T_LIMIT, SHIFT
	pid.KP = fabs(k)*_pidSigns[axes];
	return MyGenericControlBoard::setPID(j, pid, false);		//setPID internally lock and unlock
	*/
	return YARP_OK;
}
