// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundprocessing.h
// 
//     Description:  Declaration of the SoundProcessing class
// 
//         Version:  $Id: soundprocessing.h,v 1.2 2004-04-14 14:48:23 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __soundprocessinghh__
#define __soundprocessinghh__

#include <YARPMatrix.h>
#include <YARPPidFilter.h>
#include <YARPString.h>
#include <YARPSound.h>
#include <YARPLogPolar.h>
#include <YARPFft.h>

#define FREQ_T 10000      // up cutting filter frequency
#define ILD_LOW_FREQ 2000 // down cutting frequency for ILD calculation

class SoundProcessing
{
public:
	SoundProcessing(const YARPString &iniFile, int outsize);
	~SoundProcessing();

	void apply(YARPSoundBuffer &in, YVector &out)
	{
		//----------------------------------------------------------------------
		// Here the soundprocesing should be done. 
		//----------------------------------------------------------------------
		int * temp = (int *) in.GetRawBuffer();

		for (int i = 0, j = numSamples; i < numSamples; i++, j++)
		{
			Re[i] = *temp++;
			Im[i] = 0.0;
			Re[j] = *temp++;
			Im[j] = 0.0;
		}

		ComputeCrossCorrelation();
	}

private:
	int ComputeCrossCorrelation();
	void _threshold(double *v, double th)
	{
		// Surely, this is not necessary
	}

	int _inSize;
	int _outSize;

	YARPFft * fft;
	//----------------------------------------------------------------------
	// sound variables 
	//----------------------------------------------------------------------
	int _Channels;
	int _SamplesPerSec;
	int _BitsPerSample;
	int _BufferLength;
	double _microphonesdistance;

	double * Re;
	double * Im;
	double * crosscorrelation; //this is also used as a working vector
	double * ans_Im;

    int numSamples;     // number of samples for channel
    int numFreqSamples; // length of the trasformations (N/2 + 1??)
    int bias;           // translates the coordinates origin (itd=0)
    int shift;          // maximum shift value between the two signals
    int windowMax;      // 2*shift+1 numero de punti of the corralation vector
    int windowSize;     // windows used to calculate the correlation
	int timeMax;
    double windowTime;  // windowMax expressed in seconds

	YARPString _iniFile;
	YARPString _path;
};
#endif
