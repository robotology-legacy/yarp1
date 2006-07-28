/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *   Based on YARP MirrorCollector by Emmebi (Matteo Brunettini)
 *
 * @file DGSCyberGlove.cpp
 * @brief 
 * @version 1.0
 * @date 25-Jul-06 2:46:24 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSCyberGlove.cpp,v 1.1 2006-07-28 12:41:57 beltran Exp $
 */

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "DGSCyberGlove.h"
#include "DGSTask.h"
#include "DGSErrorCodes.h"
#include<iostream>

using namespace std;

//ccc class CyberGloveResources {
//ccc public:
//ccc 	CyberGloveResources (void) : _bmutex(1) {
//ccc 		_bStreamStarted = false;
//ccc 		_bError = false;
//ccc 	}
//ccc 
//ccc 	~CyberGloveResources () { }
//ccc 
//ccc 	CSerial _serialPort;
//ccc 
//ccc 	DataGloveData _data;
//ccc 
//ccc 	bool _bStreamStarted;
//ccc 	bool _bError;
//ccc 
//ccc 	YARPSemaphore _bmutex;
//ccc 
//ccc 	void _fillDataStructure(unsigned char *inBytes, DataGloveData *destStruct);
//ccc 
//ccc };

/** 
 * CyberGloveResources::_fillDataStructure Fill data structure with bytes read
 * off the serial port.
 * Notes: 
 * (1) inBytes is an array of 24 bytes, but:
 * (2) inBytes[0] must be skipped (it is the echoed 'G' command) and so must
 * be inBytes[23] (it is the terminating 0 character)
 * (3) as a consequence of this, valid data bytes are inBytes[1] to
 * inBytes[22]
 * (4) the manual seems to fail in the description of the data bytes. The
 * correct order is the one reported is this fucntion 
 * 
 * @param inBytes The pointer to the buffer containg the byted read from the
 * serial port.
 * @param destStruct The DataGloveData struct that will accomodate the readed
 * data.
 */
void DGSCyberGlove::fillDataStructure(unsigned char *inBytes, DataGloveData *destStruct)
{
	// ---------- thumb
    destStruct->thumb[0]  = inBytes[1]; // rotation across palm
    destStruct->thumb[1]  = inBytes[2]; // inner phalanx
    destStruct->thumb[2]  = inBytes[3]; // outer phalanx
	// ---------- fingers (inner, middle outer phalanx for each finger)
	// index
	destStruct->index[0]  = inBytes[5];
	destStruct->index[1]  = inBytes[6];
	destStruct->index[2]  = inBytes[7];
	// middle
	destStruct->middle[0] = inBytes[8];
	destStruct->middle[1] = inBytes[9];
	destStruct->middle[2] = inBytes[10];
	// ring
	destStruct->ring[0]   = inBytes[12];
	destStruct->ring[1]   = inBytes[13];
	destStruct->ring[2]   = inBytes[14];
	// pinkie
	destStruct->pinkie[0] = inBytes[16];
	destStruct->pinkie[1] = inBytes[17];
	destStruct->pinkie[2] = inBytes[18];
	// ---------- finger-finger abductions
	destStruct->abduction[0] = inBytes[4];  // thumb-index relative abduction
	destStruct->abduction[1] = inBytes[11]; // index-middle relative abduction
	destStruct->abduction[2] = inBytes[15]; // middle-ring abduction
	destStruct->abduction[3] = inBytes[19]; // ring-pinkie abduction
	// ---------- palm arch
	destStruct->palmArch = inBytes[20];
	// ---------- wrist pitch
	destStruct->wristPitch = inBytes[21];
	// ---------- wrist yaw
	destStruct->wristYaw = inBytes[22];
}

DGSCyberGlove::DGSCyberGlove(void) 
{
}

DGSCyberGlove::~DGSCyberGlove()
{
}

int DGSCyberGlove::open (void *res)
{
	//ccc CyberGloveOpenParameters *param = (CyberGloveOpenParameters*) res;
    //ccc 	CyberGloveResources& d = RES(system_resources);


	///@todo set up 8,N,1 hardware handshaking communciation
	//ccc if ( d._serialPort.Setup(rate,CSerial::EData8,CSerial::EParNone, CSerial::EStop1) != ERROR_SUCCESS ) {
		//ccc return YARP_FAIL;
	//ccc }
	//ccc if ( d._serialPort.SetupHandshaking(CSerial::EHandshakeHardware) != ERROR_SUCCESS ) {
		//ccc return YARP_FAIL;
	//ccc }

	// use blocking method, in order not to lose any data
	//ccc if ( d._serialPort.SetupReadTimeouts(CSerial::EReadTimeoutBlocking) != ERROR_SUCCESS ) {
		//ccc return YARP_FAIL;
	//ccc }

	// reset the glove and bail out
	return resetGlove(NULL);

}

int DGSCyberGlove::close (void)
{
	return 0;
}

