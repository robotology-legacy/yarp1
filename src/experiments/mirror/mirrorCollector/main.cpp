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
/// $Id: main.cpp,v 1.23 2006-12-16 11:44:18 babybot Exp $
///
///

// ---------- headers

#include <iostream>
#include <conio.h>

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPTime.h>

#include <yarp/YARPRobotHardware.h>

#include <yarp/YARPThread.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPConfigFile.h>

#include "CollectorCommands.h"

using namespace std;

// ---------- procedures prototypes
bool connectSensors(void);
void releaseSensors(void);
void acquireAndSend(void);
void getOptionsFromEnv(char*);
void registerPorts(void);
void unregisterPorts(void);
int main (int, char **);

// ---------- YARP communication ports

// data to be sent on ports
YARPOutputPortOf<CollectorNumericalData> _data_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<YARPGenericImage> _img0_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<YARPGenericImage> _img1_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// channel for commands
// to improve synchronous communication, write(true) on output port
YARPOutputPortOf<int> _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// to improve synchronous communication, NO_BUFFERS
YARPInputPortOf<int>  _cmd_inport (YARPInputPort::NO_BUFFERS, YARP_TCP);

// ---------- global types

// what hardware is possibly attached to this setup?
typedef struct {
	YARPGrabber			grabber0;
	YARPGrabber			grabber1;
	YARPMagneticTracker	tracker0;
	YARPMagneticTracker	tracker1;
	YARPDataGlove		glove;
	YARPPresSens		press;
	YARPGazeTracker		gt;
} CollectorHardware;

// options
typedef struct CollectorOptionsStruct {
	CollectorOptionsStruct() {
		portName			= "mirrorCollector";
		netName				= "default";
		imgNetName			= "default";
		streamingFreq		= 25.0;
		useDataGlove		= false;
		gloveComPort		= 3;
		gloveBaudRate		= 115200;
		useTracker0			= false;
		tracker0ComPort		= 2;
		tracker0BaudRate	= 115200;
		tracker0MeasRate	= 103.3;
		tracker0TransOpMode	= 2;
		tracker0Timeout		= 160;
		useTracker1			= false;
		tracker1ComPort		= 4;
		tracker1BaudRate	= 115200;
		tracker1MeasRate	= 103.3;
		tracker1TransOpMode	= 2;
		tracker1Timeout		= 160;
		usePresSens			= false;
		nPresSens			= 2;
		useCamera0			= false;
		useCamera1			= false;
		sizeX				= 384;
		sizeY				= 272;
		yOffset				= 0;
		useGazeTracker		= false;
		GTComPort			= 1;
		GTBaudRate			= 57600;
	};
	YARPString portName;
	YARPString netName;
	YARPString imgNetName;
	double streamingFreq;
	bool useCamera0;
	bool useCamera1;
	int	sizeX;
	int sizeY;
	int yOffset;
	bool useTracker0;
	short tracker0ComPort;
	int	tracker0BaudRate;
	double tracker0MeasRate;
	int	tracker0TransOpMode;
	int	tracker0Timeout;
	bool useTracker1;
	short tracker1ComPort;
	int	tracker1BaudRate;
	double tracker1MeasRate;
	int	tracker1TransOpMode;
	int	tracker1Timeout;
	bool useDataGlove;
	short gloveComPort;
	int	gloveBaudRate;
	bool usePresSens;
	short nPresSens;
	bool useGazeTracker;
	short GTComPort;
	int GTBaudRate;

} CollectorOptions;

// ---------- global variables

// the hardware and its options
CollectorHardware      _hardware;
CollectorOptions       _options;
// the data: (a) numerical (pressure, glove, tracker), (b) images from the cameras
CollectorNumericalData _data;
CollectorImage         _img0;
CollectorImage         _img1;

// the default options file name
char* CollectorConfFileName = "Y:\\conf\\mirrorCollector.conf";

// ---------- streaming thread

class streamingThread : public YARPThread {
public:

