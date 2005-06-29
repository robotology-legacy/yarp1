/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #nat#				                          ///
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
/// $Id: YARPSCIOnObreroArmAdapter.h,v 1.4 2005-06-29 15:46:15 natta Exp $
///
///

#ifndef __SCIOnObreroArmAdapterh__
#define __SCIOnObreroArmAdapterh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
// #include <ace/log_msg.h>

#include <yarp/YARPSciDeviceDriver.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPRobotMath.h>

#define YARP_OBRERO_ARM_ADAPTER_VERBOSE

// #ifdef YARP_OBRERO_ARM_ADAPTER_VERBOSE
// #define YARP_OBRERO_ARM_ADAPTER_VERBOSE(string) YARP_DEBUG("OBRERO_ARM_ADAPTER_DEBUG:", string)
// #else  YARP_OBRERO_ARM_ADAPTER_VERBOSE(string) YARP_NULL_DEBUG
// #endif
/*
#define MY_DEBUG \
	if (_parameters->_p != NULL) \
		(*_parameters->_p)
*/
/**
 * \file YARPSciOnObreroArmAdapter.h This file contains definitions of the control classes
 * for the RobotCub head acoording to the YARP device driver model.
 */

/**
 * _ObreroArm contains the default parameters of the RobotCub head control card(s).
 */

namespace _ObreroArm
{
	const int _nj = 6;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
	};
	
	const LowLevelPID _lowPIDs[_nj] = 
	{
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0)
	};

	const double _zeros[_nj]			= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const int _axis_map[_nj]			= { 4, 5, 0, 3, 1, 2};
	const double _signs[_nj]			= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const double _encoderToAngles[_nj]	= { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	const int _stiffPID[_nj]			= { 1, 1, 1, 1, 1, 1};
	const double _maxDAC[_nj]			= { 100.0, 100.0, 100.0, 100.0, 100.0, 100.0};
	const char   _portNameLength		= 80;
}; // namespace


// InvAxisMap	6 7 5 4 2 0 1 3
// AxisMap		5 6 4 7 3 2 0 1

/**
 * YARPObreroArmParameters is one of the components required to
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
 * NOTE: programmatically angles are always in radians here. Conversely in 
 * configuration files the angles and conversion factors are expressed in 
 * degrees. Please make sure you convert to radians before commanding 
 * postion, speed, etc.
 *
 * NOTE: to disable a joint control at the driver level replace the address
 * of the card with itself+128. This will tell the driver to behave normally but
 * to drop the request to that card altogether.
 *
 */

#include <yarp/YARPTime.h>

class YARPObreroArmParameters: public YARPGenericControlParameters
{
public:
	/**
	 * Default constructor.
	 * Allocates memory and sets parameters to suitable default values.
	 */
	YARPObreroArmParameters()
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
		_portName = NULL;

		_nj = _ObreroArm::_nj;
		
		_realloc(_nj);

		ACE_OS::memcpy (_highPIDs, _ObreroArm::_highPIDs, sizeof(LowLevelPID) * _nj);
		ACE_OS::memcpy (_lowPIDs, _ObreroArm::_lowPIDs, sizeof(LowLevelPID) * _nj);
		ACE_OS::memcpy (_zeros, _ObreroArm::_zeros, sizeof(double) * _nj);
		ACE_OS::memcpy (_axis_map, _ObreroArm::_axis_map, sizeof(int) * _nj);
		ACE_OS::memcpy (_signs, _ObreroArm::_signs, sizeof(double) * _nj);
		ACE_OS::memcpy (_encoderToAngles, _ObreroArm::_encoderToAngles, sizeof(double) * _nj);
		ACE_OS::memcpy (_stiffPID, _ObreroArm::_stiffPID, sizeof(int) * _nj);
		ACE_OS::memcpy (_maxDAC, _ObreroArm::_maxDAC, sizeof(double) * _nj);
	
		ACE_OS::memset (_limitsMax, 0, sizeof(double) * _nj);
		ACE_OS::memset (_limitsMin, 0, sizeof(double) * _nj);

		int i;
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

