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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPGalilOnBabybotHandAdapter.h,v 1.2 2004-01-17 00:15:15 gmetta Exp $
///
///

// //
//
// Originally feb 2003 -- by nat
//
// adapted for yarp June 2003 -- by nat


#ifndef __GALILONBABYBOTHANDDAPTER__
#define __GALILONBABYBOTHANDDAPTER__

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