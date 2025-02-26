/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPMEIOnBabybotHeadAdapter.h,v 1.11 2006-07-28 16:26:15 babybot Exp $
///
///

// by nat

#ifndef __MEIONBABYBOTHEAD__
#define __MEIONBABYBOTHEAD__

#include <ace/log_msg.h>
#include <yarp/YARPMeiDeviceDriver.h>
#include <yarp/YARPConfigFile.h>

#include <yarp/YARPBabybotInertialSensor.h>

#define YARP_BABYBOT_HEAD_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_HEAD_ADAPTER_VERBOSE
#define YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_HEAD_ADAPTER_DEBUG:", string)
#else  
#define YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

/**
 * \file YARPMEIOnBabybotAdapter.h This file contains definitions of the control classes
 * for the Babybot head acoording to the YARP device driver model.
 */

/**
 * _BabybotHead contains the default parameters of the RobotCub head control card(s).
 */
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


/**
 * YARPBabybotHeadParameters is one of the components required to
 * specialize the YARPGenericControlBoard template to do something useful.
 * This class contains parameters that are used during initialization to
 * bring the head up into a decent state.
 *
 * A note on the use of the axis map:
 * - Each map's entry represents a name of a joint (imagine it as a label).
 * - Whenever addressing the card directly thus the label is used.
 * - When addressing software data, initialization file, etc. our index (what we
 * think of an axis number is used.
 * - Since the axis map is only used internally this should be of no concern for
 * the user.
 *
 */
class YARPBabybotHeadParameters : public YARPGenericControlParameters
{
public:
	/**
	 * Default constructor.
	 * Allocates memory and sets parameters to suitable default values.
	 */
	YARPBabybotHeadParameters()
	{
		_highPIDs = NULL;
		_lowPIDs = NULL;
		_zeros = NULL;
		_signs = NULL;
		_axis_map = NULL;
		_inv_axis_map = NULL;
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

		// invert the axis map.
		ACE_OS::memset (_inv_axis_map, 0, sizeof(int) * _nj);
		for (i = 0; i < _nj; i++)
		{
			int j;
			for (j = 0; j < _nj; j++)
			{
				if (_axis_map[j] == i)
				{
					_inv_axis_map[i] = j;
					break;
				}
			}
		}
	}

	/**
	 * Destructor.
	 */
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
		if (_inv_axis_map != NULL)
			delete [] _inv_axis_map;
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

	/**
	 * Loads the paramters from a configuration file and allocates memory
	 * if necessary.
	 * @param path is the path where the initialization file is located.
	 * @param init_file is the intitialization file name.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
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

		// invert the axis map.
		ACE_OS::memset (_inv_axis_map, 0, sizeof(int) * _nj);
		for (i = 0; i < _nj; i++)
		{
			int j;
			for (j = 0; j < _nj; j++)
			{
				if (_axis_map[j] == i)
				{
					_inv_axis_map[i] = j;
					break;
				}
			}
		}

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

		// convert limits to radians (still radians to comply w/ old standard)
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

	/**
	 * Copies an existing parameter instance into this one.
	 * @param peer is the reference to the object to copy in.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int copy (const YARPBabybotHeadParameters& peer)
	{
		_nj = peer._nj;

		if (_nj != 0)
		{
			_realloc (_nj);

			memcpy (_highPIDs, peer._highPIDs, sizeof(LowLevelPID) * _nj);
			memcpy (_lowPIDs, peer._lowPIDs, sizeof(LowLevelPID) * _nj);
			memcpy (_zeros, peer._zeros, sizeof(double) * _nj);
			memcpy (_signs, peer._signs, sizeof(double) * _nj);
			memcpy (_axis_map, peer._axis_map, sizeof(int) * _nj);
			memcpy (_inv_axis_map, peer._inv_axis_map, sizeof(int) * _nj);
			memcpy (_encoderToAngles, peer._encoderToAngles, sizeof(double) * _nj);
			memcpy (_stiffPID, peer._stiffPID, sizeof(int) * _nj);
			memcpy (_maxDAC, peer._maxDAC, sizeof(double) * _nj);
			memcpy (_limitsMax, peer._limitsMax, sizeof(double) * _nj);
			memcpy (_limitsMin, peer._limitsMin, sizeof(double) * _nj);
		}
		else
		{
			if (_highPIDs != NULL) delete [] _highPIDs;
			if (_lowPIDs != NULL) delete [] _lowPIDs;
			if (_zeros != NULL)	delete [] _zeros;
			if (_signs != NULL)	delete [] _signs;
			if (_axis_map != NULL) delete [] _axis_map;
			if (_inv_axis_map != NULL) delete [] _inv_axis_map;
			if (_encoderToAngles != NULL) delete [] _encoderToAngles;
			if (_stiffPID != NULL) delete [] _stiffPID;
			if (_maxDAC != NULL) delete [] _maxDAC;
			if (_limitsMax != NULL) delete [] _limitsMax;
			if (_limitsMin != NULL) delete [] _limitsMin;

			_highPIDs = NULL;
			_lowPIDs = NULL;
			_zeros = NULL;
			_signs = NULL;
			_axis_map = NULL;
			_inv_axis_map = NULL;
			_encoderToAngles = NULL;
			_stiffPID = NULL;
			_maxDAC = NULL;
			_limitsMax = NULL;
			_limitsMin = NULL;
		}

		return YARP_OK;
	}

private:
	/**
	 * Frees memory and reallocates arrays of the new size.
	 * @param nj is the new size (number of joints).
	 */
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
		if (_inv_axis_map != NULL)
			delete [] _inv_axis_map;
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
		_inv_axis_map = new int [nj];
		_encoderToAngles = new double [nj];
		_limitsMax = new double [nj];
		_limitsMin = new double [nj];
		_stiffPID = new int [nj];
		_maxDAC = new double [nj];

