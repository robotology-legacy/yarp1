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
/// $Id: DrvTest.cpp,v 1.1 2004-09-14 10:32:52 babybot Exp $
///
///



#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <stdio.h>
#include <YARPThread.h>
#include <YARPScheduler.h>

#include <YARPParseParameters.h>

#include <YARPMirrorDataGlove.h>

#define Sensor YARPMirrorDataGlove

///
///
///

const char *DEFAULT_NAME = "collector";
///
///
///

///
/// global params.

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
	Sensor mSensor;
	DataGloveData data;
	CyberGloveOpenParameters params;
	params.comPort = 1;
	params.baudRate = 115200;
	int ret;
	cout << "Setting up the sensor.." << endl;
	mSensor.initialize(params);
	cout << "Starting the sensor streaming mode..." << endl;
	ret = mSensor.startStreaming();
	if (ret == YARP_FAIL)
	{
		cout << "error!" << endl;
		exit (1);
	}
	cout << "Data Collector thread is running." << endl;
	cout << "Hit any key to quit." << endl;

	while (!IsTerminated())
	{
		ret = mSensor.getData(&data);
		if (ret == YARP_FAIL)
		{
			cout << "error reading data!" << endl;
			exit (1);
		}
		else
			cout << "palm Arch=" << data.palmArch << endl;
	}
	cout << "Thread stopped." << endl;

	cout << "Stopping the sensor streamin mode..." << endl;
	ret = mSensor.stopStreaming();
	cout << "Releasing sensor..." << endl;  
	mSensor.uninitialize ();
	cout << "Done." << endl;
	
	return;
}

///
///
///
void sendHelp()
{
	cout << endl << "*** Mirror drivers thread ***" << endl  << endl;
	cout << "USAGE: DrvTest.exe [parameters]" << endl;
	cout << "parameters are:" << endl;
	cout << "\t-help\t\t this help screen." << endl;
}

int main (int argc, char *argv[])
{
	if ( YARPParseParameters::parse(argc, argv, "help" ) )
	{
		sendHelp();
		exit(YARP_OK);
	}

	YARPScheduler::setHighResScheduling ();

	mainthread _thread;
	_thread.Begin();
	
	char c = 0;

	getchar();
	
	_thread.End(-1);
	
	cout << "Quitting..." << endl;

	return YARP_OK;
}

