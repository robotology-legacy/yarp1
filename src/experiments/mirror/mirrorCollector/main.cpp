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
/// $Id: main.cpp,v 1.3 2006-01-04 09:59:18 claudio72 Exp $
///
///

// ---------- headers

#include <iostream>
#include <stdio.h>

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

double elapsedTime;

// ---------- procedures prototypes
void sendHelp(void);
bool connectSensors(void);
void releaseSensors(void);
void initData(void);
void acquireAndSend(void);
void setOptions(char*);
void registerPorts(void);
void unregisterPorts(void);
int main (int, char **);

// ---------- YARP communication ports

YARPOutputPortOf<MNumData> _data_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<YARPGenericImage> _img0_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<YARPGenericImage> _img1_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<int> _rep_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);

YARPInputPortOf<MCommands> _cmd_inport;

// ---------- global types

// what hardware do we possibly use? (defined onthefly in mirrorCollector.conf)
struct CollectorHardware {
	YARPGrabber			grabber0;
	YARPGrabber			grabber1;
	YARPMagneticTracker	tracker0;
	YARPMagneticTracker	tracker1;
	YARPDataGlove		glove;
	YARPPresSens		press;
};

// hardware options
struct CollectorOptions {
	YARPString portName;
	YARPString netName;
	int useDataGlove;
	short gloveComPort;
	int	gloveBaudRate;
	int useTracker0;
	short tracker0ComPort;
	int	tracker0BaudRate;
	double tracker0TransRate;
	int	tracker0Timeout;
	int useTracker1;
	short tracker1ComPort;
	int	tracker1BaudRate;
	double tracker1TransRate;
	int	tracker1Timeout;
	int usePresSens;
	short nPresSens;
	int useCamera0;
	int useCamera1;
	int	sizeX;
	int sizeY;
	int yOffset;
};

// ---------- global variables

// the hardware and its options
struct CollectorOptions   _options;
struct CollectorHardware  _hardware;
// the data: (a) numerical (pressure, glove, tracker), (b) images from the cameras
MNumData                  _data;
YARPImageOf<YarpPixelBGR> _img0;
YARPImageOf<YarpPixelBGR> _img1;

// the default options file name (may be ovverridden by --file)
char* confFileName="C:\\yarp\\src\\experiments\\mirror\\mirrorCollector\\mirrorCollector.conf";

// the streaming thread frequency
const double streamingFrequency = 1.0/25.0;

// ---------- streaming thread

class streamingThread : public YARPThread {

public:

	virtual void Body (void) {

		// start streaming peripherals

		if (_options.useDataGlove) {
			_hardware.glove.startStreaming();
		}

		if (_options.useTracker0) {
			_hardware.tracker0.startStreaming();
		}

		if (_options.useTracker1) {
			_hardware.tracker1.startStreaming();
		}

		// stream until terminated

		while ( !IsTerminated() ) {
			YARPTime::DelayInSeconds(streamingFrequency);
			acquireAndSend();
		}

		// stop streaming peripherals

		if (_options.useDataGlove) {
			_hardware.glove.stopStreaming();
		}

		if (_options.useTracker0) {
			_hardware.tracker0.stopStreaming();
		}
		
		if (_options.useTracker1) {
			_hardware.tracker1.stopStreaming();
		}
		
		return;
	}

};

// ---------- functions

void sendHelp(void)
{

	// output a help message

	cout << endl << "*** Mirror setup acquisition thread ***" << endl  << endl;
	cout << "USAGE: collector.exe [parameters]" << endl;
	cout << "parameters are:" << endl;
	cout << "\t--help            this help screen." << endl;
	cout << "\t--file filename   use option file <filename>." <<endl;

}

