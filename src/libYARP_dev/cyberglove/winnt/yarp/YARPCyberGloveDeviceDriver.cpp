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
/// $Id: YARPCyberGloveDeviceDriver.cpp,v 1.1 2004-09-13 23:22:48 babybot Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPCyberGloveDeviceDriver.h"
#include <yarp/YARPDataGloveUtils.h>

#include "../dd_orig/include/Serial.h"


class CyberGloveResources
{
public:
	CyberGloveResources (void) : _bmutex(1)
	{
		_bStreamStarted = false;
		_bError = false;
	}

	~CyberGloveResources () { }

	CSerial _serialPort;

	DataGloveData _data;

	bool _bStreamStarted;
	bool _bError;

	YARPSemaphore _bmutex;

	void _fillDataStructure(unsigned char *inBytes, DataGloveData *destStruct);
};

void CyberGloveResources::_fillDataStructure(unsigned char *inBytes, DataGloveData *destStruct)
{
	int angles[22] = { 0 };
	
	for (int i=0;i<=6;i++)
	{
		angles[i] = inBytes[i];
	}
	angles[7] = inBytes[10];

	for (i=7;i<=21;i++)
	{
		angles[i+1] = inBytes[i];
	}

	// Thumb
	destStruct->thumb[0] = angles[0];
	destStruct->thumb[1] = angles[1];
	destStruct->thumb[2] = angles[2];
	destStruct->abduction[0] = angles[3];
	// Index
	destStruct->index[0] = angles[4];
	destStruct->index[1] = angles[5];
	destStruct->index[2] = angles[6];
	destStruct->abduction[1] = angles[11];
	// Middle
	destStruct->middle[0] = angles[8];
	destStruct->middle[1] = angles[9];
	destStruct->middle[2] = angles[10];
	destStruct->abduction[2] = angles[11];
	// Ring
	destStruct->ring[0] = angles[12];
	destStruct->ring[1] = angles[13];
	destStruct->ring[2] = angles[14];
	destStruct->abduction[3] = angles[15];
	// Pinkie
	destStruct->pinkie[0] = angles[16];
	destStruct->pinkie[1] = angles[17];
	destStruct->pinkie[2] = angles[18];
	destStruct->abduction[4] = angles[19];
	// Palm arhc
	destStruct->palmArch = angles[20];
	// Wrist pitch
	destStruct->wrist[0] = angles[21];
	// Wrist yaw
	destStruct->wrist[1] = angles[22];
}

///
///
inline CyberGloveResources& RES(void *res) { return *(CyberGloveResources *)res; }

///
///
YARPCyberGloveDeviceDriver::YARPCyberGloveDeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPCyberGloveDeviceDriver>(DGCMDNCmds)
{
	system_resources = (void *) new CyberGloveResources;
	ACE_ASSERT (system_resources != NULL);

	// For the IOCtl calls
	m_cmds[DGCMDGetData] = &YARPCyberGloveDeviceDriver::getData;
	m_cmds[DGCMDStartStreaming] = &YARPCyberGloveDeviceDriver::startStreaming;
	m_cmds[DGCMDStopStreaming] = &YARPCyberGloveDeviceDriver::stopStreaming;
	m_cmds[DGCMDGetLed] = &YARPCyberGloveDeviceDriver::getLed;
	m_cmds[DGCMDGetSwitch] = &YARPCyberGloveDeviceDriver::getSwitch;
	m_cmds[DGCMDResetGlove] = &YARPCyberGloveDeviceDriver::resetGlove;
	m_cmds[DGCMDSetLed] = &YARPCyberGloveDeviceDriver::setLed;
}

YARPCyberGloveDeviceDriver::~YARPCyberGloveDeviceDriver()
{
	if (system_resources != NULL)
		delete (CyberGloveResources *)system_resources;
	system_resources = NULL;
}