	virtual void Body (void) {

		// circular buffer to evaluate mean streaming frequency
		_elapsedTop = 0;
		_previous = 0.0;

		// streaming interval
		_streamInterval = 1/_options.streamingFreq;

		// put those sensors which do have a streaming mode into streaming mode
		if (_options.useDataGlove) _hardware.glove.startStreaming();
		if (_options.useTracker0) _hardware.tracker0.startStreaming();
		if (_options.useTracker1) _hardware.tracker1.startStreaming();

		// stream until terminated
		while ( !IsTerminated() ) {

			// acquire and send data
			acquireAndSend();

			// evaluate elapsed time since last loop
            _now = YARPTime::GetTimeAsSecondsHr();
            _elapsed[_elapsedTop] = _now - _previous;
            _previous = _now;

			// insert appropriate delay
			if ( _elapsed[_elapsedTop] < _streamInterval ) {
				// no idea why, need a correction factor (1.7)
				YARPTime::DelayInSeconds( _streamInterval*1.7 - _elapsed[_elapsedTop] );
			}

			// show average loop time, over 5 runs
			if ( ++_elapsedTop == 5 ) _elapsedTop = 0;
			_meanElapsed = ( _elapsed[0] + _elapsed[1] + _elapsed[2] + _elapsed[3] + _elapsed[4] )/5;
			cout << "mean interval " << _meanElapsed << " (should be " << _streamInterval << ")       \r";

		}

		// stop streaming mode for some sensors
		if (_options.useTracker1) _hardware.tracker1.stopStreaming();
		if (_options.useTracker0) _hardware.tracker0.stopStreaming();
		if (_options.useDataGlove) _hardware.glove.stopStreaming();

		// bail out
		return;

	}

private:
	double _streamInterval, _now, _previous;
	// 5 sample intervals to evaluate a reasonable average
	double _meanElapsed, _elapsed[5];
	int _elapsedTop;
};

// ---------- functions

void acquireAndSend(void)
{

	// acquire data

	if (_options.useCamera0) {
		unsigned char *buffer = NULL;
		_hardware.grabber0.waitOnNewFrame();
		_hardware.grabber0.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img0.GetRawBuffer(), buffer, _options.sizeX * _options.sizeY * 3);
		_hardware.grabber0.releaseBuffer();

		_img0_outport.Content().Refer(_img0);
		_img0_outport.Write();
	}

	if (_options.useCamera1) {
		unsigned char *buffer = NULL;
		_hardware.grabber1.waitOnNewFrame();
		_hardware.grabber1.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img1.GetRawBuffer(), buffer, _options.sizeX * _options.sizeY * 3);
		_hardware.grabber1.releaseBuffer();

		_img1_outport.Content().Refer(_img1);
		_img1_outport.Write();
	}

	if (_options.useTracker0) {
		// Read Tracker
		_hardware.tracker0.getData(&_data.tracker0Data);
	}

	if (_options.useTracker1) {
		_hardware.tracker1.getData(&_data.tracker1Data);
	}
	
	if (_options.useGazeTracker) {
		// Read GT
		_hardware.gt.getData(&_data.GTData);
	}
	
	if (_options.useDataGlove) {
		// Read DataGlove
		_hardware.glove.getData(&_data.gloveData);
	}
	
	if (_options.usePresSens) {
		// Read Pressure Sensors
		_hardware.press.getData(&_data.pressureData);
	}

	if ( _options.useGazeTracker || _options.useDataGlove ||
		 _options.usePresSens || _options.useTracker0 || _options.useTracker1 ) {
		// if it is the case, send numerical data
		_data_outport.Content() = _data;
		_data_outport.Write();
	}

}

