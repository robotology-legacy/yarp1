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
///                    #cbeltran#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPGALILOnEurobotHeadAdapter.h,v 1.4 2004-12-29 14:11:42 beltran Exp $
///
///


#ifndef __GALILONEUROBOTHEAD__
#define __GALILONEUROBOTHEAD__

#include <ace/Log_Msg.h>
#include <yarp/YARPGalilDeviceDriver.h>
#include <yarp/YARPConfigFile.h>

#define YARP_BABYBOT_HEAD_ADAPTER_VERBOSE

#ifdef YARP_BABYBOT_HEAD_ADAPTER_VERBOSE
#define YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_DEBUG("BABYBOT_HEAD_ADAPTER_DEBUG:", string)
#else  YARP_BABYBOT_HEAD_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

/**
 * \file YARPGalilOnEurobotAdapter.h This file contains definitions of the control classes
 * for the Eurobot head acoording to the YARP device driver model.
 */

/**
 * _EurobotHead contains the default parameters of the Eurobot head control card(s).
 */
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


/**
 * YARPEurobotHeadParameters is one of the components required to
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
class YARPEurobotHeadParameters
{
public:
	/**
	 * Default constructor.
	 * Allocates memory and sets parameters to suitable default values.
	 */
	YARPEurobotHeadParameters()
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
			_encoderToAngles[i] = _EurobotHead::_encoderToAngles[i];
			_stiffPID[i] = _EurobotHead::_stiffPID[i];
			_maxDAC[i] = _EurobotHead::_maxDAC[i];
		}

		// invert the axis map.
		ACE_OS::memset (_inv_axis_map, 0, sizeof(int) * _nj);
		for (int i = 0; i < _nj; i++)
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

	/**
	 * Copies an existing parameter instance into this one.
	 * @param peer is the reference to the object to copy in.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
	int copy (const YARPEurobotHeadParameters& peer)
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
	}

public:
	LowLevelPID *_highPIDs;
	LowLevelPID *_lowPIDs;
	double *_zeros;
	double *_signs;
	int *_axis_map;
	int *_inv_axis_map;
	double *_encoderToAngles;
	int *_stiffPID;
	int _nj;
	double *_maxDAC;
	YARPString _inertialConfig;
	double *_limitsMax;
	double *_limitsMin;
	char _mask;
};


/**
 * YARPGALILOnEurobotHeadAdapter is a specialization of the Galil card device driver
 * to control the Eurobot head. This class especially implements initialize and
 * uninitialize while it leaves much of the burden of calling the device driver
 * to a generic template class called YARPGenericControlBoard.
 */
class YARPGALILOnEurobotHeadAdapter : public YARPGalilDeviceDriver
{
public:
	/**
	 * Default constructor.
	 */
	YARPGALILOnEurobotHeadAdapter()
	{
		_initialized = false;
	}
	
	/**
	 * Destructor.
	 */
	~YARPGALILOnEurobotHeadAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	/**
	 * Initializes the adapter and opens the device driver.
	 * This is a specific initialization for the Eurobot head. NOTE: that the parameter
	 * here is not copied and references to it could still be made by the code. Until
	 * this behavior is correct, the user has to make sure the pointer doesn't become
	 * invalid during the lifetime of the adapter class (this one). Generally this is true
	 * for the "generic" templates since they allocate a copy of the parameter class
	 * internally (and their lifetime is related to that of the adapter).
	 *
	 * @param par is a pointer to the class containing the parameters that has
	 * to be exactly YARPEurobotHeadParameters.
	 * @return YARP_OK on success, YARP_FAIL otherwise.
	 */
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


		//----------------------------------------------------------------------
		//  This command is important in order to activate the second FIFO communications
		//----------------------------------------------------------------------
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
			IOCtl(CMDControllerIdle, &i);
			
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
			IOCtl(CMDServoHere,NULL); //Start the motors
			//IOCtl(CMDControllerRun, &i);
			//IOCtl(CMDEnableAmp, &i);
			//IOCtl(CMDClearStop, &i);
		}
		return YARP_OK;
	}

	/**
	 * Reads the analog values from the control card (ADC values).
	 * @param val is the array to receive the analog readings.
	 * @return YARP_FAIL always.
	 */
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

	/**
	 * Calibrates the control card if needed.
	 * @param joint is the joint number/function requested to the calibrate method. 
	 * The default value -1 does nothing.
	 * @return YARP_OK always.
	 */
	int calibrate(int joint = -1)
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