int YARPCyberGloveDeviceDriver::open (void *res)
{
	CyberGloveOpenParameters *param = (CyberGloveOpenParameters*) res;
	CyberGloveResources& d = RES(system_resources);

	CSerial::EBaudrate rate;
	LONG lLastError = ERROR_SUCCESS;

	switch (param->comPort)
	{
		case 1:
			lLastError = d._serialPort.Open("COM1");
			break;
		case 2:
			lLastError = d._serialPort.Open("COM2");
			break;
		case 3:
			lLastError = d._serialPort.Open("COM3");
			break;
		case 4:
			lLastError = d._serialPort.Open("COM4");
			break;
		case 5:
			lLastError = d._serialPort.Open("COM5");
			break;
		case 6:
			lLastError = d._serialPort.Open("COM6");
			break;
		case 7:
			lLastError = d._serialPort.Open("COM7");
			break;
		case 8:
			lLastError = d._serialPort.Open("COM8");
			break;
		case 9:
			lLastError = d._serialPort.Open("COM9");
			break;
		default:
			return YARP_FAIL;
			break;
	}
	
	if (lLastError != ERROR_SUCCESS)
		return YARP_FAIL;

	switch(param->baudRate)
	{
		case 110:
			rate = CSerial::EBaud110;
			break;
		case 300:
			rate = CSerial::EBaud300;
			break;
		case 600:
			rate = CSerial::EBaud600;
			break;
		case 1200:
			rate = CSerial::EBaud1200;
			break;
		case 2400:
			rate = CSerial::EBaud2400;
			break;
		case 4800:
			rate = CSerial::EBaud4800;
			break;
		case 9600:
			rate = CSerial::EBaud9600;
			break;
		case 14400:
			rate = CSerial::EBaud14400;
			break;
		case 19200:
			rate = CSerial::EBaud19200;
			break;
		case 38400:
			rate = CSerial::EBaud38400;
			break;
		case 56000:
			rate = CSerial::EBaud56000;
			break;
		case 57600:
			rate = CSerial::EBaud57600;
			break;
		case 115200:
			rate = CSerial::EBaud115200;
			break;
		default:
			return YARP_FAIL;
			break;
	}
	
	lLastError = d._serialPort.Setup(rate,CSerial::EData8,CSerial::EParNone, CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		return YARP_FAIL;
	lLastError = d._serialPort.SetupHandshaking(CSerial::EHandshakeHardware);
	if (lLastError != ERROR_SUCCESS)
		return YARP_FAIL;
	//BLOCKING???
	lLastError = d._serialPort.SetupReadTimeouts(CSerial::EReadTimeoutBlocking);
	if (lLastError != ERROR_SUCCESS)
		return YARP_FAIL;

	resetGlove(NULL);

	return YARP_OK;
}

int YARPCyberGloveDeviceDriver::close (void)
{
	CyberGloveResources& d = RES(system_resources);

	d._serialPort.Close();

	return YARP_OK;
}

void YARPCyberGloveDeviceDriver::Body (void)
{
	CyberGloveResources& d = RES(system_resources);
	DataGloveData* pData = &(d._data);
	
	char c = 'S';
	unsigned char b[23] = {0};
	unsigned long int bytesRead = 0;
	d._serialPort.Write(&c,1);
	
	while (!IsTerminated())	
	{
		//data acquisition
		bytesRead = d._serialPort.Read(&b,1,&bytesRead);
		bytesRead = d._serialPort.Read(&b,23,&bytesRead);
		// Error Handling!!
		d._bmutex.Wait();
		d._fillDataStructure(b, pData);
		d._bmutex.Post();
	}

	resetGlove(NULL);
	d._serialPort.Purge();
	ACE_DEBUG ((LM_DEBUG, "acquisition thread returning...\n"));
}

int YARPCyberGloveDeviceDriver::startStreaming (void *)
{
	CyberGloveResources& d = RES(system_resources);

	d._bStreamStarted = true;

	//add the code to send commad
	Begin ();

	return YARP_OK;
}

int YARPCyberGloveDeviceDriver::stopStreaming (void *)
{
	CyberGloveResources& d = RES(system_resources);

	d._bStreamStarted = true;
	End ();

	return YARP_OK;
}

int YARPCyberGloveDeviceDriver::getData (void *cmd)	// (DataGloveData* data)
{
	CyberGloveResources& d = RES(system_resources);
	struct DataGloveData * data = (struct DataGloveData *)cmd;
	if (d._bStreamStarted)
	{
		d._bmutex.Wait();
		*data = d._data;
		d._bmutex.Post();
	}
	else
	{
		char c = 'G';
		unsigned char b[23];
		unsigned long int bytesRead = 0;
		d._serialPort.Write(&c,1);
		d._serialPort.Read(&b,1,&bytesRead);
		d._serialPort.Read(&b,23,&bytesRead);
		d._serialPort.Purge();
		if(b[22] != 0)
			return YARP_FAIL;
		d._fillDataStructure(b, data);
	}

	if (d._bError)
		return YARP_FAIL;
	else
		return YARP_OK;
}

int YARPCyberGloveDeviceDriver::getSwitch(void *cmd)	// (int* switchStatus)
{
	// TO Do: error handling!

	CyberGloveResources& d = RES(system_resources);
	int * data = (int *)cmd;

	if (d._bStreamStarted)
		return YARP_FAIL;
	unsigned char c[2];
	c[0] = '?';
	c[1] = 'W';
	unsigned char b[4] = { 0 };
	unsigned long bytesRead = 0;

	d._serialPort.Write(&c,2);
	d._serialPort.Read(&b,4,&bytesRead);
	d._serialPort.Purge();
	*data = b[2];

	return YARP_OK;
}

int YARPCyberGloveDeviceDriver::getLed(void *cmd) // (int* ledStatus)
{
	// TO Do: error handling!

	CyberGloveResources& d = RES(system_resources);
	int * data = (int *)cmd;

	if (d._bStreamStarted)
		return YARP_FAIL;
	unsigned char c[2];
	c[0] = '?';
	c[1] = 'L';
	unsigned char b[4] = { 0 };
	unsigned long bytesRead = 0;

	d._serialPort.Write(&c,2);
	d._serialPort.Read(&b,4,&bytesRead);
	d._serialPort.Purge();
	*data = b[2];

	return YARP_OK;
}

int YARPCyberGloveDeviceDriver::setLed(void *cmd) //(int* ledStatus)
{
	// TO Do: error handling!

	CyberGloveResources& d = RES(system_resources);
	int * data = (int *)cmd;

	if (d._bStreamStarted)
		return YARP_FAIL;
	char c[2];
	c[0] = 'L';
	c[1] = *data;
	unsigned char b[2] = { 0 };
	unsigned long bytesRead = 0;

	d._serialPort.Write(&c,2);
	d._serialPort.Read(&b,2,&bytesRead);
	d._serialPort.Purge();

	return YARP_OK;
}
	
int YARPCyberGloveDeviceDriver::resetGlove(void *)
{
	// TO DO: Error Handling
	CyberGloveResources& d = RES(system_resources);
	
	if (d._bStreamStarted)
		return YARP_FAIL;

	unsigned char c[4] = {0};
	c[0] = 18;

	unsigned long bytesRead = 0;
	d._serialPort.Write(c,1);
	d._serialPort.Read(c,4,&bytesRead);
	d._serialPort.Purge();
	return YARP_OK;
}