bool connectSensors(void)
{

	// try and connect all peripherals requested (look at _options)
	// if all of them fail to initialise, return failure; otherwise, ok, but
	// disable peripherals which did not initialise.

	bool atLeastOneIsOK = false;

	if (_options.useCamera0) {
		// Framegrabber Initialization
		cout << "Initialising camera #0... ";
		if ( _hardware.grabber0.initialize (0, _options.sizeX, _options.sizeY) == YARP_OK ) {
			cout <<  "done. W=" << _options.sizeX << ", H=" << _options.sizeY <<endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.useCamera0 = false;
		}
	}

	if (_options.useCamera1) {
		// Framegrabber Initialization
		cout << "Initialising camera #1... ";
		if ( _hardware.grabber1.initialize (1, _options.sizeX, _options.sizeY) == YARP_OK ) {
			cout <<  "done. W=" << _options.sizeX << ", H=" << _options.sizeY <<endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.useCamera1 = false;
		}
	}

	if (_options.useTracker0) {
		// Tracker Initialization
		cout << "Initialising tracker #0... ";
		FoBOpenParameters FoBparams;
//		FoBparams.nGroupID = 0;
		FoBparams.comPort = _options.tracker0ComPort;
		FoBparams.baudRate = _options.tracker0BaudRate;
		FoBparams.timeOut = _options.tracker0Timeout;
		FoBparams.measurementRate = _options.tracker0MeasRate;
		FoBparams.transOpMode = _options.tracker0TransOpMode;
		if ( _hardware.tracker0.initialize(FoBparams) == YARP_OK ) {
			cout <<  "done. On COM" << _options.tracker0ComPort << ", " << _options.tracker0BaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.useTracker0 = false;
		}
	}
	
	if (_options.useTracker1) {
		// Tracker Initialization
		cout << "Initialising tracker #1... ";
		FoBOpenParameters FoBparams;
//		FoBparams.nGroupID = 0;
		FoBparams.comPort = _options.tracker1ComPort;
		FoBparams.baudRate = _options.tracker1BaudRate;
		FoBparams.timeOut = _options.tracker1Timeout;
		FoBparams.measurementRate = _options.tracker1MeasRate;
		FoBparams.transOpMode = _options.tracker1TransOpMode;
		if ( _hardware.tracker1.initialize(FoBparams) == YARP_OK ) {
			cout <<  "done. On COM" << _options.tracker1ComPort << ", " << _options.tracker1BaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.useTracker1 = false;
		}
	}
	
	if (_options.useDataGlove) {
		// DataGlove Initialization
		cout << "Initialising DataGlove... ";
		if ( _hardware.glove.initialize (_options.gloveComPort, _options.gloveBaudRate) == YARP_OK ) {
			cout <<  "done. On COM" << _options.gloveComPort << ", " << _options.gloveBaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.useDataGlove = false;
		}
	}
	
	if (_options.useGazeTracker) {
		// GT Initialization
		cout << "Initialising GazeTracker... ";
		E504OpenParameters params;
		params.baudRate = _options.GTBaudRate;
		params.comPort = _options.GTComPort;
		if ( _hardware.gt.initialize (params) == YARP_OK ) {
			cout <<  "done. On COM" << _options.GTComPort << ", " << _options.GTBaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.useGazeTracker = false;
		}
	}
	
	if (_options.usePresSens) {
		// PresSensors Initialization
		cout << "Initialising pressure sensors... ";
		if ( _hardware.press.initialize (_options.nPresSens) == YARP_OK ) {
			cout <<  "done. " << _options.nPresSens << " sensor(s) connected." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_options.usePresSens = false;
		}
	}

	return atLeastOneIsOK;

}

void releaseSensors(void)
{

	if (_options.useCamera0) {
		// Framegrabber 0
		cout << "Releasing camera 0... ";  
		_hardware.grabber0.uninitialize ();
		cout << "done." << endl;
	}
	
	if (_options.useCamera1) {
		// Framegrabber 1
		cout << "Releasing camera 1... ";  
		_hardware.grabber1.uninitialize ();
		cout << "done." << endl;
	}
	
	if (_options.useTracker0) {
		// Tracker 
		cout << "Releasing tracker #0... ";
		_hardware.tracker0.uninitialize ();
		cout << "done." << endl;
	}

	if (_options.useTracker1) {
		// Tracker 
		cout << "Releasing tracker #1... ";
		_hardware.tracker1.uninitialize ();
		cout << "done." << endl;
	}

	if (_options.useDataGlove) {
		// DataGlove 
		cout << "Releasing DataGlove... ";
		_hardware.glove.uninitialize ();
		cout << "done." << endl;
	}
	
	if (_options.useGazeTracker) {
		// GT
		cout << "Releasing GazeTracker... ";
		_hardware.gt.uninitialize ();
		cout << "done." << endl;
	}
	
	if (_options.usePresSens) {
		// Pressure Sensors
		cout << "Releasing pressure sensors... ";
		_hardware.press.uninitialize ();
		cout << "done." << endl;
	}

}

