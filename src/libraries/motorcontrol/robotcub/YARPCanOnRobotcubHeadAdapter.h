/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #pasa#				                          ///
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
/// $Id: YARPCanOnRobotcubHeadAdapter.h,v 1.2 2004-06-30 13:37:51 gmetta Exp $
///
///

#ifndef __CanOnRobotcubHeadAdapterh__
#define __CanOnRobotcubHeadAdapterh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/log_msg.h>

#include <YARPValueCanDeviceDriver.h>
#include <YARPConfigFile.h>


#define YARP_ROBOTCUB_HEAD_ADAPTER_VERBOSE

#ifdef YARP_ROBOTCUB_HEAD_ADAPTER_VERBOSE
#define YARP_ROBOTCUB_HEAD_ADAPTER_DEBUG(string) YARP_DEBUG("ROBOTCUB_HEAD_ADAPTER_DEBUG:", string)
#else  YARP_ROBOTCUB_HEAD_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

///
///
///
namespace _RobotcubHead
{
	const int _nj = 8;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0)
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0)
	};

	const double _zeros[_nj]			= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	const int _axis_map[_nj]			= { 0, 1, 2, 3, 4, 5, 6, 7 };
	const int _signs[_nj]				= { 0, 0, 0, 0, 0, 0, 0, 0 };
	const double _encoderToAngles[_nj]	= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	const int _stiffPID[_nj]			= { 1, 1, 1, 1, 1, 1, 1, 1 };
	const double _maxDAC[_nj]			= { 32767.0, 32767.0, 32767.0, 32767.0, 32767.0, 32767.0, 32767.0, 32767.0 };

	const int CANBUS_DEVICE_NUM			= 0;
	const int CANBUS_ARBITRATION_ID		= 0;
	const int CANBUS_MY_ADDRESS			= 0;
	const int CANBUS_POLLING_INTERVAL	= 2;			/// [ms]
	const int CANBUS_TIMEOUT			= 10;			/// 10 * POLLING
	const int CANBUS_MAXCARDS			= MAX_CARDS;

	const char _destinations[_nj]		= { 0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8 };

}; // namespace


class YARPRobotcubHeadParameters
{
public:
	YARPRobotcubHeadParameters()
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
		
		_nj = _RobotcubHead::_nj;
		_realloc(_nj);
		int i;
		for(i = 0; i<_nj; i++) 
		{
			_highPIDs[i] = _RobotcubHead::_highPIDs[i];
			_lowPIDs[i] = _RobotcubHead::_lowPIDs[i];
			_zeros[i] = _RobotcubHead::_zeros[i];
			_axis_map[i] = _RobotcubHead::_axis_map[i];
			_signs[i] = _RobotcubHead::_signs[i];
			_encoderToAngles[i] = _RobotcubHead::_encoderToAngles[i];
			_stiffPID[i] = _RobotcubHead::_stiffPID[i];
			_maxDAC[i] = _RobotcubHead::_maxDAC[i];
		}
	}

	~YARPRobotcubHeadParameters()
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

	///
	///
	int load(const YARPString &path, const YARPString &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(), init_file.c_str());
		
		// get number of joints
		if (cfgFile.get("[GENERAL]", "Joints", &_nj, 1) == YARP_FAIL)
		{
			return YARP_FAIL;
		}

		ACE_ASSERT (_nj == 8);

		// delete and allocate new memory
		_realloc(_nj);
		
		int i;
		for(i = 0; i < _nj; i++)
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
	int				_nj;

	LowLevelPID		*_highPIDs;
	LowLevelPID		*_lowPIDs;
	double			*_zeros;
	double			*_signs;
	int				*_axis_map;
	double			*_encoderToAngles;
	int				*_stiffPID;
	double			*_maxDAC;
	double			*_limitsMax;
	double			*_limitsMin;
};



///
///
///
class YARPCanOnRobotcubHeadAdapter : public YARPValueCanDeviceDriver
{
public:
	YARPCanOnRobotcubHeadAdapter()
	{
		_initialized = false;
	}
	
	~YARPCanOnRobotcubHeadAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(YARPRobotcubHeadParameters *par)
	{
		_parameters = par;
		
		ValueCanOpenParameters op_par;
		op_par._port_number = CANBUS_DEVICE_NUM;
		op_par._arbitrationID = CANBUS_ARBITRATION_ID;
		memcpy (op_par._destinations, _parameters->_destinations, sizeof(unsigned char) * CANBUS_MAXCARDS);
		op_par._my_address = CANBUS_MY_ADDRESS;					/// my address.
		op_par._polling_interval = CANBUS_POLLING_INTERVAL;		/// thread polling interval [ms].
		op_par._timeout = CANBUS_TIMEOUT;						/// approx this value times the polling interval [ms].

		op_par._njoints = _parameters->_nj;
		op_par._p = NULL;


		if (YARPValueCanDeviceDriver::open ((void *)&op_par) < 0)
		{
			YARPValueCanDeviceDriver::close();
			return YARP_FAIL;
		}

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

		_initialized = false;
		return YARP_OK;
	}

	///
	///
	///
	int idleMode()
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			IOCtl(CMDControllerIdle, &i);
			IOCtl(CMDDisableAmp, &i);
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
		ACE_UNUSED_ARG(val);
		return YARP_FAIL;
	}

	int calibrate(void)
	{
		YARP_ROBOTCUB_HEAD_ADAPTER_DEBUG(("Starting head calibration routine"));
		ACE_OS::printf("..done!\n");
		return YARP_OK;
	}

private:
	bool _initialized;
	YARPBabybotHeadParameters *_parameters;
};

#endif	// .h