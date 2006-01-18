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
/// $Id: YARPCyberGloveDeviceDriver.cpp,v 1.4 2006-01-18 11:26:41 claudio72 Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPCyberGloveDeviceDriver.h"
#include <yarp/YARPDataGloveUtils.h>

#include "../dd_orig/include/Serial.h"

#include<iostream>

using namespace std;

class CyberGloveResources {
public:
	CyberGloveResources (void) : _bmutex(1) {
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

	// fill data structure with bytes read off the serial port.

	// NOTES:
	//
	// (1) inBytes is an array of 24 bytes, but:
	// (2) inBytes[0] must be skipped (it is the echoed 'G' command) and so
	//     must be inBytes[23] (it is the terminating 0)
	// (3) the index abduction is not yet implemented in the hardware
	//     (cfr. the dataglove user manual, table 1, page 20) so we follow
	//     the manual's suggestion and fill it with the middle-index abduction.
	//     this is why destStruct->abduction[1]=inBytes[11] below
	// (4) as a consequence of this, valid data bytes are inBytes[1] to inBytes[22]

	// Thumb
	destStruct->thumb[0] = inBytes[1];
	destStruct->thumb[1] = inBytes[2];
	destStruct->thumb[2] = inBytes[3];
	destStruct->abduction[0] = inBytes[4];
	// Index
	destStruct->index[0] = inBytes[5];
	destStruct->index[1] = inBytes[6];
	destStruct->index[2] = inBytes[7];
	destStruct->abduction[1] = inBytes[11];
	// Middle
	destStruct->middle[0] = inBytes[8];
	destStruct->middle[1] = inBytes[9];
	destStruct->middle[2] = inBytes[10];
	destStruct->abduction[2] = inBytes[11]; // not yet implemented in the glove
	// Ring
	destStruct->ring[0] = inBytes[12];
	destStruct->ring[1] = inBytes[13];
	destStruct->ring[2] = inBytes[14];
	destStruct->abduction[3] = inBytes[15];
	// Pinkie
	destStruct->pinkie[0] = inBytes[16];
	destStruct->pinkie[1] = inBytes[17];
	destStruct->pinkie[2] = inBytes[18];
	destStruct->abduction[4] = inBytes[19];
	// Palm arhc
	destStruct->palmArch = inBytes[20];
	// Wrist pitch
	destStruct->wrist[0] = inBytes[21];
	// Wrist yaw
	destStruct->wrist[1] = inBytes[22];

}

inline CyberGloveResources& RES(void *res) { return *(CyberGloveResources *)res; }

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

	if (system_resources != NULL) {
		delete (CyberGloveResources *)system_resources;
	}

	system_resources = NULL;

}

