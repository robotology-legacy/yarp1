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
//         Version:  $Id: soundlocalization.cpp,v 1.11 2004-06-03 17:09:43 beltran Exp $
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

#include "soundprocessing.h"

const int   __outSize    = 5;
const char *__baseName   = "/soundlocalization/";
const char *__configFile = "sound.ini";
const int imgsizex  = 300;
const int imgsizey  = 255;
const int functionx = 50;

int main(int argc, char* argv[])
{
	const int N    = 200;
	int counter    = 0;
	int size       = 0;
	int tempfactor = 0;   // used to clean the image in function of a time variable
	double time1;
	double time2;
	double period = 0.0;
	double *_pcrosscorrelation     = NULL;
	double *_pfreccrosscorrelation = NULL;
	double scalefactor     = 0.0;
	double integer_part    = 0.0;
	double fractional_part = 0.0;
	double sccvector[100]; // scaled cross correlation vector
	double sccvector2[100]; // scaled cross correlation vector
	char * ppixel;
	int _x, _y;
	YARPImageOf<YarpPixelBGR> _sl_img; // Image to create the sound localization map
	//_sl_img.Resize (100, 255);
	_sl_img.Resize (imgsizex, imgsizey);
	YARPScheduler::setHighResScheduling();

	//----------------------------------------------------------------------
	// Port declarations 
	//----------------------------------------------------------------------
	YARPInputPortOf<YARPSoundBuffer> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP); 	
	YARPOutputPortOf<YVector> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPOutputPortOf<YVector> _outPort_srm(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPOutputPortOf<YARPGenericImage> _slimageoutPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);

	SoundProcessing _soundprocessor(__configFile,  __outSize);
	
	size = _soundprocessor.GetSize();

	YARPString base1(__baseName);
	YARPString base2(__baseName);
	YARPString base3(__baseName);
	YARPString base4(__baseName);

	_inPort.Register(base1.append("i").c_str());
	_outPort.Register(base2.append("o").c_str());
	_slimageoutPort.Register(base4.append("image:o").c_str());

	// Control vector declaration and initializantion
	YVector v(__outSize);
	v = 0.0;
	
	time1 = YARPTime::GetTimeAsSeconds();

    YarpPixelBGR pixr(128,64,0);
    YarpPixelBGR pixg(0,128,0);
	//----------------------------------------------------------------------
	// Main loop.
	//----------------------------------------------------------------------
	while(true)
	{
		int i;
		int j  = 0;
		double ild, left, right;
		counter++;
		_inPort.Read();
        _soundprocessor.apply(_inPort.Content(), v); // This is the sound buffer

		_x = _soundprocessor.GetFilteredITD();
		_y = _soundprocessor.GetFilteredILD();

		_y *= 10;

		// Move the origin to the center of the image
		_x = (imgsizex/2 + functionx) - _x;
		_y = (imgsizey/2) - _y;

		_sl_img.Zero(); // clear the image 

		AddCircle(_sl_img,pixr,_x,_y,10);
		AddCrossHair(_sl_img,pixg, (imgsizex/2 + functionx), (imgsizey/2), 5);

		//----------------------------------------------------------------------
		// crosscorrelation painting in the image 
		//----------------------------------------------------------------------
		_pcrosscorrelation     = _soundprocessor.GetCrossCorrelationBuffer(0);
		_pfreccrosscorrelation = _soundprocessor.GetCrossCorrelationBuffer(1);
		double max  = 0.0;
		double max2 = 0.0;
	
		for ( i = 0; i < functionx; i++)
		{
			// maping the crosscorrelation vector in the scaled crosscorrelation vector
            ///sccvector[i]  = (0.5 * sccvector[i])  + (0.5 * _pcrosscorrelation[i]);
            ///sccvector2[i] = (0.5 * sccvector2[i]) + (0.5 * _pfreccrosscorrelation[i]);
            sccvector[i]  = _pcrosscorrelation[i];
            sccvector2[i] = _pfreccrosscorrelation[i];
			
			if (sccvector[i] > max)
				max = sccvector[i];
			if (sccvector2[i] > max2)
				max2 = sccvector2[i];
		}

		//----------------------------------------------------------------------
		//  Paint the crosscorrelation functions in the left part of the image
		//----------------------------------------------------------------------
		for ( i = 0; i < functionx; i++)
		{
			int y  = (int)(sccvector[i]*125);
			int y2 = (int)(sccvector2[i]*125);

			y  = 250 - y;  // invert coordinates system. Painting in the lower-half part of the image
			y2 = 125 - y2; // invert coordinates system. Painting in the upper-half part of the image

			for ( j = 250; j > y && j > 0; j-- )
			{
				ppixel  = _sl_img.RawPixel(i,j);
				*ppixel = 230;
			}
			for ( j = 125; j > y2 && j > 0; j-- )
			{
				ppixel  = _sl_img.RawPixel(i,j);
				ppixel++; ppixel++;
				*ppixel = 130;
			}
		}

		/// sends the image.
		_slimageoutPort.Content().Refer(_sl_img);
		_slimageoutPort.Write();

		// sends the control vector
        _outPort.Content() = v;                    
		_outPort.Write();

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
