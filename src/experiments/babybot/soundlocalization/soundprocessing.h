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
//     Description:  Declaration of the SoundProcessing class
// 
//         Version:  $Id: soundprocessing.h,v 1.11 2004-04-30 16:43:18 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __soundprocessinghh__
#define __soundprocessinghh__

#include <conf/YARPConfig.h>
#include <YARPConfigFile.h>
#include <YARPMatrix.h>
#include <YARPPidFilter.h>
#include <YARPString.h>
#include <YARPSound.h>
#include <YARPImages.h>
#include <YARPLogPolar.h>
#include <YARPFft.h>
#include "ITDBuffer.h"
#include "ILDBuffer.h"

#define FREQ_T 10000      // up cutting filter frequency
#define ILD_LOW_FREQ 2000 // down cutting frequency for ILD calculation
#define CORR_THRESHOLD 0
#define ILD_THRESHOLD 10
#define NUM_ILD 6
#define NUM_ITD 4

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

	//--------------------------------------------------------------------------------------
	//      Method: Apply  
	// Description: It transforms the buffer coming from the network, applies the FFT and the 
	// it calls the crosscorrlation and computelevel methods.
	//--------------------------------------------------------------------------------------
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
			// rimuovere PutItem ...
			// itd.PutItem(newITD);
			itd.SetGlobals(0,0.0);
			// ild.PutItem(newILD);
			ild.SetGlobals(0);
		}

		itd_filtered = itd.GetSimpleMedia();
		ild_filtered = ild.GetMedia();
	}

	//--------------------------------------------------------------------------------------
	//      Method: GetILD
	// Description: It returns the ILD as explained in the Lorenzo's master thesis, and the 
	// right and left channels energy
	//--------------------------------------------------------------------------------------
	inline void GetILD(double &ild, double &left, double &right)
	{
		// Return the value of teh ILD and the energy of the right and left channels
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
        if ( !tag ) return corrVect;     // Return correlation calculated in the time space
        else return corrVectFreq; // Return correlation calculated in the frequency space
	}

	inline int GetSize() { return numSamples;}
	inline double GetMaxCC() { return corrMax; }

	int GetThresholds(Thresholds *);
	int SetThresholds(Thresholds *);

private:
	int CrossCorrelation(double *, double *);
	int ComputeCrossCorrelation(double *,double *,double *,double *);
	int ConjComplexMultiplication(double *,double *,double *,double *,double *,double *);
	int ComplexMultiplication(double *, double *, double *, double *);
	double squareMean(double * , double * , double, double);
	int ComputeLevels();
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
    double * crosscorrelation_Re;     // The correlation buffer (it is also used to store the spectrum when calculating the fft)
    double * crosscorrelation_Im;     // Well, really, the crosscorralation has not an imaginary part
                                      // this is just used to store the imaginary part of the fft that is later used
                                      // to obtain the crosscorrelation using the inverse fft
    double * leftcorrelation_Re;      // This is for the auto correlation of the left audio signal
    double * leftcorrelation_Im;      // Here applies the same discusion than in the crosscorrelation case
    double * rightcorrelation_Re;     // The same but for the right audio signal
    double * rightcorrelation_Im;
	double * middleSampleRight;
	double * middleSampleLeft;
    double * SCOToperator_Re;        // The buffer for the SCOToperator
    double * SCOToperator_Im;
	double * corrVect;
	double * corrVectFreq;
    double corrMax;                  // Maximum value in the correlation vector

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
};
#endif
