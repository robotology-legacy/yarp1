// -- by carlos

#ifndef __GALILONEUROBOTARMADAPTER__
#define __GALILONEUROBOTARMADAPTER__

// $Id: YARPGALILOnEurobotArmAdapter.h,v 1.5 2003-08-19 08:14:29 beltran Exp $

#include <ace/Log_Msg.h>
#include <YARPGalilDeviceDriver.h>
#include <string>

#define YARP_BABYBOT_ARM_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_ARM_ADAPTER_VERBOSE
#define YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <YARPConfigFile.h>

namespace _EurobotArm
{
	const int _nj = 6;
	//char _mask = (char) 0x3F;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(320.63, 2509.63, 85.71, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -30.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(104.38, 2726.88, 5.41, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(192.0, 2694.13, 20.63, 0.0, -150.0, 32767.0, 0.0, 32767.0, 0.0, -100),	
		LowLevelPID(179.50, 1341.25, 69.10, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(238.63, 1782.63, 91.85, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -100.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		// LowLevelPID(-310.0, -1500.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -30.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-5.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(-320.0, -600.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(-300.0, -900.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, -100.0),	
		LowLevelPID(-300.0, -600.0, 0.0, 0.0, -100.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
	};

	const double _zeros[_nj] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const int _axis_map[_nj] = {0, 1, 2, 3, 4, 5};
	const int _signs[_nj] = {0, 0, 0, 0, 0, 0};
	const double _encWheels[_nj] = {1000.0, 1000.0, 1000.0, 800.0, 800.0, 800.0};
	const double _encoders[_nj] = {-46.72, 69.9733, -42.9867, 43.5111, 39.3846, 31.7692};
	const double _fwdCouple[_nj] = {0.0, 0.0, 0.0, -9.8462*_encWheels[3], 1.0*_encWheels[4], -5.5999886532*_encWheels[5]};
	const int _stiffPID[_nj] = {0, 0, 0, 1, 1, 1};
	const double _maxDAC[_nj] = {32767.0, 32767.0, 32767.0, 32767.0, 32767.0, 32767.0};
}; // namespace

class YARPEurobotArmParameters
{
public:
	YARPEurobotArmParameters()
	{
		_highPIDs = NULL;
		_lowPIDs = NULL;
		_zeros = NULL;
		_signs = NULL;
		_axis_map = NULL;
		_encoderToAngles = NULL;
		_fwdCouple = NULL;
		_invCouple = NULL;
		_stiffPID = NULL;
		_maxDAC = NULL;
		_nj = 0;
		
		_nj = _EurobotArm::_nj;
		//_mask = _EurobotArm::_mask;
		_mask = (char) 0x3F;
		_realloc(_nj);
		int i;
		for(i = 0; i<_nj; i++) {
			_highPIDs[i] = _EurobotArm::_highPIDs[i];
			_lowPIDs[i] = _EurobotArm::_lowPIDs[i];
			_zeros[i] = _EurobotArm::_zeros[i];
			_axis_map[i] = _EurobotArm::_axis_map[i];
			_signs[i] = _EurobotArm::_signs[i];
			_encoderToAngles[i] = _EurobotArm::_encoders[i]*_EurobotArm::_encWheels[i];
			_fwdCouple[i] = _EurobotArm::_fwdCouple[i];
			_stiffPID[i] = _EurobotArm::_stiffPID[i];
			_maxDAC[i] = _EurobotArm::_maxDAC[i];
		}

		// compute inv couple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) +
						(_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
		_invCouple[5] = -_fwdCouple[5] / (_encoderToAngles[4] * _encoderToAngles[5]);
	}

	~YARPEurobotArmParameters()
	{
		if (_highPIDs != NULL)
			delete [] _highPIDs;
		if (_lowPIDs != NULL)
			delete [] _lowPIDs;
		if (_zeros != NULL)
			delete [] _zeros;
		if (_signs != NULL)
			delete [] _signs;
		if (_axis_map != NULL)
			delete [] _axis_map;
		if (_encoderToAngles != NULL)
			delete [] _encoderToAngles;
		if (_fwdCouple != NULL)
			delete [] _fwdCouple;
		if (_invCouple != NULL)
			delete [] _invCouple;
		if (_stiffPID != NULL)
			delete [] _stiffPID;
		if (_maxDAC != NULL)
			delete [] _maxDAC;
	}

	int load(const std::string &path, const std::string &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(),init_file.c_str());
		
		// get number of joints
		if (cfgFile.get("[GENERAL]", "Joints", &_nj, 1) == YARP_FAIL)
			return YARP_FAIL;

		// delete and allocate new memory
		_realloc(_nj);
		
		int i;
		for(i = 0; i<_nj; i++)
		{
			char dummy[80];
			double tmp[12];
			sprintf(dummy, "%s%d", "Pid", i);
			if (cfgFile.get("[HIGHPID]", dummy, tmp, 12) == YARP_FAIL)
				return YARP_FAIL;
			_highPIDs[i] = LowLevelPID(tmp);
			
			if (cfgFile.get("[LOWPID]", dummy, tmp, 12) == YARP_FAIL)
				return YARP_FAIL;
			_lowPIDs[i] = LowLevelPID(tmp);
		}
		
		if (cfgFile.get("[GENERAL]", "Zeros", _zeros, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "AxisMap", _axis_map, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "Signs", _signs, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "FwdCouple", _fwdCouple, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "MaxDAC", _maxDAC, _nj) == YARP_FAIL)
			return YARP_FAIL;

		// build encoder to angles
		double *encoders = new double [_nj];
		double *encWheels = new double [_nj];
		if (cfgFile.get("[GENERAL]", "Encoder", encoders, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "EncWheel", encWheels, _nj) == YARP_FAIL)
			return YARP_FAIL;
		for(i = 0; i< _nj; i++)
			_encoderToAngles[i] = encoders[i]*encWheels[i];
		delete [] encoders;
		delete [] encWheels;
		///////////////////////////////////////////////////

		// build _invCouple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) +
						(_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
		_invCouple[5] = -_fwdCouple[5] / (_encoderToAngles[4] * _encoderToAngles[5]);
		////////////////////////////////////////

		return YARP_OK;
	}

private:
	void _realloc(int nj)
	{
		if (_highPIDs != NULL)
			delete [] _highPIDs;
		if (_lowPIDs != NULL)
			delete [] _lowPIDs;
		if (_zeros != NULL)
			delete [] _zeros;
		if (_signs != NULL)
			delete [] _signs;
		if (_axis_map != NULL)
			delete [] _axis_map;
		if (_encoderToAngles != NULL)
			delete [] _encoderToAngles;
		if (_fwdCouple != NULL)
			delete [] _fwdCouple;
		if (_invCouple != NULL)
			delete [] _invCouple;
		if (_stiffPID != NULL)
			delete [] _stiffPID;
		if (_maxDAC != NULL)
			delete [] _maxDAC;
		
		_highPIDs = new LowLevelPID [nj];
		_lowPIDs = new LowLevelPID [nj];
		_zeros = new double [nj];
		_signs = new double [nj];
		_axis_map = new int [nj];
		_encoderToAngles = new double [nj];
		_fwdCouple = new double [nj];
		_invCouple = new double [nj];
		_stiffPID = new int [nj];
		_maxDAC = new double [nj];
	}

public:
	LowLevelPID *_highPIDs;
	LowLevelPID *_lowPIDs;
	double *_zeros;
	double *_signs;
	int *_axis_map;
	double *_encoderToAngles;
	double *_fwdCouple;
	double *_invCouple;
	int *_stiffPID;
	int _nj;
	char _mask;
	double *_maxDAC;
};

class YARPGALILOnEurobotArmAdapter : public YARPGalilDeviceDriver
{
public:
	YARPGALILOnEurobotArmAdapter()
	{
		_initialized = false;
		_amplifiers = false;
		_softwareLimits = false;
	}
	
	~YARPGALILOnEurobotArmAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(YARPEurobotArmParameters *par)
	{
		//// open device
		_parameters = par;
		GalilOpenParameters op_par;
		op_par.nj= _parameters->_nj; 
		op_par.mask = _parameters->_mask;
		if (YARPGalilDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		/* First the card needs to be reseted */
		IOCtl(CMDResetController, NULL);
		//idleMode();
		_amplifiers = false;

		// amp level and limits
		for(int i=0; i < _parameters->_nj; i++)
		{

			/************
			SingleAxisParameters cmd;
			cmd.axis=i;
			// amp enable
			short level = 1;
			cmd.parameters=&level;
			IOCtl(CMDSetAmpEnableLevel, &cmd);
			IOCtl(CMDSetAmpEnable, &cmd);
			*************/

			///pid a cero
			///motors type MT -1,-1,-1,-1,-1,-1,-1,-1
			///activate outputport OP 255

			SingleAxisParameters cmd;
			cmd.axis=i;

			double motor_type = -1; //Servo motor with reversed polarity
			cmd.parameters=&motor_type;

			IOCtl(CMDMotorType,&cmd);

			IOCtl(CMDGetMotorType,&cmd);

			YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Motor type motor %d = %f\n",cmd.axis, motor_type));
			
			int error = 10000;
			cmd.parameters=&error;
			
			///Set the error limit
			IOCtl(CMDErrorLimit, &cmd);
			
			int value = 1;
			cmd.parameters=&value;
			///set the off_on error
			IOCtl(CMDOffOnError,&cmd); 


			
			// PUMA has no hw limits
			// set limit events-> none
			/***************
			ControlBoardEvents event;
			event = CBNoEvent;
			cmd.parameters=&event;
			IOCtl(CMDSetPositiveLimit, &cmd);
			IOCtl(CMDSetNegativeLimit, &cmd);
			******************/
		}
		
		/*****************
		// amp enable off
		IOParameters cmd;
		cmd.port = 1;
		cmd.value = (short) 0x01;
		******************/
		
		/////////////////////////
		// This activates the necesary bit in the output port to be able to start the puma
		// The value has been obtained empirically. It is sure it can be improved by using
		// the time to search the exact bit

		IOParameters cmd;
		cmd.port = 1; //This is ignored
		cmd.value = (short) 255;

		IOCtl(CMDSetOutputPort,&cmd);		
	
		_initialized = true;
		return YARP_OK;
	}

	int uninitialize()
	{
		/**************************
		/// disable amplifiers
		IOParameters cmd;
		cmd.port = 1;
		cmd.value = (short) 0x01;
		IOCtl(CMDSetOutputPort, &cmd);
		_amplifiers = false;
		//////////////////////////
		****************************/

		if (YARPGalilDeviceDriver::close() != 0)
			return YARP_FAIL;

		_initialized = false;
		return YARP_OK;
	}
	int idleMode()
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
		}
		return YARP_OK;
	}

	int activatePID()
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
			SingleAxisParameters cmd;
			cmd.axis = i;
			cmd.parameters = &_parameters->_highPIDs[i];
			IOCtl(CMDSetPID, &cmd);
			double pos = 0.0;
			cmd.parameters = &pos;
			IOCtl(CMDDefinePosition, &cmd);
			IOCtl(CMDServoHere,NULL); //Start the motors
			_amplifiers = true;
		}
		/////_setHomeConfig(CBNoEvent);
		/////_clearStop();
		/// activate amplifiers
		/********
		IOParameters cmd;
		cmd.port = 1;
		cmd.value = (short) 0x00;
		IOCtl(CMDSetOutputPort, &cmd);
		****************/
		
		//////////////////////////

		return YARP_OK;
	}

	bool checkPowerOn()
	{
		/***
		IOParameters cmd;
		cmd.port = 0;
		IOCtl(CMDGetOutputPort, &cmd);

		if (cmd.value & 0x8)
			return true;
		else
			return false;
		***/

		//I didn't find a bit in the input/output Galil ports showing the
		//activation of the armpoweron button. For the moment just
		//return false. Next implement a message asking the user to activate
		//manually the bottom.
		return true;
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

	// returns max torque on axis; note: this is not the current value, this is
	// the maximum possible value for the board (i.e. MEI = 32767.0, Galil 9.99)
	double getMaxTorque(int axis)
	{
		int tmp = _parameters->_axis_map[axis];
		return _parameters->_maxDAC[tmp];
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
		for(int i = 0; i < _parameters->_nj; i++)
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
		for (int i = 0; i < _parameters->_nj; i++)
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
		for (int i = 0; i < _parameters->_nj; i++)
			IOCtl(CMDClearStop, &i);	// clear stop event
	}

	bool _initialized;
	bool _amplifiers;
	bool _softwareLimits;
	YARPEurobotArmParameters *_parameters;

};

#endif
