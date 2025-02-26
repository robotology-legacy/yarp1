/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                     ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: soundprocessing.h,v 1.18 2004-08-30 17:51:44 beltran Exp $
///

#ifndef __soundprocessinghh__
#define __soundprocessinghh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPMatrix.h>
#include <yarp/YARPPidFilter.h>
#include <yarp/YARPString.h>
#include <yarp/YARPSound.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPLogPolar.h>
#include <yarp/YARPFft.h>
#include "ITDBuffer.h"
#include "ILDBuffer.h"

#define FREQ_T 10000      // up cutting filter frequency
#define ILD_LOW_FREQ 2000 // down cutting frequency for ILD calculation
#define CORR_THRESHOLD 0
//#define ILD_THRESHOLD 10
#define ILD_THRESHOLD 3
#define NUM_ILD 10
#define NUM_ITD 10
#define L_VECTOR_SRM 20  // Lengh of the self reorganizing map vector data

typedef struct Thrshlds
{
	int nvalidpoints;
	int max_left;
	int max_right;
	int min_left;
	int min_right;
}   Thresholds;


class SoundProcessing
{
public:
	SoundProcessing(const YARPString &iniFile, int outsize);
	~SoundProcessing();

	/** 
	  * It transform the incoming buffer; applies the FFT and it calls the crosscorrelation
	  * and computelevel methods.
	  * 
	  * @param in The incoming sound buffer
	  * @param out The output vector with the sound parametrization
	  */
	inline void apply(YARPSoundBuffer &in, YVector &out)
	{
		unsigned char * buff = (unsigned char *) in.GetRawBuffer();
		int dim[1] = {numSamples};
		int threshold = 0;
		double * leftChannel = Re;
		double * rightChannel= Re + numSamples;
		
		//----------------------------------------------------------------------
		// Fill the Re and Im vectors from the sound buffer
		// The sound buffer is in unsigned chars. Each sound sample occupies 
		// two bytes. Right and left channel alternate inside the buffer
		// The bytes are reorganice in the correct order
		//----------------------------------------------------------------------
		for (int i = 0, j = numSamples; i < numSamples; i++, j++)
		{
			short temp;

            temp = buff[1] << 8; // More significant byte
            temp += buff[0];     // less significant byte
			Re[i] = (double) temp;
			Im[i] = 0.0;
			buff += 2;

            temp = buff[1] << 8; // More significant byte
            temp += buff[0];     // less significant byte
			Re[j] = (double) temp;
			Im[j] = 0.0;
			buff += 2;

			//----------------------------------------------------------------------
			//  Compute threshold
			//----------------------------------------------------------------------
			if ((fabs(leftChannel[i])>thresholds.max_left) || (fabs(rightChannel[i])>thresholds.max_right))
			{
				threshold = 0;
				break;
			}
			if ((fabs(leftChannel[i])>thresholds.min_left) || (fabs(rightChannel[i])>thresholds.min_right))
				threshold ++;
		}

		//----------------------------------------------------------------------
		//  Compute crosscorrelation in the time space
		//----------------------------------------------------------------------
		CrossCorrelation(Re, Re + numSamples);

		//----------------------------------------------------------------------
		//  Compute correlation in the frequency space
		//----------------------------------------------------------------------
		fft->Fft(1, dim, Re, Im, 1, -1);                           // Calculate first signal FFT
		fft->Fft(1, dim, Re + numSamples, Im + numSamples, 1, -1); // Calculate second signal FFT
		ComputeCrossCorrelation( Re, Im, Re + numSamples, Im + numSamples);

		//----------------------------------------------------------------------
		//  Compute levels for the ILD 
		//----------------------------------------------------------------------
		ComputeLevels();

		//----------------------------------------------------------------------
		//  Fill the itd and ild buffers controlling the thresholds 
		//----------------------------------------------------------------------
		CORR_DATA newITD;
		newITD.itd   = GetITD();
		newITD.shift = (double) GetShift();
		newITD.valid = 0;
		newITD.peak_level= GetCorrMax();

		LEVEL_DATA newILD;
		newILD.valid=0;
		GetILD(newILD.ild,newILD.left_level,newILD.right_level);

		if (threshold > thresholds.nvalidpoints)
		{
			if (newITD.peak_level > CORR_THRESHOLD )
			{
				newITD.valid=1;
				itd.PutItem(newITD);
			}

			if ((newILD.left_level > ILD_THRESHOLD) || 
				(newILD.right_level > ILD_THRESHOLD))
			{
				newILD.valid=1;
				ild.PutItem(newILD);
			}

			itd.SetGlobals(newITD.valid,newITD.peak_level);
			ild.SetGlobals(newILD.valid);
		}
		else
		{
			itd.SetGlobals(0,0.0);
			ild.SetGlobals(0);
		}

		itd_filtered = itd.GetSimpleMedia();
		ild_filtered = ild.GetMedia();

		//----------------------------------------------------------------------
		//  Calculate the vector with the motor commands
		//----------------------------------------------------------------------
		/*
		 * out(1) = (itd_filtered * _k_gain(1)) * degToRad; //This should be the pan
		 * out(2) = (ild_filtered * _k_gain(2)) * degToRad; //This should be the tilt
		 */
	}