int YARPCyberGloveDeviceDriver::open (void *res)
{

	CyberGloveOpenParameters *param = (CyberGloveOpenParameters*) res;
	CyberGloveResources& d = RES(system_resources);

	CSerial::EBaudrate rate;
	LONG lLastError;

	switch ( param->comPort ) {
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
	
	if (lLastError != ERROR_SUCCESS) {
		return YARP_FAIL;
	}

	switch( param->baudRate ) {
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

	// set up 8,N,1 hardware handshaking communciation
	if ( d._serialPort.Setup(rate,CSerial::EData8,CSerial::EParNone, CSerial::EStop1) != ERROR_SUCCESS ) {
		return YARP_FAIL;
	}
	if ( d._serialPort.SetupHandshaking(CSerial::EHandshakeHardware) != ERROR_SUCCESS ) {
		return YARP_FAIL;
	}

	// use blocking method, in order not to lose any data
	if ( d._serialPort.SetupReadTimeouts(CSerial::EReadTimeoutBlocking) != ERROR_SUCCESS ) {
		return YARP_FAIL;
	}

	// reset the glove and bail out
	return resetGlove(NULL);

}

int YARPCyberGloveDeviceDriver::close (void)
{

	CyberGloveResources& d = RES(system_resources);

	if ( d._serialPort.Close() != ERROR_SUCCESS ) {
		return YARP_FAIL;
	}

	return YARP_OK;

}

void YARPCyberGloveDeviceDriver::Body (void)
{

	CyberGloveResources& d = RES(system_resources);
	
	// gather streamed data
	unsigned char response[24];
	while ( ! IsTerminated() ) {
		d._serialPort.Read(response,24);
		d._bmutex.Wait();
		d._fillDataStructure(response, &(d._data));
		d._bmutex.Post();
	}

}

int YARPCyberGloveDeviceDriver::startStreaming (void *)
{

	CyberGloveResources& d = RES(system_resources);
	
	// send streaming command
	unsigned char command1[1] = { 'S' };
	d._serialPort.Write(command1,1);

	// start thread and set flag accordingly
	Begin ();
	d._bStreamStarted = true;

	return YARP_OK;

}

int YARPCyberGloveDeviceDriver::stopStreaming (void *)
{

	CyberGloveResources& d = RES(system_resources);

	// stop stream gathering thread and set flag accordingly
	End ();
	d._bStreamStarted = false;

	// FIXUP: the "terminate streaming" command, ctrl-c, is sent and processed
	// correctly, but it is not clear how to gather the response string
	// (there is still garbage in the serial buffer before the response).
	// so we ignore the response check and return success.
	// lines which won't work are commented out.

	// send stream termination command (control-c, ASCII code 3)
	// expect to receive echo of termination command [3,NUL]
	unsigned char command[1] = { 3 };
//	unsigned char response[2];
//	unsigned char expected[2] = { 3, 0 };
	d._serialPort.Purge();
	d._serialPort.Write(command,1);
//	d._serialPort.Read(response,2);
	d._serialPort.Purge();
//	if ( memcmp(response,expected,2) != 0 ) {
//		return YARP_FAIL;
//	}

	return YARP_OK;

}

int YARPCyberGloveDeviceDriver::getData (void *cmd)	// (DataGloveData* data)
{

	CyberGloveResources& d = RES(system_resources);
	struct DataGloveData* data = (struct DataGloveData*)cmd;

	if (d._bStreamStarted) {
		// if data is requested for while streaming is active, must
		// ensure we respect the semaphore
		d._bmutex.Wait();
		*data = d._data;
		d._bmutex.Post();
	} else {
		// otherwise, send a G command and read values
		unsigned char command[1] = { 'G' };
		unsigned char response[24];
		d._serialPort.Purge();
		d._serialPort.Write(command,1);
		d._serialPort.Read(response,24);
		d._serialPort.Purge();
		if ( response[23] != 0 ) {
			return YARP_FAIL;
		}
		d._fillDataStructure(response, data);
	}

	if (d._bError) {
		return YARP_FAIL;
	} else {
		return YARP_OK;
	}

}

int YARPCyberGloveDeviceDriver::getSwitch(void *cmd)	// (int* switchStatus)
{

	CyberGloveResources& d = RES(system_resources);
	int* data = (int*)cmd;

	if (d._bStreamStarted) {
		return YARP_FAIL;
	}

	// send switch status query command and gather the result
	unsigned char command[2] = { '?', 'W' };
	unsigned char response[4];
	d._serialPort.Purge();
	d._serialPort.Write(command,2);
	d._serialPort.Read(response,4);
	d._serialPort.Purge();
	*data = response[2];

	return YARP_OK;

}

int YARPCyberGloveDeviceDriver::getLed(void *cmd) // (int* ledStatus)
{

	CyberGloveResources& d = RES(system_resources);
	int* data = (int*)cmd;

	if (d._bStreamStarted) {
		return YARP_FAIL;
	}

	unsigned char command[2] = { '?', 'L' };
	unsigned char response[4];
	d._serialPort.Purge();
	d._serialPort.Write(command,2);
	d._serialPort.Read(response,4);
	d._serialPort.Purge();
	*data = response[2];

	return YARP_OK;

}

int YARPCyberGloveDeviceDriver::setLed(void *cmd) //(int* ledStatus)
{

	CyberGloveResources& d = RES(system_resources);
	int* data = (int*)cmd;

	if (d._bStreamStarted) {
		return YARP_FAIL;
	}

	unsigned char command[2] = { 'L', 0 }; command[1] = *data;
	unsigned char response[2];
	d._serialPort.Purge();
	d._serialPort.Write(command,2);
	d._serialPort.Read(response,2);
	d._serialPort.Purge();

	return YARP_OK;

}
	
int YARPCyberGloveDeviceDriver::resetGlove(void *)
{

	CyberGloveResources& d = RES(system_resources);

	// send "restart firmware" command: ASCII 18
	// expect to receive confirmation sequence [18,CR,LF,NUL]
	unsigned char command[1] = { 18 };
	unsigned char response[4];
	unsigned char expected[4] = { 18, 13, 10, 0 };
	d._serialPort.Purge();
	d._serialPort.Write(command,1);
	d._serialPort.Read(response,4);
	d._serialPort.Purge();
	if ( memcmp(response,expected,4) != 0 ) {
		return YARP_FAIL;
	}

	return YARP_OK;

}
