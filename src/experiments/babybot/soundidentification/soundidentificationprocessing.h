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
//     Description:  Declaration of the SoundIdentificationProcessing class
// 
//         Version:  $Id: soundidentificationprocessing.h,v 1.2 2004-06-14 16:33:14 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __soundidentificationprocessinghh__
#define __soundidentificationprocessinghh__

#include <conf/YARPConfig.h>
#include <YARPConfigFile.h>
#include <YARPMatrix.h>
#include <YARPPidFilter.h>
#include <YARPString.h>
#include <YARPSound.h>
#include <YARPImages.h>
#include <YARPLogPolar.h>
#include <YARPFft.h>


#define FREQ_T 10000    // up cutting filter frequency
#define ILD_LOW_FREQ 1  // down cutting frequency for ILD calculation
#define L_VECTOR_SRM 20 // Lengh of the self reorganizing map vector data

class SoundIdentificationProcessing
{
public:
	SoundIdentificationProcessing(const YARPString &iniFile, int outsize);
	~SoundIdentificationProcessing();

	//--------------------------------------------------------------------------------------
	//      Method: Apply  
	// Description: It transforms the buffer coming from the network, applies the FFT and 
	// computes the energy signature of the frequency space 
	//--------------------------------------------------------------------------------------
	inline int apply(YARPSoundBuffer &in, YVector &out)
	{
		unsigned char * buff = (unsigned char *) in.GetRawBuffer();
		int dim[1] = {numSamples};
		
		//----------------------------------------------------------------------
		// Fill the Re and Im vectors from the sound buffer
		// The sound buffer is in unsigned chars. Each sound sample occupies 
		// two bytes. Right and left channel alternate inside the buffer
		// The bytes are reorganice in the correct order
		// In this case I get only the data from one of the channels enough for
		// sound indentification.
		//----------------------------------------------------------------------
		for (int i = 0; i < numSamples; i++)
		{
			short temp;

            temp = buff[1] << 8; // More significant byte
            temp += buff[0];     // less significant byte
			Re[i] = (double) temp;
			Im[i] = 0.0;
			buff += 4;
		}

		//----------------------------------------------------------------------
		//  Compute correlation in the frequency space
		//----------------------------------------------------------------------
		fft->Fft(1, dim, Re, Im, 1, -1); // Calculate FFT of the signal

		//----------------------------------------------------------------------
		//  Compute the energy for the signal
		//----------------------------------------------------------------------
		ConjComplexMultiplication(Re, Im);

		//----------------------------------------------------------------------
		//  Compute discrete mapping of the energy.
		//  It generates a vector used to fill a self-organizing map to learn 
		//  sounds from different objects.
		//----------------------------------------------------------------------
		ComputeDiscreteLevels(out);
		
		return 1; //We have a vector output
	}

	inline int GetSize() { return numSamples;}

private:
	int ComputeDiscreteLevels(YVector &);
	double squareMean(double * , double * , double, double);
	void filter(double *, double *,double, double);
	int ConjComplexMultiplication(double *, double *);

	int _inSize;
	int _outSize;

	YARPFft * fft;

	//----------------------------------------------------------------------
	// sound variables 
	//----------------------------------------------------------------------
	int _Channels;
	int _SamplesPerSec;
	int _BitsPerSample;
	int _InputBufferLength;
    int _sombufferlengthinsec;

    double * Re;
    double * Im;
	int counter;

    int numSamples; // number of samples for channel in the incoming sound stream
    int numFreqSamples;   // length of the trasformations (N/2 + 1??)
    int total_numSamples; // The number of samples being analised by the Self Organizing Map

	YARPString _iniFile;
	YARPString _path;
};
#endif
