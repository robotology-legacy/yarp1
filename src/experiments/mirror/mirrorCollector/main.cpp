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
/// $Id: main.cpp,v 1.2 2005-12-01 13:57:48 beltran Exp $
///
///



#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <stdio.h>

#include <yarp/YARPRobotHardware.h>

#include <yarp/YARPThread.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPConfigFile.h>

#include "CollectorCommands.h"

using namespace std;
///
///
///
YARPOutputPortOf<MNumData> _data_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<YARPGenericImage> _img_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<int> _rep_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<MCommands> _cmd_inport;

///
/// Functions Declaration
void sendHelp(void);
bool connect2Sensors(void);
void releaseSensors(void);
void cleanDataStructures(void);
void prepareDataStructures(void);
void acquireAndPack(void);
void sendData(void);

///
/// global params.

struct CollectorOptions
{
	int	sizeX;
	int sizeY;
	int yOffset;
	short gloveComPort;
	short trackerComPort;
	int	gloveBaudRate;
	int	trackerBaudRate;
	int	trackerTimeout;
	short nPresSens;
	YARPString portName;
	YARPString netName;
	int	boardN;
	int useCamera;
	int useTracker;
	int useDataGlove;
	int usePresSens;
};

typedef struct CollectorOptions PgmOptions;

struct CollectorHardware
{
	YARPGrabber			grabber;
	YARPMagneticTracker tracker;   //redefined form YARPGenericTracker
	YARPDataGlove       glove;     //redefined from YARPGenericDataGlove
	YARPPresSens        press;     //redefined from YARPGenericPresSens
};

typedef CollectorHardware PgmHardware;


/// GLOBAL VARIABLES
PgmOptions _options;
PgmHardware _hardware;
YARPImageOf<YarpPixelBGR> _img;
MNumData			_data;
////

///
///
///

class mainthread : public YARPThread
{
public:
	virtual void Body (void);
};

void mainthread::Body (void)
{
// Start the peripherals threads
	if (_options.useDataGlove)
		_hardware.glove.startStreaming();
	if (_options.useTracker)
		_hardware.tracker.startStreaming();

	while (!IsTerminated())
	{
		acquireAndPack();
		sendData();
	}

// Stop the peripherals threads
	if (_options.useDataGlove)
		_hardware.glove.stopStreaming();
	if (_options.useTracker)
	_hardware.tracker.stopStreaming();
		
	return;
}

///
///
///
void sendHelp(void)
{
	cout << endl << "*** Mirror setup acquisition thread ***" << endl  << endl;
	cout << "USAGE: collector.exe [parameters]" << endl;
	cout << "parameters are:" << endl;
	cout << "\t-help\t\t this help screen." << endl;
	cout << "\t-file filename use option file 'filename'." <<endl;
}

bool connect2Sensors(void)
{
	if (_options.useCamera)
	{
		// Framegrabber Initialization
		cout << "Initializing Framegrabber..";
		_hardware.grabber.initialize (_options.boardN, _options.sizeX, _options.sizeY);
		cout <<  " Done, acquisition size is Width " << _options.sizeX << "  and Height" << _options.sizeY <<endl;
	}

	if (_options.useDataGlove)
	{
		// DataGlove Initialization
		cout << "Initializing DataGlove..";
		_hardware.glove.initialize (_options.gloveComPort, _options.gloveBaudRate);
		cout <<  " Done, Glove is on port COM" << _options.gloveComPort << " with a speed of " << _options.gloveBaudRate << " Baud." << endl;
	}
	
	if (_options.useTracker)
	{
		// Tracker Initialization
		cout << "Initializing Tracker..";
		_hardware.tracker.initialize (_options.trackerComPort, _options.trackerBaudRate, _options.trackerTimeout);
		cout <<  " Done, Tracker is on port COM" << _options.trackerComPort << " with a speed of " << _options.trackerBaudRate << " Baud." << endl;
	}
	
	if (_options.usePresSens)
	{
		// PresSensors Initialization
		cout << "Initializing Pressure Sensor..";
		_hardware.press.initialize (_options.nPresSens);
		cout <<  " Done, there are " << _options.nPresSens << " sensors(s) connected." << endl;
	}

	return true;
}

void releaseSensors(void)
{
	if (_options.useCamera)
	{
		// Framegrabber 
		cout << "Releasing Framegrabber..";  
		_hardware.grabber.uninitialize ();
		cout << " Done." << endl;
	}
	
	if (_options.useDataGlove)
	{
		// DataGlove 
		cout << "Releasing DataGlove..";  
		_hardware.glove.uninitialize ();
		cout << " Done." << endl;
	}
	
	if (_options.useTracker)
	{
		// Tracker 
		cout << "Releasing tracker..";  
		_hardware.tracker.uninitialize ();
		cout << " Done." << endl;
	}

	if (_options.usePresSens)
	{
		// Pressure Sensors
		cout << "Releasing Pressure Sensor..";  
		_hardware.press.uninitialize ();
		cout << " Done." << endl;
	}

}

