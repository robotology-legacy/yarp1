//$Id: YARPGALILOnEurobotHeadAdapter.h,v 1.6 2003-11-14 13:48:35 beltran Exp $

#ifndef __GALILONEUROBOTHEAD__
#define __GALILONEUROBOTHEAD__

#include <ace/Log_Msg.h>
#include <YARPGalilDeviceDriver.h>
#include <string>
#include <YARPConfigFile.h>

#define YARP_BABYBOT_HEAD_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_HEAD_ADAPTER_VERBOSE
#define YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_HEAD_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

namespace _EurobotHead
{
	/***************************************************************************
	 * 				Range 	Velocità 	Accelerazione 	Risoluzione 	Rad/Tic 
	 * Tilt		 	±60°	>=73°/s	 	>=1600°/s2		0.007°			95237.81
	 * Vergenza		±45°	>=73°/s	 	>=2100°/s2		0.007°			89588.31  
	 * Pan 			±45°	>=330°/s	>=5100°/s2		0.003°			22605.73
	 * 
	 * 	#define AXIS0_RAD2TICK 95237.81; // Tilt
	 *	#define AXIS1_RAD2TICK 22605.73; //Cam Left
	 *	#define AXIS2_RAD2TICK 23092.74; // Cam Right
	 *	#define AXIS3_RAD2TICK 89588.31; // Pan
	 **************************************************************************/

	//ATENTION: the I_LIMIT and the T_LIMIT must be fixed later
	const int _nj = 4;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(5.0, 50.0, 0.08, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(5.0, 50.0, 0.08, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(5.0, 50.0, 0.08, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(5.0, 50.0, 0.08, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0)
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),		//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),	
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0),
		LowLevelPID(1.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 0.0, 0.0)
	};

	const double _zeros[_nj] = {0.0, 0.0, 0.0, 0.0};
	const int _axis_map[_nj] = {3, 2, 1, 0};
	const int _signs[_nj] = {0, 0, 0, 0}; //This must be fixed in the eurohead
	const double _encoderToAngles[_nj] = {(2*pi)*89588.31, (2*pi)*23092.74, (2*pi)*22605.73, (2*pi)*95237.81}; 
	const int _stiffPID[_nj] = {1, 1, 1, 1};
	const double _maxDAC[_nj] = {9.9, 9.9, 9.9, 9.9};
}; // namespace

class YARPEurobotHeadParameters
{
public:
	YARPEurobotHeadParameters()
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

		_nj = _EurobotHead::_nj;
		_mask = (char) 0x0F;
		_realloc(_nj);
		for(int i = 0; i<_nj; i++) {
			_highPIDs[i] = _EurobotHead::_highPIDs[i];
			_lowPIDs[i] = _EurobotHead::_lowPIDs[i];
			_zeros[i] = _EurobotHead::_zeros[i];
			_axis_map[i] = _EurobotHead::_axis_map[i];
			_signs[i] = _EurobotHead::_signs[i];
			_encoderToAngles[i] = _EurobotHead::_encoderToAngles[i];
		}
	}

	~YARPEurobotHeadParameters()
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
	char _mask;
};

class YARPGALILOnEurobotHeadAdapter : public YARPGalilDeviceDriver
{
public:
	YARPGALILOnEurobotHeadAdapter()
	{
		_initialized = false;
	}
	
	~YARPGALILOnEurobotHeadAdapter()
	{
		if (_initialized)
			uninitialize();
	}
	
	int initialize(YARPEurobotHeadParameters *par)
	{
		_parameters = par;
		GalilOpenParameters op_par;
		op_par.nj= _parameters->_nj; 
		op_par.mask = _parameters->_mask;
		if (YARPGalilDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;
			
		/* First the card needs to be reseted */
		IOCtl(CMDResetController, NULL);

		for(int i=0; i < _parameters->_nj; i++)
		{
			SingleAxisParameters cmd;
			cmd.axis=i;
			
			int error = 2000;
			cmd.parameters=&error;
			
			///Set the error limit
			IOCtl(CMDErrorLimit, &cmd);
			
			int value = 1;
			cmd.parameters=&value;
			///set the off_on error
			IOCtl(CMDOffOnError,&cmd); 
			
			//Here I should add the positive and negative software limits
			//BL
			//FL
			
			//This is from Alberto software
			/***
			  			res = set_negative_limit(0, -0.9); // Tilt -pi/3
                        res = set_positive_limit(0, 0.9);   // Tilt pi/3
                        res = set_negative_limit(1, -0.7); // Cam Left -pi/4
                        res = set_positive_limit(1, 0.7);  // Cam Left pi/4
                        res = set_negative_limit(2, -0.7); // Cam Right -pi/4
                        res = set_positive_limit(2, 0.7);  // Cam Right pi/4
                        res = set_negative_limit(3, -0.7); // Pan -pi/4
                        res = set_positive_limit(3, 0.7); // Pan pi/4

			 ***/
		}

		_initialized = true;
		return YARP_OK;
	}

	int uninitialize()
	{
		if (YARPGalilDeviceDriver::close() != 0)
			return YARP_FAIL;

		_initialized = false;
		return YARP_OK;
	}
	int idleMode()
	{
		for(int i = 0; i < _parameters->_nj; i++)
			IOCtl(CMDControllerIdle, &i);
			
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
			//IOCtl(CMDControllerRun, &i);
			//IOCtl(CMDEnableAmp, &i);
			//IOCtl(CMDClearStop, &i);
		}
		return YARP_OK;
	}

	int readAnalogs(double *val)
	{
		
		int i;
		int ret;

		//This should be included in the low level device driver
		/**
		SingleAxisParameters cmd;
		for(i = 0; i < _inertial->_ns; i++)
		{
			cmd.axis = i;
			cmd.parameters = &_tmpShort[i];;
			ret = IOCtl(CMDReadAnalog, &cmd);
		}
		_inertial->convert(_tmpShort, val);
		**/
		return ret;
	}

	int calibrate()
	{
		YARP_BABYBOT_HEAD_ADAPTER_DEBUG(("Starting head calibration routine"));
		ACE_OS::printf("..done!\n");
	/*

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
		ACE_OS::printf("\n..done!\n");
	*/
		return YARP_OK;
	}

private:
	bool _initialized;
	YARPEurobotHeadParameters * _parameters;
};

#endif	// .h