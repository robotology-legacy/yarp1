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
/// $Id: main.cpp,v 1.1 2005-02-18 10:15:36 babybot Exp $
///
///


#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <stdio.h>
#include <conio.h>

#include <yarp/YARPThread.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPImageFile.h>

#include <yarp/YARPRobotHardware.h>

#include "CollectorCommands.h"

struct SaverOptions
{
	int	sizeX;
	int sizeY;
	YARPString portName;
	YARPString netName;
	int useCamera;
	int useTracker;
	int useDataGlove;
	int usePresSens;
};

typedef struct SaverOptions PgmOptions;


const char *DEFAULT_NAME = "mirrorSaver";

YARPInputPortOf<MNumData> _data_inport;
YARPInputPortOf<YARPGenericImage> _img_inport;
YARPInputPortOf<int> _rep_inport;
YARPOutputPortOf<MCommands> _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
PgmOptions _options;
char _prefix[255];
int _seqN;
FILE *_outDataFile;
MNumData _data;
YARPImageOf<YarpPixelBGR> _img;

void writeDataToFile(MNumData data, int i);
void writeHeaderToFile(void);
void prepareDataStructures(void);
void cleanDataStructures(void);

class mainthread : public YARPThread
{
public:
	virtual void Body (void);
};

void mainthread::Body (void)
{
	int j = 0;
	
	cleanDataStructures();
	
	char imageFileName[255];
	
	while (!IsTerminated())
	{
		j++;
		if ( _options.useDataGlove || _options.useTracker || _options.usePresSens)
		{
			_data_inport.Read();
			_data = _data_inport.Content();
			writeDataToFile(_data,j);
		}
		if ( _options.useCamera)
		{	
			_img_inport.Read();
			_img.Refer(_img_inport.Content());
			ACE_OS::sprintf(imageFileName,"%s_%03d.pgm", _prefix, j);
			YARPImageFile::Write(imageFileName, _img,YARPImageFile::FORMAT_PPM);
		}
	}

	return;
}

void writeDataToFile(MNumData data, int i)
{
	fprintf(_outDataFile,"%d ",i);
	fprintf(_outDataFile," %f %f %f %f %f %f", data.tracker.x, data.tracker.y, data.tracker.z, data.tracker.azimuth, data.tracker.elevation, data.tracker.roll);
	fprintf(_outDataFile," %d %d %d", data.glove.thumb[0], data.glove.thumb[1], data.glove.thumb[2]);
	fprintf(_outDataFile," %d %d %d", data.glove.index[0], data.glove.index[1], data.glove.index[2]);
	fprintf(_outDataFile," %d %d %d", data.glove.middle[0], data.glove.middle[1], data.glove.middle[2]);
	fprintf(_outDataFile," %d %d %d", data.glove.ring[0], data.glove.ring[1], data.glove.ring[2]);
	fprintf(_outDataFile," %d %d %d", data.glove.pinkie[0], data.glove.pinkie[1], data.glove.pinkie[2]);
	fprintf(_outDataFile," %d %d %d %d %d", data.glove.abduction[0], data.glove.abduction[1], data.glove.abduction[2], data.glove.abduction[3], data.glove.abduction[4]);
	fprintf(_outDataFile," %d", data.glove.palmArch);
	fprintf(_outDataFile," %d %d", data.glove.wrist[0], data.glove.wrist[1]);
	fprintf(_outDataFile," %d %d %d %d", data.pressure.channelA, data.pressure.channelB, data.pressure.channelC, data.pressure.channelD);
	fprintf(_outDataFile, "\n");
}

void writeHeaderToFile(void)
{
	fprintf(_outDataFile,"frameN " );
	fprintf(_outDataFile,"X Y Z Azimuth Elevation Roll " );
	fprintf(_outDataFile,"ThumbInner ThumbMiddle ThumbOuter " );
	fprintf(_outDataFile,"IndexInner IndexMiddle IndexOuter " );
	fprintf(_outDataFile,"MiddleInner MiddleMiddle MiddleOuter " );
	fprintf(_outDataFile,"RingInner RingMiddle RingOuter " );
	fprintf(_outDataFile,"PinkieInner PinkieMiddle PinkieOuter " );
	fprintf(_outDataFile,"Abduct1 Abduct2 Abduct3 Abduct4 Abduct5" );
	fprintf(_outDataFile,"PalmArch " );
	fprintf(_outDataFile,"Wrist1 Wrist2 " );
	fprintf(_outDataFile,"PressureA PressureB PressureC PressureC\n" );
}

