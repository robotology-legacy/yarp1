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
/// $Id: YARPJoyPresDeviceDriver.cpp,v 1.1 2004-09-13 23:22:49 babybot Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPJoyPresDeviceDriver.h"
#include <yarp/YARPPresSensUtils.h>

#include "../dd_orig/include/Joystick.h"

class JoyPresResources
{
public:
	JoyPresResources (void) : _bmutex(1)
	{
		_data.x = 0;
		_data.y = 0;
		_data.z = 0;
		_data.u = 0;
		_data.buttons = 0;
		_bStreamStarted = false;
		_bError = false;
	}

	~JoyPresResources () { }

	CJoystick _sensor;

	JoyData _data;

	bool _bStreamStarted;
	bool _bError;

	int _sensN;
	
	YARPSemaphore _bmutex;

};

inline JoyPresResources& RES(void *res) { return *(JoyPresResources *)res; }

///
///
YARPJoyPresDeviceDriver::YARPJoyPresDeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPJoyPresDeviceDriver>(PSCMDNCmds)
{
	system_resources = (void *) new JoyPresResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[PSCMDGetData] = &YARPJoyPresDeviceDriver::getData;
	m_cmds[PSCMDStartStreaming] = &YARPJoyPresDeviceDriver::startStreaming;
	m_cmds[PSCMDStopStreaming] = &YARPJoyPresDeviceDriver::stopStreaming;
}

YARPJoyPresDeviceDriver::~YARPJoyPresDeviceDriver()
{
	if (system_resources != NULL)
		delete (JoyPresResources *)system_resources;
	system_resources = NULL;
}

///
///
int YARPJoyPresDeviceDriver::open (void *res)
{
	int *param = (int*) res;
	JoyPresResources& d = RES(system_resources);

	return YARP_OK;
}

int YARPJoyPresDeviceDriver::close (void)
{
	return YARP_OK;
}

///
///
/// acquisition thread for real!
void YARPJoyPresDeviceDriver::Body (void)
{
	JoyPresResources& d = RES(system_resources);
	JoyData* pData = &(d._data);
	while (!IsTerminated())	
	{
		d._bmutex.Wait();
		d._bError = d._sensor.getRawData(pData);
		d._bmutex.Post();
	}

	ACE_DEBUG ((LM_DEBUG, "acquisition thread returning...\n"));
}

int YARPJoyPresDeviceDriver::startStreaming (void *)
{
	JoyPresResources& d = RES(system_resources);

	d._bStreamStarted = true;
	
	Begin ();

	return YARP_OK;
}

int YARPJoyPresDeviceDriver::stopStreaming (void *)
{
	JoyPresResources& d = RES(system_resources);

	d._bStreamStarted = true;
	
	End ();

	return YARP_OK;
}

int YARPJoyPresDeviceDriver::getData (void *cmd)
{
	JoyPresResources& d = RES(system_resources);
	struct PresSensData * data = (struct PresSensData *)cmd;
	if (d._bStreamStarted)
	{
		d._bmutex.Wait();
		data->channelA = d._data.x;
		data->channelB = d._data.y;
		data->channelC = d._data.z;
		data->channelD = d._data.u;
		d._bmutex.Post();
	}
	else
	{
		JoyData* pData = &(d._data);
		d._bError = d._sensor.getRawData(pData);
		data->channelA = d._data.x;
		data->channelB = d._data.y;
		data->channelC = d._data.z;
		data->channelD = d._data.u;
	}

	if (d._bError)
		return YARP_FAIL;
	else
		return YARP_OK;
}