int DGSCyberGlove::svc (void)
{
	//ccc CyberGloveResources& d = RES(system_resources);
	
	// gather streamed data
    //ccc unsigned char response[24];
    //ccc 	while ( ! IsTerminated() ) {
    //ccc 		d._serialPort.Read(response,24);
    //ccc 		d._bmutex.Wait();
    //ccc 		d._fillDataStructure(response, &(d._data));
    //ccc 		d._bmutex.Post();
	//ccc }
	return 0;

}

int DGSCyberGlove::startStreaming (void *)
{
	//ccc CyberGloveResources& d = RES(system_resources);
	
	// send streaming command
	unsigned char command1[1] = { 'S' };
	//ccc d._serialPort.Write(command1,1);

	// start thread and set flag accordingly
	activate();
	_bStreamStarted = true;

	return DGS_OK;
}

int DGSCyberGlove::stopStreaming (void *)
{
	//ccc CyberGloveResources& d = RES(system_resources);

	// stop stream gathering thread and set flag accordingly
    ///@todo how do I stop the svc loop?
	////End (); 
	_bStreamStarted = false;

	/// @todo: the "terminate streaming" command, ctrl-c, is sent and processed
	/// correctly, but it is not clear how to gather the response string
	/// (there is still garbage in the serial buffer before the response).
	/// so we ignore the response check and return success.
	/// lines which won't work are commented out.

	// send stream termination command (control-c, ASCII code 3)
	// expect to receive echo of termination command [3,NUL]
	unsigned char command[1] = { 3 };
	//ccc d._serialPort.Purge();
	//ccc d._serialPort.Write(command,1);
	//ccc d._serialPort.Purge();

	return DGS_OK;
}

int DGSCyberGlove::getData (void *cmd)	// (DataGloveData* data)
{
	//ccc CyberGloveResources& d = RES(system_resources);
	struct DataGloveData* data = (struct DataGloveData*)cmd;

	if (_bStreamStarted) {
		// if data is requested for while streaming is active, must
		// ensure we respect the semaphore
        ///@todo Use the necesary ACE mutex.
		//ccc _bmutex.Wait();
		//ccc *data = _data;
		//ccc _bmutex.Post();
	} else {
		// otherwise, send a G command and read values
		unsigned char command[1] = { 'G' };
		unsigned char response[24];
		//ccc d._serialPort.Purge();
		//ccc d._serialPort.Write(command,1);
		//ccc d._serialPort.Read(response,24);
		//ccc d._serialPort.Purge();
		//ccc if ( response[23] != 0 ) {
		//ccc 	return YARP_FAIL;
		//ccc }
		fillDataStructure(response, data);
	}

	if (_bError) {
		return DGS_FAIL;
	} else {
		return DGS_OK;
	}
}

int DGSCyberGlove::getSwitch(void *cmd)	// (int* switchStatus)
{

	//ccc CyberGloveResources& d = RES(system_resources);
	int* data = (int*)cmd;

	if (_bStreamStarted) {
		return DGS_FAIL;
	}

	// send switch status query command and gather the result
	unsigned char command[2] = { '?', 'W' };
	unsigned char response[4];
	//ccc d._serialPort.Purge();
	//ccc d._serialPort.Write(command,2);
	//ccc d._serialPort.Read(response,4);
	//ccc d._serialPort.Purge();
	*data = response[2];

	return DGS_OK;
}

int DGSCyberGlove::getLed(void *cmd) // (int* ledStatus)
{

	//ccc CyberGloveResources& d = RES(system_resources);
	int* data = (int*)cmd;

	if (_bStreamStarted) {
		return DGS_FAIL;
	}

	unsigned char command[2] = { '?', 'L' };
	unsigned char response[4];
	//ccc d._serialPort.Purge();
	//ccc d._serialPort.Write(command,2);
	//ccc d._serialPort.Read(response,4);
	//ccc d._serialPort.Purge();
	*data = response[2];

	return DGS_OK;
}

int DGSCyberGlove::setLed(void *cmd) //(int* ledStatus)
{
	//ccc CyberGloveResources& d = RES(system_resources);
	int* data = (int*)cmd;

	if (_bStreamStarted) {
		return DGS_FAIL;
	}

	unsigned char command[2] = { 'L', 0 }; command[1] = *data;
	unsigned char response[2];
	//ccc d._serialPort.Purge();
	//ccc d._serialPort.Write(command,2);
	//ccc d._serialPort.Read(response,2);
	//ccc d._serialPort.Purge();

	return DGS_OK;

}
	
int DGSCyberGlove::resetGlove(void *)
{

	//CyberGloveResources& d = RES(system_resources);

	// send "restart firmware" command: ASCII 18
	// expect to receive confirmation sequence [18,CR,LF,NUL]
	unsigned char command[1] = { 18 };
	unsigned char response[4];
	unsigned char expected[4] = { 18, 13, 10, 0 };
	//ccc d._serialPort.Purge();
	//ccc d._serialPort.Write(command,1);
	//ccc d._serialPort.Read(response,4);
	//ccc d._serialPort.Purge();
	if ( memcmp(response,expected,4) != 0 ) {
		return DGS_FAIL;
	}

	return DGS_OK;
}
