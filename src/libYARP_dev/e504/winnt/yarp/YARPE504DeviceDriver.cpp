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
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPE504DeviceDriver.cpp,v 1.2 2006-04-07 20:48:46 claudio72 Exp $
///
///

#include<iostream>

using namespace std;

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPE504DeviceDriver.h"

// definitions for serial ports
#include <atlbase.h>
// Definition of ASLControllerPort COM server
#import "../dd_orig/lib/ASLControllerLib.tlb" no_namespace raw_interfaces_only

#include <yarp/YARPGazeTrackerUtils.h>

class E504Resources
{
public:
	E504Resources () {
		connected = false;
		sceneUpLeftX = sceneUpLeftY = sceneDnRightX = sceneDnRightY = 0;
	};

	~E504Resources () { };

	bool connected;
	long sceneUpLeftX, sceneUpLeftY, sceneDnRightX, sceneDnRightY;

	CComPtr<IASLControllerPort> E504ControllerPort;

};

///
/// actual device driver class.
///
inline E504Resources& RES(void *res) { return *(E504Resources *)res; }

YARPE504DeviceDriver::YARPE504DeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPE504DeviceDriver>(GTCMDNCmds)
{

	system_resources = (void *) new E504Resources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[GTCMDGetData] = &YARPE504DeviceDriver::getData;

}

YARPE504DeviceDriver::~YARPE504DeviceDriver()
{

	if (system_resources != NULL) {
		delete (E504Resources *)system_resources;
	}
	system_resources = NULL;

}

int YARPE504DeviceDriver::open (void *res)
{

	// gather system resources and parameters
	E504Resources& d = RES(system_resources);
	E504OpenParameters* param = (E504OpenParameters*) res;

	if ( d.connected ) {
		return YARP_FAIL;
	}

	// initialize COM environment
	if (FAILED(CoInitialize(NULL))) {
		return YARP_FAIL;
	}

	// create COM object
	if (FAILED(d.E504ControllerPort.CoCreateInstance(__uuidof(ASLControllerPort)))) {
		return YARP_FAIL;
	}

	// connect me!!
	if ( SUCCEEDED(d.E504ControllerPort->Connect(param->comPort, param->baudRate)) ) {
		// initialize control unit:
		d.E504ControllerPort->set_illuminator_power_mode(1);
		d.E504ControllerPort->set_tracking_mode(1);
 		d.E504ControllerPort->set_data_acquisition_mode(1);
	} else {
		return YARP_FAIL;
	}

	// gather scene target points - used to evaluate
	// scene POG upon getData()
	d.E504ControllerPort->get_scene_target_point_position (1, &d.sceneUpLeftX, &d.sceneUpLeftY);
	d.E504ControllerPort->get_scene_target_point_position (9, &d.sceneDnRightX, &d.sceneDnRightY);

	// ok, bail out
	d.connected = true;
	return YARP_OK;	

}

int YARPE504DeviceDriver::close (void)
{

	E504Resources& d = RES(system_resources);
	
	if ( ! d.connected ) {
		return YARP_FAIL;
	}

	d.E504ControllerPort->set_illuminator_power_mode(0);
	d.E504ControllerPort->set_data_acquisition_mode(0);
	if (FAILED(d.E504ControllerPort->Disconnect())) {
		return YARP_FAIL;
	}
	
	d.connected = false;

	// destroy CComPtr object
	d.E504ControllerPort.Detach();

	// uninitialize COM environment
	CoUninitialize();

	return YARP_OK;

}


int YARPE504DeviceDriver::getData (void *cmd)
{

	E504Resources& d = RES(system_resources);

	if ( ! d.connected ) {
		return YARP_FAIL;
	}

	struct GazeTrackerData* data = (struct GazeTrackerData*) cmd;

	data->targetPoints[0] = (double)d.sceneUpLeftX;
	data->targetPoints[1] = (double)d.sceneUpLeftY;
	data->targetPoints[2] = (double)d.sceneDnRightX;
	data->targetPoints[3] = (double)d.sceneDnRightY;

	ASLController_Results results;
	VARIANT_BOOL valid = VARIANT_FALSE;

	// Read Eye Tracker messages in the buffer
	d.E504ControllerPort->updateComm();
	d.E504ControllerPort->get_results(&results, &valid);
	// Extract all messages from the buffer
	while (valid == VARIANT_TRUE) {
		if (results.pupil_diameter != 0 && results.cr_diameter != 0) {
			data->valid = true;
			data->pupilX = (double)results.scene_x / (double)results.gaze_res_factor;
			data->pupilY = (double)results.scene_y / (double)results.gaze_res_factor;
		} else {
			data->valid = false;
			data->pupilX = 0.0;
			data->pupilY = 0.0;
		}
		d.E504ControllerPort->get_results(&results, &valid);
	}

	return YARP_OK;

}