void cleanDataStructures(void)
{
// Pressure Sensors
	_data.pressure.channelA = 0;
	_data.pressure.channelB = 0;
	_data.pressure.channelC = 0;
	_data.pressure.channelD = 0;
// Tracker
	_data.tracker.x = 0.0;
	_data.tracker.y = 0.0;
	_data.tracker.z = 0.0;
	_data.tracker.azimuth = 0.0;
	_data.tracker.elevation = 0.0;
	_data.tracker.roll = 0.0;
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

	_img.Zero();
}

void prepareDataStructures(void)
{
	_img.Resize (_options.sizeX, _options.sizeY);
	cleanDataStructures();
}

void acquireAndPack(void)
{
	if (_options.useCamera)
	{
		// Wait for a new Framegrabber frame and get the data
		unsigned char *buffer = NULL;
		_hardware.grabber.waitOnNewFrame ();
		_hardware.grabber.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img.GetRawBuffer(), buffer, _options.sizeX * _options.sizeY * 3);
		_hardware.grabber.releaseBuffer ();
		// Send data to the port
		_img_outport.Content().Refer(_img);
	}

	if (_options.useDataGlove)
	{
		// Read DataGlove
		_hardware.glove.getData(&_data.glove);
	}

	if (_options.useTracker)
	{
		// Read Tracker
		_hardware.tracker.getData(&_data.tracker);
	}

	if (_options.usePresSens)
	{
		// Read Pressure Sensors
		_hardware.press.getData(&_data.pressure);
	}

	if ( _options.useDataGlove || _options.usePresSens || _options.useTracker )
	{
		// Send the data to the port
		_data_outport.Content() = _data;
	}
}

void sendData(void)
{
	if (_options.useCamera)
		_img_outport.Write();

	if ( _options.useDataGlove || _options.usePresSens || _options.useTracker )
		_data_outport.Write();
}

void setOptions(char *fileName)
{
	YARPConfigFile optFile;
	
	_options.portName			= "mirrorCollector";
	_options.netName			= "default";
	_options.sizeX				= 384;
	_options.sizeY				= 272;
	_options.yOffset			= 0;
	_options.gloveComPort		= 1;
	_options.trackerComPort		= 2;
	_options.gloveBaudRate		= 115200;
	_options.trackerBaudRate	= 115200;
	_options.trackerTimeout		= 160;
	_options.nPresSens			= 2;
	_options.useCamera			= 1;
	_options.useTracker			= 1;
	_options.useDataGlove		= 1;
	_options.usePresSens		= 1;

	char buf[255];

	optFile.setName(fileName);
	if ( optFile.getString("[NETWORK]", "PortName", buf) == YARP_OK)
		_options.portName = buf;
	if ( optFile.getString("[NETWORK]", "NetName", buf) == YARP_OK)
		_options.netName = buf;
	optFile.get("[HARDWARE]", "ImageSizeX", &_options.sizeX);
	optFile.get("[HARDWARE]", "ImageSizeY", &_options.sizeY);
	optFile.get("[HARDWARE]", "YOffSet", &_options.yOffset);
	optFile.get("[HARDWARE]", "GloveComPort", &_options.gloveComPort);
	optFile.get("[HARDWARE]", "TrackerComPort", &_options.trackerComPort);
	optFile.get("[HARDWARE]", "GloveBaudRate", &_options.gloveBaudRate);
	optFile.get("[HARDWARE]", "TrackerBaudRate", &_options.trackerBaudRate);
	optFile.get("[HARDWARE]", "TrackerTimeout", &_options.trackerTimeout);
	optFile.get("[HARDWARE]", "NPresSens", &_options.nPresSens);
	optFile.get("[HARDWARE]", "UseCamera", &_options.useCamera);
	optFile.get("[HARDWARE]", "UseTracker", &_options.useTracker);
	optFile.get("[HARDWARE]", "UseDataGlove", &_options.useDataGlove);
	optFile.get("[HARDWARE]", "UsePresSens", &_options.usePresSens);

}

