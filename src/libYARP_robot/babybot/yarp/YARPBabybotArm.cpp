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
/// $Id: YARPBabybotArm.cpp,v 1.8 2005-08-21 00:30:47 gmetta Exp $
///
///

#include "YARPBabybotArm.h"

int YARPBabybotArm::setGainsSmoothly(LowLevelPID *finalPIDs, int s)
{
	_lock ();

	ACE_OS::printf("Setting gains");

	double steps = (double) s;
	ACE_Time_Value sleep_period (0, 40*1000);
	
	LowLevelPID *actualPIDs;
	LowLevelPID *deltaPIDs;
	actualPIDs = new LowLevelPID [_parameters._nj];
	deltaPIDs = new LowLevelPID [_parameters._nj];
	ACE_ASSERT (actualPIDs != NULL && deltaPIDs != NULL);

	double *shift;
	double *currentPos;
	shift = new double[_parameters._nj];
	currentPos = new double[_parameters._nj];
	ACE_ASSERT (shift != NULL && currentPos != NULL);

	// set command "here"
	// getPositions(currentPos);
	// setCommands(currentPos);

	for(int i = 0; i < _parameters._nj; i++) 
	{
		SingleAxisParameters cmd;
		cmd.axis = _parameters._axis_map[i];
		cmd.parameters = &actualPIDs[i];
		_adapter.IOCtl(CMDGetPID, &cmd);

		// handle shift (scale)
		double actualShift = actualPIDs[i].SHIFT;
		double finalShift = finalPIDs[i].SHIFT;
		if (actualShift > finalShift)
		{
			shift[i] = actualShift;
			finalPIDs[i] = finalPIDs[i]*(pow(2,(finalShift+actualShift)));
		}
		else
		{
			shift[i] = finalShift;
			actualPIDs[i] = actualPIDs[i]*(pow(2,(actualShift+finalShift)));
		}
			
		deltaPIDs[i] = (finalPIDs[i] - actualPIDs[i])/steps;
	}
	
	for(int t = 0; t < (int) steps; t++)
	{
		for(int i = 0; i < _parameters._nj; i++)
		{
			actualPIDs[i] = actualPIDs[i] + deltaPIDs[i];
			actualPIDs[i].SHIFT = shift[i];
		
			SingleAxisParameters cmd;
			cmd.axis = _parameters._axis_map[i];
			cmd.parameters = &actualPIDs[i];
			_adapter.IOCtl(CMDSetPID, &cmd);
		}
		ACE_OS::sleep(sleep_period);
		ACE_OS::printf(".");

		fflush(stdout);
	}
	ACE_OS::printf("done !\n");

	// if !NULL...
	delete [] actualPIDs;
	delete [] deltaPIDs;
	delete [] shift;
	delete [] currentPos; 

	_unlock ();

	return YARP_OK;
}

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
	return YARP_OK;
}

int YARPBabybotArm::setPositionsAll(const double *pos)
{
	_lock();
	angleToEncoders(pos, _temp_double);
	_adapter.IOCtl(CMDSetPositions, _temp_double);
	_unlock();
	return YARP_OK;
}

int YARPBabybotArm::setVelocities(const double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDSetSpeeds, _temp_double);
	_unlock();
	return YARP_OK;
}

int YARPBabybotArm::setAccs(const double *acc)
{
	_lock();
	angleToEncoders(acc, _temp_double);
	_adapter.IOCtl(CMDSetAccelerations, _temp_double);
	_unlock();
	return YARP_OK;
}

int YARPBabybotArm::velocityMove(const double *vel)
{
	_lock();
	angleToEncoders(vel, _temp_double);
	_adapter.IOCtl(CMDVMove, _temp_double);
	_unlock();
	return YARP_OK;
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
	return YARP_OK;
}

int YARPBabybotArm::getPositions(double *pos)
{
	_lock();
	_adapter.IOCtl(CMDGetPositions, _temp_double);
	encoderToAngles(_temp_double, pos);
	_unlock();
	return YARP_OK;
}

int YARPBabybotArm::getVelocities(double *vel)
{
	_lock();
	_adapter.IOCtl(CMDGetSpeeds, _temp_double);
	encoderVelToAngles(_temp_double, vel, _parameters);
	_unlock();
	return YARP_OK;
}

int YARPBabybotArm::setG(int i, double g)
{
	return MyGenericControlBoard::setOffset(i,g);
}

int YARPBabybotArm::setGs(const double *g)
{
	return MyGenericControlBoard::setOffsets(g);
}

int YARPBabybotArm::setStiffness(int j, double stiff)
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
	return MyGenericControlBoard::setPID(j, pid);		//setPID internally lock and unlock
}

int YARPBabybotArm::_initialize()
{
	_pidSigns = new int [_parameters._nj];
	ACE_ASSERT (_pidSigns != NULL);

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

int YARPBabybotArm::initialize()
{
	_lock();
	int ret = _initialize();
	_unlock();
	return ret;
}

int YARPBabybotArm::initialize(const YARPString &path, const YARPString &init_file)
{
	_lock();
	_parameters.load(path, init_file);
	int ret = _initialize();
	_unlock();
	return ret;
}

int YARPBabybotArm::initialize(YARPBabybotArmParameters &par)
{
	_lock();
	_parameters.copy (par);
	int ret = _initialize();
	_unlock();
	return ret;
}

int YARPBabybotArm::uninitialize()
{
	_lock();
	if (_pidSigns != NULL)
		delete [] _pidSigns;

	_pidSigns = NULL;

	_unlock();
	return MyGenericControlBoard::uninitialize();
}

int YARPBabybotArm::setPIDs(const LowLevelPID *pids)
{
	int j = 0;
	if (pids == NULL)
		return YARP_FAIL;

	for(j = 0; j<_parameters._nj; j++)
	{
		LowLevelPID *tmp = const_cast<LowLevelPID *>(&pids[j]);
		MyGenericControlBoard::setPID(j, *tmp);
	}

	return YARP_OK;
}
