// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundidentificationprocessing.cpp
// 
//     Description:  Implements all the sound identification processing algorithms.
//     This implementatin is partially based in the sound software used by Lorenzo Natale
//     is his master thesis.
// 
//         Version:  $Id: soundidentificationprocessing.cpp,v 1.1 2004-06-03 17:13:29 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "soundidentificationprocessing.h"
#include <YARPConfigFile.h>

const double __soundgain = 0.01;
//--------------------------------------------------------------------------------------
//       Class: SoundIdentificationProcessing
//      Method: SoundIdentificationProcessing
// Description: The constructor of the class
//--------------------------------------------------------------------------------------
SoundIdentificationProcessing::SoundIdentificationProcessing(const YARPString &iniFile, int outsize)
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
	_InputBufferLength  = 8192;
	_iniFile       = iniFile;
	_outSize       = outsize;
	_sombufferlengthinsec = 10;

	_path.append(root);
	_path.append("/conf/babybot/"); 

	//----------------------------------------------------------------------
	//  Just recover from the configuration file the real sound parameters the user
	//  is using  
	//----------------------------------------------------------------------
	file.set(_path, _iniFile);
	file.get("[GENERAL]"   , "Channels"             , &_Channels             , 1);
	file.get("[GENERAL]"   , "SamplesPerSec"        , &_SamplesPerSec        , 1);
	file.get("[GENERAL]"   , "BitsPerSample"        , &_BitsPerSample        , 1);
	file.get("[GENERAL]"   , "BufferLength"         , &_InputBufferLength    , 1);
	file.get("[SOMBUFFER]" , "BufferLengthInSec"    , &_sombufferlengthinsec , 1);

    input_numSamples = _InputBufferLength/4;
    numFreqSamples   = input_numSamples/2 + 1;                      
    total_numSamples  = _SamplesPerSec * _sombufferlengthinsec;

	counter = 0; 

	//----------------------------------------------------------------------
	//  I give the max possible value to the parameters max_factors and max_perm
	//  May be to ajust this was necessary in the original fortran code to be
	//  use in computers with low memory.
	//----------------------------------------------------------------------
	fft = new YARPFft(total_numSamples, total_numSamples);

	Re = new double[total_numSamples]; // this contains the Re of both channels
	Im = new double[total_numSamples]; // this contains the Im of both channels

}

//--------------------------------------------------------------------------------------
//       Class: SoundIdentificationProcessing
//      Method: ~SoundIdentificationProcessing 
// Description: The destructor 
//--------------------------------------------------------------------------------------
SoundIdentificationProcessing::~SoundIdentificationProcessing()
{
	delete fft;
	delete[] Re;
	delete[] Im;
}

//--------------------------------------------------------------------------------------
//       Class: SoundIdentificationProcessing
//      Method: ComputeDiscreteLevels 
// Description: Divides the spectrum in zones between the ILD_LOW_FREQ and FREQ_T. It calculates
// the energy in those chucks to create a vector of the energy distribution along the signal.
// This vector is later used to feed a Self Organizing map that learns diferent sounds. 
// We use only the data from the left channel.
//--------------------------------------------------------------------------------------
int
SoundIdentificationProcessing::ComputeDiscreteLevels(YVector &vector)
{
	int size = vector.Length();
	int chuck_freq = (FREQ_T - ILD_LOW_FREQ)/size;
	int low_freq   = ILD_LOW_FREQ;
	int high_freq  = ILD_LOW_FREQ + chuck_freq;
	int i = 0;

	//----------------------------------------------------------------------
	//  The signal spectrum has been already calculated 
	//----------------------------------------------------------------------
	while ( high_freq < FREQ_T)
	{
		vector(i) = squareMean(Re,
							   Im,
							   low_freq,
							   high_freq);
		i++;
		low_freq = high_freq;
		high_freq += chuck_freq;
	}

	return 0;
}

//--------------------------------------------------------------------------------------
//       Class: SoundIdentificationProcessing 
//      Method: squareMean 
// Description:  
//--------------------------------------------------------------------------------------
double 
SoundIdentificationProcessing::squareMean(double * channel_Re, double * channel_Im, double lowFreq, double highFreq)
{
	double energy = 0.0;

	int lowIndex  = int ((lowFreq/_SamplesPerSec)*total_numSamples);
	int highIndex = int ((highFreq/_SamplesPerSec)*total_numSamples);

	for(int i= lowIndex; i < highIndex; i++)
		energy = energy + 2*channel_Re[i];

	return energy;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  filter(input, lowFreq, highFreq)
// Description:  filters the spectrum with the low and hight frequencies passed as para
// meters. It is a generic filter. It cuts all the frequencies between the interval
// [lowFreq, highFreq]
// input - pointer to the frequencies vector. The fucntion modifies this input vector
//--------------------------------------------------------------------------------------
void SoundIdentificationProcessing::filter(double *input_Re, double *input_Im,
							 double lowFreq, double highFreq)
{
	int i = 0;
	int lowIndex  = int ((lowFreq  / _SamplesPerSec)*total_numSamples);
	int highIndex = int ((highFreq / _SamplesPerSec)*total_numSamples);

	ACE_ASSERT(highIndex < numFreqSamples);
	ACE_ASSERT(lowIndex  < highIndex);
	
	//----------------------------------------------------------------------
	//  Setting to cero the frequencies over the highindex
	//----------------------------------------------------------------------
	for ( i = highIndex; i < numFreqSamples - highIndex; i++ )
	{
		input_Re[i] = 0.0;
		input_Im[i] = 0.0;
	}
	// Original code
	/*************************************************************************
	 * nspzbZero(input+highIndex,numFreqSamples-highIndex); 				 *
	 *************************************************************************/
	
	//----------------------------------------------------------------------
	//  Setting to cero the frequencies under the lowIndex
	//  Note: is it sure this is correct???????
	//----------------------------------------------------------------------
#if 0
	if (lowIndex>0)
		for ( i = lowIndex; i < lowIndex; i++)
		{
			input_Re[i] = 0.0;
			input_Im[i] = 0.0;
		}
#endif
	//Original code
	/***************************************
	 * if (lowIndex>0)                     *
	 * nspzbZero(input+lowIndex,lowIndex); *
	 ***************************************/
}
