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
//         Version:  $Id: soundidentificationprocessing.h,v 1.14 2004-12-30 10:51:53 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __soundidentificationprocessinghh__
#define __soundidentificationprocessinghh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPMatrix.h>
#include <yarp/YARPPidFilter.h>
#include <yarp/YARPString.h>
#include <yarp/YARPSound.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPFft.h>

#define LOCAL_NTRACE 1

# define ACE_TRACE_IMPL(X) ACE_Trace ____ (ACE_LIB_TEXT (X), __LINE__, ACE_LIB_TEXT (__FILE__))

# if (LOCAL_NTRACE == 1)
#   define LOCAL_TRACE(X)
# else
#   define LOCAL_TRACE(X) ACE_TRACE_IMPL(X) 
#   include<ace/Trace.h>
# endif /* MY_NTRACE */

#define FREQ_T 10000	 // up cutting filter frequency
#define ILD_LOW_FREQ 1	 // down cutting frequency for ILD calculation
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
	inline int apply(YARPSoundBuffer &in, YVector &out, double &rms)
	{
		//LOCAL_TRACE("SoundIdentificationProcessing: Entering apply");
		//unsigned char * buff = (unsigned char *) in.GetRawBuffer();
		char * buff = (char *) in.GetRawBuffer();
		int dim[1] = {numSamples};
		int i = 0;
		double sum = 0.0;
		int bufferIncrement = 0; 

		switch ( _BitsPerSample ) {
			case 8: bufferIncrement  = 1; break;
			case 16: bufferIncrement = 2; break;
			default: bufferIncrement = 2; break;
		}

		if ( _Channels == 2)
			bufferIncrement *= 2;
		
		//----------------------------------------------------------------------
		// Fill the Re and Im vectors from the sound buffer
		// The sound buffer is in unsigned chars. Each sound sample occupies 
		// two bytes. Right and left channel alternate inside the buffer
		// The bytes are reorganice in the correct order
		// In this case I get only the data from one of the channels enough for
		// sound indentification.
		// The data is introduced in the Re_Hamm variable for later processing
		//----------------------------------------------------------------------
		for ( i = 0; i < numSamples; i++)
		{
			short shortemp;

			if ( _BitsPerSample == 16 ) {
				shortemp  = buff[1] << 8;            // More significant byte
				shortemp += buff[0];                 // less significant byte
				Re[i] = _pSoundData[i] = (double) shortemp;
			}
			else { //I assume the samples is 8 bits
				Re[i] = _pSoundData[i] = (double) buff[0];
			}

			sum  += (double)(Re[i] * Re[i]);
            Im[i] = 0.0;
            buff += bufferIncrement;
		}

		//----------------------------------------------------------------------
		//  Return sound RMS value.
		//----------------------------------------------------------------------
		rms = sqrt(sum/(double)numSamples);

		//----------------------------------------------------------------------
		//  From here I implement the MFCC (Mel Frequency Ceptrum Coefficients)
		//----------------------------------------------------------------------
		
#ifdef SAVEDATA
		/*----------------------------------------------------------------------
		 *  Write the signal in a file for comparation
		 *----------------------------------------------------------------------*/
		{
			FILE *f = fopen ("signal.txt", "a");
			for ( i = 0; i < numSamples; i++)
				fprintf(f,"%f ",Re[i]);
			fclose(f);
		}
#endif
		
		//----------------------------------------------------------------------
		//  Compute Hamming ponderation and PreAccent
		//----------------------------------------------------------------------
		for ( i = 0; i < numSamples; i++)
			//Re[i] = PreAccent(Re[i]) * HammingPonderation(numSamples, i);
			Re[i] = Re[i] * HammingPonderation(numSamples,i);

#ifdef SAVEDATA
		/*----------------------------------------------------------------------
		 *  Write the signal in a file for comparation
		 *----------------------------------------------------------------------*/
		{
			FILE *f = fopen ("./output/signalhamming.txt", "w");
			for ( i = 0; i < numSamples; i++)
				fprintf(f,"%f ",Re[i]);
			fclose(f);
		}
#endif

		//----------------------------------------------------------------------
		//  Compute FFT of the signal
		//----------------------------------------------------------------------
		fft->Fft(1, dim, Re, Im, 1, -1); 

		//----------------------------------------------------------------------
		//  Compute the energy for the signal
		//----------------------------------------------------------------------
		ConjComplexMultiplication(Re, Im);

#ifdef SAVEDATA
		/*----------------------------------------------------------------------
		 *  Write the signal in a file for comparation
		 *----------------------------------------------------------------------*/
		{
			FILE *f = fopen ("./output/fftenergy.txt", "a");
			for ( i = 0; i < numSamples/2; i++)
				fprintf(f,"%f ",Re[i]);
			fprintf(f,"\n");
			fclose(f);
		}
#endif

		//----------------------------------------------------------------------
		//  Calculate the Mel-Filter bank spectral response and get the log10
		//  energy for each filter
		//----------------------------------------------------------------------
		for ( i = 0; i < totalfilters; i++)
			filters_energy_vector(1,i+1) = Triangularfilter(Re, i);
		
// Printing the mel filter result
#ifdef SAVEDATA
		{
			FILE *f = fopen ("./output/melscale.txt", "a");
			for ( i = 0; i < totalfilters; i++)
				fprintf(f,"%f ",filters_energy_vector(1,i+1));
			fprintf(f,"\n");
			fclose(f);
		}
#endif

		//----------------------------------------------------------------------
		//  Calulate ceptral coefficients
		//----------------------------------------------------------------------
		/*
        idct( filters_energy_vector.Length(), // the size of the filters_energy_vector
              out.Length(),                   // the size of the ccoefficients_vector
              filters_energy_vector.data(),   // the pointer to the vector of filters
              out.data());                    // the pointer to the vector of coefficients
		 */
		YMatrix result;
		result.Resize(1, cepstralCoefficients);
		result = filters_energy_vector * _mIdctMatrix;

		for ( i = 0; i < cepstralCoefficients; i++)
			out[i] = result(1,i+1);
				 
		//out = filters_energy_vector * _mIdctMatrix;
		
		//----------------------------------------------------------------------
		//  Testing to seee if the triangular filter is beeing generated correctly
		//----------------------------------------------------------------------
#if 0
		//memset(Re,1,sizeof(double)*numSamples); //Set Re all to one
		for (i=0;i<numSamples;i++)
			Re[i]=1.0;
		
		for ( i = 0; i < totalfilters; i++)     //Apply the filter
			filters_energy_vector[i] = Triangularfilter(Re, i);
		//Save the result in a file to be visualized
		{
			FILE *f = fopen ("filterout.txt", "a");
			for ( i = 0; i < totalfilters; i++)
				fprintf(f,"%f ",filters_energy_vector[i]);
			fprintf(f,"\n");
			fclose(f);
		}
#endif

		return 1; //We have a vector output
	}

	inline int GetSize() { return numSamples;}
	int TruncateSoundToVector(const int, YVector &);
	int NormalizeSoundArray(const double);
	int GetInteger(double);

private:

	//----------------------------------------------------------------------
	//  Private Functions
	//----------------------------------------------------------------------
	int ConjComplexMultiplication(double *, double *);
	void ComputeIDCMatrix( int, int, YMatrix &, int write = 0);
	void idct(int, int, double *, double *);
	double Triangularfilter(const double *, int);
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
	double * _pSoundData; /** Contains the sound data for one of the channels. */
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

	YMatrix _mIdctMatrix;

	YARPString _iniFile;
	YARPString _path;
	//YVector filters_energy_vector;
	YMatrix filters_energy_vector;
};
#endif
