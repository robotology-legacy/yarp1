// $Id: YARPBabybotArm.cpp,v 1.1 2003-05-05 16:23:25 natta Exp $

#include "YARPBabybotArm.h"

int YARPBabybotArm::setPositions(double *pos)
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
	_adapter.IOCtl(CMDSetCommands, _temp_double);
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