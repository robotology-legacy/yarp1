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
//     This implementatin is partially based in the sound software used by Lorenzo Natale
//     is his master thesis.
// 
//         Version:  $Id: soundprocessing.cpp,v 1.13 2004-05-24 13:33:11 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "soundprocessing.h"
#include <YARPConfigFile.h>

const double __soundgain = 0.01;
//--------------------------------------------------------------------------------------
//       Class: SoundProcessing
//      Method: SoundProcessing
// Description: The constructor of the class
//--------------------------------------------------------------------------------------
SoundProcessing::SoundProcessing(const YARPString &iniFile, int outsize):
ild(NUM_ILD), itd(NUM_ITD)
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
	_SCOTfiltering = 0;

	_path.append(root);
	_path.append("/conf/babybot/"); 

	_k_gains.Resize(2); //This is the gain vector for the tilt and the pan
	_k_gains = __soundgain;

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
	file.get("[GENERAL]" , "SCOTfiltering"       , &_SCOTfiltering       , 1);
	file.get("[GENERAL]" , "TempCrossCorrelation", &_TempCC              , 1);

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
	corrVect            = new double[numSamples];
	crosscorrelation_Re = new double[2 * numSamples];
	corrVectFreq        = &crosscorrelation_Re[numSamples];
	crosscorrelation_Im = new double[numSamples];
	leftcorrelation_Re  = new double[numSamples];
	leftcorrelation_Im  = new double[numSamples];
	rightcorrelation_Re = new double[numSamples];
	rightcorrelation_Im = new double[numSamples];
	middleSampleRight   = new double[windowSize];
	middleSampleLeft    = new double[windowSize];
	SCOToperator_Re     = new double[numSamples];
	SCOToperator_Im     = new double[numSamples];
	Re = new double[2 * numSamples]; // this contains the Re of both channels
	Im = new double[2 * numSamples]; // this contains the Im of both channels

	//----------------------------------------------------------------------
	//  Initialize thresholds. These values have been optained from Lorenzo's
	//  code. May be the should be included in the sound.ini file and loaded
	//  dinamically....
	//----------------------------------------------------------------------
	thresholds.nvalidpoints = 200;
	thresholds.max_left     = 32000;
	thresholds.max_right    = 32000;
	thresholds.min_left     = 1000;
	thresholds.min_right    = 1000;
}

//--------------------------------------------------------------------------------------
//       Class: SoundProcessing
//      Method: ~SoundProcessing 
// Description: The destructor 
//--------------------------------------------------------------------------------------
SoundProcessing::~SoundProcessing()
{
	delete fft;
	delete[] corrVect;
	delete[] Re;
	delete[] Im;
	delete[] crosscorrelation_Re; 
    delete[] crosscorrelation_Im; 
	delete[] leftcorrelation_Re;
	delete[] leftcorrelation_Im;
	delete[] rightcorrelation_Re;
	delete[] rightcorrelation_Im;
	delete[] middleSampleRight;
	delete[] middleSampleLeft;
	delete[] SCOToperator_Re;
	delete[] SCOToperator_Im;
}


