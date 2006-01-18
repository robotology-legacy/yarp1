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
/// $Id: YARPCanOnJamesArmAdapter.h,v 1.3 2006-01-18 10:32:21 gmetta Exp $
///
///

#ifndef __CanOnJamesArmAdapterh__
#define __CanOnJamesArmAdapterh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/log_msg.h>

#include <yarp/YARPEsdCanDeviceDriver.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPRobotMath.h>

#define YARP_ROBOTCUB_ARM_ADAPTER_VERBOSE

#ifdef YARP_ROBOTCUB_ARM_ADAPTER_VERBOSE
#define YARP_ROBOTCUB_ARM_ADAPTER_DEBUG(string) YARP_DEBUG("ROBOTCUB_ARM_ADAPTER_DEBUG:", string)
#else  YARP_ROBOTCUB_ARM_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

/**
 * \file YARPCanOnJamesArmAdapter.h This file contains definitions of the control classes
 * for the RobotCub arm and hand according to the YARP device driver model.
 */

/**
 * _JamesArm contains the default parameters of the RobotCub arm control cards.
 */
namespace _JamesArm
{
	const int _nj = 15;
	const LowLevelPID _highPIDs[_nj] =
	{
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	//KP, KD, KI, AC_FF, VEL_FF, I_LIMIT, OFFSET, T_LIMIT, SHIFT, FRICT_FF
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
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
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),	
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0),
		LowLevelPID(0.0, 0.0, 0.0, 0.0, 0.0, 32767.0, 0.0, 32767.0, 8.0, 0.0)
	};

	const double _zeros[_nj]			= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	const int _axis_map[_nj]			= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	const double _signs[_nj]			= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	const double _encoderToAngles[_nj]	= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	const int _stiffPID[_nj]			= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	const double _maxDAC[_nj]			= { 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0 };
	const double _currentLimits[_nj]	= { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	const int CANBUS_DEVICE_NUM			= 1; ///1;	[must be 1 if two cards are present]
	const int CANBUS_MY_ADDRESS			= 0;
	const int CANBUS_POLLING_INTERVAL	= 20;			/// [ms]
	const int CANBUS_TIMEOUT			= 10;			/// 10 * POLLING
	const int CANBUS_MAXCARDS			= 16;			/// because of the structure of the protocol.

	const unsigned char _destinations[CANBUS_MAXCARDS] = { 0x0f, 0x0e, 0x0d, 0x0c, 
														   0x0b, 0x0a, 0x09, 0x08,
														   0x80, 0x80, 0x80, 0x80,
														   0x80, 0x80, 0x80, 0x80 };
}; // namespace


