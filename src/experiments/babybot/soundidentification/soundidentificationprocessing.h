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
//         Version:  $Id: soundidentificationprocessing.h,v 1.4 2004-07-08 14:47:31 beltran Exp $
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


#define FREQ_T 10000     // up cutting filter frequency
#define ILD_LOW_FREQ 1   // down cutting frequency for ILD calculation
#define L_VECTOR_MFCC 13 // Lengh of the MFCC coefficients vector

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
		// The data is introduced in the Re_Hamm variable for later processing
		//----------------------------------------------------------------------
		for (int i = 0; i < numSamples; i++)
		{
			short temp;

            temp  = buff[1] << 8;  // More significant byte
            temp += buff[0];       // less significant byte
            Re[i] = (double) temp;
            Im[i] = 0.0;
            buff += 4;
		}

		//----------------------------------------------------------------------
		//  From here I implement the MFCC (Mel Frequency Ceptrum Coefficients)
		//----------------------------------------------------------------------

		//----------------------------------------------------------------------
		//  Compute Hamming ponderation and PreAccent
		//----------------------------------------------------------------------
		for (int i = 0; i < numSamples; i++)
			Re[i] = PreAccent(Re[i]) * HammingPonderation(numSamples, i);

		//----------------------------------------------------------------------
		//  Compute FFT of the signal
		//----------------------------------------------------------------------
		fft->Fft(1, dim, Re, Im, 1, -1); 

		//----------------------------------------------------------------------
		//  Compute the energy for the signal
		//----------------------------------------------------------------------
		ConjComplexMultiplication(Re, Im);

		//----------------------------------------------------------------------
		//  Calculate the Mel-Filter bank spectral response and get the log10
		//  energy for each filter
		//----------------------------------------------------------------------
		for (int i = 0; i < totalfilters; i++)
			filters_energy_vector[i] = TriangularFilter(Re, i);

		//----------------------------------------------------------------------
		//  Calulate ceptral coefficients
		//----------------------------------------------------------------------
        idct( filters_energy_vector.size(), // the size of the filters_energy_vector
              out.size(),                   // the size of the ccoefficients_vector
              filters_energy_vector.data(), // the pointer to the vector of filters
              out.data());                  // the pointer to the vector of coefficients

		return 1; //We have a vector output
	}

	inline int GetSize() { return numSamples;}

private:

	//----------------------------------------------------------------------
	//  Private Functions
	//----------------------------------------------------------------------
	int ConjComplexMultiplication(double *, double *);
	void idct(int, int, double, double);
	void Triangularfilter(const double *, const int);
	double HammingPonderation(const unsigned int, const unsigned int);
	double PreAccent(double);
	
	//----------------------------------------------------------------------
	//  Class private variables
	//----------------------------------------------------------------------
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

    double * Re; // Contains de data in succesive phase of the processing
                 // (1) first the sound samples in time space
                 // (2) next the real part of the FFT
                 // (3) next the energy of the FFT

    double * Im; // Contains the imaginary part of the FFT
	int counter;

    int numSamples;       // number of samples for channel in the incoming sound stream
    int numFreqSamples;   // length of the trasformations (N/2 + 1??)
    int total_numSamples; // The number of samples being analised by the Self Organizing Map

	//----------------------------------------------------------------------
	//  The filter bank parameters
	//----------------------------------------------------------------------
    int linearFilters;        // The number of linear filters
    int logFilters;           // The number of log filters
    int cepstralCoefficients; // The number of cepstral coefficients
    double lowestFrequency;   // The lowest frequency used by the filters
    double logSpacing;        // The separation between the log filters
    double linearSpacing;     // The separation between the linear filters

	int totalfilters;

	YARPString _iniFile;
	YARPString _path;
};
#endif
