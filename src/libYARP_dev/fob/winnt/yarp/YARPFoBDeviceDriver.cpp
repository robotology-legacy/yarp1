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
/// $Id: YARPFoBDeviceDriver.cpp,v 1.1 2004-09-13 23:22:49 babybot Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPFoBDeviceDriver.h"
#include <yarp/YARPTrackerUtils.h>

#include "../dd_orig/include/Bird.h"

class FoBResources
{
public:
	FoBResources (void)
	{
		posScaling = 0;
		xMapping = 1;
		yMapping = -1;
		zMapping = -1;
		streamingStarted = false;
		connected = false;
		groupID = 1;
		standalone = true;
		nDevices = 1;
	};

	~FoBResources () 
	{
	
	};

	int groupID;
	bool standalone;
	bool streamingStarted;
	bool connected;
	double posScaling;
	int nDevices;
	int xMapping;
	int yMapping;
	int zMapping;

};


///
/// actual device driver class.
///
inline FoBResources& RES(void *res) { return *(FoBResources *)res; }

YARPFoBDeviceDriver::YARPFoBDeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPFoBDeviceDriver>(TCMDNCmds)
{
	system_resources = (void *) new FoBResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[TCMDGetData] = &YARPFoBDeviceDriver::getData;
	m_cmds[TCMDStartStreaming] = &YARPFoBDeviceDriver::startStreaming;
	m_cmds[TCMDStopStreaming] = &YARPFoBDeviceDriver::stopStreaming;

}

YARPFoBDeviceDriver::~YARPFoBDeviceDriver()
{
	if (system_resources != NULL)
		delete (FoBResources *)system_resources;
	system_resources = NULL;
}

///
///
int YARPFoBDeviceDriver::open (void *res)
{
	FoBResources& d = RES(system_resources);
	FoBOpenParameters *param = (FoBOpenParameters*) res;

// Open the device Driver

	int ret;
	ret = birdRS232WakeUp(d.groupID,d.standalone,d.nDevices,&(param->comPort),param->baudRate,param->timeOut,param->timeOut);
	if (!ret)
		return YARP_FAIL;
	
	BIRDDEVICECONFIG birdConfig;
	birdGetDeviceConfig(d.groupID,0,&birdConfig);
	d.posScaling = birdConfig.wScaling;
	d.connected = true;
	return YARP_OK;	
}

int YARPFoBDeviceDriver::close (void)
{
	FoBResources& d = RES(system_resources);
	
	if (d.streamingStarted)
		stopStreaming(NULL);

	if (d.connected)
		birdShutDown(d.groupID);
	
	d.connected = false;
	return YARP_OK;
}


int YARPFoBDeviceDriver::getData (void *cmd)
{
	FoBResources& d = RES(system_resources);
	if (!d.connected)
		return YARP_FAIL;

	BIRDFRAME dataFrame;
	BIRDREADING *birdReading;
	
	if (!d.streamingStarted)
	{
		birdStartSingleFrame(d.groupID);
		birdGetFrame(d.groupID,&dataFrame);
	}
	else
		birdGetMostRecentFrame(d.groupID,&dataFrame);
	
	birdReading = &dataFrame.reading[0];
	struct TrackerData * data = (struct TrackerData *)cmd;
	data->x = birdReading->position.nX * d.posScaling / 32767.0 * d.xMapping;
	data->y = birdReading->position.nY * d.posScaling / 32767.0 * d.yMapping;
	data->z = birdReading->position.nZ * d.posScaling / 32767.0 * d.zMapping;
	data->azimuth = birdReading->angles.nAzimuth * 180.0 / 32767.0;
	data->elevation = birdReading->angles.nElevation * 180.0 / 32767.0;
	data->roll = birdReading->angles.nRoll * 180.0 / 32767.0;
	return YARP_OK;
}

int YARPFoBDeviceDriver::startStreaming (void *)
{
	FoBResources& d = RES(system_resources);
	int ret;
	ret = birdStartFrameStream(d.groupID);
	if (!ret)
		return YARP_FAIL;
	d.streamingStarted = true;
	return YARP_OK;
}

int YARPFoBDeviceDriver::stopStreaming (void *)
{
	FoBResources& d = RES(system_resources);
	d.streamingStarted = false;
	birdStopFrameStream(d.groupID);
	return YARP_OK;
}

