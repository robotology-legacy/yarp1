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
//         Version:  $Id: soundidentificationprocessing.cpp,v 1.8 2004-10-15 14:35:28 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "soundidentificationprocessing.h"

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
	_iniFile       = iniFile;
	_outSize       = outsize;
	_sombufferlengthinsec = 10;
	_InputBufferLength    = 8192;

	//----------------------------------------------------------------------
	//  Initialization of the filter bank parameters, this should be added
	//  later to the initialization file
	//----------------------------------------------------------------------
	lowestFrequency      = 133.3333;
	linearFilters        = 13;
	linearSpacing        = 66.66666666;
	logFilters           = 27;
	logSpacing           = 1.0711703;
	cepstralCoefficients = 13;

	totalfilters = linearFilters + logFilters;

	filters_energy_vector.Resize(totalfilters);

	_path.append(root);
	_path.append("/conf/babybot/"); 

	//----------------------------------------------------------------------
	//  Just recover from the configuration file the real sound parameters the user
	//  is using  
	//----------------------------------------------------------------------
	file.set(_path, _iniFile);
	file.get("[GENERAL]"   , "Channels"          , &_Channels             , 1);
	file.get("[GENERAL]"   , "SamplesPerSec"     , &_SamplesPerSec        , 1);
	file.get("[GENERAL]"   , "BitsPerSample"     , &_BitsPerSample        , 1);
	file.get("[GENERAL]"   , "BufferLength"      , &_InputBufferLength    , 1);
	file.get("[SOMBUFFER]" , "BufferLengthInSec" , &_sombufferlengthinsec , 1);

    numSamples     = _InputBufferLength/4;
    numFreqSamples = numSamples/2 + 1;

	counter = 0; 

	//----------------------------------------------------------------------
	//  I give the max possible value to the parameters max_factors and max_perm
	//  May be to ajust this was necessary in the original fortran code to be
	//  use in computers with low memory.
	//----------------------------------------------------------------------
	//fft = new YARPFft(numSamples, numSamples);
	fft = new YARPFft(2, 11);

	Re = new double[numSamples]; // this contains the Re of both channels
	Im = new double[numSamples]; // this contains the Im of both channels
	_pSoundData = new double[numSamples]; // This contains the sound samples of
										  // one of the channels.
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
	delete[] _pSoundData;
	delete[] Im;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  Triangularfilter(input, lowFreq, highFreq)
// Description:  
//
// Filters the spectrum with a triangular filter between low and hight frequencies.
// The number of the filter is specify with the k parameter. 
// This function applies a filter bank of filters when called from a loop with variable k.
// The filter first are linear until k reaches 'linerarFilters' then the filters decrease
// in a logarithmic fasion.
// The function return the log-energy respose of the filter.
// Input parameters
// 		input_Re - a pointer to the furier spectrum
// 		k - filter number
// Output parameters
//		log-energy of the 'k' filter
//--------------------------------------------------------------------------------------
double 
SoundIdentificationProcessing::Triangularfilter(const double *input_Re, 
												int k)
{
	int i                   = 0;
	double filter_output    = 0.0;
	double energy           = 0.0;
	double lowFreq          = 0.0;
	double centerFreq       = 0.0;
	double upperFreq        = 0.0;
	double triangleHeight   = 0.0;
	double last_linear_freq = 0.0;
	double fftFreq          = 0.0;

	//----------------------------------------------------------------------
	// Calculate the lowFreq, centerFreq and upperFreq of the filter 
	//----------------------------------------------------------------------
	k++;
	if (k <= linearFilters)
	{
		lowFreq    = lowestFrequency + (k - 1) * linearSpacing;
		centerFreq = lowestFrequency +  k      * linearSpacing;
		upperFreq  = lowestFrequency + (k + 1) * linearSpacing;
	}
	else // In the case we are in the logarithmic filter distribution
	{
		last_linear_freq = lowestFrequency + (linearFilters * linearSpacing);
		lowFreq          = last_linear_freq * pow(logSpacing, (k-linearFilters+1));
		centerFreq       = last_linear_freq * pow(logSpacing, (k-linearFilters+2));
		upperFreq        = last_linear_freq * pow(logSpacing, (k-linearFilters+3));
	}

	//Calculate the height of the triangle filter
	triangleHeight = (double)2.0 / (double)(upperFreq-lowFreq);

	//----------------------------------------------------------------------
	//  Apply the filter taking care to apply the correct triangle height
	//  Acumulate the energy response of the filter
	//----------------------------------------------------------------------
	for ( i = 0; i < numSamples; i++)
	{
		int fftsize = numSamples;
		fftFreq = (((double)i/(double)fftsize) * (double)_SamplesPerSec);

		if ( fftFreq > lowFreq && fftFreq <= centerFreq)
			filter_output = input_Re[i] * triangleHeight * ((double)(fftFreq - lowFreq)/(double)(centerFreq - lowFreq));
		else if ( fftFreq > centerFreq && fftFreq < upperFreq)
			filter_output = input_Re[i] * triangleHeight * ((double)(upperFreq - fftFreq)/(double)(upperFreq - centerFreq));
		else
			filter_output = 0.0;

		energy += filter_output;
	}

	return log10(energy);
}

