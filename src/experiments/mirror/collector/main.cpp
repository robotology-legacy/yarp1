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
/// $Id: main.cpp,v 1.1 2004-09-14 10:32:52 babybot Exp $
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

#include "CollectorCommands.h"

///
///
///
YARPOutputPortOf<MNumData> _data_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<YARPGenericImage> _img_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPOutputPortOf<int> _rep_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
YARPInputPortOf<MCommands> _cmd_inport;

const char *DEFAULT_NAME = "collector";

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
int					_sizeX				= -1;
int					_sizeY				= -1;
int					_yOffset			= 0;
short				_gloveComPort		= 1;
short				_trackerComPort		= 2;
int					_gloveBaudRate		= 115200;
int					_trackerBaudRate	= 115200;
int					_trackerTimeout		= 160;
short				_nPresSens			= 2;
YARPString			_portName;
YARPString			_netName;
int					_boardN				= 0;

YARPGrabber			_grabber;
YARPMagneticTracker	_tracker;
YARPDataGlove		_glove;
YARPPresSens		_press;

YARPImageOf<YarpPixelBGR> _img;
MNumData			_data;

using namespace std;

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
	_glove.startStreaming();
	_tracker.startStreaming();
	_press.startStreaming();

	while (!IsTerminated())
	{
		acquireAndPack();
		sendData();
	}

// Stop the peripherals threads
	_glove.stopStreaming();
	_tracker.stopStreaming();
	_press.stopStreaming();
		
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
	cout << "\t-w dim\t\t\t sets images with to 'dim'." << endl;
	cout << "\t-h dim\t\t\t sets images height to 'dim'." << endl;
	cout << "\t-name portname\t\t use yarp port called 'portname'." << endl;
	cout << "\t-net netname\t\t use network 'netname'." << endl;
}

bool connect2Sensors(void)
{
// Framegrabber Initialization
	cout << "Initializing Framegrabber..";
	_grabber.initialize (_boardN, _sizeX, _sizeY);
	cout <<  " Done, acquisition size is h=" << _sizeX << " w=" << _sizeY <<endl;

// DataGlove Initialization
	cout << "Initializing DataGlove..";
	_glove.initialize (_gloveComPort, _gloveBaudRate);
	cout <<  " Done, Glove is on port COM" << _gloveComPort << " with a speed of " << _gloveBaudRate << " Baud." << endl;

// Tracker Initialization
	cout << "Initializing Tracker..";
	_tracker.initialize (_trackerComPort, _trackerBaudRate, _trackerTimeout);
	cout <<  " Done, Tracker is on port COM" << _trackerComPort << " with a speed of " << _trackerBaudRate << " Baud." << endl;

// PresSensors Initialization
	cout << "Initializing Pressure Sensor..";
	_press.initialize (_nPresSens);
	cout <<  " Done, there are " << _nPresSens << " sensors(s) connected." << endl;
	
	return true;
}

void releaseSensors(void)
{
// Framegrabber 
	cout << "Releasing Framegrabber..";  
	_grabber.uninitialize ();
	cout << " Done." << endl;

// DataGlove 
	cout << "Releasing DataGlove..";  
	_glove.uninitialize ();
	cout << " Done." << endl;

// Tracker 
	cout << "Releasing tracker..";  
	_tracker.uninitialize ();
	cout << " Done." << endl;

// Pressure Sensors
	cout << "Releasing Pressure Sensor..";  
	_press.uninitialize ();
	cout << " Done." << endl;
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
	_img.Resize (_sizeX, _sizeY);
	cleanDataStructures();
}

void acquireAndPack(void)
{
// Wait for a new Framegrabber frame and get the data
	unsigned char *buffer = NULL;
	_grabber.waitOnNewFrame ();
	_grabber.acquireBuffer(&buffer);
	memcpy((unsigned char *)_img.GetRawBuffer(), buffer, _sizeX * _sizeY * 3);
	_grabber.releaseBuffer ();
// Read DataGlove
	_glove.getData(&_data.glove);
// Read Tracker
	_tracker.getData(&_data.tracker);
// Read Pressure Sensors
	_press.getData(&_data.pressure);
// Send the data to the port
	_img_outport.Content().Refer(_img);
	_data_outport.Content() = _data;
}