		// not checked?
	}

public:
	LowLevelPID *_highPIDs;
	LowLevelPID *_lowPIDs;
	int *_stiffPID;
	double *_maxDAC;
	YARPString _inertialConfig;
};


/**
 * YARPMEIOnBabybotHeadAdapter is a specialization of the MEI card device driver
 * to control the Babybot head. This class especially implements initialize and
 * uninitialize while it leaves much of the burden of calling the device driver
 * to a generic template class called YARPGenericControlBoard.
 *
 * NOTE: functions here in the adapter are not protected by the mutex.
 *
 */
class YARPMEIOnBabybotHeadAdapter : 
	public YARPMEIDeviceDriver,
	public YARPGenericControlAdapter<YARPMEIOnBabybotHeadAdapter, YARPBabybotHeadParameters>
{
public:
	/**
	 * Default constructor.
	 */
	YARPMEIOnBabybotHeadAdapter()
	{
		_initialized = false;
	}
	
	/**
	 * Destructor.
	 */
	~YARPMEIOnBabybotHeadAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	/*
	 * implemented from the abstract base class.
	 */

	/**
	 * Initializes the adapter and opens the device driver.
	 * This is a specific initialization for the Babybot head. NOTE: that the parameter
	 * here is not copied and references to it could still be made by the code. Until
	 * this behavior is correct, the user has to make sure the pointer doesn't become
	 * invalid during the lifetime of the adapter class (this one). Generally this is true
	 * for the "generic" templates since they allocate a copy of the parameter class
	 * internally (and their lifetime is related to that of the adapter).
	 *
	 * @param par is a pointer to the class containing the parameters that has
	 * to be exactly YARPBabybotHeadParameters.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int initialize(YARPBabybotHeadParameters *par)
	{
		_parameters = par;
		MEIOpenParameters op_par;
		op_par.nj= _parameters->_nj; 
		if (YARPMEIDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		// inertial sensor
		_inertial = new YARPBabybotInertialSensor;
		ACE_ASSERT (_inertial != NULL);
		_inertial->load("",par->_inertialConfig);

		_tmpShort = new short [_inertial->_ns];
		ACE_ASSERT (_tmpShort != NULL);

		for(int i=0; i < _parameters->_nj; i++)
		{
			SingleAxisParameters cmd;
			cmd.axis = _parameters->_axis_map[i];

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

	/**
	 * Uninitializes the controller and closes the device driver.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int uninitialize()
	{
		if (YARPMEIDeviceDriver::close() != 0)
			return YARP_FAIL;

		if (_inertial != NULL) delete _inertial;
		if (_tmpShort != NULL) delete [] _tmpShort;

		_initialized = false;
		return YARP_OK;
	}

	/**
	 * Disables simultaneously the controller and the amplifier (these are
	 * usually two separate commands on control cards).
	 * @return YARP_OK always.
	 */
	int idleMode()
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			int j = _parameters->_axis_map[i];
			IOCtl(CMDControllerIdle, &j);

			SingleAxisParameters cmd;
			cmd.axis = j;
			short level = 0;
			cmd.parameters=&level;
			IOCtl(CMDSetAmpEnable, &cmd);
		}
		return YARP_OK;
	}

	/**
	 * Calibrates the control card if needed.
	 * @param joint is the joint number/function requested to the calibrate method. 
	 * The default value -1 does nothing.
	 * @return YARP_OK always.
	 */
	int calibrate(int joint = -1)
	{

		// head absolute calibration
		//
		// does not work so far. better to do it using the higher-level
		// routines, because of axis mapping and so on.

		YARP_BABYBOT_HEAD_ADAPTER_DEBUG(("Starting head calibration routine..."));
		ACE_OS::printf("done.\n");

		// inertial sensor calibration

		YARP_BABYBOT_HEAD_ADAPTER_DEBUG(("Starting inertial sensor calibration..."));

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
				cmd.axis = int(_inertial->_parameters[i][SP_MeiChannel]);
				cmd.parameters = &_tmpShort[i];
				ret = IOCtl(CMDGetAnalogChannel, &cmd);
			}

			cal = _inertial->calibrate(_tmpShort, &p);
