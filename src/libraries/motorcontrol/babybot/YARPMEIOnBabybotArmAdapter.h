#ifndef __MEIONBABYBOTARMADAPTER__
#define __MEIONBABYBOTARMADAPTER__

#include <ace/log_msg.h>
#include <YarpMeiDeviceDriver.h>
#include <string>

// $Id: YARPMEIOnBabybotArmAdapter.h,v 1.3 2003-04-30 08:29:00 natta Exp $

namespace _BabybotArm
{
	const int _nj = 6;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(-310.0, -1500.0, -2.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -30.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(-270.0, -500.0, -2.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-290.0, -700.0, -2.0, 0.0, -150.0, 32767.0, 0.0, 32767.0, 0.0, -100),	
		LowLevelPID(-320.0, -600.0, -2.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-300.0, -900.0, -2.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -100.0),	
		LowLevelPID(-300.0, -600.0, -2.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};

	const double _zeros[_nj] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const int _axis_map[_nj] = {0, 1, 2, 3, 4, 5};
	const int _signs[_nj] = {0, 0, 0, 0, 0, 0};
	const double _encWheels[_nj] = {1000.0, 1000.0, 1000.0, 800.0, 800.0, 800.0};
	const double _encoders[_nj] = {-46.72, 69.9733, -42.9867, 43.5111, 39.3846, 31.7692};
	const double _fwdCouple[3] = {-9.8462, 1.0, -5.5999886532};
}; // namespace

class YARPBabybotArmParameters
{
public:
	YARPBabybotArmParameters()
	{
		_nj = _BabybotArm::_nj;
		for(int i = 0; i<_nj; i++) {
			_highPIDs[i] = _BabybotArm::_highPIDs[i];
			_lowPIDs[i] = _BabybotArm::_lowPIDs[i];
			_zeros[i] = _BabybotArm::_zeros[i];
			_axis_map[i] = _BabybotArm::_axis_map[i];
			_signs[i] = _BabybotArm::_signs[i];
			_encoderToAngles[i] = _BabybotArm::_encoders[i]*_BabybotArm::_encWheels[i];
		}
	}

	int load(const std::string &init_file)
	{
		ACE_ASSERT(0);	//not implemented yet !
	}

	LowLevelPID _highPIDs[_BabybotArm::_nj];
	LowLevelPID _lowPIDs[_BabybotArm::_nj];
	double _zeros[_BabybotArm::_nj];
	double _signs[_BabybotArm::_nj];
	int _axis_map[_BabybotArm::_nj];
	double _encoderToAngles[_BabybotArm::_nj];
	int _nj;
};

class YARPMEIOnBabybotArmAdapter : public YARPMEIDeviceDriver
{
public:
	YARPMEIOnBabybotArmAdapter()
	{
		_initialized = false;
		_amplifiers = false;
		_softwareLimits = false;
	}
	
	~YARPMEIOnBabybotArmAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(const YARPBabybotArmParameters &par)
	{
		//// open device
		_parameters = par;
		MEIOpenParameters op_par;
		op_par.nj= _parameters._nj; 
		if (YARPMEIDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		// amp level and limits
		for(int i=0; i < _parameters._nj; i++)
		{
			SingleAxisParameters cmd;
			cmd.axis=i;
			// amp enable
			short level = 1;
			cmd.parameters=&level;
			IOCtl(CMDSetAmpEnableLevel, &cmd);
			IOCtl(CMDSetAmpEnable, &cmd);
			
			// PUMA has no hw limits
			// set limit events-> none
			ControlBoardEvents event;
			event = CBNoEvent;
			cmd.parameters=&event;
			IOCtl(CMDSetPositiveLimit, &cmd);
			IOCtl(CMDSetNegativeLimit, &cmd);
		}

		/////////////// set ports
		int port;
		port = 0;
		IOCtl(CMDInitPortAsInput, &port);	// port 0 is INPUT
		port = 1;
		IOCtl(CMDInitPortAsOutput, &port);	// port 1 is OUTPUT
		//////////////////

		// amp enable off
		IOParameters cmd;
		cmd.port = 1;
		cmd.value = (short) 0x01;
		_amplifiers = false;
		/////////////////////////

		_initialized = true;
		return YARP_OK;
	}

	int uninitialize()
	{
		/// disable amplifiers
		IOParameters cmd;
		cmd.port = 1;
		cmd.value = (short) 0x01;
		IOCtl(CMDSetOutputPort, &cmd);
		_amplifiers = false;
		//////////////////////////

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
			// IOCtl(CMDEnableAmp, &i);	// this is not required
			IOCtl(CMDClearStop, &i);
		}
		/// activate amplifiers
		IOParameters cmd;
		cmd.port = 1;
		cmd.value = (short) 0x00;
		IOCtl(CMDSetOutputPort, &cmd);
		_amplifiers = true;
		//////////////////////////

		return YARP_OK;
	}

	bool checkPowerOn()
	{
		IOParameters cmd;
		cmd.port = 0;
		IOCtl(CMDGetOutputPort, &cmd);

		if (cmd.value & 0x8)
			return true;
		else
			return false;
	}

	int disableLimitCheck(){
		_softwareLimits = false;
		// LATER disable software limit check (encoders)
		return YARP_OK;
	}
	int enableLimitCheck(){
		_softwareLimits = true;
		// LATER enable software limit check (encoders)
		return YARP_OK;
	}

private:
	bool _initialized;
	bool _amplifiers;
	bool _softwareLimits;
	YARPBabybotArmParameters _parameters;
};

#endif
