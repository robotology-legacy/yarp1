// //
//
// Originally feb 2003 -- by nat
//
// adapted for yarp June 2003 -- by nat


#ifndef __GALILONBABYBOTHANDDAPTER__
#define __GALILONBABYBOTHANDDAPTER__

// $Id: YARPGalilOnBabybotHandAdapter.h,v 1.1 2003-07-26 22:11:20 babybot Exp $

#include <ace/log_msg.h>
#include <YARPGalilDeviceDriver.h>
#include <string>

#define YARP_GALIL_ON_BABYBOT_HAND_ADAPTER_VERBOSE

#ifdef YARP_GALIL_ON_BABYBOT_HAND_ADAPTER_VERBOSE
#define YARP_GALIL_ON_BABYBOT_HAND_ADAPTER_DEBUG(string) YARP_DEBUG("YARP_GALIL_ON_BABYBOT_HAND_ADAPTER_DEBUG:", string)
#else  YARP_GALIL_ON_BABYBOT_HAND_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <YARPConfigFile.h>

#include "YARPBabybotHandParameters.h"

class YARPGalilOnBabybotHandAdapter : public YARPGalilDeviceDriver
{
public:
	YARPGalilOnBabybotHandAdapter()
	{
		_initialized = false;
	}
	
	~YARPGalilOnBabybotHandAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(YARPBabybotHandParameters *par)
	{
		// open galil
		int rc;
		_parameters = par;
		GalilOpenParameters gopen;
		gopen.device_id = par->_galil_id;
		gopen.nj = par->_nj;
		gopen.mask = par->_mask;
		
		rc = YARPGalilDeviceDriver::open(&gopen);

		if (rc != 0)
		{
			YARP_GALIL_ON_BABYBOT_HAND_ADAPTER_DEBUG(("Error: cannot initialize Galil control board !\n"));
			return rc;
		}

		_initialized = true;
		return YARP_OK;
	}

	int stop()
	{
		int ret;
		ret = IOCtl(CMDAbortAxes, NULL);
		ret = IOCtl(CMDServoHere, NULL);
		return ret;
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
		// set output port to 1..1 (disable motors)
		IOParameters out;
		out.value = (i16) 65535;
		return IOCtl(CMDSetOutputPort, &out);
	}

	int activatePID()
	{
		// set gains
		for(int i = 0; i < _parameters->_naj; i++)
		{
			IOCtl(CMDControllerIdle, &i); // disable PID
			SingleAxisParameters cmd;
			cmd.axis = i;
			cmd.parameters = &_parameters->_highPIDs[i];
			IOCtl(CMDSetPID, &cmd);
			double pos = 0.0;
			cmd.parameters = &pos;
			IOCtl(CMDDefinePosition, &cmd);
			IOCtl(CMDServoHere, &cmd);	//enable PID
		}

		// set output port to 0 (enable motors)
		IOParameters out;
		out.value = (i16) 0;
		return IOCtl(CMDSetOutputPort, &out);
	}

	// returns max torque on axis; note: this is not the current value, this is
	// the maximum possible value for the board (i.e. MEI = 32767.0, Galil 9.99)
	double getMaxTorque(int axis)
	{
		int tmp = _parameters->_axes_lut[axis];
		return _parameters->_torque_limits[tmp];
	}

	int calibrate() {
		// not calibration routine available yet
		return YARP_OK;
	}

private:

	bool _initialized;

	YARPBabybotHandParameters *_parameters;
};

#endif