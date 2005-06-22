/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran-Gonzalez#                      ///
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
///                    #Carlos Beltran-Gonzalez#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPGALILOnEurobotArmAdapter.h,v 1.6 2005-06-22 15:51:16 beltran Exp $
///
///

#ifndef __GALILONEUROBOTARMADAPTER__
#define __GALILONEUROBOTARMADAPTER__

#include <ace/Log_Msg.h>
#include <yarp/YARPGalilDeviceDriver.h>
#include <yarp/YARPString.h>

#define YARP_BABYBOT_ARM_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_ARM_ADAPTER_VERBOSE
#define YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_ARM_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <yarp/YARPConfigFile.h>

/**
 * \file YARPGALILOnEurobotArmAdapter.h This file contains definitions of the control classes
 * for the Eurobot arm according to the YARP device driver model.
 */

/**
 * _EurobotArm contains the default parameters of the Eurobot arm control card(s).
 */
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


/**
 * YARPEurobotArmParameters is one of the components required to
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
 * Note on the wrist mechanical coupling:
 * 
 *			1/G00	0		0		0		0		0
 *			0		1/G11	0		0		0		0
 *	joint =	0		0		1/G22	0		0		0       * [motor0 motor1 ... motor5]'
 *			0		0		0		1/G33	0		0
 *			0		0		0		-G34	1/G44	0
 *			0		0		0		-G35	-G45	1/G55
 *
 * Where: [G00 G11 G22 ... G55] = _encoders[]	// gear ratios
 *		  [0 0 0 -G34 -G45 -G55] = fwdCouple[]	// coupling between joints
 * 
 * Consider also: 
 * - motor[j] = 2*pi*enc[j]/encWheels[j]
 * - In the code _fwdCouple is substituted with _fwdCouple *= encWheels
 *
 * This leads to the encoderToAngles() and anglesToEncoders() functions as they are in the YARPEurobotArm class.
 * 
 */

class YARPEurobotArmParameters : public YARPGenericControlParameters
{
public:
	/**
	 * Default constructor.
	 * Allocates memory and sets parameters to suitable default values.
	 */
	YARPEurobotArmParameters()
	{
		_highPIDs = NULL;
		_lowPIDs = NULL;
		_zeros = NULL;
		_signs = NULL;
		_axis_map = NULL;
		_inv_axis_map = NULL;
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
		for(i = 0; i<_nj; i++) 
		{
			_highPIDs[i]        = _EurobotArm::_highPIDs[i];
			_lowPIDs[i]         = _EurobotArm::_lowPIDs[i];
			_zeros[i]           = _EurobotArm::_zeros[i];
			_axis_map[i]        = _EurobotArm::_axis_map[i];
			_signs[i]           = _EurobotArm::_signs[i];
			_encoderToAngles[i] = _EurobotArm::_encoders[i]*_EurobotArm::_encWheels[i];
			_fwdCouple[i]       = _EurobotArm::_fwdCouple[i];
			_stiffPID[i]        = _EurobotArm::_stiffPID[i];
			_maxDAC[i]          = _EurobotArm::_maxDAC[i];
		}

		// compute inv couple
		for (i = 0; i < 3; i++)
			_invCouple[i] = 0.0;	// first 3 joints are not coupled

		_invCouple[3] = -_fwdCouple[3] / (_encoderToAngles[3] * _encoderToAngles[4]);
		_invCouple[4] = -_fwdCouple[4] / (_encoderToAngles[3] * _encoderToAngles[5]) +
			(_fwdCouple[3] * _fwdCouple[5]) / (_encoderToAngles[3] * _encoderToAngles[4] * _encoderToAngles[5]);
		_invCouple[5] = -_fwdCouple[5] / (_encoderToAngles[4] * _encoderToAngles[5]);

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
		if (_inv_axis_map != NULL)
			delete [] _inv_axis_map;
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
		
		_fwdCouple[3] = _fwdCouple[3]*encWheels[3];
		_fwdCouple[4] = _fwdCouple[4]*encWheels[4];
		_fwdCouple[5] = _fwdCouple[5]*encWheels[5];

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

	/**
	 * Copies an existing parameter instance into this one.
	 * @param peer is the reference to the object to copy in.
	 * @return YARP_OK always.
	 */
	int copy (const YARPEurobotArmParameters& peer)
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
			memcpy (_fwdCouple, peer._fwdCouple, sizeof(double) * _nj);
			memcpy (_invCouple, peer._invCouple, sizeof(double) * _nj);
			memcpy (_stiffPID, peer._stiffPID, sizeof(int) * _nj);
			memcpy (_maxDAC, peer._maxDAC, sizeof(double) * _nj);
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
			if (_fwdCouple != NULL) delete [] _fwdCouple;
			if (_invCouple != NULL)	delete [] _invCouple;
			if (_stiffPID != NULL) delete [] _stiffPID;
			if (_maxDAC != NULL) delete [] _maxDAC;

			_highPIDs = NULL;
			_lowPIDs = NULL;
			_zeros = NULL;
			_signs = NULL;
			_axis_map = NULL;
			_inv_axis_map = NULL;
			_encoderToAngles = NULL;
			_fwdCouple = NULL;
			_invCouple = NULL;
			_stiffPID = NULL;
			_maxDAC = NULL;
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
		_inv_axis_map = new int [nj];
		_encoderToAngles = new double [nj];
		_fwdCouple = new double [nj];
		_invCouple = new double [nj];
		_stiffPID = new int [nj];
		_maxDAC = new double [nj];
	}

public:
	LowLevelPID *_highPIDs;
	LowLevelPID *_lowPIDs;
	double *_fwdCouple;
	double *_invCouple;
	int *_stiffPID;
	double *_maxDAC;
	char _mask;
};