//--------------------------------------------------------------------------------------
//       Class: SoundIdentificationProcessing 
//      Method: ConjComplexMultiplication 
// Description: Multiplication between the normal spectrum and the conj of the same spectrum.
// This is used to optain printable spectrum (usual fourier graphical representation)
// (a+ib)(a-ib)=(aa+bb)+i(ba-ab) = (aa+bb)
//--------------------------------------------------------------------------------------
int 
SoundIdentificationProcessing::ConjComplexMultiplication(double * a, 
														 double * b)
{
	double temp_a = 0.0;
 	double temp_b = 0.0;

	for	( int i = 0; i < numSamples; i++) 
	{
		temp_a = *a;
		temp_b = *b;
		*a = ((temp_a) * (temp_a) + (temp_b) * (temp_b))/numSamples;
		*b = ((temp_b) * (temp_a) - (temp_a) * (temp_b))/numSamples;
		a++; b++; 
	}

	return 0;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  HammingPonderation
// Description:  Calculates the k element of a hamming window
//--------------------------------------------------------------------------------------
double
SoundIdentificationProcessing::HammingPonderation(const unsigned int N,
												  const unsigned int k)
{
	double hamm = 0.54 - 0.46 * cos( (2 * PI / (N -1)) * k );
	return hamm;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  PreAccent
// Description:  Get high frequencies stronger
//--------------------------------------------------------------------------------------
double
SoundIdentificationProcessing::PreAccent(double z)
{
	double factor = 0.9;
	return (z)?(1 - factor * pow(z,-1)) :(0);
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  idct
// Description:  Discrete cosine transform
//  \begin{equation}
//   		c_i =\frac{2}{N} \sum_{k=1}^{N} Y_k \cos[i (k+0.5)\frac{\pi}{N}] , i = 1,2,\dots,M, 
//  \end{equation}
// 
// Ref: "Numerical Recipes in C", page 519
//--------------------------------------------------------------------------------------
void 
SoundIdentificationProcessing::idct(int size_in, 
									int size_out,
									double * data_in, 
									double * data_out)
{
	int i = 0;                                                                                                                                
	int k = 0;                                                                                                                                

	memset(data_out,0,sizeof(double)*size_out);                                                                                               
	for (  i=0; i < size_out; i++ )                                                                                                           
	{                                                                                                                                         
		for (  k=0 ; k < size_in ; k++ )                                                                                                      
			data_out[i] += data_in[k]*cos((double)(PI*i*(k+0.5))/(double)size_in);                                                           
		data_out[i] *= ((double)1.0/(double)sqrt(size_in/2));                                                                                 
		if( i == 0)                                                                                                                           
			data_out[i] *= (sqrt(2)/2);                                                                                                       
	}                                                                                                                                         
}

/** 
  * Truncates the sound array into a vector. The vector is resized internally.
  * 
  * @param length The length of the vector.
  * @param vector A refence vector where to truncate the sound samples.
  * 
  * @return YARP_OK success
  *         YARP_FAIL failiture
  */
int 
SoundIdentificationProcessing::TruncateSoundToVector(const int length,
													 YVector &vector)
{
	LOCAL_TRACE("SoundIdentificationProcessing: Entering TruncateSoundToVector");
	double _dPositionDiferential = 0.0; /** The position increment/decrement.       */
	double _dAproxPosition       = 0.0; /** Aproximate new position.                */
	double _dFractionalPart      = 0.0; /** Fractional part of the aprox position.  */
	double _dIntegerPart         = 0.0; /** The integer part of the aprox position. */
	int    _iPosition            = 0;   /** The position in integer format.         */

	vector.Resize(length); 

	_dPositionDiferential = (double)numSamples/(double)length;

	int i;
	for( i=0; i<length; i++){
		_dAproxPosition  = _dAproxPosition + _dPositionDiferential;
		_dFractionalPart = modf(_dAproxPosition,&_dIntegerPart);
		_iPosition = (_dFractionalPart < 0.5) ? _dIntegerPart : (_dIntegerPart+1);
		if (_iPosition < numSamples || _iPosition >= 0)
			vector[i] = _pSoundData[_iPosition];
	}

	return YARP_OK;
}

/** 
  * Normalize the sound array into a giving limit.
  * 
  * @param topLimit The top limit of the normalized range 
  * values.
  */
int
SoundIdentificationProcessing::NormalizeSoundArray( const double Limit)
{
	LOCAL_TRACE("SoundIdentificationProcessing: Entering NormalizeSoundArray");
	int i;

	for( i=0; i<numSamples; i++){
		_pSoundData[i] = _pSoundData[i] / (double)Limit; 
	}
	return YARP_OK;
}
