// $Id: YARPEurobotArm.cpp,v 1.1 2003-05-21 14:55:54 beltran Exp $

#include "YARPBabybotArm.h"

int YARPBabybotArm::setPositions(double *pos)
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

int YARPBabybotArm::setPositionsAll(double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);
	 _adapter.IOCtl(CMDSetPositions, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::setVelocities(double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDSetSpeeds, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::setAccs(double *acc)
{
	_lock();
	angleToEncoders(acc, _temp_double);
	_adapter.IOCtl(CMDSetAccelerations, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::velocityMove(double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDVMove, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::setCommands(double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);
	for(int i = 0; i < _parameters._nj; i++)
	{
		SingleAxisParameters cmd;
		cmd.axis = i;
		cmd.parameters = &_temp_double[i];
		if (_parameters._stiffPID[i] != 1)
			_adapter.IOCtl(CMDSetCommand, &cmd);
	}
	_unlock();
	return -1;
}

int YARPBabybotArm::getPositions(double *pos)
{
	_lock();
	_adapter.IOCtl(CMDGetPositions, _temp_double);
	encoderToAngles(_temp_double, pos);
	_unlock();
	return -1;
}