/**
 * YARPJamesArmParameters is one of the components required to
 * specialize the YARPGenericControlBoard template to do something useful.
 * This class contains parameters that are used during initialization to
 * bring the arm up into a decent state.
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
class YARPJamesArmParameters
{
public:
	/**
	 * Default constructor.
	 * Allocates memory and sets parameters to suitable default values.
	 */
	YARPJamesArmParameters()
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
		_destinations = NULL;
		_currentLimits = NULL;

		_nj = _JamesArm::_nj;
		_realloc(_nj);

		ACE_OS::memcpy (_highPIDs, _JamesArm::_highPIDs, sizeof(LowLevelPID) * _nj);
		ACE_OS::memcpy (_lowPIDs, _JamesArm::_lowPIDs, sizeof(LowLevelPID) * _nj);
		ACE_OS::memcpy (_zeros, _JamesArm::_zeros, sizeof(double) * _nj);
		ACE_OS::memcpy (_axis_map, _JamesArm::_axis_map, sizeof(int) * _nj);
		// signs should be int or bool?
		ACE_OS::memcpy (_signs, _JamesArm::_signs, sizeof(double) * _nj);
		ACE_OS::memcpy (_encoderToAngles, _JamesArm::_encoderToAngles, sizeof(double) * _nj);
		ACE_OS::memcpy (_stiffPID, _JamesArm::_stiffPID, sizeof(int) * _nj);
		ACE_OS::memcpy (_maxDAC, _JamesArm::_maxDAC, sizeof(double) * _nj);
		ACE_OS::memcpy (_destinations, _JamesArm::_destinations, sizeof(unsigned char) * _JamesArm::CANBUS_MAXCARDS);
		ACE_OS::memcpy (_currentLimits, _JamesArm::_currentLimits, sizeof(double) * _nj);

		// invert the axis map.
		ACE_OS::memset (_inv_axis_map, 0, sizeof(int) * _nj);
		int i;
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

		_p = NULL;
		_message_filter = 20;
	}

	/**
	 * Destructor.
	 */
	~YARPJamesArmParameters()
	{
		if (_highPIDs != NULL) delete [] _highPIDs;
		if (_lowPIDs != NULL) delete [] _lowPIDs;
		if (_zeros != NULL)	delete [] _zeros;
		if (_signs != NULL) delete [] _signs;
		if (_axis_map != NULL) delete [] _axis_map;
		if (_inv_axis_map != NULL) delete [] _inv_axis_map;
		if (_encoderToAngles != NULL) delete [] _encoderToAngles;
		if (_stiffPID != NULL) delete [] _stiffPID;
		if (_maxDAC != NULL) delete [] _maxDAC;
		if (_limitsMax != NULL) delete [] _limitsMax;
		if (_limitsMin != NULL) delete [] _limitsMin;
		if (_destinations != NULL) delete[] _destinations;
		if (_currentLimits != NULL) delete[] _currentLimits;
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
		{
			return YARP_FAIL;
		}

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

		if (cfgFile.get("[GENERAL]", "Signs", _signs, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "MaxDAC", _maxDAC, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "Stiff", _stiffPID, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[GENERAL]", "CurrentLimits", _currentLimits, _nj) == YARP_FAIL)
			return YARP_FAIL;

		int tmp[_JamesArm::CANBUS_MAXCARDS];
		if (cfgFile.get("[GENERAL]", "CanAddresses", tmp, _JamesArm::CANBUS_MAXCARDS) == YARP_FAIL)
			return YARP_FAIL;

		for (i = 0; i < _JamesArm::CANBUS_MAXCARDS; i++)
			_destinations[i] = (tmp[i] & 0xff);

		///////////////// ARM LIMITS
		if (cfgFile.get("[LIMITS]", "Max", _limitsMax, _nj) == YARP_FAIL)
			return YARP_FAIL;
		if (cfgFile.get("[LIMITS]", "Min", _limitsMin, _nj) == YARP_FAIL)
			return YARP_FAIL;

		// convert limits to radiants
		//for(i = 0; i < _nj; i++)
		//{
		//	_limitsMax[i] *= degToRad;
		//	_limitsMin[i] *= degToRad;
		//}
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
	int copy (const YARPJamesArmParameters& peer)
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
			ACE_OS::memcpy (_destinations, peer._destinations, sizeof(unsigned char) * _JamesArm::CANBUS_MAXCARDS);
			ACE_OS::memcpy (_currentLimits, peer._currentLimits, sizeof(double) * _nj);
			_p = peer._p;
			_message_filter = peer._message_filter;
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
			if (_destinations != NULL) delete[] _destinations;
			if (_currentLimits != NULL) delete[] _currentLimits;

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
			_destinations = NULL;
			_currentLimits = NULL;

			_p = peer._p;
			_message_filter = peer._message_filter;
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
		if (_zeros != NULL) delete [] _zeros;
		if (_signs != NULL)	delete [] _signs;
		if (_axis_map != NULL) delete [] _axis_map;
		if (_inv_axis_map != NULL) delete [] _inv_axis_map;
		if (_encoderToAngles != NULL) delete [] _encoderToAngles;
		if (_stiffPID != NULL) delete [] _stiffPID;
		if (_maxDAC != NULL) delete [] _maxDAC;
		if (_limitsMax != NULL) delete [] _limitsMax;
		if (_limitsMin != NULL) delete [] _limitsMin;
		if (_destinations != NULL) delete[] _destinations;
		if (_currentLimits != NULL) delete[] _currentLimits;

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
		_destinations = new unsigned char[_JamesArm::CANBUS_MAXCARDS];
		_currentLimits = new double [nj];

		// LATER: add missing check on memory allocation.
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
	double			*_currentLimits;

	int (* _p) (const char *fmt, ...);
	int _message_filter;
};



/**
 * YARPCanOnJamesArmAdapter is a specialization of the Can device driver
 * to control the RobotCub arm. This class especially implements initialize and
 * uninitialize while it leaves much of the burden of calling the device driver
 * to a generic template class called YARPGenericControlBoard.
 */
class YARPCanOnJamesArmAdapter : public YARPEsdCanDeviceDriver
{
public:
	/**
	 * Default constructor.
	 */
	YARPCanOnJamesArmAdapter()
	{
		_initialized = false;
	}
	
	/**
	 * Destructor.
	 */
	~YARPCanOnJamesArmAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	/**
	 * Initializes the adapter and opens the device driver.
	 * This is a specific initialization for the RobotCub arm. NOTE: that the parameter
	 * here is not copied and references to it could still be made by the code. Until
	 * this behavior is correct, the user has to make sure the pointer doesn't become
	 * invalid during the lifetime of the adapter class (this one). Generally this is true
	 * for the "generic" templates since they allocate a copy of the parameter class
	 * internally (and their lifetime is related to that of the adapter).
	 *
	 * @param par is a pointer to the class containing the parameters that has
	 * to be exactly YARPJamesArmParameters.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int initialize(YARPJamesArmParameters *par)
	{
		using namespace _JamesArm;

		_parameters = par;

		/// either of the two available cards.
		EsdCanOpenParameters op_par;
		memcpy (op_par._destinations, _parameters->_destinations, sizeof(unsigned char) * CANBUS_MAXCARDS);
		op_par._my_address = CANBUS_MY_ADDRESS;					/// my address.
		op_par._polling_interval = CANBUS_POLLING_INTERVAL;		/// thread polling interval [ms].
		op_par._timeout = CANBUS_TIMEOUT;						/// approx this value times the polling interval [ms].
		op_par._networkN = CANBUS_DEVICE_NUM;
		op_par._njoints = _parameters->_nj;
		op_par._p = _parameters->_p;

		if (YARPEsdCanDeviceDriver::open ((void *)&op_par) < 0)
		{
			YARPEsdCanDeviceDriver::close();
			return YARP_FAIL;
		}


		// filters out certain messages.
		int msg = _parameters->_message_filter;
		IOCtl(CMDSetDebugMessageFilter, (void *)&msg);

		for(int i=0; i < _parameters->_nj; i++)
		{
			int actual_axis = _parameters->_axis_map[i];
			SingleAxisParameters cmd;
			cmd.axis = actual_axis;
			
			// amp enable level
			short level = 1;
			cmd.parameters = &level;
			IOCtl(CMDSetAmpEnableLevel, &cmd);
			
			// limit levels
			cmd.parameters = &level;
			IOCtl(CMDSetPositiveLevel, &cmd);
			IOCtl(CMDSetNegativeLevel, &cmd);
			
			// limit events
			ControlBoardEvents event;
			event = CBNoEvent;
			cmd.parameters = &event;
			IOCtl(CMDSetPositiveLimit, &cmd);
			IOCtl(CMDSetNegativeLimit, &cmd);

			// analog input
			level = 1;
			cmd.parameters = &level;
			IOCtl(CMDSetAxisAnalog, &cmd);

			// what to do about the PID gain, who should I trust?
			// data are both on flash memory and on the configuration file.
			// using the numbers on the initialization file for now.
			IOCtl(CMDControllerIdle, &actual_axis);
			cmd.parameters = &_parameters->_highPIDs[i];
				
			IOCtl(CMDSetPID, &cmd);

			// reset encoders.
			// just in case :)
			double pos = 0.0;
			cmd.parameters = &pos;
			IOCtl(CMDDefinePosition, &cmd);

			// and set the limits too from the data in the initialization file.
			double min, max;
			if (_parameters->_signs[i] == 1)
				min = -(_parameters->_limitsMin[i] * 
						_parameters->_encoderToAngles[i]) / 360.0 + 
						_parameters->_zeros[i];
			else
				min = (_parameters->_limitsMin[i] * 
						_parameters->_encoderToAngles[i]) / 360.0 + 
						_parameters->_zeros[i];

			if (_parameters->_signs[i] == 1)
				max = -(_parameters->_limitsMax[i] * 
						_parameters->_encoderToAngles[i]) / 360.0 + 
						_parameters->_zeros[i];
			else
				max = (_parameters->_limitsMax[i] * 
						_parameters->_encoderToAngles[i]) / 360.0 + 
						_parameters->_zeros[i];

			cmd.parameters = &min;
			IOCtl(CMDSetSWNegativeLimit, &cmd);

			cmd.parameters = &max;
			IOCtl(CMDSetSWPositiveLimit, &cmd);
			
			// sets the current limit on each joint according to the configuration file.
			cmd.parameters = &_parameters->_currentLimits[i];
			IOCtl(CMDSetCurrentLimit, &cmd);
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
		if (YARPEsdCanDeviceDriver::close() != 0)
			return YARP_FAIL;

		_initialized = false;
		return YARP_OK;
	}

	/**
	 * Disables simultaneously the controller and the amplifier (they are
	 * usually two separate commands on control cards).
	 * @return YARP_OK always.
	 */
	int idleMode()
	{
		for(int i = 0; i < _parameters->_nj; i++)
		{
			int j = _parameters->_axis_map[i];
			IOCtl(CMDControllerIdle, &j);
			IOCtl(CMDDisableAmp, &j);
		}
		return YARP_OK;
	}

	/**
	 * Sets the PID values specified in a second set of parameters 
	 * typically read from the intialization file.
	 * @param reset if true resets the encoders.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int activateLowPID(bool reset = true)
	{
		return activatePID(reset, _parameters->_lowPIDs);
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
			int actual_axis = _parameters->_axis_map[i];
			IOCtl(CMDControllerIdle, &actual_axis);
			SingleAxisParameters cmd;
			cmd.axis = actual_axis;

			if (pids == NULL)
				cmd.parameters = &_parameters->_highPIDs[i];
			else
				cmd.parameters = &pids[i];
				
			IOCtl(CMDSetPID, &cmd);

			// reset encoders
			if (reset) 
			{
				double pos = 0.0;
				cmd.parameters = &pos;
				IOCtl(CMDDefinePosition, &cmd);
			}
			//////////////////////////
			IOCtl(CMDControllerRun, &actual_axis);
			IOCtl(CMDEnableAmp, &actual_axis);
			IOCtl(CMDClearStop, &actual_axis);
		}
		return YARP_OK;
	}

	/**
	 * Reads the analog value from the control card.
	 * This method is not implemented at the moment.
	 * @return YARP_FAIL always.
	 */
	int readAnalogs(double *val)
	{
		ACE_UNUSED_ARG(val);
		return YARP_FAIL;
	}

	/**
	 * Moves with constant speed until an obstacle is encountered.
	 * @param joint is the axis to move.
	 * @param speed is the desired speed.
	 * @param accel is the desired accel.
	 * @return the position of the encoder reached at the moment of impact.
	 */
	double speedMove (int joint, double speed, double accel, double threshold)
	{
		double *tmpv = new double[_parameters->_nj];
		ACE_ASSERT (tmpv != NULL);
		ACE_OS::memset (tmpv, 0, sizeof(double) * _parameters->_nj);

		SingleAxisParameters x;
		x.axis = joint;	
		x.parameters = &accel;

		tmpv[joint] = speed;

		IOCtl(CMDSetAcceleration, (void *)&x);
		IOCtl(CMDVMove, (void *)tmpv);

		double error = 0;
		SingleAxisParameters s;
		s.axis = joint;
		s.parameters = &error;
		int ret = YARP_FAIL;
		do 
		{
			// gets error.
			ret = IOCtl(CMDGetTorque, (void *)&s);
			YARPTime::DelayInSeconds(0.01);
		}
		while (fabs(error) < threshold && ret != YARP_FAIL);

		// stop motion.
		ACE_OS::memset (tmpv, 0, sizeof(double) * _parameters->_nj);
		tmpv[joint] = 0;

		IOCtl(CMDSetAcceleration, (void *)&x);
		IOCtl(CMDVMove, (void *)tmpv);

		// get current position.
		double position = 0;
		s.parameters = &position;
		IOCtl(CMDGetPosition, (void *)&s);

		delete[] tmpv;

		return position;
	}

	/**
	 * Just a generic calibration helper function. WARNING: errors are not handled!
	 * @param joint is the joint to be calibrated.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int genericCalibrate (int joint)
	{
		const double DESIRED_ACC = 2;
		const double DESIRED_SPEED = 40;
		const double THRESHOLD = 100;
		const double MARGIN = 500;

		double max_position = 0;
		double min_position = 0;

		max_position = speedMove (joint, DESIRED_SPEED, DESIRED_ACC, THRESHOLD);
		min_position = speedMove (joint, -DESIRED_SPEED, DESIRED_ACC, THRESHOLD);

		const double center = (max_position+min_position)/2;

		SingleAxisParameters cmd;
		cmd.axis = joint;
		double tmp = 0;
		cmd.parameters = &tmp;

		tmp = DESIRED_SPEED;
		IOCtl(CMDSetSpeed, &cmd);

		tmp = center;
		IOCtl(CMDSetPosition, &cmd);
		
		// should wait for movement completion instead.
		YARPTime::DelayInSeconds (0.5);

		// zero encoder here.
		tmp = 0;
		IOCtl(CMDDefinePosition, &cmd);

		// can set limits, right?
		tmp = (max_position-min_position)/2 - MARGIN;
		IOCtl(CMDSetSWPositiveLimit, &cmd);
		tmp = -(max_position-min_position)/2 + MARGIN;
		IOCtl(CMDSetSWNegativeLimit, &cmd);

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
		switch (joint)
		{
		default:
			return YARP_FAIL;

		case -1:
			YARP_ROBOTCUB_ARM_ADAPTER_DEBUG(("Starting arm calibration routine"));
			ACE_OS::printf("..done!\n");
			return YARP_OK;

		// LATER: to be completed.
		}

		return YARP_OK;
	}

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
	YARPJamesArmParameters *_parameters;
};

#endif	// .h