bool connectSensors(void)
{

	if (_options.useCamera0) {
		// Framegrabber Initialization
		cout << "Initialising camera #0... ";
		_hardware.grabber0.initialize (0, _options.sizeX, _options.sizeY);
		cout <<  "done. W=" << _options.sizeX << ", H=" << _options.sizeY <<endl;
	}

	if (_options.useCamera1) {
		// Framegrabber Initialization
		cout << "Initialising camera #1... ";
		_hardware.grabber1.initialize (1, _options.sizeX, _options.sizeY);
		cout <<  "done. W=" << _options.sizeX << ", H=" << _options.sizeY <<endl;
	}

	if (_options.useTracker0) {
		// Tracker Initialization
		cout << "Initialising tracker #0... ";
		_hardware.tracker0.initialize (_options.tracker0ComPort, _options.tracker0BaudRate, _options.tracker0Timeout);
		cout <<  "done. On COM" << _options.tracker0ComPort << ", " << _options.tracker0BaudRate << " baud." << endl;
	}
	
	if (_options.useTracker1) {
		// Tracker Initialization
		cout << "Initialising tracker #1... ";
		_hardware.tracker1.initialize (_options.tracker1ComPort, _options.tracker1BaudRate, _options.tracker1Timeout);
		cout <<  "done. On COM" << _options.tracker1ComPort << ", " << _options.tracker1BaudRate << " baud." << endl;
	}
	
	if (_options.useDataGlove) {
		// DataGlove Initialization
		cout << "Initialising DataGlove... ";
		_hardware.glove.initialize (_options.gloveComPort, _options.gloveBaudRate);
		cout <<  "done. On COM" << _options.gloveComPort << ", " << _options.gloveBaudRate << " baud." << endl;
	}
	
	if (_options.usePresSens) {
		// PresSensors Initialization
		cout << "Initialising pressure sensors... ";
		_hardware.press.initialize (_options.nPresSens);
		cout <<  "done. " << _options.nPresSens << " sensor(s) connected." << endl;
	}

	return true;

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
	
	if (_options.usePresSens) {
		// Pressure Sensors
		cout << "Releasing pressure sensors... ";
		_hardware.press.uninitialize ();
		cout << "done." << endl;
	}

}

void initData(void)
{

	// image buffers
	_img0.Resize (_options.sizeX, _options.sizeY);
	_img0.Zero();
	_img1.Resize (_options.sizeX, _options.sizeY);
	_img1.Zero();

	// trackers
	_data.tracker0.x = 0.0;
	_data.tracker0.y = 0.0;
	_data.tracker0.z = 0.0;
	_data.tracker0.azimuth = 0.0;
	_data.tracker0.elevation = 0.0;
	_data.tracker0.roll = 0.0;
	_data.tracker1.x = 0.0;
	_data.tracker1.y = 0.0;
	_data.tracker1.z = 0.0;
	_data.tracker1.azimuth = 0.0;
	_data.tracker1.elevation = 0.0;
	_data.tracker1.roll = 0.0;

	// DataGlove
	_data.glove.thumb[0] = 0;	// inner
	_data.glove.thumb[1] = 0;	// middle
	_data.glove.thumb[2] = 0;	// outer
	_data.glove.index[0] = 0;	// inner
	_data.glove.index[1] = 0;	// middle
	_data.glove.index[2] = 0;	// outer
	_data.glove.middle[0] = 0;	// inner
	_data.glove.middle[1] = 0;	// middle
	_data.glove.middle[2] = 0;	// outer
	_data.glove.ring[0] = 0;	// inner
	_data.glove.ring[1] = 0;	// middle
	_data.glove.ring[2] = 0;	// outer
	_data.glove.pinkie[0] = 0;	// inner
	_data.glove.pinkie[1] = 0;	// middle
	_data.glove.pinkie[2] = 0;	// outer
	_data.glove.abduction[0] = 0; // thumb-index
	_data.glove.abduction[1] = 0; // index-middle
	_data.glove.abduction[2] = 0; // middle-ring
	_data.glove.abduction[3] = 0; // ring-pinkie
	_data.glove.abduction[4] = 0; // palm
	_data.glove.palmArch = 0;
	_data.glove.wrist[0] = 0; // pitch
	_data.glove.wrist[1] = 0; // yaw

	// pressure sensors
	_data.pressure.channelA = 0;
	_data.pressure.channelB = 0;
	_data.pressure.channelC = 0;
	_data.pressure.channelD = 0;

}