	/** 
	  * It returns the ILD as explained in the Lorenzo Natale master thesis.
	  * The right and left channels energy are also returned
	  * 
	  * @param ild The external variable to asing the ild value 
	  * @param left The external variable to assing the left channel energy
	  * @param right The external variable to assing the right channel energy
	  */
	inline void GetILD(double &ild, double &left, double &right)
	{
		// Return the value of the ILD and the energy of the right and left channels
		left = squareMiddleValLeft;
		right= squareMiddleValRight;
		ild = (10 * log10(left/right));
	}

	inline double GetFilteredILD() { return ild_filtered.ild; }
	
	//--------------------------------------------------------------------------------------
	//      Method: GetITD 
	// Description: It returns the ITD in millisecconds 
	//--------------------------------------------------------------------------------------
	inline double GetITD()
	{
		return(((double (corrShift - shift)) / _SamplesPerSec) * 1e6);
	}

	inline double GetFilteredITD() { return itd_filtered.itd; }

	inline int GetShift() { return (corrShift-shift);}
	inline double GetCorrMax() { return corrMax;}

	inline double * GetCrossCorrelationBuffer(int tag) 
	{ 
        if ( !tag ) return corrVect; // Return correlation calculated in the time space
        else return corrVectFreq;    // Return correlation calculated in the frequency space
	}

	inline int GetSize() { return numSamples;}
	inline double GetMaxCC() { return corrMax; }

	int GetThresholds(Thresholds *);
	int SetThresholds(Thresholds *);

	void SetGain(double gain){ _k_gains = gain;}

private:
	int CrossCorrelation(double *, double *);
	int ComputeCrossCorrelation(double *,double *,double *,double *);
	int ConjComplexMultiplication(double *,double *,double *,double *,double *,double *);
	int ComplexMultiplication(double *, double *, double *, double *);
	int ComputeLevels();
	double squareMean(double * , double * , double, double);
	double scalarProduct(double *, double *, int);
	double correlation(double *, double *, int);
	void filter(double *, double *,double, double);
	
	void _threshold(double *v, double th)
	{
		// Surely, this is not necessary
	}

	int _inSize;
	int _outSize;

	YARPFft * fft;

	CITDBuffer itd;
	CILDBuffer ild;

	CORR_DATA  itd_filtered;
	LEVEL_DATA ild_filtered;

	Thresholds thresholds;
	//----------------------------------------------------------------------
	// sound variables 
	//----------------------------------------------------------------------
	int _Channels;
	int _SamplesPerSec;
	int _BitsPerSample;
	int _BufferLength;
	int _SCOTfiltering;
	int _TempCC;
	double _microphonesdistance;

    double squareMiddleValLeft;       // E{l-E{l}^2}
    double squareMiddleValRight;      // E{r-E{r}^2}

    double * Re;
    double * Im;
    double * crosscorrelation_Re;      // The correlation buffer (it is also used to store the spectrum when calculating the fft)
    double * crosscorrelation_Im;      // Well, really, the crosscorralation has not an imaginary part
                                       // this is just used to store the imaginary part of the fft that is later used
                                       // to obtain the crosscorrelation using the inverse fft
    double * leftcorrelation_Re;       // This is for the auto correlation of the left audio signal
    double * leftcorrelation_Im;       // Here applies the same discusion than in the crosscorrelation case
    double * rightcorrelation_Re;      // The same but for the right audio signal
    double * rightcorrelation_Im;
    double * middleSampleRight;
    double * middleSampleLeft;
    double * SCOToperator_Re;          // The buffer for the SCOToperator
    double * SCOToperator_Im;
    double * corrVect;
    double * corrVectFreq;
    double corrMax;                    // Maximum value in the correlation vector

    int numSamples;     // number of samples for channel
    int numFreqSamples; // length of the trasformations (N/2 + 1??)
    int bias;           // translates the coordinates origin (itd=0)
    int shift;          // maximum shift value between the two signals
    int corrShift;      // point where the maximum is placed
    int windowMax;      // 2*shift+1 numero de punti of the corralation vector
    int windowSize;     // windows used to calculate the correlation
	int timeMax;
    double windowTime;  // windowMax expressed in seconds

	YARPString _iniFile;
	YARPString _path;
	YVector _k_gains;
};
#endif
