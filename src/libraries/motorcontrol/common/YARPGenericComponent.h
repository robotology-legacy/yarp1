// class for babybot head 
#ifndef __YARPGENERICCOMPONENTHH__
#define __YARPGENERICCOMPONENTHH__

#include "YARPMEIOnBabybotHead.h"
#include <YARPSemaphore.h>

#include <math.h>
#include <vector>
#include <string>

#define YARP_GEN_COMPONENT_VERBOSE

#ifdef YARP_GEN_COMPONENT_VERBOSE
#define YARP_GEN_COMPONENT_DEBUG(string) YARP_DEBUG("GEN_COMPONENT_DEBUG :", string)
#else YARP_GEN_COMPONENT_DEBUG(string) YARP_NULL_DEBUG
#endif

template <class ADAPTER, class PARAMETERS>
class YARPGenericComponent
{
public:
	YARPGenericComponent<ADAPTER, PARAMETERS> ()
	{_temp_double = NULL;}
	~YARPGenericComponent<ADAPTER, PARAMETERS>(){}

	int initialize()
	{
		_lock();
		int ret = _initialize();
		_unlock();
		return ret;
	}

	int initialize(const std::string &init_file)
	{
		_lock();
		_parameters.load(init_file);
		int ret = _initialize();
		_unlock();
		return ret;
	}

	int idleMode()
	{	
		int ret;
		_lock();
		ret = _adapter.idleMode();
		_unlock();
		return ret;
	}

	int activatePID()
	{
		_lock();
		int ret = _adapter.activatePID();
		_unlock();
		return ret;
	}

	int uninitialize()
	{
		_lock();
		if (_temp_double != NULL)
			delete [] _temp_double;
		_temp_double = NULL;

		if (_adapter.uninitialize() == YARP_FAIL) {
			YARP_GEN_COMPONENT_DEBUG(("Error un-initializing MEI board!\n"));
			_unlock();
			return YARP_FAIL;
		}
		YARP_GEN_COMPONENT_DEBUG(("MEI board uninitialized!\n"));
		_unlock();
		return YARP_OK;
	}

	int setPositions(double *pos)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++)
			_temp_double[i] = angleToEncoder(pos[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											_parameters._zeros[i],
											_parameters._signs[i]);

		_adapter.IOCtl(CMDSetPositions, _temp_double);
		_unlock();
		return -1;
	}

	int setVelocities(double *vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) {
			_temp_double[i] = angleToEncoder(vel[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetSpeeds, _temp_double);
		_unlock();
		return -1;
	}
	int setAccs(double *acc)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) {
			_temp_double[i] = angleToEncoder(acc[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetAccelerations, _temp_double);
		_unlock();
		return -1;
	}

	int velocityMove(double *vel)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) {
			_temp_double[i] = angleToEncoder(vel[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDVMove, _temp_double);
		_unlock();
		return -1;
	}

	int setCommands(double *pos)
	{
		_lock();
		for (int i = 0; i < _parameters._nj; i++) {
			_temp_double[i] = angleToEncoder(pos[_parameters._axis_map[i]],
											_parameters._encoderToAngles[i],
											0.0,
											_parameters._signs[i]);
		}

		_adapter.IOCtl(CMDSetCommands, _temp_double);
		_unlock();
		return -1;
	}

	bool checkMotionDone()
	{
		bool ret;
		_lock();
		_adapter.IOCtl(CMDCheckMotionDone, &ret);
		_unlock();
		return ret;
	}

	int getPositions(double *pos)
	{
		_lock();
		_adapter.IOCtl(CMDGetPositions, _temp_double);
		for (int i = 0; i < _parameters._nj; i++) {
			pos[_parameters._axis_map[i]] = encoderToAngle(_temp_double[i],
												_parameters._encoderToAngles[i],
												_parameters._zeros[i],
												_parameters._signs[i]);
		
		}
		_unlock();
		return -1;
	}

	int setGainsSmoothly(LowLevelPID *finalPIDs, int s = 150);
	
public:
	PARAMETERS _parameters;

private:
	inline void _lock(void)
	{_mutex.Wait();}	// add timeout ?

	inline void _unlock(void)
	{_mutex.Post();}

	int _initialize()
	{
		_temp_double = new double [_parameters._nj];
		if (_adapter.initialize(_parameters) == YARP_FAIL) {
			YARP_GEN_COMPONENT_DEBUG(("Error initializing MEI board!\n"));
			_unlock();
			return YARP_FAIL;
		}
		YARP_GEN_COMPONENT_DEBUG(("MEI board initialized!\n"));
		return YARP_OK;
	}

	YARPSemaphore _mutex;

	int *_tmp_int;
	ADAPTER _adapter;
	double *_temp_double;
};

// This procedure set new gains smoothly in 's' steps.
// Care must be taken in dealing with the SHIFT (scale) parameter
// Here we take the maximum value between the actual and the new one.
// "finalPIDs" are scaled accordingly.
template <class ADAPTER, class PARAMETERS>
int YARPGenericComponent<ADAPTER, PARAMETERS>::setGainsSmoothly(LowLevelPID *finalPIDs, int s)
{
	ACE_OS::printf("Setting gains");

	double steps = (double) s;
	ACE_Time_Value sleep_period (0, 40*1000);
	
	std::vector<LowLevelPID> actualPIDs(_parameters._nj);
	std::vector<LowLevelPID> deltaPIDs(_parameters._nj);
	double *shift;
	double *currentPos;
	shift = new double[_parameters._nj];
	currentPos = new double[_parameters._nj];
	
	// set command "here"
	getPositions(currentPos);
	setCommands(currentPos);
		
	for(int i = 0; i < _parameters._nj; i++) {
		SingleAxisParameters cmd;
		cmd.axis = i;
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
		for(i = 0; i < _parameters._nj; i++)
		{
			actualPIDs[i] = actualPIDs[i] + deltaPIDs[i];
			actualPIDs[i].SHIFT = shift[i];
		
			SingleAxisParameters cmd;
			cmd.axis = i;
			cmd.parameters = &actualPIDs[i];
			_adapter.IOCtl(CMDSetPID, &cmd);
		}
		ACE_OS::sleep(sleep_period);
		ACE_OS::printf(".");
	}
	ACE_OS::printf("done !\n");

	delete [] shift;
	delete [] currentPos; 
	return -1;
}

#endif // h