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
///  $Id: YARPMEIOnBabybotHeadAdapter.h,v 1.1 2004-07-28 17:17:35 babybot Exp $
///
///

// by nat

#ifndef __MEIONBABYBOTHEAD__
#define __MEIONBABYBOTHEAD__

#include <ace/log_msg.h>
#include <yarp/YARPMeiDeviceDriver.h>
#include <yarp/YARPConfigFile.h>

#include "YARPBabybotInertialSensor.h"

#define YARP_BABYBOT_HEAD_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_HEAD_ADAPTER_VERBOSE
#define YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_HEAD_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

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

	const double _zeros[_nj] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
	const int _axis_map[_nj] = { 4, 3, 2, 1, 0 };
	const int _signs[_nj] = { 0, 1, 0, 1, 0 };
	const double _encoderToAngles[_nj] = { 116000.0, 116000.0, 72000.0, 164000.0, 128000.0 };
	const int _stiffPID[_nj] = { 1, 1, 1, 1, 1 };
	const double _maxDAC[_nj] = { 32767.0, 32767.0, 32767.0, 32767.0, 32767.0 };
		
}; // namespace

class YARPBabybotHeadParameters
{
public:
	YARPBabybotHeadParameters()
	{
		_highPIDs = NULL;
		_lowPIDs = NULL;
		_zeros = NULL;
		_signs = NULL;
		_axis_map = NULL;
		_encoderToAngles = NULL;
		_stiffPID = NULL;
		_maxDAC = NULL;
		_limitsMax = NULL;
		_limitsMin = NULL;
		_nj = 0;
		
		_nj = _BabybotHead::_nj;
		_realloc(_nj);
		int i;
		for(i = 0; i<_nj; i++) 
		{
			_highPIDs[i] = _BabybotHead::_highPIDs[i];
			_lowPIDs[i] = _BabybotHead::_lowPIDs[i];
			_zeros[i] = _BabybotHead::_zeros[i];
			_axis_map[i] = _BabybotHead::_axis_map[i];
			_signs[i] = _BabybotHead::_signs[i];
			_encoderToAngles[i] = _BabybotHead::_encoderToAngles[i];
			_stiffPID[i] = _BabybotHead::_stiffPID[i];
			_maxDAC[i] = _BabybotHead::_maxDAC[i];
		}
	}

