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
//     Description:  This module receives sound chucks and adds them to produce a sound stream
//     of some seconds. This sound stream is converted into frequency space using a FFT. The
//     frequency space is divided and the energy of each zone is calculated to produce a
//     energy vector of the signal (energy signature). This energy vector is feed to a self-organizing
//     map that learns the vector. Hopfully, this can be used to recognize sounds.
//     
// 
//         Version:  $Id: soundidentification.cpp,v 1.2 2004-06-14 16:33:14 beltran Exp $
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
	const int N    = 200;
	int counter    = 0;
	int size       = 0;
	double time1;
	double time2;
	double period = 0.0;
	
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

	// The vector for the self reorganizing algorithm
	YVector _out_srm(L_VECTOR_SRM);

	time1 = YARPTime::GetTimeAsSeconds();

	//----------------------------------------------------------------------
	// Main loop.
	//----------------------------------------------------------------------
	while(true)
	{
		counter++;
		_inPort.Read(); // Read sound stream from the network

        _soundIndprocessor.apply(_inPort.Content(),_out_srm); 
		
		// Sends energy mapping vector for the self reorganizing network
		_outPort_srm.Content() = _out_srm;
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
