/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPMEIOnBabybotArmAdapter.h,v 1.21 2004-01-17 00:15:15 gmetta Exp $
///
///

// -- by nat

#ifndef __MEIONBABYBOTARMADAPTER__
#define __MEIONBABYBOTARMADAPTER__

#include <ace/log_msg.h>
#include <YARPMeiDeviceDriver.h>
#include <YARPString.h>

#define YARP_BABYBOT_ARM_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_ARM_ADAPTER_VERBOSE
#define YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <YARPConfigFile.h>

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

class YARPBabybotArmParameters
{
public:
	YARPBabybotArmParameters()
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
		
		_nj = _BabybotArm::_nj;
		_realloc(_nj);
		int i;
		for(i = 0; i<_nj; i++) {
			_highPIDs[i] = _BabybotArm::_highPIDs[i];
			_lowPIDs[i] = _BabybotArm::_lowPIDs[i];
			_zeros[i] = _BabybotArm::_zeros[i];
			_axis_map[i] = _BabybotArm::_axis_map[i];
			_signs[i] = _BabybotArm::_signs[i];
			_encoderToAngles[i] = _BabybotArm::_encoders[i]*_BabybotArm::_encWheels[i];
			_fwdCouple[i] = _BabybotArm::_fwdCouple[i];
			_stiffPID[i] = _BabybotArm::_stiffPID[i];
			_maxDAC[i] = _BabybotArm::_maxDAC[i];
		}

		// compute inv couple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) +
						(_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
		_invCouple[5] = -_fwdCouple[5] / (_encoderToAngles[4] * _encoderToAngles[5]);
	}

	~YARPBabybotArmParameters()
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

	int load(const YARPString &path, const YARPString &init_file)
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
		if (cfgFile.get("[GENERAL]", "Stiff", _stiffPID, _nj) == YARP_FAIL)
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
	double *_maxDAC;
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

	int initialize(YARPBabybotArmParameters *par)
	{
		//// open device
		_parameters = par;
		MEIOpenParameters op_par;
		op_par.nj= _parameters->_nj; 
		if (YARPMEIDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		// amp level and limits
		for(int i=0; i < _parameters->_nj; i++)
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
		for(int i = 0; i < _parameters->_nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
		}
		return YARP_OK;
	}

	int activateLowPID()
	{
		return activatePID(_parameters->_lowPIDs);
	}

	int activatePID(LowLevelPID *pids = NULL)
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
			SingleAxisParameters cmd;
			cmd.axis = i;

			if (pids == NULL)
				cmd.parameters = &_parameters->_highPIDs[i];
			else
				cmd.parameters = &pids[i];

			IOCtl(CMDSetPID, &cmd);
			double pos = 0.0;
			cmd.parameters = &pos;
			IOCtl(CMDDefinePosition, &cmd);
			IOCtl(CMDControllerRun, &i);
			// IOCtl(CMDEnableAmp, &i);	// this is not required
		}
		_setHomeConfig(CBNoEvent);
		_clearStop();
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
	YARPBabybotArmParameters *_parameters;

};

#endif