void registerPorts(void)
{
	char buf[256];
	int ret;

// Data port registration
	cout << "Registering Data port..." << endl;
	ACE_OS::sprintf(buf, "/%s/o:str", _options.portName.c_str());
	ret = 0;	
	ret = _data_outport.Register(buf,_options.netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;
	
// Images port registration
	cout << "Registering Images port..." << endl;
	ACE_OS::sprintf(buf, "/%s/o:img", _options.portName.c_str());
	ret = 0;	
	ret = _img_outport.Register(buf,_options.netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;
	
// Commands port registration
	cout << "Registering Commands port..." << endl;
	ACE_OS::sprintf(buf, "/%s/i:int", _options.portName.c_str());
	ret = 0;	
	ret = _cmd_inport.Register(buf,_options.netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering input port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;

// Response port registration
	cout << "Registering Response port..." << endl;
	ACE_OS::sprintf(buf, "/%s/o:int", _options.portName.c_str());
	ret = 0;	
	ret = _rep_outport.Register(buf,_options.netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;
}

void unregisterPorts(void)
{
	_cmd_inport.Unregister();
	_rep_outport.Unregister();
	_data_outport.Unregister();
	_img_outport.Unregister();
}

int main (int argc, char *argv[])
{
	char buf[256];

	if ( YARPParseParameters::parse(argc, argv, "help" ) )
	{
		sendHelp();
		exit(YARP_OK);
	}

	if ( YARPParseParameters::parse(argc, argv, "file", buf) )
	{
		setOptions(buf);
	}
	else
	{
		setOptions("mirrorCollector.conf");
	}

	if (_options.sizeX > 384)
	{
		cout << "*** WARNING: Width can't be more than 384 pixels. Width forced to 384." << endl;
		_options.sizeX = 384;
	}

		if (_options.sizeY > 272)
	{
		cout << "*** WARNING: Height can't be more than 272 pixels. Height forced to 272.";
		_options.sizeY = 272;
	}

// Network declaration
    cout << "Using " << _options.netName.c_str() << " network" << endl;

	registerPorts();

	cout << "Waiting for Commands.." << endl;

// main command parser loop
	MCommands command;
	bool bQuit = false;
	bool bStreamingStarted = false;
	bool bConnected = false;
	mainthread collectThread;
	int ret;
		
	YARPScheduler::setHighResScheduling ();

	prepareDataStructures();

	do
	{
	//Read Commands for port
		_cmd_inport.Read ();
		command = _cmd_inport.Content();
		switch(command)
		{
		// Connect to peripherals
		case CCMDConnect:
			if (!bConnected)
			{
				cleanDataStructures();
				ret = connect2Sensors();		
				if (!ret)
				{
					_rep_outport.Content() = CMD_FAILED;
					_rep_outport.Write();
					cout << endl << "FATAL ERROR: unable to connect to peripherals." << endl;
					exit(YARP_FAIL);
				}
				else
				{
					bConnected = true;
					ret = 0;
					if ( _options.useDataGlove)
						ret += HW_DATAGLOVE;
					if ( _options.useTracker)
						ret += HW_TRACKER;
					if ( _options.usePresSens)
						ret += HW_PRESSENS;
					if ( _options.useCamera)
						ret += HW_CAMERA;
					_rep_outport.Content() = ret;
					_rep_outport.Write(1);
					if ( _options.useCamera)
					{
						_rep_outport.Content() = _options.sizeX;
						_rep_outport.Write(1);
						_rep_outport.Content() = _options.sizeY;
						_rep_outport.Write(1);
					}
				}
			}
			else
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;
		// Disconnect from peripherals
		case CCMDDisconnect:
			if (bConnected)
			{
				releaseSensors();
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write(1);
				bConnected = false;
			}
			else
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;
		// Relase peripherals and Quit
		case CCMDQuit:
			if (!bConnected)
			{
				bConnected = false;
				bQuit = true;
				releaseSensors();
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			}
			else
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;
		// Read Sensors
		case CCMDGetData:
			if (bConnected)
			{
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			// Acquire data and prepare them for sending
				acquireAndPack();
			// Send the Data.
				sendData();
			}
			else
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;
		// Start the acquisition thread
		case CCMDStartStreaming:
			if ((bStreamingStarted) || (!bConnected) )
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			else
			{
				collectThread.Begin();
				bStreamingStarted = true;
				cout << "Data Collector thread is running." << endl;
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			}
			break;
		// Stop the acquisition thread
		case CCMDStopStreaming:
			if (bStreamingStarted)
			{
				collectThread.End();
				bStreamingStarted = false;
				cout << "Data Collector thread is stopped." << endl;
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			}
			else
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;
		// Reset the glove
		case CCMDResetGlove:
			if (bConnected)
			{
				if ( _options.useDataGlove )
					_hardware.glove.resetGlove();
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			}
			else
			{
				_rep_outport.Content() = CMD_FAILED;
				_rep_outport.Write();
			}
			break;
		}

	} while (!bQuit);

	cout << "Quitting..." << endl;
	unregisterPorts();
	cout << "Bye." << endl;

	return 0;
}