void getOptionsFromEnv(char* fileName)
{

	// read specific options from conf file
	YARPConfigFile optFile;
	// network specs
	optFile.setName(fileName);
	char buf[255];
	if ( optFile.getString("[NETWORK]", "PortName", buf) == YARP_OK) {
		_options.portName = buf;
	}
	if ( optFile.getString("[NETWORK]", "NetName", buf) == YARP_OK) {
		_options.netName = buf;
	}
	if ( optFile.getString("[NETWORK]", "ImgNetName", buf) == YARP_OK) {
		_options.imgNetName = buf;
	}
	optFile.get("[NETWORK]", "StreamFreq", &_options.streamingFreq);
	// glove
	int yesNo;
	optFile.get("[HARDWARE]", "UseDataGlove", &yesNo); _options.useDataGlove = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "GloveComPort", &_options.gloveComPort);
	optFile.get("[HARDWARE]", "GloveBaudRate", &_options.gloveBaudRate);
	// GT
	optFile.get("[HARDWARE]", "UseGazeTracker", &yesNo); _options.useGazeTracker = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "GTComPort", &_options.GTComPort);
	optFile.get("[HARDWARE]", "GTBaudRate", &_options.GTBaudRate);
	// trackers
	optFile.get("[HARDWARE]", "UseTracker0", &yesNo); _options.useTracker0 = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "Tracker0ComPort", &_options.tracker0ComPort);
	optFile.get("[HARDWARE]", "Tracker0BaudRate", &_options.tracker0BaudRate);
	optFile.get("[HARDWARE]", "Tracker0MeasRate", &_options.tracker0MeasRate);
	optFile.get("[HARDWARE]", "Tracker0TransOpMode", &_options.tracker0TransOpMode);
	optFile.get("[HARDWARE]", "Tracker0Timeout", &_options.tracker0Timeout);
	optFile.get("[HARDWARE]", "UseTracker1", &yesNo); _options.useTracker1 = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "Tracker1ComPort", &_options.tracker1ComPort);
	optFile.get("[HARDWARE]", "Tracker1BaudRate", &_options.tracker1BaudRate);
	optFile.get("[HARDWARE]", "Tracker1MeasRate", &_options.tracker1MeasRate);
	optFile.get("[HARDWARE]", "Tracker1TransOpMode", &_options.tracker1TransOpMode);
	optFile.get("[HARDWARE]", "Tracker1Timeout", &_options.tracker1Timeout);
	// pressure sensors
	optFile.get("[HARDWARE]", "UsePresSens", &yesNo); _options.usePresSens = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "NPresSens", &_options.nPresSens);
	// cameras
	optFile.get("[HARDWARE]", "UseCamera0", &yesNo); _options.useCamera0 = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "UseCamera1", &yesNo); _options.useCamera1 = (yesNo ? true : false);
	optFile.get("[HARDWARE]", "ImageSizeX", &_options.sizeX);
	optFile.get("[HARDWARE]", "ImageSizeY", &_options.sizeY);
	optFile.get("[HARDWARE]", "YOffSet", &_options.yOffset);

	if (_options.sizeX > 384) {
		cout << "*** WARNING: max width is 384 - forced to 384." << endl;
		_options.sizeX = 384;
	}

	if (_options.sizeY > 272) {
		cout << "*** WARNING: max height is 272 - forced to 272." << endl;
		_options.sizeY = 272;
	}


}

