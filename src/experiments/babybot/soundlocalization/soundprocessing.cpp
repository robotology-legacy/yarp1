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
//         Version:  $Id: soundprocessing.cpp,v 1.3 2004-04-15 14:37:36 beltran Exp $
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
	crosscorrelation = new double[numSamples];
	ans_Im = new double[numSamples];
	Re     = new double[2 * numSamples];
	Im     = new double[2 * numSamples];
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
	delete[] crosscorrelation;
	delete[] ans_Im;
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing 
//      Method: ComputeCorrelation2 
// Description: Calculate the correlation function using the Fourier Transform 
//--------------------------------------------------------------------------------------
int SoundProcessing::ComputeCrossCorrelation()
{
	int i = 0;
	int dim[1] = {numSamples};
	double *fft1_Re = Re;
	double *fft1_Im = Im;
	double *fft2_Re = fft1_Re + numSamples;
	double *fft2_Im = fft1_Im + numSamples;

	fft->Fft(1, dim, fft1_Re, fft1_Im, 1, -1); // Calculate FFT
	fft->Fft(1, dim, fft2_Re, fft2_Im, 1, -1); // Calculate FFT

	//----------------------------------------------------------------------
	//  Multiplication between the spectrum of the first signal and the complex
	//  conjugation of the spectrum of the second (discrete correlation theorem)
	//  (a+ib)(c-id) = (ac+bd)+i(bc-ad)
	//  The vector crosscorrelation is used to store the real part. Ans_Im for
	//  the imaginary part.
	//----------------------------------------------------------------------
	for	( i = 0; i < numSamples; i++) 
	{
		*(crosscorrelation+i) = ((*fft1_Re) * (*fft2_Re) + (*fft1_Im) * (*fft2_Im))/numSamples;
		*(ans_Im+i)           = ((*fft1_Im) * (*fft2_Re) - (*fft1_Re) * (*fft2_Im))/numSamples;
		fft1_Re++; fft1_Im++; fft2_Re++; fft2_Im++;
	}

	//----------------------------------------------------------------------
	//  Calculate the inverse Fast Fourier Transform
	//  At the end the crosscorrelation vector should contain the crosscorre-
	//  lation data
	//----------------------------------------------------------------------
	fft->Fft(1, dim, crosscorrelation, ans_Im, 1, -1);
	
	return 0;
}
