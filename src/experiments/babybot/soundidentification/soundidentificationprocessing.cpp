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
//         Version:  $Id: soundidentificationprocessing.cpp,v 1.3 2004-07-07 17:13:22 beltran Exp $
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
	_iniFile       = iniFile;
	_outSize       = outsize;
	_sombufferlengthinsec = 10;
	_InputBufferLength    = 8192;

	//----------------------------------------------------------------------
	//  Initialization of the filter bank parameters
	//----------------------------------------------------------------------
	lowestFrequency      = 133.3333;
	linearFilters        = 13;
	linearSpacing        = 66.66666666;
	logFilters           = 27;
	logSpacing           = 1.0711703;
	cepstralCoefficients = 13;

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
	fft = new YARPFft(numSamples, numSamples);

	Re = new double[numSamples]; // this contains the Re of both channels
	Im = new double[numSamples]; // this contains the Im of both channels
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

	int lowIndex  = int ((lowFreq/_SamplesPerSec)*numSamples);
	int highIndex = int ((highFreq/_SamplesPerSec)*numSamples);

	for(int i= lowIndex; i < highIndex; i++)
		energy = energy + 2*channel_Re[i];

	return energy;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  Triangularfilter(input, lowFreq, highFreq)
// Description:  filters the spectrum with the low and hight frequencies passed as para
// meters. It is a Triangular filter. It cuts all the frequencies out the interval
// [lowFreq, highFreq] defined by the triangular filter
// input - pointer to the frequencies vector. The function modifies this input vector
// The filters first are linear until "linearFilters". Later, they decrease in a log.
// The parameter linearSpacing and logSpacing determines the distribution of these filters
// in the frequency space.
// k - filter number
//--------------------------------------------------------------------------------------
void SoundIdentificationProcessing::Triangularfilter(double *input_Re, 
													 int k);
{
	int i         = 0;
	int lowIndex  = 0;
	int highIndex = 0;
	double lowFreq       = 0.0;
	double centerFreq    = 0.0;
	double upperFreq     = 0.0;
	double triangleHeigh = 0.0;
	double last_linear_freq = 0.0;

	//----------------------------------------------------------------------
	// Calculate the lowFreq, centerFreq and upperFreq of the filter 
	//----------------------------------------------------------------------
	if (k <= linearFilters)
	{
		lowFreq    = lowestFrequency + (k - 1) * linearSpacing;
		centerFreq = lowestFrequency +  k      * linearSpacing;
		upperFreq  = lowestFrequency + (k + 1) * linearSpacing;
	}
	else // In the case we are in the logarithmic filter distribution
	{
		last_linear_freq = lowestFrequency + k * linearSpacing;
		lowFreq    = last_linear_freq * pow(logSpacing, (k-linearFilters-1));
		centerFreq = last_linear_freq * pow(logSpacing, (k-linearFilters));
		upperFreq  = last_linear_freq * pow(logSpacing, (k-linearFilters+1));
	}

	//Calculate the height of the triangle filter
	triangleHeight = 2 / (upperFreq-lowFreq);
	
	//----------------------------------------------------------------------
	//  Calculate the indexes in the Spectral space
	//----------------------------------------------------------------------
	lowIndex    = int ((lowFreq    / _SamplesPerSec) * numSamples);
	centerIndex = int ((centerFreq / _SamplesPerSec) * numSamples);
	upperIndex  = int ((upperFreq  / _SamplesPerSec) * numSamples);
	
	ACE_ASSERT(upperIndex  < numFreqSamples);
	ACE_ASSERT(lowIndex    < centerIndex);
	ACE_ASSERT(centerIndex < highIndex);

	//----------------------------------------------------------------------
	//  Apply the filter taking care to apply the correct triangle height
	//----------------------------------------------------------------------
	for ( i = 0; i < numFreqSamples; i++)
	{
		if ( i > lowIndex && i <= centerIndex)
			input_Re[i] = input_Re[i] * triangleHeight * ((i - lowIndex)/(centerIndex-lowerIndex));
		else if ( i > centerIndex && i < upperIndex)
			input_Re[i] = input_Re[i] * triangleHeight * ((upperIndex - i)/(upperIndex - centerIndex));
		else
			input_Re[i] = 0.0;
	}
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
	double hamm;

	hamm = 0.54 - 0.46 * cos( (2 * PI / (N -1)) * k );
	return hamm;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  Freq2Mel
// Description:  Transforms from frequency space into Mels space
//--------------------------------------------------------------------------------------
double
SoundIdentificationProcessing::Freq2Mel(double f)
{
	return 2595.037 * log10(1.0 + f / 700.0);
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  Mel2Freq
// Description:  
//--------------------------------------------------------------------------------------
double
SoundIdentificationProcessing::Mel2Freq(double Mel)
{
	return 700.0 * (pow(10.0, Mel / 2595.037) - 1.0);
}

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  PreAccent
// Description:  Get high frequencies stronger
//--------------------------------------------------------------------------------------
inline double
SoundIdentificationProcessing::PreAccent(double z)
{
	double factor = 0.9;
	return (z)?(1 - factor * pow(z,-1)) :(0);
}

/********************************************************************************************************************
	MFCC Functions
********************************************************************************************************************/

/* Create Mel Frequecy Cepstral Coefficients
 * s : signal to create MFCC's from
 * k : number of coefficients to create
 * n : size of hamming windows (must be a power of 2)
 * c : list of VECTOR which will receive MFCC's
 *     MUST BE UNINITIALIZED!
 */

//--------------------------------------------------------------------------------------
//       Class:  SoundIdentificationProcessing
//      Method:  CalculateMFCC
// Description:  Calculates the mfcc(Mel Frequency Cepstral Coefficients) of a signal
// Legent:
//		s : signal pointer
//		k : number of Mfcc coefficients
//--------------------------------------------------------------------------------------
bool
SoundIdentificationProcessing::CalculateMFCC(double * s, int k, YVector &c)
{
#if 0
	const mfcc_double fl=0,
		fh = 0.5;
	/* Creating Frames */
	mfcc_VECTORLIST frames;
	mfcc_VECTORLIST fftframes;
	mfcc_VECTORLIST idctframes;
	mfcc_UINT frame_number;
	mfcc_UINT i,j;

	frames.vector_number=0;
	frames.vector=0;
	frames.vector_dimension=0;

	if (s.signal_data_lenght < n /2)
	{
		return false;
	}
#endif


	//frame_number = (mfcc_UINT) (s.signal_data_lenght / (n / 2) ) - 1;

	//VectorListCreate(&frames, frame_number, n);

	//for ( i = 0 ; i < frame_number; ++i)
	//{
	for ( j = ((int)(n/2)) * i; j < ((int)(n/2)) * i + n; ++j )
	{
		// hamming ponderation
		frames.vector[i].data[j - ((int)(n/2)) * i] = PreAccent(s.signal_data[j]) *  HammingPonder(n,j - ((int)(n/2)) * i);
	}
	//	frames.vector[i].data_dimension=n;
	//}
	/* frame created*/

	/* computing FFT of each frame */
	fftframes.vector_number=0;
	fftframes.vector=0;
	fftframes.vector_dimension=0;
	VectorListCreate(&fftframes, frame_number,n);
	for ( i = 0 ; i < frame_number; ++i)
	{
		memcpy(fftframes.vector[i].data,frames.vector[i].data,n*sizeof(mfcc_double));
		rdft(n,1,fftframes.vector[i].data);
	}
	// warning: fftframes.vector[i].data[2*j] => Real part of fft
	//			fftframes.vector[i].data[2*j+1] => Im part of fft
	/* frame FFT computed */

	/* Creating Mel Filter bank */

    // création du banc de filtre (techniquement, il s'agit 
	// de k filtres passe bandes (de forme Hamming)
	// conversion de l'axe des abscisses (fréquences)
	// de Hertz en Mel
	mfcc_double melsize = Frq2Mel(s.fs/2);
	// en effet, dans cet échelle, le découpage sera
	// linéraire (donc de taille constante == melfiltersize!)
	// taille de chacun des filtres dans l'échelle des mels
	mfcc_double melfiltersize = 2 * (melsize / (k+1 +1));
	// explication: si k+1 est le nombre de filtre qu'il faut créer 
	// (car le 0ème est inutile donc il sera virer)
	// on veut faire un découpage de l'étendu melsize pour y mettre les
	// k+1 filtres mais qui se recouvrent 2 à deux par moitier:
	/* pour k+1=12 filtres:
        |------||------||------||------||------||------|
	|------||------||------||------||------||------|
    .0                                                 .melsize
	ou pour k+1=12 filtres:
	    |------||------||------||------||------||------|
	|------||------||------||------||------||------||------|
	.0                                                     .melsize
	si on découpe chaque étendu de filtre (|-----|) en 2
	on se rencompte qu'il y aura (k+1) + 1 "mini-filtres":
	pour
        |------||------||------||------||------||------|
	|------||------||------||------||------||------|
	on a
	|--||--||--||--||--||--||--||--||--||--||--||--||--| 13 mini-filtres
	un filtre sera 2 fois plus gros.
    */
	typedef struct MELFILTERREP_tag {mfcc_double begin_point , end_point;} MELFILTERREP;
	MELFILTERREP *melfilterrep;
	melfilterrep=(MELFILTERREP *) MEMALLOC(sizeof(MELFILTERREP)*(k+1));
	for (i = 0 ; i < k+1 ; ++i )
	{
		melfilterrep[i].begin_point = (i * (melfiltersize / 2));
		melfilterrep[i].end_point =  (i *  (melfiltersize / 2)) + melfiltersize;
	}

	mfcc_VECTORLIST melfilterbank;
	melfilterbank.vector_number=0;
	VectorListCreate(&melfilterbank,k+1, n/2);

	// creating mel filter bank
	// note: it's between frequencies 0 and n/2
	// cause of mirror effect (from FFT)
	mfcc_double a;
	mfcc_double b;

	for ( i = 0 ; i < k+1 ; ++i )
	{
		a = Mel2Frq(melfilterrep[i].begin_point) / (s.fs/2) * n/2;
		b = Mel2Frq(melfilterrep[i].end_point) / (s.fs/2) * n/2;
		memset(melfilterbank.vector[i].data,0,sizeof(mfcc_VECTOR));
		for (j = (mfcc_UINT) floor(a) ; j < (mfcc_UINT) floor(b) ; ++j)
		{
			melfilterbank.vector[i].data[j] = 
				HammingPonder((mfcc_UINT) (int) floor( floor(b) - floor(a) ),
					(mfcc_UINT) (int) floor(j - floor(a)));
		}
	}

	MEMFREE(melfilterrep);
	/* Mel Filter bank created */
	mfcc_VECTORLIST melframes;
	mfcc_UINT l;

	melframes.vector_number=0;
	melframes.vector=0;
	melframes.vector_dimension=0;
	VectorListCreate(&melframes, frame_number, k+1);
	// doing matrix multiplication 
	// for each frame
	for (i = 0; i < frame_number ; ++i)
	{
		// computing the mel frenquency scaled coefficients
		// computing each coefficient
		for (j = 0 ; j < k+1; ++j)
		{
			//doing c_ij = sum (a_il * b_lj, l=1..n/2)
			for (l = 0; l < (mfcc_UINT) n / 2; ++l)
			{
				/*  Computing coefficient from power spectrum: 
                                    _____________________        2
						        /  |                          \
				formula: ... x (   |  Re(fft)^2 + Im(fft)^2    )  ^         =  Re(fft)^2 + Im(fft)^2
							    \ \|                          /   |
								\_____________________________/   |
								         module                   power
										                           
				*/
				melframes.vector[i].data[j] += melfilterbank.vector[j].data[l] * ( pow(fftframes.vector[i].data[2*l], 2) + pow(fftframes.vector[i].data[2*l+1], 2) );
			}
			// take logarithm
			melframes.vector[i].data[j] = log10(melframes.vector[i].data[j]);
		}
	}

	// note: the 0'th coefficient seems to be always null (should not).
	// it's not important, because we will not use it (use the k coefficents between
	// 1'th and k+1'th coeff (corresponding to MFCC theory, 0'th coeff is unsual because it only
	// supplies energy information)
	// TODO: fix problem with 0'th coeff
	// DONE

	/* Computing idct(log|.|) of each frame */
	idctframes.vector_number=0;
	idctframes.vector=0;
	idctframes.vector_dimension=0;
	VectorListCreate(&idctframes, frame_number, k+1);
	for ( i = 0 ; i < frame_number; ++i)
	{
		//memcpy(idctframes.vector[i].data,melframes.vector[i].data,(k+1)*sizeof(double));
//        idctframes.vector[i].data[0] *= 0.5;
//		ddct(k+1,1,idctframes.vector[i].data); // idct
		idct(k+1,melframes.vector[i].data, idctframes.vector[i].data); // idct
//        for (j = 0; j < (k+1); ++j) 
//		{
//            idctframes.vector[i].data[j] *= 2.0 / (k+1);
//		}
		//ddct(k+1,-1,idctframes.vector[i].data);

	}
	/* MFCC created!!! */

	c->vector_number=0;
	c->vector=0;
	c->vector_dimension=0;
	VectorListCreate(c,frame_number,k);
	for ( i = 0 ; i < frame_number; ++i)
	{
		for ( j = 1 ; j < k + 1; ++j) // do not take the first coefficient!
		{
			c->vector[i].data[j-1]=idctframes.vector[i].data[j];
		}
	}
	VectorListFree(&frames);
	VectorListFree(&fftframes);
	VectorListFree(&idctframes);
	VectorListFree(&melfilterbank);
	VectorListFree(&melframes);
	return true;
}