void registerPorts(void)
{

	char buf[256];

	// register data port
	cout << "Registering data port... " << endl;
	ACE_OS::sprintf(buf, "/%s/o:str", _options.portName.c_str());
	if ( _data_outport.Register(buf,_options.netName.c_str()) != YARP_OK) {
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "done." << endl;
	}
	
	// register images ports
	cout << "Registering camera port 0... " << endl;
	ACE_OS::sprintf(buf, "/%s/o:img0", _options.portName.c_str());
	if ( _img0_outport.Register(buf,_options.imgNetName.c_str()) != YARP_OK) {
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "Done." << endl;
	}
	cout << "Registering camera port 1... " << endl;
	ACE_OS::sprintf(buf, "/%s/o:img1", _options.portName.c_str());
	if ( _img1_outport.Register(buf,_options.imgNetName.c_str()) != YARP_OK) {
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "Done." << endl;
	}
	
	// register commands port
	cout << "Registering commands port... " << endl;
	ACE_OS::sprintf(buf, "/%s/i:int", _options.portName.c_str());
	if ( _cmd_inport.Register(buf,_options.netName.c_str()) != YARP_OK) {
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "Done." << endl;
	}

	// register response port
	cout << "Registering response port... " << endl;
	ACE_OS::sprintf(buf, "/%s/o:int", _options.portName.c_str());
	if ( _cmd_outport.Register(buf,_options.netName.c_str()) != YARP_OK)	{
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "Done." << endl;
	}

	// this is for safety reasons. sometimes shared memory does not work.
	_cmd_outport.SetAllowShmem(0);
	_data_outport.SetAllowShmem(0);
	_img0_outport.SetAllowShmem(0);
	_img1_outport.SetAllowShmem(0);

}

void unregisterPorts(void)
{

	_cmd_inport.Unregister();
	_cmd_outport.Unregister();
	_data_outport.Unregister();
	_img0_outport.Unregister();
	_img1_outport.Unregister();

}

// ---------- main