	~YARPObreroArmParameters()
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
		if (_limitsMax != NULL)	delete [] _limitsMax;
		if (_limitsMin != NULL)	delete [] _limitsMin;
    	if (_portName != NULL) delete [] _portName;
	}

	/**
	 * Loads the paramters from a configuration file and allocates memory
	 * if necessary. Error handling is not particularly smart here!
	 * @param path is the path where the initialization file is located.
	 * @param init_file is the intitialization file name.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int load(const YARPString &path, const YARPString &init_file)
	{
		printf("entering YARPObreroArm load\n");
		
		YARPConfigFile cfgFile;
		
		// set path and filename
		cfgFile.set(path.c_str(), init_file.c_str());

		// get number of joints
		if (cfgFile.get("[GENERAL]", "Joints", &_nj, 1) == YARP_FAIL)
			return YARP_FAIL;

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

		if (cfgFile.getString("[GENERAL]", "PortName", _portName) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GENERAL]", "Signs", _signs, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "MaxDAC", _maxDAC, _nj) == YARP_FAIL)
			return YARP_FAIL;
	//	if (cfgFile.get("[GENERAL]", "Stiff", _stiffPID, _nj) == YARP_FAIL)
	//		return YARP_FAIL;
		///////////////// ARM LIMITS
		if (cfgFile.get("[LIMITS]", "Max", _limitsMax, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[LIMITS]", "Min", _limitsMin, _nj) == YARP_FAIL)
			return YARP_FAIL;
	
		// convert limits to radiants
		for(i = 0; i < _nj; i++)
		{
			_limitsMax[i] *= degToRad;
			_limitsMin[i] *= degToRad;
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
	int copy (const YARPObreroArmParameters& peer)
	{
		_nj = peer._nj;

		if (_nj != 0)
		{
			_realloc (_nj);

			ACE_OS::memcpy (_highPIDs, peer._highPIDs, sizeof(LowLevelPID) * _nj);
			ACE_OS::memcpy (_lowPIDs, peer._lowPIDs, sizeof(LowLevelPID) * _nj);
			ACE_OS::memcpy (_zeros, peer._zeros, sizeof(double) * _nj);
			ACE_OS::memcpy (_signs, peer._signs, sizeof(double) * _nj);
			ACE_OS::memcpy (_axis_map, peer._axis_map, sizeof(int) * _nj);
			ACE_OS::memcpy (_inv_axis_map, peer._inv_axis_map, sizeof(int) * _nj);
			ACE_OS::memcpy (_encoderToAngles, peer._encoderToAngles, sizeof(double) * _nj);
			ACE_OS::memcpy (_stiffPID, peer._stiffPID, sizeof(int) * _nj);
			ACE_OS::memcpy (_maxDAC, peer._maxDAC, sizeof(double) * _nj);
			ACE_OS::memcpy (_limitsMax, peer._limitsMax, sizeof(double) * _nj);
			ACE_OS::memcpy (_limitsMin, peer._limitsMin, sizeof(double) * _nj);
			
			ACE_OS::memcpy (_portName, peer._portName, _ObreroArm::_portNameLength);
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
			
			_portName = NULL;

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
		if (_highPIDs != NULL) delete [] _highPIDs;
		if (_lowPIDs != NULL) delete [] _lowPIDs;
		if (_zeros != NULL)	delete [] _zeros;
		if (_signs != NULL)	delete [] _signs;
		if (_axis_map != NULL) delete [] _axis_map;
		if (_inv_axis_map != NULL) delete [] _inv_axis_map;
		if (_encoderToAngles != NULL) delete [] _encoderToAngles;
		if (_stiffPID != NULL) delete [] _stiffPID;
		if (_maxDAC != NULL) delete [] _maxDAC;
		if (_limitsMax != NULL)	delete [] _limitsMax;
		if (_limitsMin != NULL)	delete [] _limitsMin;
		if (_portName != NULL) delete [] _portName;

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
		_portName = new char [_ObreroArm::_portNameLength];
		// !NULL not checked!
	}

public:
	int				_nj;

	LowLevelPID		*_highPIDs;
	LowLevelPID		*_lowPIDs;
	double			*_zeros;
	double			*_signs;
	int				*_axis_map;
	int				*_inv_axis_map;
	double			*_encoderToAngles;
	int				*_stiffPID;
	double			*_maxDAC;
	double			*_limitsMax;
	double			*_limitsMin;
	unsigned char	*_destinations;
	char			*_portName;
};



/**
 * ObreroArm is a specialization of the Can device driver
 * to control the RobotCub head. This class especially implements initialize and
 * uninitialize while it leaves much of the burden of calling the device driver
 * to a generic template class called YARPGenericControlBoard.
 */