void sendData(void)
{
	_img_outport.Write();
	_data_outport.Write();
}

int main (int argc, char *argv[])
{
	char buf[256];

	if ( YARPParseParameters::parse(argc, argv, "help" ) )
	{
		sendHelp();
		exit(YARP_OK);
	}

	if ( !YARPParseParameters::parse(argc, argv, "name", _portName) )
	{
		_portName = DEFAULT_NAME;
	}
	
	if ( !YARPParseParameters::parse(argc, argv, "net", _netName) )
	{
		_netName = "default";
	}
	
	if ( !YARPParseParameters::parse(argc, argv, "w", &_sizeX) )
	{
		_sizeX = 384;
	}
	
	if ( !YARPParseParameters::parse(argc, argv, "h", &_sizeY) )
	{
		_sizeY = 272;
	}
	
	if (_sizeX > 384)
	{
		cout << "*** WARNING: Width can't be more than 384 pixels. Width forced to 384." << endl;
		_sizeX = 384;
	}

		if (_sizeY > 272)
	{
		cout << "*** WARNING: Height can't be more than 272 pixels. Height forced to 272.";
		_sizeY = 272;
	}

// Network declaration
    cout << "Using " << _netName.c_str() << " network" << endl;

// Data port registration
	cout << "Registering Data port..." << endl;
	ACE_OS::sprintf(buf, "/%s/o:str", _portName.c_str());
	int ret = 0;	
	ret = _data_outport.Register(buf,_netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;

// Images port registration
	cout << "Registering Images port..." << endl;
	ACE_OS::sprintf(buf, "/%s/o:img", _portName.c_str());
	ret = 0;	
	ret = _img_outport.Register(buf,_netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;
	
// Commands port registration
	cout << "Registering Commands port..." << endl;
	ACE_OS::sprintf(buf, "/%s/i:int", _portName.c_str());
	ret = 0;	
	ret = _cmd_inport.Register(buf,_netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering input port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;

// Response port registration
	cout << "Registering Response port..." << endl;
	ACE_OS::sprintf(buf, "/%s/o:int", _portName.c_str());
	ret = 0;	
	ret = _rep_outport.Register(buf,_netName.c_str());
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '" << buf << "' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	else
		cout << "Done." << endl;

	cout << "Waiting for Commands.." << endl;

// main command parser loop
	MCommands command;
	bool bQuit = false;
	bool bStreamingStarted = false;
	bool bConnected = false;
	mainthread collectThread;
		
	YARPScheduler::setHighResScheduling ();

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
				prepareDataStructures();
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
					_rep_outport.Content() = CMD_ACK;
					_rep_outport.Write(1);
					_rep_outport.Content() = _sizeX;
					_rep_outport.Write(1);
					_rep_outport.Content() = _sizeY;
					_rep_outport.Write(1);
				}
			}
			else
			{
				_rep_outport.Content() = INVALID_CMD;
				_rep_outport.Write();
			}
			break;
		// Relase peripherals and Quit
		case CCMDQuit:
			if (bConnected)
			{
				bConnected = false;
				bQuit = true;
				releaseSensors();
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			}
			else
			{
				_rep_outport.Content() = INVALID_CMD;
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
				_rep_outport.Content() = INVALID_CMD;
				_rep_outport.Write();
			}
			break;
		// Start the acquisition thread
		case CCMDStartStreaming:
			if ((bStreamingStarted) || (!bConnected) )
			{
				_rep_outport.Content() = INVALID_CMD;
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
				_rep_outport.Content() = INVALID_CMD;
				_rep_outport.Write();
			}
			break;
		// Reset the glove
		case CCMDResetGlove:
			if (bConnected)
			{
				_glove.resetGlove();
				_rep_outport.Content() = CMD_ACK;
				_rep_outport.Write();
			}
			else
			{
				_rep_outport.Content() = INVALID_CMD;
				_rep_outport.Write();
			}
			break;
		}

	} while (!bQuit);

	cout << "Quitting..." << endl;
	_cmd_inport.Unregister();
	_data_outport.Unregister();
	_img_outport.Unregister();
	_rep_outport.Unregister();
	cout << "Bye." << endl;

	return YARP_OK;
}



