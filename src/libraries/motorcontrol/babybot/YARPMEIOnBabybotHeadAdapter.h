// by nat

#ifndef __MEIONBABYBOTHEAD__
#define __MEIONBABYBOTHEAD__

#include <ace/log_msg.h>
#include <YARPMeiDeviceDriver.h>
#include <string>
#include <YARPConfigFile.h>

// $Id: YARPMEIOnBabybotHeadAdapter.h,v 1.5 2003-10-17 16:34:40 babybot Exp $

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
	const int _stiffPID[_nj] = {1, 1, 1, 1, 1};
	const double _maxDAC[_nj] = {32767.0, 32767.0, 32767.0, 32767.0, 32767.0};
		
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
		_nj = 0;
		
		_nj = _BabybotHead::_nj;
		_realloc(_nj);
		int i;
		for(i = 0; i<_nj; i++) {
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

		// build encoder to angles
		if (cfgFile.get("[GENERAL]", "Encoder", _encoderToAngles, _nj) == YARP_FAIL)
			return YARP_FAIL;
		///////////////////////////////////////////////////

		return YARP_OK;
	}

private:
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
		
		_highPIDs = new LowLevelPID [nj];
		_lowPIDs = new LowLevelPID [nj];
		_zeros = new double [nj];
		_signs = new double [nj];
		_axis_map = new int [nj];
		_encoderToAngles = new double [nj];
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
};

class YARPBabybotInertialParameters
{
public:
	YARPBabybotInertialParameters()
	{
		_ns = 0;
		_parameters = NULL;	
	}

	~YARPBabybotInertialParameters()
	{
		int j;
		if (_parameters != NULL)
		{
			for (j = 0; j<_ns; j++)
			{
				if (_parameters[j]!=NULL)
					delete [] _parameters[j];
			}
			delete [] _parameters;
			_parameters = NULL;
		}
	}

	int load(const YARPString &path, const YARPString &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(),init_file.c_str());
		
		// get number of joints
		if (cfgFile.get("[GENERAL]", "Sensors", &_ns, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GENERAL]", "CalibrationSteps", &_nsteps, 1) == YARP_FAIL)
			return YARP_FAIL;

		// delete and allocate new memory
		_realloc(_ns);
		
		int i;
		for(i = 0; i<_ns; i++)
		{
			char dummy[80];
			sprintf(dummy, "%s%d", "Sensor", i);
			if (cfgFile.get("[PARAMETERS]", dummy, _parameters[i], 6) == YARP_FAIL)
				return YARP_FAIL;
		}
		
		return YARP_OK;
	}

private:
	private:
	void _realloc(int ns)
	{
		int j;
		if (_parameters != NULL)
		{
			for (j = 0; j<_ns; j++)
			{
				if (_parameters[j]!=NULL)
					delete [] _parameters[j];
			}
			delete [] _parameters;
			_parameters = NULL;
		}
		
		_ns = ns;
		
		_parameters = new double *[ns];
		for (j = 0; j<ns; j++)
			_parameters[j] = new double [6];
	}

public:
	int _ns;
	int _nsteps;
	double **_parameters;
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
		_inertial = new YARPBabybotInertialParameters;
		_inertial->load("",par->_inertialConfig);

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
			IOCtl(CMDControllerRun, &i);
			IOCtl(CMDEnableAmp, &i);
			IOCtl(CMDClearStop, &i);
		}
		return YARP_OK;
	}

	int readAnalog(int axis, double *val)
	{
		SingleAxisParameters cmd;
		short v;
		cmd.axis = axis;
		cmd.parameters = &v;
		int ret = IOCtl(CMDReadAnalog, &cmd);
		*val = (double) v;
		return ret;
	}

private:
	bool _initialized;
	YARPBabybotHeadParameters *_parameters;
	YARPBabybotInertialParameters *_inertial;
};

#endif	// .h