void acquireAndSend(void)
{

	// acquire data

	if (_options.useCamera0) {
		unsigned char *buffer = NULL;
		_hardware.grabber0.waitOnNewFrame();
		_hardware.grabber0.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img0.GetRawBuffer(), buffer, _options.sizeX * _options.sizeY * 3);
		_hardware.grabber0.releaseBuffer();
	}

	if (_options.useCamera1) {
		unsigned char *buffer = NULL;
		_hardware.grabber1.waitOnNewFrame();
		_hardware.grabber1.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img1.GetRawBuffer(), buffer, _options.sizeX * _options.sizeY * 3);
		_hardware.grabber1.releaseBuffer();
	}

	if (_options.useTracker0) {
		// Read Tracker
		_hardware.tracker0.getData(&_data.tracker0);
	}

	if (_options.useTracker1) {
		_hardware.tracker1.getData(&_data.tracker1);
	}

	if (_options.useDataGlove) {
		// Read DataGlove
		_hardware.glove.getData(&_data.glove);
	}

	if (_options.usePresSens) {
		// Read Pressure Sensors
		_hardware.press.getData(&_data.pressure);
	}

	// send data
	
	if (_options.useCamera0) {
		_img0_outport.Content().Refer(_img0);
		_img0_outport.Write();
	}

	if (_options.useCamera1) {
		_img1_outport.Content().Refer(_img1);
		_img1_outport.Write();
	}

	if ( _options.useDataGlove || _options.usePresSens || _options.useTracker0 || _options.useTracker1 ) {
		_data_outport.Content() = _data;
		_data_outport.Write();
	}

}

