// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundprocessing.cpp
// 
//     Description:  Implements all the sound processing algorithms.
// 
//         Version:  $Id: soundprocessing.cpp,v 1.5 2004-04-23 09:43:58 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "soundprocessing.h"
#include <YARPConfigFile.h>

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing
//      Method: SoundProcessing
// Description: The constructor of the class
//--------------------------------------------------------------------------------------
SoundProcessing::SoundProcessing(const YARPString &iniFile, int outsize)
{
	YARPConfigFile file;
	char *root = GetYarpRoot();

	//----------------------------------------------------------------------
	//  Default sound parameters. These are calculated to have a stream of
	//  sound data each 40 miliseconds (the same that for the images)
	//----------------------------------------------------------------------
	_Channels      = 2;
	_SamplesPerSec = 44100;
	_BitsPerSample = 16;
	_BufferLength  = 8192;
	_iniFile       = iniFile;
	_outSize       = outsize;
	_SCOTfiltering  = 0;

	_path.append(root);
	_path.append("/conf/babybot/"); 

	//----------------------------------------------------------------------
	//  Just recover from the configuration file the real sound parameters the user
	//  is using  
	//----------------------------------------------------------------------
	file.set(_path, _iniFile);
	file.get("[GENERAL]" , "Channels"            , &_Channels            , 1);
	file.get("[GENERAL]" , "SamplesPerSec"       , &_SamplesPerSec       , 1);
	file.get("[GENERAL]" , "BitsPerSample"       , &_BitsPerSample       , 1);
	file.get("[GENERAL]" , "BufferLength"        , &_BufferLength        , 1);
	file.get("[GENERAL]" , "MicrophonesDistance" , &_microphonesdistance , 1);
	file.get("[GENERAL]" , "SCOTfiltering"       , &_SCOTfiltering        , 1);

	double maxITD = _microphonesdistance/331.7;
	shift         = int((maxITD * _SamplesPerSec) + 0.5 );   // what is the 0.5 for?
	windowMax     = 2 * shift + 1;
	double tmp1   = ((double) windowMax) / _SamplesPerSec;
	double tmp2   = (_BufferLength / (4.0 * _SamplesPerSec));
	windowTime    = tmp2 - tmp1;
	windowSize    = int (windowTime * _SamplesPerSec);
	timeMax       = windowSize + shift;

    numSamples     = _BufferLength/4;  
    numFreqSamples = numSamples/2 + 1; // Check SP documentation

	bias = 0;

	//----------------------------------------------------------------------
	//  I give the max possible value to the parameters max_factors and max_perm
	//  May be to ajust this was necessary in the original fortran code to be
	//  use in computers with low memory.
	//----------------------------------------------------------------------
	fft = new YARPFft(numSamples, numSamples);

	// allocate vectors
	crosscorrelation_Re = new double[numSamples];
	crosscorrelation_Im = new double[numSamples];
	leftcorrelation_Re  = new double[numSamples];
	leftcorrelation_Im  = new double[numSamples];
	rightcorrelation_Re = new double[numSamples];
	rightcorrelation_Im = new double[numSamples];
	Re = new double[2 * numSamples];
	Im = new double[2 * numSamples];
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing
//      Method: ~SoundProcessing 
// Description: The destructor 
//--------------------------------------------------------------------------------------
SoundProcessing::~SoundProcessing()
{
	delete fft;
	delete[] Re;
	delete[] Im;
	delete[] crosscorrelation_Re; 
    delete[] crosscorrelation_Im; 
	delete[] leftcorrelation_Re;
	delete[] leftcorrelation_Im;
	delete[] rightcorrelation_Re;
	delete[] rightcorrelation_Im;
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing 
//      Method: ComputeCrossCorrelation 
// Description: Calculate the correlation function using the Fourier Transform 
//--------------------------------------------------------------------------------------
int 
SoundProcessing::ComputeCrossCorrelation(double * left_Re, double * left_Im,
										 double * right_Re, double * right_Im)
{
	int i = 0;
	int dim[1] = {numSamples};

	ConjComplexMultiplication(left_Re, left_Im,
							  right_Re, right_Im,
							  crosscorrelation_Re,
							  crosscorrelation_Im);
	ConjComplexMultiplication(left_Re, left_Im,
							  left_Re, left_Im,
							  leftcorrelation_Re,
							  leftcorrelation_Im);
	ConjComplexMultiplication(left_Re, left_Im,
							  right_Re, right_Im,
							  rightcorrelation_Re,
							  rightcorrelation_Im);

	
	//----------------------------------------------------------------------
	//  Consider here to apply the SCOT filtering
	//----------------------------------------------------------------------
	if ( _SCOTfiltering )
	{
		for( int i=0; i<numSamples; i++)
		{
			if ((rightcorrelation_Re[i] > 0.6) && (leftcorrelation_Re[i] > 0.6))
				SCOToperator_Re[i] = 1/sqrt(rightcorrelation_Re[i] * leftcorrelation_Re[i]);
			else
				SCOToperator_Re[i] = 0.0;

			SCOToperator_Im[i] = 0.0;
		}

		ComplexMultiplication(crosscorrelation_Re, 
							  crosscorrelation_Im,
							  SCOToperator_Re, 
							  SCOToperator_Im);
	}
	
	//----------------------------------------------------------------------
	//  Calculate the inverse Fast Fourier Transform
	//  At the end the crosscorrelation vector should contain the crosscorre-
	//  lation data
	//----------------------------------------------------------------------
	fft->Fft(1, dim, crosscorrelation_Re, crosscorrelation_Im, 1, -1);
	
	return 0;
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing
//      Method: ComputeLevels 
// Description: This method calculates the ILD 
//--------------------------------------------------------------------------------------
int
SoundProcessing::ComputeLevels()
{
	//----------------------------------------------------------------------
	//  The left and right correlation spectrums have been already calculated 
	//  in the CrossCorrelation computation
	//----------------------------------------------------------------------
	squareMiddleValLeft  = squareMean(leftcorrelation_Re,
									  leftcorrelation_Im,
									  ILD_LOW_FREQ,
									  FREQ_T);
	squareMiddleValRight = squareMean(rightcorrelation_Re,
									  rightcorrelation_Im,
									  ILD_LOW_FREQ,
									  FREQ_T);

	return 0;
}
//--------------------------------------------------------------------------------------
//       Class: SoundProcessing 
//      Method: ConjComplexMultiplication 
// Description: Multiplication between the spectrum of the first signal and the complex
// conjugation of the spectrum of the second (discrete correlation theorem) 
// (a+ib)(c-id)=(ac+bd)+i(bc-ad)
//--------------------------------------------------------------------------------------
int 
SoundProcessing::ConjComplexMultiplication(double * a, double * b,
										   double * c, double * d,
										   double * ans_Re , double * ans_Im)
{
	for	( int i = 0; i < numSamples; i++) 
	{
		*(ans_Re+i) = ((*a) * (*c) + (*b) * (*d))/numSamples;
		*(ans_Im+i) = ((*b) * (*c) - (*a) * (*d))/numSamples;
		a++; b++; c++; d++;
	}

	return 0;
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing 
//      Method: ComplexMultiplication 
// Description: Complex multiplication
// (a+ib)(c+id)=(ac+bd)+i(bc+ad)
//--------------------------------------------------------------------------------------
int 
SoundProcessing::ComplexMultiplication(double * a, double * b,
										   double * c, double * d)
{
	double temp_a;
	
	for	(int i = 0; i < numSamples; i++) 
	{
		temp_a = (*a);
		*(a)   = ((*a) * (*c) + (*b)     * (*d))/numSamples;
		*(b)   = ((*b) * (*c) + (temp_a) * (*d))/numSamples;
		a++; b++; c++; d++;
	}

	return 0;
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing 
//      Method: squareMean 
// Description:  
//--------------------------------------------------------------------------------------
double 
SoundProcessing::squareMean(double * channel_Re, double * channel_Im, double lowFreq, double highFreq)
{
	double energy = 0.0;

	int lowIndex  = int ((lowFreq/_SamplesPerSec)*numSamples);
	int highIndex = int ((highFreq/_SamplesPerSec)*numSamples);

	for(int i= lowIndex; i < highIndex; i++)
		energy = energy + 2*channel_Re[i];

	return energy;
}