//--------------------------------------------------------------------------------------
//       Class: SoundProcessing 
//      Method: CrossCorralation 
// Description: Calculates the crosscorrelation in the time space 
//--------------------------------------------------------------------------------------
int
SoundProcessing::CrossCorrelation(double *lChannel, double *rChannel)
{
	int middleValLeft = 0;	//mean(lChannel, shift, timeMax);
	int middleValRight = 0; //mean(rChannel, shift, timeMax);

	double tempCorr = 0;
	int ind = shift; 
	
	for (int i=(shift+bias); i<(timeMax+bias); i++)
	{
		middleSampleLeft[i-(shift+bias)]=lChannel[i]; 
	}

	for (int z=0; z<windowMax; z++)
	{
		for (int i=windowMax-z; i<(windowMax-z+windowSize); i++)
		{
			middleSampleRight[i-windowMax+z]=rChannel[i]-middleValRight;
		}

		corrVect[z]=correlation(middleSampleLeft, middleSampleRight, windowSize);
		if (corrVect[z] > tempCorr)
		{
			tempCorr=corrVect[z];
			ind=z;
		}
	}

    corrMax   = tempCorr; // correlation peak
    corrShift = ind;      // shift between the two waves (itd)
	
	return 1;
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

	//----------------------------------------------------------------------
	//  Filter the signals
	//----------------------------------------------------------------------
	filter(left_Re  , left_Im  , 0 , FREQ_T);
	filter(right_Re , right_Im , 0 , FREQ_T);

	//----------------------------------------------------------------------
	//  Compute the complex conjugate multiplications
	//----------------------------------------------------------------------
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
	//  Appling SCOT filter is selected by the user. ( option in the 
	//  sound.ini file) 
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
	//  At the end the crosscorrelation_Re vector should contain the crosscorre-
	//  lation data
	//----------------------------------------------------------------------
	fft->Fft(1, dim, crosscorrelation_Re, crosscorrelation_Im, -1, -1);

	//----------------------------------------------------------------------
	//  Rearange the crosscorrelation_Re to have a coheren visualization of
	//  the crosscorrelation. Assign the corrVectFreq to the correct position
	//  inside crosscorrelation_Re
	//----------------------------------------------------------------------
	for (i = 0; i < numSamples; i++)
		crosscorrelation_Re[i + numSamples] = crosscorrelation_Re[i];

	corrVectFreq = &crosscorrelation_Re[numSamples - shift - bias];
	
	double tempCorr = 0.0;
	int ind = shift;
	
	//----------------------------------------------------------------------
	//  Calculate the crosscorrelation maximum 
	//----------------------------------------------------------------------
	double * ptempcross = corrVectFreq;

	for ( i = 0; i < windowMax; i++)
		if ( ptempcross[i] > tempCorr)
		{
			tempCorr = ptempcross[i];
			ind = i;
		}

	corrMax   = tempCorr;
	corrShift = ind;

	//----------------------------------------------------------------------
	//  Forcing the vector to be between +/- 1
	//----------------------------------------------------------------------
	if ( corrMax > 0)
		for ( i = 0; i < windowMax; i++)
			corrVectFreq[i] *= 1/corrMax;

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
// conjugation of the spectrum of the second (discrete correlation theorem). To find ins-
// piration have a look at Numerical Recipies book. Chapter "Correlation and Autocorrelation
// using the FFT (pag 545). 
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
SoundProcessing::ComplexMultiplication(double * a, double * b, double * c, double * d)
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

//--------------------------------------------------------------------------------------
//       Class:  SoundProcessing
//      Method:  scalarProduct
// Description:  
//--------------------------------------------------------------------------------------
double
SoundProcessing::scalarProduct(double * channel1, double *channel2, int sizeChannel)
{
	double scalar = 0;
	for (int i = 0; i < sizeChannel; i++)
	{
		scalar += double (channel1[i]) * channel2[i];
	}
	return scalar;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundProcessing
//      Method:  correlation
// Description:  Performs the temporal correlation 
//--------------------------------------------------------------------------------------
double
SoundProcessing::correlation(double *channel1, double *channel2, int sizeChannel)
{
	double llScalar = scalarProduct(channel1, channel1, sizeChannel);
	double lrScalar = scalarProduct(channel1, channel2, sizeChannel);
	double rrScalar = scalarProduct(channel2, channel2, sizeChannel);

	return lrScalar/ (sqrt(llScalar * rrScalar));
}

//--------------------------------------------------------------------------------------
//       Class:  SoundProcessing
//      Method:  GetThresholds(actual_thresholds)
// Description:  Return the actual thresholds values; it always returns 1
//--------------------------------------------------------------------------------------
int 
SoundProcessing::GetThresholds(Thresholds *actual_thresholds)
{
	actual_thresholds->nvalidpoints = thresholds.nvalidpoints;
	actual_thresholds->max_left     = thresholds.max_left;
	actual_thresholds->max_right    = thresholds.max_right;
	actual_thresholds->min_left     = thresholds.min_left;
	actual_thresholds->min_right    = thresholds.min_right;
	
	return 1;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundProcessing
//      Method:  SetThresholds(new_thresholds)
// Description:  Set the new thresholds values. If they are valid a 1 is returned
//--------------------------------------------------------------------------------------
int 
SoundProcessing::SetThresholds(Thresholds *new_thresholds)
{
	if ( ((new_thresholds->nvalidpoints)>=0)
		 &&((new_thresholds->max_left)  >=0)
		 &&((new_thresholds->max_right) >=0)
		 &&((new_thresholds->min_left)  >=0)
		 &&((new_thresholds->min_right) >=0) )
	{
		thresholds.nvalidpoints = new_thresholds->nvalidpoints;
		thresholds.max_left     = new_thresholds->max_left;
		thresholds.max_right    = new_thresholds->max_right;
		thresholds.min_left     = new_thresholds->min_left;
		thresholds.min_right    = new_thresholds->min_right;
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundProcessing
//      Method:  filter(input, lowFreq, highFreq)
// Description:  filters the spectrum with the low and hight frequencies passed as para
// meters. It is a generic filter. It cuts all the frequencies between the interval
// [lowFreq, highFreq]
// input - pointer to the frequencies vector. The fucntion modifies this input vector
//--------------------------------------------------------------------------------------
void SoundProcessing::filter(double *input_Re, double *input_Im,
							 double lowFreq, double highFreq)
{
	int i = 0;
	int lowIndex  = int ((lowFreq  / _SamplesPerSec)*numSamples);
	int highIndex = int ((highFreq / _SamplesPerSec)*numSamples);

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