void prepareDataStructures(void)
{
	_img.Resize (_options.sizeX, _options.sizeY);
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

void registerPorts()
{
	char portName[255];
// Data port registration
	cout << "Registering Data port (input)..." << endl;
	int ret = 0;
	ACE_OS::sprintf(portName,"/%s/i:str", DEFAULT_NAME);
	ret = _data_inport.Register(portName,"default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name " << portName <<" (see above)."  << endl;
		exit(YARP_FAIL);
	}

// Images port registration
	cout << "Registering Images port (input)..." << endl;
	ret = 0;
	ACE_OS::sprintf(portName,"/%s/i:img", DEFAULT_NAME);
	ret = _img_inport.Register(portName,"default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name " << portName <<" (see above)."  << endl;
		exit(YARP_FAIL);
	}

// Commands port registration
	cout << "Registering Commands port (output)..." << endl;
	ret = 0;
	ACE_OS::sprintf(portName,"/%s/o:int", DEFAULT_NAME);
	ret = _cmd_outport.Register(portName,"default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name " << portName <<" (see above)."  << endl;
		exit(YARP_FAIL);
	}

// Response port registration
	cout << "Registering Response port..." << endl;
	ret = 0;
	ACE_OS::sprintf(portName,"/%s/i:int", DEFAULT_NAME);
	ret = _rep_inport.Register(portName, "default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name " << portName <<" (see above)."  << endl;
		exit(YARP_FAIL);
	}
}

void unregisterPorts()
{
	_cmd_outport.Unregister();
	_rep_inport.Unregister();
	_data_inport.Unregister();
	_img_inport.Unregister();
}

int main(int argc, char* argv[])
{
	registerPorts();
	
	char c;
	cout << endl << "Please establish a connection to the server and hit any key." << endl;
	c = getch();

	int reply;
	MNumData data;
	YARPImageOf<YarpPixelBGR> img;
	_options.sizeX = 0;
	_options.sizeY = 0;
	_seqN = 0;
	_outDataFile = NULL;
	int i=0;
	mainthread saverThread;
	char imageFileName[255];
	char dataFileName[255];

	ACE_OS::sprintf(dataFileName,"data.txt");
	ACE_OS::sprintf(imageFileName,"camera.pgm");
		
	YARPScheduler::setHighResScheduling ();

	bool bQuit = false;
	bool bConnected = false;
	do
	{
		cout << endl;
		cout << "***********************" << endl;
		cout << "1 - Connect" << endl;
		cout << "2 - Disconnect" << endl;
		cout << "3 - Get Data" << endl;
		cout << "4 - Streaming" << endl;
		cout << "5 - Send KILL command and quit" << endl;
		cout << "6 - Reset Glove" << endl;
		cout << "q - Quit" << endl;
		cout << "**********************" << endl;
		c = getche();
		switch (c)
		{
		case '1':
				if (bConnected)
			{
				cout << endl << "ERROR : Already connected!";
				break;
			}
			cout << endl << "Connecting..";
			_cmd_outport.Content() = CCMDConnect;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if (reply != CMD_FAILED)
			{
				cout << endl << "Done.";
				_options.useDataGlove = reply & HW_DATAGLOVE;
				_options.useTracker = reply & HW_TRACKER;
				_options.usePresSens = reply & HW_PRESSENS;
				_options.useCamera = reply & HW_CAMERA;
				if (_options.useCamera)
				{
					_rep_inport.Read();
					_options.sizeX = _rep_inport.Content();
					_rep_inport.Read();
					_options.sizeY = _rep_inport.Content();
					cout << "Image Size is " << _options.sizeX << " x " << _options.sizeY << endl;
				}
				prepareDataStructures();
				bConnected = true;
			}
			else
				cout << " Failed." << endl;
			break;
		case '2':
		if (!bConnected)
			{
				cout << endl << "ERROR : not connected yet!";
				break;
			}
			cout << endl << "Sending the Disconnect command..";
			_cmd_outport.Content() = CCMDDisconnect;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				bConnected = false;
				cout << " Done." << endl;
			}	
			else
				cout << " Failed." << endl;
			break;
		case '3':
			if (!bConnected)
			{
				cout << endl << "ERROR : not connected yet!";
				break;
			}

			if (_options.useCamera)
			{
				cout << endl << "Image file name is " << imageFileName;
				cout << endl << "Do you want to change it [y/N]";
				c = getche();
				if (c == 'y')
				{
					cout << endl << "File name ? ";
					scanf("%s",imageFileName);
				}
			}
			if ( _options.useDataGlove || _options.useTracker || _options.usePresSens)
			{
				cout << endl << "Data file name is " << dataFileName;
				cout << endl << "Do you want to change it [y/N]";
				c = getche();
				if (c == 'y')
				{
					cout << endl << "File name ? ";
					scanf("%s",dataFileName);
				}
				cout << "Append data[y/N] ? ";
				c = getche();
				_outDataFile = NULL;
				if ( c == 'y' )
					_outDataFile = fopen(dataFileName,"w+");
				else
					_outDataFile = fopen(dataFileName,"w");
				if (_outDataFile == NULL)
				{
					cout << "FATAL ERROR: impossible to open output file" << dataFileName << endl;
					exit(YARP_FAIL);
				}

				if (c != 'y')
					writeHeaderToFile();
			}

			cleanDataStructures();
			cout << endl << "Hit any key to start.";
			c = getch();
			cout << endl << "Asking for data..";
			_cmd_outport.Content() = CCMDGetData;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				cout << " Receiving data..";
				i++;
				if ( _options.useDataGlove || _options.useTracker || _options.usePresSens)
				{
					_data_inport.Read();
					data = _data_inport.Content();
					writeDataToFile(data,i);
					cout << endl << "Data written on file " << dataFileName << endl;
				}
				if (_options.useCamera)
				{
					_img_inport.Read();
				
					img.Refer(_img_inport.Content());
					YARPImageFile::Write(imageFileName,img,YARPImageFile::FORMAT_PPM);
					cout << endl << "Image written on file " << imageFileName << endl;
				}
			}	
			else
				cout << " Failed." << endl;
			if (_outDataFile != NULL)
				fclose(_outDataFile);
			break;
		case '4':
			if (!bConnected)
			{
				cout << endl << "ERROR : not connected yet!";
				break;
			}
			_seqN++;
			ACE_OS::sprintf(_prefix,"seq%02d",_seqN);
			cout << endl << "Files will be written with prefix " << _prefix;
			cout << endl << "Do you want to change it [y/N] ";
			c = getche();
			if (c == 'y')
			{
				cout << endl << "Prefix ? ";
				scanf("%s",_prefix);
			}
			
			if ( _options.useDataGlove || _options.useTracker || _options.usePresSens)
			{
				ACE_OS::sprintf(dataFileName, "%s.txt", _prefix);
				_outDataFile = NULL;
				_outDataFile = fopen(dataFileName,"w");
				if (_outDataFile == NULL)
				{
					cout << "FATAL ERROR: impossible to open output file "  << dataFileName << endl;
					exit(YARP_FAIL);
				}
				else
					cout << endl << "Data will be written on file " << dataFileName;
				writeHeaderToFile();
			}
			cleanDataStructures();
			cout << endl << "HIT ANY KEY TO START STREAMING";
			c = getch();
			cout << endl << "Starting streaming mode..";
			_cmd_outport.Content() = CCMDStartStreaming;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				saverThread.Begin();
				cout << " Done." << endl;
			}
			else
			{
				cout << " Failed." << endl;
				break;
			}
			cout << endl << "HIT ANY KEY TO STOP STREAMING";
			c = getch();
			cout << endl << "Stopping streaming mode..";
			_cmd_outport.Content() = CCMDStopStreaming;
			_cmd_outport.Write();
			saverThread.End();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				
				cout << " Done." << endl;
			}
			else
				cout << " Failed." << endl;
			break;
			if (_outDataFile != NULL)
				fclose(_outDataFile);
			break;
		case '5':
			if (bConnected)
			{
				cout << endl << "Sending the Disconnect command..";
				_cmd_outport.Content() = CCMDDisconnect;
				_cmd_outport.Write();
				_rep_inport.Read();
				reply = _rep_inport.Content();
				if ( reply == CMD_ACK)
				{
					cout << " Done." << endl;
					bConnected = false;
				}	
				else
				{
					cout << " Failed." << endl;
					break;
				}
			}

			cout << endl << "Sending the KILL command..";
			_cmd_outport.Content() = CCMDQuit;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				bQuit = true;
				cout << " Done." << endl;
				bConnected = false;
			}	
			else
			{
				cout << " Failed." << endl;
			}
			break;
		case '6':
			cout << endl << "Resetting the glove..";
			_cmd_outport.Content() = CCMDResetGlove;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
				cout << " Done." << endl;
			else
				cout << " Failed." << endl;
			break;
		case 'q':
			if (bConnected)
			{
				cout << endl << "Sending the Disconnect command..";
				_cmd_outport.Content() = CCMDDisconnect;
				_cmd_outport.Write();
				_rep_inport.Read();
				reply = _rep_inport.Content();
				if ( reply == CMD_ACK)
				{
					cout << " Done." << endl;
					bConnected = false;
				}	
				else
				cout << " Failed." << endl;
			}
			bQuit = true;
		}
	} while (!bQuit);

	unregisterPorts();
	
	return 0;
}