int main (int argc, char *argv[])
{

	// preliminaries

	cout.precision(3); cout.setf(ios::fixed);
	YARPScheduler::setHighResScheduling();

	getOptionsFromEnv(CollectorConfFileName);

	_img0.Resize (_options.sizeX, _options.sizeY);
	_img1.Resize (_options.sizeX, _options.sizeY);

	registerPorts();

	cout << "Now waiting for commands." << endl;

	// ------------ main command parsing loop

	bool bQuit = false;
	bool bStreaming = false;
	bool bConnected = false;
	streamingThread stream;
		
	do {

		while ( ! _cmd_inport.Read(false) ) {

			// if no command has arrived thru the command port, check the keyboard
			if ( _kbhit() ) {
				switch ( _getch() ) {
				case 'r':
					// RESET: if streaming, stop it, then if connected, disconnect sensors
					if ( bStreaming ) {
						stream.End();
						bStreaming = false;
						cout << endl << "No longer streaming." << endl;
					}
					if ( bConnected ) {
						releaseSensors();
						bConnected = false;
						cout << endl << "Disconnected." << endl;
					}
					break;
				case 'q':
					// QUIT: unregister ports and bail out
					if ( bConnected ) {
						cout << endl << "Reset me first ('r')." << endl;
					} else {
						bConnected = false;
						bQuit = true;
						cout << endl << "Quitting." << endl;
						unregisterPorts();
						return 0;
					}
					break;
				default:
					// unrecognised keyboard command
					cout << "Unrecognised keyboard command." << endl;
					cout << "'r' (stop streaming and) disconnect sensors" << endl;
					cout << "'q' quit" << endl;
					cout << endl;
					break;
				}
			}

			// in order not to suck the CPU up, sleep .1 seconds at every cycle. this does not
			// affect the actual streaming frequency, which is set in the streaming thread.
			YARPTime::DelayInSeconds(0.1);

		}

		switch( _cmd_inport.Content() ) {

		case CCmdConnect:
			// connect peripherals
			if ( ! bConnected ) {
				if ( connectSensors() ) {
					// if connectSensors returns true, at least one sensor has been
					// connected. the fields _options.use* tell us which sensors are on.
					bConnected = true;
					// signal that connection was successful
					_cmd_outport.Content() = CCmdSucceeded;
					_cmd_outport.Write(true);
					// tell the client what hardware we use
					int tmpOptions = 0;
					if ( _options.useDataGlove )	tmpOptions |= HardwareUseDataGlove;
					if ( _options.useGazeTracker )	tmpOptions |= HardwareUseGT;
					if ( _options.useTracker0 )		tmpOptions |= HardwareUseTracker0;
					if ( _options.useTracker1 )		tmpOptions |= HardwareUseTracker1;
					if ( _options.usePresSens )		tmpOptions |= HardwareUsePresSens;
					if ( _options.useCamera0 ) 		tmpOptions |= HardwareUseCamera0;
					if ( _options.useCamera1 )		tmpOptions |= HardwareUseCamera1;
					_cmd_outport.Content() = tmpOptions;
					_cmd_outport.Write(true);
					// store and send size of the images
					if ( _options.useCamera0 || _options.useCamera1 ) {
						_cmd_outport.Content() = _options.sizeX;
						_cmd_outport.Write(true);
						_cmd_outport.Content() = _options.sizeY;
						_cmd_outport.Write(true);
					}
					cout << endl << "Connect succeeded." << endl;
				} else {
					_cmd_outport.Content() = CCmdFailed;
					_cmd_outport.Write(true);
					cout << endl << "Connect failed." << endl;
				}
			} else {
				_cmd_outport.Content() = CCmdFailed;
				_cmd_outport.Write(true);
				cout << endl << "Connect failed." << endl;
			}
			break;

		case CCmdDisconnect:
			// Disconnect peripherals
			if ( bConnected ) {
				releaseSensors();
				_cmd_outport.Content() = CCmdSucceeded;
				_cmd_outport.Write(true);
				bConnected = false;
				cout << endl << "Disconnect succeeded." << endl;
			} else {
				_cmd_outport.Content() = CCmdFailed;
				_cmd_outport.Write(true);
				cout << endl << "Disconnect failed." << endl;
			}
			break;

		case CCmdGetData:
			// gather data off peripherals (need to be connected, obviously)
			if ( bConnected ) {
				_cmd_outport.Content() = CCmdSucceeded;
				_cmd_outport.Write(true);
				// acquire data and send
				acquireAndSend();
			} else {
				_cmd_outport.Content() = CCmdFailed;
				_cmd_outport.Write(true);
				cout << endl << "Getdata failed." << endl;
			}
			break;

		case CCmdStartStreaming:
			// start streaming thread
			if ( !bStreaming && bConnected ) {
				stream.Begin();
				bStreaming = true;
				_cmd_outport.Content() = CCmdSucceeded;
				_cmd_outport.Write(true);
				cout << endl << "StartStreaming succeeded - now streaming." << endl;
			} else {
				_cmd_outport.Content() = CCmdFailed;
				_cmd_outport.Write(true);
				cout << endl << "StartStreaming failed (must be connected and not streaming)." << endl;
			}
			break;

		case CCmdStopStreaming:
			// start streaming thread
			if ( bStreaming ) {
				stream.End();
				bStreaming = false;
				_cmd_outport.Content() = CCmdSucceeded;
				_cmd_outport.Write(true);
				cout << endl << "StopStreaming succeeded - no longer streaming." << endl;
			} else {
				_cmd_outport.Content() = CCmdFailed;
				_cmd_outport.Write(true);
				cout << endl << "StopStreaming failed (must be streaming)." << endl;
			}
			break;

		case CCmdQuit:
			// quit (need to disconnect first)
			if ( bConnected ) {
				_cmd_outport.Content() = CCmdFailed;
				_cmd_outport.Write(true);
				cout << endl << "Quit failed (must disconnect first)." << endl;
			} else {
				bConnected = false;
				bQuit = true;
				_cmd_outport.Content() = CCmdSucceeded;
				_cmd_outport.Write(true);
				cout << endl << "Quit succeeded." << endl;
			}
			break;

		default:
			// what the hell happened on that port?
			cout << endl << "Received unknown command on command port." << endl;
			break;

		} // switch

	} while ( !bQuit );

	cout << "Bailing out." << endl;
	unregisterPorts();
	return 0;

}