/**
 * YARPGALILOnEurobotArmAdapter is a specialization of the GALIL card device driver
 * to control the Eurobot head. This class especially implements initialize and
 * uninitialize while it leaves much of the burden of calling the device driver
 * to a generic template class called YARPGenericControlBoard.
 *
 *
 */
class YARPGALILOnEurobotArmAdapter : 
	public YARPGalilDeviceDriver,
	public YARPGenericControlAdapter<YARPGALILOnEurobotArmAdapter, YARPEurobotArmParameters>
{
public:
	/**
	 * Default constructor.
	 */
	YARPGALILOnEurobotArmAdapter()
	{
		_initialized = false;
		_amplifiers = false;
		_softwareLimits = false;
	}
	
	/**
	 * Destructor.
	 */
	~YARPGALILOnEurobotArmAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	/*
	 * implemented from the abstract base class.
	 */

	/**
	 * Initializes the adapter and opens the device driver.
	 * This is a specific initialization for the Eurobot arm. NOTE: that the parameter
	 * here is not copied and references to it could still be made by the code. Until
	 * this behavior is correct, the user has to make sure the pointer doesn't become
	 * invalid during the lifetime of the adapter class (this one). Generally this is true
	 * for the "generic" templates since they allocate a copy of the parameter class
	 * internally (and their lifetime is related to that of the adapter).
	 *
	 * @param par is a pointer to the class containing the parameters that has
	 * to be exactly YARPEurobotArmParameters.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int initialize(YARPEurobotArmParameters *par)
	{
		//// open device
		_parameters = par;
		GalilOpenParameters op_par;
		op_par.nj= _parameters->_nj; 
		op_par.mask = _parameters->_mask;
		if (YARPGalilDeviceDriver::open(&op_par) != 0)
			return YARP_FAIL;

		delay(1000); //Just wait a little for the galil to initialize

		//Reset the card
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Reseting control card\n"));
		IOCtl(CMDResetController, NULL);

		delay(1000);

		//idleMode();

		_amplifiers = false;

		// amp level and limits
		for(int i=0; i < _parameters->_nj; i++)
		{
			///motors type MT -1 (REVERSED POLARITY) ATENTION!!-This is the polarity of the eurobot arm
			SingleAxisParameters cmd;
			cmd.axis=i;
			double motor_type = -1; 
			cmd.parameters=&motor_type;
			IOCtl(CMDMotorType,&cmd);
			IOCtl(CMDGetMotorType,&cmd);
			YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Motor type motor %d = %f\n",cmd.axis, motor_type));

			///Set the error limit
			int error = 10000;
			cmd.parameters=&error;
			IOCtl(CMDErrorLimit, &cmd);

			///set the off_on error
			int value = 1;
			cmd.parameters=&value;
			IOCtl(CMDOffOnError,&cmd); 

			//////////////////////////////////////////////////////////////////////////////////////
			// PUMA has no hw limits
			// IMPORTANT: A software limits has been hardcoded in the eurobot arm galil controller.
			// Check the comments in the file $(YARP_ROOT)/conf/eurobot/NOTEGALILREADME.txt
			// Check also the previous note to see the initial state of the motors 
			//////////////////////////////////////////////////////////////////////////////////////
		}

		//
		// This activates the necesary bit in the output port to be able to start the puma
		// The value has been obtained empirically. It is sure it can be improved by using
		// the time to search the exact bit
		//

		IOParameters cmd;
		cmd.port = 1; //This is ignored
		cmd.value = (short) 255;
		IOCtl(CMDSetOutputPort,&cmd);

		int frec = -3;
		IOCtl(CMDSetDR,&frec); //Set second FIFO refresh frecuency (DR command)

		_initialized = true;
		return YARP_OK;
	}

	/**
	 * Uninitializes the controller and closes the device driver.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
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
		}
		return YARP_OK;
	}

	/**
	 * Calibrates the control card if needed.
	 * NOTE: this function doesn't do the mapping (_axis_map) and assumes the controlled
	 * axes for the puma are numbered from 0 to 5. Please make sure this is the case
	 * before calling it!
	 * The finding index procedure in the Galil motion card is different from that
	 * on the MEI. The FI (find index) command it is used together with JG and BG.
	 * The problem is that when the card find and index it reset the position of
	 * the encoders to 0. Therefore, it is no way to implement a similar procedure
	 * that the one used in the MEI. 
	 * Then I have implemented a simplified procedure. Empirically I have found
	 * the nearest indexes to the initial position (marked with red signs on the
	 * arm) and it came out that they are found in the negative direction of the joints,
	 * for all of them. So, the speed used to find the indexes is -500 for all of them.
     * Note: The wait for motion done is not used because we dont need to wait to recover
     * the new position.
	 *
	 * @param joint is the joint number/function requested to the calibrate method.
	 * The default value -1 does nothing.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int calibrate(int joint = -1) 
	{
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Starting calibration routine...\n"));
		if (! (_initialized && _amplifiers) )
		{
			YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Sorry cannot calibrate encoders, the arm not initialized or the power is down!\n"));
			return YARP_FAIL;
		}

		bool indexsearchFlag = true;
		bool limitFlag = false;
		if (_softwareLimits)
			limitFlag = true;
		disableLimitCheck();

		double speeds1[6] = {-500.0, -500.0, -500.0, -500.0, -500.0, -500.0};

		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("Searching indexes...\n"));
		IOCtl(CMDIndexSearch, &indexsearchFlag);
		IOCtl(CMDVMove, speeds1);
		//IOCtl(CMDWaitForMotionDone, NULL);
		//IOCtl(CMDGetPositions, pos1);
		YARP_BABYBOT_ARM_ADAPTER_DEBUG(("done !\n"));
		////////////////////////////

		if (limitFlag)
			enableLimitCheck();

		return YARP_OK;
	}

	/**
	 * Sets the PID values.
	 * @param reset is not used.
	 * @param pids is an array of PID data structures. If NULL the values
	 * contained into an internal variable are used (presumably read from the
	 * initialization file) otherwise the actual argument is used.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int activatePID(bool reset, LowLevelPID *pids = NULL)
	{
		ACE_UNUSED_ARG (reset);

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
			double pos = 0.0;
			cmd.parameters = &pos;
			IOCtl(CMDDefinePosition, &cmd);
			IOCtl(CMDServoHere,NULL); //Start the motors. This activates the amplifiers
		}
		_amplifiers = true;

		return YARP_OK;
	}

	/*
	 * More functions: these are called from the higher level code.
	 */

	/**
	 * Checks whether the power is on on the Puma amplifier/controller.
	 * @return true if the ON switch is pressed.
	 */
	bool checkPowerOn()
	{
		//I didn't find a bit in the input/output Galil ports showing the
		//activation of the armpoweron button. For the moment just
		//return false. Next implement a message asking the user to activate
		//manually the bottom.

		return true;
	}

	/**
	 * Disables the software limit check.
	 * @return always YARP_OK.
	 */
	int disableLimitCheck()
	{
		_softwareLimits = false;
		// LATER disable software limit check (encoders)
		return YARP_OK;
	}

	/**
	 * Enables the software limit check.
	 * @return always YARP_OK.
	 */
	int enableLimitCheck()
	{
		_softwareLimits = true;
		// LATER enable software limit check (encoders)
		return YARP_OK;
	}

	/**
	 * Returns the maximum torque on a given axis; NOTE: this is not the current value, this is
	 * the maximum possible value for the board (i.e. MEI = 32767.0, Galil 9.99).
	 * @param axis is the axis the request refers to.
	 * @return the maximum allowed torque (at the output of the amplifier) espressed in some
	 * internal reference (e.g. 16 bits, voltage).
	 */
	double getMaxTorque(int axis)
	{
		int tmp = _parameters->_axis_map[axis];
		return _parameters->_maxDAC[tmp];
	}

protected:

	/**
	 * Sets the home configuration behavior.
	 * @param event is one of the possible events to apply when a homing condition is
	 * detected (e.g. STOP).
	 */
	void _setHomeConfig(int event)
	{
		for (int i = 0; i < _parameters->_nj; i++)
		{
			SingleAxisParameters cmd;
			int ipar;
			double dpar;
			cmd.axis = _parameters->_axis_map[i];
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

	/**
	 * Clears the STOP event from all axes. Needs to be done before
	 * controlling the robot.
	 */
	void _clearStop() 
	{
		for (int i = 0; i < _parameters->_nj; i++)
		{
			int j = _parameters->_axis_map[i];
			IOCtl(CMDClearStop, &j);	// clear stop event
		}
	}

	bool _initialized;
	bool _amplifiers;
	bool _softwareLimits;
	YARPEurobotArmParameters *_parameters;
};

#endif