	~YARPBabybotHeadParameters()
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
		if (_stiffPID != NULL)
			delete [] _stiffPID;
		if (_maxDAC != NULL)
			delete [] _maxDAC;
		if (_limitsMax != NULL)
			delete [] _limitsMax;
		if (_limitsMin != NULL)
			delete [] _limitsMin;
	}

	int load(const YARPString &path, const YARPString &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(), init_file.c_str());
		
		// get number of joints
		if (cfgFile.get("[GENERAL]", "Joints", &_nj, 1) == YARP_FAIL)
			return YARP_FAIL;

		YARPString tmp;
		if (cfgFile.getString("[GENERAL]", "InertialFile", tmp) == YARP_FAIL)
			return YARP_FAIL;

		_inertialConfig = path;
		_inertialConfig.append(tmp);

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
		if (cfgFile.get("[GENERAL]", "MaxDAC", _maxDAC, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "Stiff", _stiffPID, _nj) == YARP_FAIL)
			return YARP_FAIL;

		///////////////// HEAD LIMITS
		if (cfgFile.get("[LIMITS]", "Max", _limitsMax, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[LIMITS]", "Min", _limitsMin, _nj) == YARP_FAIL)
			return YARP_FAIL;

		// convert limits to radiants
		for(i = 0; i < _nj; i++)
		{
			_limitsMax[i] = _limitsMax[i] * degToRad;
			_limitsMin[i] = _limitsMin[i] * degToRad;
		}
		//////////////////////////////////////////////////////////////////

		// build encoder to angles
		if (cfgFile.get("[GENERAL]", "Encoder", _encoderToAngles, _nj) == YARP_FAIL)
			return YARP_FAIL;
		///////////////////////////////////////////////////

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
		if (_stiffPID != NULL)
			delete [] _stiffPID;
		if (_maxDAC != NULL)
			delete [] _maxDAC;
		if (_limitsMax != NULL)
			delete [] _limitsMax;
		if (_limitsMin != NULL)
			delete [] _limitsMin;
		
		_highPIDs = new LowLevelPID [nj];
		_lowPIDs = new LowLevelPID [nj];
		_zeros = new double [nj];
		_signs = new double [nj];
		_axis_map = new int [nj];
		_encoderToAngles = new double [nj];
		_limitsMax = new double [nj];
		_limitsMin = new double [nj];
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
	int *_stiffPID;
	int _nj;
	double *_maxDAC;
	YARPString _inertialConfig;
	double *_limitsMax;
	double *_limitsMin;
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

	int initialize(YARPBabybotHeadParameters *par)
	{
		_parameters = par;
		MEIOpenParameters op_par;
		op_par.nj= _parameters->_nj; 
		if (YARPMEIDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		// inertial sensor
		_inertial = new YARPBabybotInertialSensor;
		_inertial->load("",par->_inertialConfig);

		_tmpShort = new short [_inertial->_ns];

		for(int i=0; i < _parameters->_nj; i++)
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

			// analog input
			level = 1;
			cmd.parameters=&level;
			IOCtl(CMDSetAxisAnalog, &cmd);
		}

		_initialized = true;
		return YARP_OK;
	}

	int uninitialize()
	{
		if (YARPMEIDeviceDriver::close() != 0)
			return YARP_FAIL;

		delete _inertial;
		delete [] _tmpShort;

		_initialized = false;
		return YARP_OK;
	}

	int idleMode()
	{
		for(int i = 0; i < _parameters->_nj; i++)
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

	int activateLowPID(bool reset = true)
	{
		return activatePID(reset, _parameters->_lowPIDs);
	}

	int activatePID(bool reset, LowLevelPID *pids = NULL)
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

			// reset encoders
			if (reset) {
				double pos = 0.0;
				cmd.parameters = &pos;
				IOCtl(CMDDefinePosition, &cmd);
			}
			//////////////////////////
			IOCtl(CMDControllerRun, &i);
			IOCtl(CMDEnableAmp, &i);
			IOCtl(CMDClearStop, &i);
		}
		return YARP_OK;
	}

	int readAnalogs(double *val)
	{
		int i;
		int ret;
		SingleAxisParameters cmd;
		for(i = 0; i < _inertial->_ns; i++)
		{
			cmd.axis = i;
			cmd.parameters = &_tmpShort[i];;
			ret = IOCtl(CMDReadAnalog, &cmd);
		}
		_inertial->convert(_tmpShort, val);
		return ret;
	}

	int calibrate()
	{
		YARP_BABYBOT_HEAD_ADAPTER_DEBUG(("Starting head calibration routine"));
		ACE_OS::printf("..done!\n");

		YARP_BABYBOT_HEAD_ADAPTER_DEBUG(("Starting inertial sensor calibration:\n"));

		int p = 0;
		bool cal = false;
		while (!cal)
		{
			// read new values from analog input
			int i;
			int ret;
			SingleAxisParameters cmd;
			for(i = 0; i < _inertial->_ns; i++)
			{
				cmd.axis = i;
				cmd.parameters = &_tmpShort[i];;
				ret = IOCtl(CMDReadAnalog, &cmd);
			}

			cal = _inertial->calibrate(_tmpShort, &p);
			ACE_OS::printf("%d\r",p);
			ACE_OS::sleep(ACE_Time_Value(0,40000));
		}

		_inertial->save("",_parameters->_inertialConfig);
		
		return YARP_OK;
	}

private:
	bool _initialized;
	short *_tmpShort;
	YARPBabybotHeadParameters *_parameters;
	YARPBabybotInertialSensor *_inertial;
};

#endif	// .h