class YARPSciOnObreroArmAdapter : public YARPSciDeviceDriver,
				  public YARPGenericControlAdapter<YARPSciOnObreroArmAdapter, YARPObreroArmParameters>
{
public:
	/**
	 * Default constructor.
	 */
	YARPSciOnObreroArmAdapter()
	{
		_initialized = false;
	}
	
	/**
	 * Destructor.
	 */
	~YARPSciOnObreroArmAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	/**
	 * Initializes the adapter and opens the device driver.
	 * This is a specific initialization for the Obrero arm. NOTE: that the parameter
	 * here is not copied and references to it could still be made by the code. Until
	 * this behavior is correct, the user has to make sure the pointer doesn't become
	 * invalid during the lifetime of the adapter class (this one). Generally this is true
	 * for the "generic" templates since they allocate a copy of the parameter class
	 * internally (and their lifetime is related to that of the adapter).
	 *
	 * @param par is a pointer to the class containing the parameters that has
	 * to be exactly YARPObreroArmParameters.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int initialize(YARPObreroArmParameters *par)
	{
		using namespace _ObreroArm;

		_parameters = par;
		
		SciOpenParameters op_par(par->_portName);
		op_par._njoints = _parameters->_nj;

		if (YARPSciDeviceDriver::open ((void *)&op_par) < 0)
		{
			YARPSciDeviceDriver::close();
			return YARP_FAIL;
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
		if (YARPSciDeviceDriver::close() != 0)
			return YARP_FAIL;

		_initialized = false;
		return YARP_OK;
	}

	/**
	 * In the Obrero arm this disables the position controller and enables the zero
	 * force mode. As a result the arm can be passively controlled.
	 * @return YARP_OK always.
	 */
	int idleMode()
	{ 
		int actual_axis;
		for(int i = 0; i < _parameters->_nj; i++)
		{
			actual_axis = _parameters->_axis_map[i];
			SingleAxisParameters cmd;
			cmd.axis = actual_axis;
		
			IOCtl(CMDSetForceControlMode, &cmd);
		}	
		return YARP_OK; 
	}

	/**
	 * Sets the PID values specified in a second set of parameters 
	 * typically read from the intialization file.
	 * Not available on Obrero Arm
	 * @param reset if true resets the encoders.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int activateLowPID(bool reset = true)
	{ return YARP_OK; }

	/**
	 * Sets the PID values. On Obrero this activates the position control
	 * @param reset if true resets the encoder values to zero.
	 * @param pids is an array of PID data structures. If NULL the values
	 * contained into an internal variable are used (presumably read from the
	 * initialization file) otherwise the actual argument is used.
	 * @return YARP_OK always.
	 */
	int activatePID(bool reset, LowLevelPID *pids = NULL)
	{
		int actual_axis;
		for(int i = 0; i < _parameters->_nj; i++)
		{
			actual_axis = _parameters->_axis_map[i];
			SingleAxisParameters cmd;
			cmd.axis = actual_axis;
		
			IOCtl(CMDSetPositionControlMode, &cmd);
		}
		return YARP_OK;
	}

	/**
	 * Reads the analog value from the control card.
	 * This method is not implemented yet
	 * @return YARP_FAIL always.
	 */
	int readAnalogs(double *val)
	{
		return YARP_FAIL;
	}

	/**
	 * Reads the analog value from the control card.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int readAnalog(int axis, double *val)
	{
		int ret;
		SingleAxisParameters cmd;
		cmd.axis = axis;
		cmd.parameters = val;
		
		ret = IOCtl(CMDReadAnalog, &cmd);
		return ret;
	}

	/**
	 * Calibrates the control card if needed.
	 * No calibration for the Obrero Arm
	 * @param joint is the joint number/function requested to the calibrate method. 
	 * The default value -1 does nothing.
	 * @return YARP_OK always.
	 */
	int calibrate(int joint = -1)
	{ return YARP_OK; }

	/**
	 * Gets the maximum torque reference.
	 * @param axis is the axis to request information about.
	 * @return the maximum torque reference: i.e. the maximum output of the amplifier.
	 * 100 is the return value of the RobotCub controllers meaning the 100% of the PWM signal.
	 */
	inline double getMaxTorque(int axis) const 
	{ 
		if (axis < 0 || axis >= _parameters->_nj)
			return YARP_FAIL;
		else
			return _parameters->_maxDAC[axis];
	}

private:
	/** Whether the initialize() succeeded. */
	bool _initialized;

	/** Allocation and management of this object is typically done by the generic template. */
	YARPObreroArmParameters *_parameters;
};


#endif
