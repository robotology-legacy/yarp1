// $Id: YARPEurobotArm.cpp,v 1.8 2003-12-18 16:37:25 beltran Exp $

#include "YARPEurobotArm.h"

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
	printf(" Vel in Encoders: %f %f %f %f \n",_temp_double[0],_temp_double[1],_temp_double[2],_temp_double[3]);
	_adapter.IOCtl(CMDVMove, _temp_double);
	printf(" Vel in Encoders2: %f %f %f %f \n",_temp_double[0],_temp_double[1],_temp_double[2],_temp_double[3]);
	_unlock();
	return -1;
}

int YARPEurobotArm::setCommands(const double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);

	_adapter.IOCtl(CMDSetCommands,_temp_double);
	/*
	for(int i = 0; i < _parameters._nj; i++)
	{
		SingleAxisParameters cmd;
		cmd.axis = i;
		cmd.parameters = &_temp_double[i];
		if (_parameters._stiffPID[i] != 1)
			_adapter.IOCtl(CMDSetCommand, &cmd);
	}
	*/
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

int YARPEurobotArm::setGs(double *g)
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

