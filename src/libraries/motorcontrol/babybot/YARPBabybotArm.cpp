// $Id: YARPBabybotArm.cpp,v 1.6 2003-11-21 14:01:32 natta Exp $

#include "YARPBabybotArm.h"

int YARPBabybotArm::setPositions(const double *pos)
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

int YARPBabybotArm::setPositionsAll(const double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);
	 _adapter.IOCtl(CMDSetPositions, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::setVelocities(const double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDSetSpeeds, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::setAccs(const double *acc)
{
	_lock();
	angleToEncoders(acc, _temp_double);
	_adapter.IOCtl(CMDSetAccelerations, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::velocityMove(const double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDVMove, _temp_double);
	_unlock();
	return -1;
}

int YARPBabybotArm::setCommands(const double *pos)
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

int YARPBabybotArm::getVelocities(double *vel)
{
	_lock();
	_adapter.IOCtl(CMDGetSpeeds, _temp_double);
	encoderVelToAngles(_temp_double, vel, _parameters);
	_unlock();
	return -1;
}

int YARPBabybotArm::setG(int i, double g)
{
	_lock();
	SingleAxisParameters cmd;
	cmd.axis = _parameters._axis_map[i];
	cmd.parameters = &g;
	_adapter.IOCtl(CMDSetOffset, &cmd);
	_unlock();
	return -1;
}

int YARPBabybotArm::setGs(double *g)
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