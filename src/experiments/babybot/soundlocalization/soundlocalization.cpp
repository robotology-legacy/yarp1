// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundlocalization.cpp
// 
//     Description:  This is the main loop receiving the sound streams. Another class called
//     soundprocessing is used to perform all the analysis.
// 
//         Version:  $Id: soundlocalization.cpp,v 1.2 2004-04-14 14:48:23 beltran Exp $
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

#include "soundprocessing.h"

const int   __outSize    = 5;
const char *__baseName   = "/soundlocalization/";
const char *__configFile = "sound.ini";

int main(int argc, char* argv[])
{
	int    counter = 0;
	double time1;
	double time2;
	double period = 0.0;
	const int N   = 200;
	YARPScheduler::setHighResScheduling();

	YVector _out(__outSize); // This port should be used to send some motor vector to the head
	YARPInputPortOf<YARPSoundBuffer> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP); 	
	YARPOutputPortOf<YVector> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	SoundProcessing _soundprocessor(__configFile,  __outSize);

	YARPString base1(__baseName);
	YARPString base2(__baseName);

	_inPort.Register(base1.append("i").c_str());
	_outPort.Register(base2.append("o").c_str());

	YVector v(__outSize);
	v = 0.0;

	time1 = YARPTime::GetTimeAsSeconds();

	while(true)
	{
		counter++;
		_inPort.Read();
        _soundprocessor.apply(_inPort.Content(), v); // This is the sound buffer
        _outPort.Content() = v;                      // This is the control vector
		_outPort.Write();
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