//			ACE_OS::printf("%d\r",p);
			ACE_OS::sleep(ACE_Time_Value(0,40000));
		}

		_inertial->save("",_parameters->_inertialConfig);
		
		ACE_OS::printf("done.\n");

		return YARP_OK;
	}

	/**
	 * Sets the PID values.
	 * @param reset if true resets the encoder values to zero.
	 * @param pids is an array of PID data structures. If NULL the values
	 * contained into an internal variable are used (presumably read from the
	 * initialization file) otherwise the actual argument is used.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int activatePID(bool reset, LowLevelPID *pids = NULL)
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			int j = _parameters->_axis_map[i];
			IOCtl(CMDControllerIdle, &j);
			SingleAxisParameters cmd;
			cmd.axis = j;

			if (pids == NULL)
				cmd.parameters = &_parameters->_highPIDs[i];
			else
				cmd.parameters = &pids[i];
				
			IOCtl(CMDSetPID, &cmd);

			// reset encoders.
			if (reset) {
				double pos = 0.0;
				cmd.parameters = &pos;
				IOCtl(CMDDefinePosition, &cmd);
			}
			//////////////////////////
			IOCtl(CMDControllerRun, &j);
			IOCtl(CMDEnableAmp, &j);
			IOCtl(CMDClearStop, &j);
		}
		return YARP_OK;
	}

	/*
	 * More functions: these are called from the higher level code.
	 */

	/**
	 * Reads the analog values from the control card (ADC values).
	 * @param val is the array to receive the analog readings.
	 * @return YARP_FAIL always.
	 */
	int readAnalogs(double *val)
	{
		int i;
		int ret;
		SingleAxisParameters cmd;
		for(i = 0; i < _inertial->_ns; i++)
		{
			cmd.axis = int(_inertial->_parameters[i][SP_MeiChannel]);
			cmd.parameters = &_tmpShort[i];;
			ret = IOCtl(CMDGetAnalogChannel, &cmd);
		}
		_inertial->convert(_tmpShort, val);
		return ret;
	}

	/**
	 * Reads the analog values from the control card for a specified input.
	 * @param index is the input number to read from.
	 * @param val is a double pointer to receive the analog reading.
	 * @return YARP_FAIL always.
	 */
	int readAnalog(int index, double *val)
	{
		int ret;
		SingleAxisParameters cmd;
		ACE_ASSERT (index >= 0 && index < _inertial->_ns);
		cmd.axis = int(_inertial->_parameters[index][SP_MeiChannel]);
		cmd.parameters = &_tmpShort[index];;
		ret = IOCtl(CMDGetAnalogChannel, &cmd);
		_inertial->convert(_tmpShort, val);
		return ret;
	}

private:
	bool _initialized;
	short *_tmpShort;
	YARPBabybotHeadParameters *_parameters;
	YARPBabybotInertialSensor *_inertial;
};

#endif	// .h