//$Id: YARPGALILOnEurobotHeadAdapter.h,v 1.3 2003-10-17 16:34:40 babybot Exp $

#ifndef __GALILONEUROBOTHEAD__
#define __GALILONEUROBOTHEAD__

#include <ace/Log_Msg.h>
#include <YARPGalilDeviceDriver.h>
#include <string>

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
	char _mask = (char) 0x0F;
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
}; // namespace

class YARPEurobotHeadParameters
{
public:
	YARPEurobotHeadParameters()
	{
		_nj = _EurobotHead::_nj;
		_mask = _EurobotHead::_mask;
		for(int i = 0; i<_nj; i++) {
			_highPIDs[i] = _EurobotHead::_highPIDs[i];
			_lowPIDs[i] = _EurobotHead::_lowPIDs[i];
			_zeros[i] = _EurobotHead::_zeros[i];
			_axis_map[i] = _EurobotHead::_axis_map[i];
			_signs[i] = _EurobotHead::_signs[i];
			_encoderToAngles[i] = _EurobotHead::_encoderToAngles[i];
		}
	}

	int load(const YARPString &path, const YARPString &init_file)
	{
		ACE_ASSERT(0);	//not implemented yet !
	}

	LowLevelPID _highPIDs[_EurobotHead::_nj];
	LowLevelPID _lowPIDs[_EurobotHead::_nj];
	double _zeros[_EurobotHead::_nj];
	double _signs[_EurobotHead::_nj];
	int _axis_map[_EurobotHead::_nj];
	double _encoderToAngles[_EurobotHead::_nj];
	int _nj;
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
	
	/*
	int initialize()
	{
		YARPEurobotHeadParameters parameters;
		initialize((const YARPEurobotHeadParameters)parameters);
	}
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

private:
	bool _initialized;
	YARPEurobotHeadParameters * _parameters;
};

#endif	// .h