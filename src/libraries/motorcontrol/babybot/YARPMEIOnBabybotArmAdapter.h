#ifndef __MEIONBABYBOTARMADAPTER__
#define __MEIONBABYBOTARMADAPTER__

// $Id: YARPMEIOnBabybotArmAdapter.h,v 1.8 2003-05-05 17:18:18 natta Exp $

#include <ace/log_msg.h>
#include <YarpMeiDeviceDriver.h>
#include <string>

#define YARP_BABYBOT_ARM_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_ARM_ADAPTER_VERBOSE
#define YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

namespace _BabybotArm
{
	const int _nj = 6;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(-310.0, -1500.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -30.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(-270.0, -500.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-290.0, -700.0, 0.0, 0.0, -150.0, 32767.0, 0.0, 32767.0, 0.0, -100),	
		LowLevelPID(-320.0, -600.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-300.0, -900.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -100.0),	
		LowLevelPID(-300.0, -600.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};

	const double _zeros[_nj] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const int _axis_map[_nj] = {0, 1, 2, 3, 4, 5};
	const int _signs[_nj] = {0, 0, 0, 0, 0, 0};
	const double _encWheels[_nj] = {1000.0, 1000.0, 1000.0, 800.0, 800.0, 800.0};
	const double _encoders[_nj] = {-46.72, 69.9733, -42.9867, 43.5111, 39.3846, 31.7692};
	const double _fwdCouple[_nj] = {0.0, 0.0, 0.0, -9.8462*_encWheels[3], 1.0*_encWheels[4], -5.5999886532*_encWheels[5]};
	// const double _invCouple[_nj] = {0.0, 0.0, 0.0, 1/_fwdCouple[3], 1/_fwdCouple[4], 1/_fwdCouple[5]};
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
			_fwdCouple[i] = _BabybotArm::_fwdCouple[i];
		}

		// compute inv couple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) + (_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
		_invCouple[5] = -_fwdCouple[5] / (_encoderToAngles[4] * _encoderToAngles[5]);
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
	double _fwdCouple[_BabybotArm::_nj];
	double _invCouple[_BabybotArm::_nj];
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

	int calibrate() {
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Starting calibration routine...\n"));
		if (! (_initialized && _amplifiers) )
		{
			YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Sorry cannot calibrate encoders, the arm not initialized or the power is down!\n"));
			return YARP_FAIL;
		}

		bool limitFlag = false;
		if (_softwareLimits)
			limitFlag = true;
		disableLimitCheck();

		double speeds1[6] = {500.0, 500.0, 500.0, 500.0, 500.0, 500.0};
		double speeds2[6] = {-500.0, -500.0, -500.0, -500.0, -500.0, -500.0};
		double acc[6] = {5000.0, 5000.0, 5000.0, 5000.0, 5000.0, 5000.0};
		double posHome[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		double pos1[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		double pos2[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		double newHome[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

		//////////// find first index
		_setHomeConfig(CBStopEvent);
		_clearStop();
		//
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Searching first index...\n"));
		IOCtl(CMDSetAccelerations, acc);
		IOCtl(CMDVMove, speeds1);
		IOCtl(CMDWaitForMotionDone, NULL);
		IOCtl(CMDGetPositions, pos1);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("done !\n"));
		////////////////////////////

		//////////// go back to home position
		_setHomeConfig(CBNoEvent);
		_clearStop();
		//
		IOCtl(CMDSetSpeeds, speeds1);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Going back to initial position... \n"));
		IOCtl(CMDSetPositions, posHome);
		IOCtl(CMDWaitForMotionDone, NULL);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("done !\n"));
		///////////////////////////////

		//////////// find second index
		_setHomeConfig(CBStopEvent);
		_clearStop();
		//
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Searching second index ... \n"));
		IOCtl(CMDSetAccelerations, acc);
		IOCtl(CMDVMove, speeds2);
		IOCtl(CMDWaitForMotionDone, NULL);
		IOCtl(CMDGetPositions, pos2);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("done !\n"));
		////////////////////////////

		// compute new home position
		for(int i = 0; i < _parameters._nj; i++)
		{
			newHome[i] = (pos1[i]+pos2[i])/2;
		}
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Calibration: %lf %lf %lf %lf %lf %lf!\n", newHome[0], newHome[1], newHome[2], newHome[3], newHome[5]));

		//////////// go back to the calibrated position
		_setHomeConfig(CBNoEvent);
		_clearStop();
		//
		IOCtl(CMDSetSpeeds, speeds1);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Finally move to the calibrated position... \n"));
		IOCtl(CMDSetPositions, newHome);
		IOCtl(CMDWaitForMotionDone, NULL);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("done!\n"));
		///////////////////////////////

		// reset encoders here
		_setHomeConfig(CBNoEvent);
		_clearStop();
		IOCtl(CMDDefinePositions, posHome);	// posHome is still '0'
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Reset encoders... done!\n"));
		
		if (limitFlag)
			enableLimitCheck();

		return YARP_OK;
	}

private:

	void _setHomeConfig(int event)
	{
		for (int i = 0; i < _parameters._nj; i++)
		{
			SingleAxisParameters cmd;
			int ipar;
			double dpar;
			cmd.axis = i;
			cmd.parameters = &ipar;
			
			ipar = CBIndexOnly;			// index_only
			IOCtl(CMDSetHomeIndexConfig, &cmd);
			ipar = 0;					// (active low)
			IOCtl(CMDSetHomeLevel, &cmd);
			ipar = event;
			IOCtl(CMDSetHome, &cmd);
			cmd.parameters = &dpar;
			dpar = 50000.0;				// stop rate (acc)
			IOCtl(CMDSetStopRate, &cmd);
		}
	}

	void _clearStop() {
		for (int i = 0; i < _parameters._nj; i++)
			IOCtl(CMDClearStop, &i);	// clear stop event
	}

	bool _initialized;
	bool _amplifiers;
	bool _softwareLimits;
	YARPBabybotArmParameters _parameters;
};

#endif
