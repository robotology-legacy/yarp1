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
//         Version:  $Id: soundlocalization.cpp,v 1.3 2004-04-23 17:42:04 beltran Exp $
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
	double * _pcrosscorrelation = NULL;
	int size = 0;
	double scalefactor     = 0.0;
	double integer_part    = 0.0;
	double fractional_part = 0.0;
	int scalevector[256];
	double temp_scalevector[256];
	double sccvector[256]; // scaled cross correlation vector
	char * ppixel;
	YARPImageOf<YarpPixelBGR> _sl_img; // Image to create the sound localization map
	_sl_img.Resize (256, 256);
	YARPScheduler::setHighResScheduling();

	YVector _out(__outSize); 

	//----------------------------------------------------------------------
	// Port declarations 
	//----------------------------------------------------------------------
	YARPInputPortOf<YARPSoundBuffer> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP); 	
	YARPOutputPortOf<YVector> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPOutputPortOf<YARPGenericImage> _slimageoutPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);

	SoundProcessing _soundprocessor(__configFile,  __outSize);
	
	size = _soundprocessor.GetSize();

	YARPString base1(__baseName);
	YARPString base2(__baseName);
	YARPString base3(__baseName);

	_inPort.Register(base1.append("i").c_str());
	_outPort.Register(base2.append("o").c_str());
	_slimageoutPort.Register(base3.append("image:o").c_str());

	YVector v(__outSize);
	v = 0.0;

	time1 = YARPTime::GetTimeAsSeconds();

	//----------------------------------------------------------------------
	//  calculate the scalevector. Used later to map the crosscorrelation data
	//  into the soundlocalization image
	//----------------------------------------------------------------------
	scalefactor         = (double)size / (double)256;
	scalevector[0]      = 0.0;
	temp_scalevector[0] = 0.0;

	for (int i = 1; i < 256; i++)
	{
		temp_scalevector[i] = temp_scalevector[i-1] + scalefactor;
		fractional_part     = modf(temp_scalevector[i], &integer_part);
		scalevector[i]      = (fractional_part < 0.5) ? integer_part : (integer_part + 1);
	}

	//----------------------------------------------------------------------
	// Main loop.
	//----------------------------------------------------------------------
	while(true)
	{
		int i;
		counter++;
		_inPort.Read();
        _soundprocessor.apply(_inPort.Content(), v); // This is the sound buffer

		//----------------------------------------------------------------------
		//  soundlocalization image calculation
		//----------------------------------------------------------------------
		_pcrosscorrelation  = _soundprocessor.GetCrossCorrelationBuffer();
	
		for ( i = 0; i < 256; i++)
            sccvector[i] = _pcrosscorrelation[scalevector[i]]; // maping the crosscorrelation vector in the
                                                               // scaled crosscorrelation vector

		// Paint the scaled crosscorrelation function in the image
		for ( i = 0; i < 256; i++)
		{
			ppixel =  _sl_img.RawPixel(128,sccvector[i]);
			*ppixel = 255; // Just paint a white pixel
		}

		/// sends the image.
		_slimageoutPort.Content().Refer (_sl_img);
		_slimageoutPort.Write();

		////////////////////////////////////////////////////////////////////////
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
