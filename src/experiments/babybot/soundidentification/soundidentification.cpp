// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundidentification.cpp
// 
//     Description:  
// 
//         Version:  $Id: soundidentification.cpp,v 1.4 2004-07-08 14:47:30 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPString.h>
#include <YARPTime.h>
#include <YARPSound.h>
#include <YARPSoundPortContent.h>
#include <YARPImageDraw.h>

#include "soundidentificationprocessing.h"

const int   __outSize    = 5;
const char *__baseName   = "/soundidentification/";
const char *__configFile = "sound.ini";

int main(int argc, char* argv[])
{
	const int N   = 200;
	
	int    counter = 0;
	int    size    = 0;
	double time1   = 0.0;
	double time2   = 0.0;
	double period  = 0.0;
	
	YARPScheduler::setHighResScheduling();

	//----------------------------------------------------------------------
	// Port declarations 
	//----------------------------------------------------------------------
	YARPInputPortOf<YARPSoundBuffer> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP); 	
	YARPOutputPortOf<YVector> _outPort_srm(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

	SoundIdentificationProcessing _soundIndprocessor(__configFile,  __outSize);
	
	size = _soundIndprocessor.GetSize();

	YARPString base1(__baseName);
	YARPString base2(__baseName);
	_inPort.Register(base1.append("i").c_str());
	_outPort_srm.Register(base2.append("srmo").c_str());

	// The vector for the MFCC coefficients
	YVector _out_mfcc(L_VECTOR_MFCC);

	time1 = YARPTime::GetTimeAsSeconds();

	//----------------------------------------------------------------------
	// Main loop.
	//----------------------------------------------------------------------
	while(true)
	{
		counter++;
		_inPort.Read(); // Read sound stream from the network

        _soundIndprocessor.apply(_inPort.Content(),_out_mfcc); 
		
		// Sends the mfcc coefficients
		_outPort_srm.Content() = _out_mfcc;
		_outPort_srm.Write();

		//----------------------------------------------------------------------
		//  Time calculation stuff
		//----------------------------------------------------------------------
		time2 = time1;
		time1 = YARPTime::GetTimeAsSeconds();

		period += (time1-time2);
		if (counter == N)
		{
			printf("average= %lf \n", period/N);
			period = 0.0;
			counter = 0;
		}
	}
	return 0;
}
