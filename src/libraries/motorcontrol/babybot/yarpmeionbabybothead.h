#ifndef __MEIONBABYBOTHEAD__
#define __MEIONBABYBOTHEAD__

#include <ace/log_msg.h>
#include <YarpMeiDeviceDriver.h>
#include <string>

// $Id: yarpmeionbabybothead.h,v 1.3 2003-04-26 11:01:36 natta Exp $

namespace _BabybotHead
{
	const int _nj = 5;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(150.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(150.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(100.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(100.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(300.0, 1200.0, 1.0, 100.0, 200.0, 32767.0, 0.0, 32767.0, 0.0, 0.0)
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0)
	};

	const double _zeros[_nj] = {0.0, 0.0, 0.0, 0.0, 0.0};
	const int _axis_map[_nj] = {4, 3, 2, 1, 0};
	const int _signs[_nj] = {0, 1, 0, 1, 0};
	const double _encoderToAngles[_nj] = {116000.0, 116000.0, 72000.0, 164000.0, 128000.0};
	
}; // namespace

class YARPBabybotHeadParameters
{
public:
	YARPBabybotHeadParameters()
	{
		_nj = _BabybotHead::_nj;
		for(int i = 0; i<_nj; i++) {
			_highPIDs[i] = _BabybotHead::_highPIDs[i];
			_lowPIDs[i] = _BabybotHead::_lowPIDs[i];
			_zeros[i] = _BabybotHead::_zeros[i];
			_axis_map[i] = _BabybotHead::_axis_map[i];
			_signs[i] = _BabybotHead::_signs[i];
			_encoderToAngles[i] = _BabybotHead::_encoderToAngles[i];
		}
	}

	int load(const std::string &init_file)
	{
		ACE_ASSERT(0);	//not implemented yet !
	}

	LowLevelPID _highPIDs[_BabybotHead::_nj];
	LowLevelPID _lowPIDs[_BabybotHead::_nj];
	double _zeros[_BabybotHead::_nj];
	double _signs[_BabybotHead::_nj];
	int _axis_map[_BabybotHead::_nj];
	double _encoderToAngles[_BabybotHead::_nj];
	int _nj;
};

class YARPMEIOnBabybotHeadAdapter : public YARPMEIDeviceDriver
{
public:
	YARPMEIOnBabybotHeadAdapter()
	{
		_initialized = false;
	}
	
	~YARPMEIOnBabybotHeadAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(const YARPBabybotHeadParameters &par)
	{
		_parameters = par;
		MEIOpenParameters op_par;
		op_par.nj= _parameters._nj; 
		if (YARPMEIDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		for(int i=0; i < _parameters._nj; i++)
		{
			SingleAxisParameters cmd;
			cmd.axis=i;
			// amp enable level
			short level = 1;
			cmd.parameters=&level;
			IOCtl(CMDSetAmpEnableLevel, &cmd);
			// limit levels
			cmd.parameters=&level;
			IOCtl(CMDSetPositiveLevel, &cmd);
			IOCtl(CMDSetNegativeLevel, &cmd);
			// limit events
			ControlBoardEvents event;
			event = CBNoEvent;
			cmd.parameters=&event;
			IOCtl(CMDSetPositiveLimit, &cmd);
			IOCtl(CMDSetNegativeLimit, &cmd);
		}

		_initialized = true;
		return YARP_OK;
	}

	int uninitialize()
	{
		if (YARPMEIDeviceDriver::close() != 0)
			return YARP_FAIL;

		_initialized = false;
		return YARP_OK;
	}
	int idleMode()
	{
		for(int i = 0; i < _parameters._nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
			SingleAxisParameters cmd;
			cmd.axis = i;
			short level = 0;
			cmd.parameters=&level;
			IOCtl(CMDSetAmpEnable, &cmd);
		}
		return YARP_OK;
	}

	int activatePID()
	{
		for(int i = 0; i < _parameters._nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
			SingleAxisParameters cmd;
			cmd.axis = i;
			cmd.parameters = &_parameters._highPIDs[i];
			IOCtl(CMDSetPID, &cmd);
			double pos = 0.0;
			cmd.parameters = &pos;
			IOCtl(CMDDefinePosition, &cmd);
			IOCtl(CMDControllerRun, &i);
			IOCtl(CMDEnableAmp, &i);
			IOCtl(CMDClearStop, &i);
		}
		return YARP_OK;
	}

private:
	bool _initialized;
	YARPBabybotHeadParameters _parameters;
};

#endif	// .h