void setOptions(char* fileName)
{

	// set options default values
	_options.portName			= "mirrorCollector";
	_options.netName			= "default";
	_options.useDataGlove		= 0;
	_options.gloveComPort		= 3;
	_options.gloveBaudRate		= 115200;
	_options.useTracker0		= 1;
	_options.tracker0ComPort	= 2;
	_options.tracker0BaudRate	= 115200;
	_options.tracker0TransRate	= 103.3;
	_options.tracker0Timeout	= 160;
	_options.useTracker1		= 0;
	_options.tracker1ComPort	= 4;
	_options.tracker1BaudRate	= 115200;
	_options.tracker1TransRate	= 103.3;
	_options.tracker1Timeout	= 160;
	_options.usePresSens		= 0;
	_options.nPresSens			= 2;
	_options.useCamera0			= 0;
	_options.useCamera1			= 0;
	_options.sizeX				= 384;
	_options.sizeY				= 272;
	_options.yOffset			= 0;

	// now read specific options from conf file
	YARPConfigFile optFile;
	char buf[255];
	// network specs
	optFile.setName(fileName);
	if ( optFile.getString("[NETWORK]", "PortName", buf) == YARP_OK) {
		_options.portName = buf;
	}
	if ( optFile.getString("[NETWORK]", "NetName", buf) == YARP_OK) {
		_options.netName = buf;
	}
	// glove
	optFile.get("[HARDWARE]", "UseDataGlove", &_options.useDataGlove);
	optFile.get("[HARDWARE]", "GloveComPort", &_options.gloveComPort);
	optFile.get("[HARDWARE]", "GloveBaudRate", &_options.gloveBaudRate);
	// trackers
	optFile.get("[HARDWARE]", "UseTracker0", &_options.useTracker0);
	optFile.get("[HARDWARE]", "Tracker0ComPort", &_options.tracker0ComPort);
	optFile.get("[HARDWARE]", "Tracker0BaudRate", &_options.tracker0BaudRate);
	optFile.get("[HARDWARE]", "Tracker0TransRate", &_options.tracker0TransRate);
	optFile.get("[HARDWARE]", "Tracker0Timeout", &_options.tracker0Timeout);
	optFile.get("[HARDWARE]", "UseTracker1", &_options.useTracker1);
	optFile.get("[HARDWARE]", "Tracker1ComPort", &_options.tracker1ComPort);
	optFile.get("[HARDWARE]", "Tracker1BaudRate", &_options.tracker1BaudRate);
	optFile.get("[HARDWARE]", "Tracker1TransRate", &_options.tracker1TransRate);
	optFile.get("[HARDWARE]", "Tracker1Timeout", &_options.tracker1Timeout);
	// pressure sensors
	optFile.get("[HARDWARE]", "UsePresSens", &_options.usePresSens);
	optFile.get("[HARDWARE]", "NPresSens", &_options.nPresSens);
	// cameras
	optFile.get("[HARDWARE]", "UseCamera0", &_options.useCamera0);
	optFile.get("[HARDWARE]", "UseCamera1", &_options.useCamera1);
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
	if ( _img0_outport.Register(buf,_options.netName.c_str()) != YARP_OK) {
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "Done." << endl;
	}
	cout << "Registering camera port 1... " << endl;
	ACE_OS::sprintf(buf, "/%s/o:img1", _options.portName.c_str());
	if ( _img1_outport.Register(buf,_options.netName.c_str()) != YARP_OK) {
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
	if ( _rep_outport.Register(buf,_options.netName.c_str()) != YARP_OK)	{
		cout << endl << "FATAL: could not register " << buf << endl;
		exit(YARP_FAIL);
	} else {
		cout << "Done." << endl;
	}

}

void unregisterPorts(void)
{

	_cmd_inport.Unregister();
	_rep_outport.Unregister();
	_data_outport.Unregister();
	_img0_outport.Unregister();
	_img1_outport.Unregister();

}

// ---------- main

int main (int argc, char *argv[])
{

	char buf[256];

	if ( YARPParseParameters::parse(argc, argv, "-help" ) ) {
		sendHelp();
		exit(YARP_OK);
	}

	if ( YARPParseParameters::parse(argc, argv, "-file", buf) ) {
		setOptions(buf);
	} else {
		setOptions(confFileName);
	}

	registerPorts();

	cout << "Waiting for Commands.." << endl;

	// ------------ main command parser loop

	MCommands command;
	bool bQuit = false;
	bool bStreaming = false;
	bool bConnected = false;
	streamingThread stream;
	int ret;
		
	YARPScheduler::setHighResScheduling();

	initData();

	do {

		//Read Commands for port
		_cmd_inport.Read ();
		command = _cmd_inport.Content();

		// what command was that?

		switch(command) {

		case CCMDConnect:
			// connect peripherals
			if (!bConnected) {
				ret = connectSensors();		
				if (!ret) {
					_rep_outport.Content() = CMD_FAILED;
					_rep_outport.Write();
					cout << endl << "FATAL ERROR: unable to connect to peripherals." << endl;
					exit(YARP_FAIL);
				} else {
					bConnected = true;
					ret = 0;
					if ( _options.useDataGlove) {
						ret |= HW_DATAGLOVE;
					}
					if ( _options.useTracker0 ) {
						ret |= HW_TRACKER0;
					}
					if ( _options.useTracker1 ) {
						ret |= HW_TRACKER1;
					}
					if ( _options.usePresSens) {
						ret |= HW_PRESSENS;
					}
					if ( _options.useCamera0) {
						ret |= HW_CAMERA0;
					}
					if ( _options.useCamera1) {
						ret |= HW_CAMERA1;
					}
					_rep_outport.Content() = ret;
					_rep_outport.Write();
					if ( _options.useCamera0 || _options.useCamera1 ) {
						_rep_outport.Content() = _options.sizeX;
						_rep_outport.Write();
						_rep_outport.Content() = _options.sizeY;
						_rep_outport.Write();
					}
				}
			} else {
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;

		case CCMDDisconnect:
			// Disconnect peripherals
			if (bConnected) {
				releaseSensors();
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
				bConnected = false;
			} else {
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;

		case CCMDQuit:
			// quit (need to disconnect first...)
			if (!bConnected) {
				bConnected = false;
				bQuit = true;
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			} else {
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;

		case CCMDGetData:
			// gather data off peripherals (need to be connected, obviously)
			if (bConnected) {
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
				// acquire data and send
				acquireAndSend();
   // DEBUG: is the frequency respected?
   double currentTime = YARPTime::GetTimeAsSeconds();
   cout << currentTime-elapsedTime << "\r";
   elapsedTime = currentTime;
   // DEBUG
			} else {
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;

		case CCMDStartStreaming:
			// start streaming thread
			if ( !bStreaming && bConnected ) {
				stream.Begin();
				bStreaming = true;
				cout << "Streaming thread is running." << endl;
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			} else {
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;

		case CCMDStopStreaming:
			// start streaming thread
			if ( bStreaming ) {
				stream.End();
				bStreaming = false;
				cout << "Streaming thread ended." << endl;
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			} else {
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;

		} // switch

	} while ( !bQuit );

	cout << "Bailing out." << endl;
	unregisterPorts();
	return 0;

}
