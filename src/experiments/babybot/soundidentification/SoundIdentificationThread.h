/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                      ///
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
/// $Id: SoundIdentificationThread.h,v 1.5 2004-11-12 10:05:47 beltran Exp $
///

/** 
 * @file SoundIdentificationThread.h Implements a soundprocessing front-end.
 * This consist in a sound parametrization throuth a Short Time Fast Fourier
 * Transform procedure. Later, a MFCC (Mel-Frequency Cepstral Coefficients) is
 * implemented to provide a sound representation with a parameter reduced representation.
 * 
 * @author Carlos Beltran
 * @date 2004-08-24
 */
#ifndef __SoundIdentificationThreadh__
#define __SoundIdentificationThreadh__

#include <iostream>
#include <ace/Log_Msg.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPVectorPortContent.h>
#include <yarp/YARPString.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPSound.h>
#include <yarp/YARPSoundPortContent.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPBottleContent.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPList.h>

#include "YARPSoundTemplate.h"
#include "soundidentificationprocessing.h"
#include "YARPExMatrix.h"

#define MIN(a,b) (a>b ? b : a)
#define MAX(a,b) (a>b ? a : b)
#define MIN3(a,b,c) MIN(MIN(a,b),c)
#define MAX3(a,b,c) MAX(MAX(a,b),c)

#define MEMORYMAX 60
#define MIXELSMAX 40
#define MIXELTHRESHOLD 180

using namespace std;
using namespace _logpolarParams;

extern const int   __outSize;
extern const char *__baseName;
extern const char *__configFile;
extern bool _sharedmem;
extern int  _protocol;

class SoundImagePair
{
public:
	int weight;
	YARPSoundTemplate soundTemplate;
	YARPImageOf<YarpPixelBGR> image;

	SoundImagePair() {
		weight = 0; 
	}

	SoundImagePair& operator=( const SoundImagePair & srcpair) {
		soundTemplate = srcpair.soundTemplate; 
		image.CastCopy(srcpair.image);
		return (*this);
	}
};

/** 
 * Runs the main processing loop. 
 */
class SoundIdentificationThread: public YARPThread
{
private:
	
	int learningPhase;
	typedef YARPImageOf<YarpPixelBGR> ColorImage ;
	typedef YARPImageOf<YarpPixelMono> MonoImage;
	int _iSValue;								   /** Contains the time samples used for the time buffers.		  */
	double _dDecayValue;						   /** Decay value for the temporal images decay.				  */
	YARPSemaphore _sema;						   /** Semaphore to synchronize access to class data.			  */
	YARPSoundTemplate _soundTemplate;			   /** The sound template containing the mfcc parametrized sound. */
	YARPImageOf<YarpPixelMono> _imgMixelgram;	   /** The mixelgram image.										  */
	YARPImageOf<YarpPixelBGR>  _imgInput;		   /** The normal input image.									  */
	YARPImageOf<YarpPixelMono> _imgSoundHistogram; /** The Sound histogram.										  */
	YARPImageOf<YarpPixelMono> _imgHSHistogram;    /** The HS histogram.										  */
	YARPImageOf<YarpPixelMono> _newLogPolarImage ; /** The image with the segmented object.						  */
	YARPImageOf<YarpPixelMono> _inputLogPolarImage;
	YARPImageOf<YarpPixelBGR> _coloredImage;
	YARPLogpolar _logPolarMapper;
	YARPList<ColorImage> _imagesList;
	YARPList<MonoImage> _logPolarImagesList;
	YARPList<SoundImagePair> _pairList;
	const int __sizex;
	const int __sizey; 
	const int __histoWidth;
	const int __histoHeight;
	const int __hsHistoWidth;
	const int __hsHistoHeight;
	const int __iSoundHistogramHeight;
	const double __rmsthreshold;

public:
	/** 
	  * Constructor.
	  */
	SoundIdentificationThread();

	/** 
	  * Set the value of S (the temporal memory of the sound
	  * identification).
	  * 
	  * @param svalue The new value.
	  */
	void setSValue(const int &svalue);

	/** 
	  * Set the segmented image decaing factor; this is, the factor
	  * of forgivness.
	  * 
	  * @param dfactor The factor. 
	  */
	void setDecaingFactor(const double &dfactor);

	/** 
	  * sets value of the learningPhase variable.
	  * 
	  * @param value The value to be set.
	  */
	void setLearningState(const int value);

	/** 
	  * Get the value of the learning state.
	  * 
	  * @param value The value to be returned.
	  */
	int getLearningState() {
		 LOCAL_TRACE("SoundIdentification: Entering getLearningState");
		 return learningPhase;
	}

	/** 
	  * Returns the S value.
	  * 
	  * @return The S value.
	  */
	inline int getSValue() const {
		LOCAL_TRACE("SoundIdentification: Entering getSValue");
		return _iSValue;
	}
	/** 
	  * Returns the Decaing factor.
	  * 
	  * @return The decaing factor.
	  */
	inline double getDecaingFactor() const {
		LOCAL_TRACE("SoundIdentification: Entering getDecaingFactor");
		return _dDecayValue; 
	}

	/** 
	  * The main body of the thread.
	  */
	virtual void Body (void);

private:

	/** 
	  * Computes the mean of a vector of sound RMS (Root Mean Square)
	  * values.
	  * 
	  * @param rmsvector A pointer to the vector.
	  */
	void CalculateRMSMean(
		const double * rmsvector,
		const int vectorSize,
		double &rmsMean);

	/** 
	  * Transforms the RGB values into HSV values.
	  * 
	  * @param r Red. Range [0-1]
	  * @param g Green. Range [0-1]
	  * @param b Blue. Range [0-1]
	  * @param h Hue. Range [0-360]
	  * @param s Saturation. Range [0-1]
	  * @param v Value. Range [0-1]
	  */
	void RGBtoHSV( float, float, float, float &, float &, float & );

	/** 
	  * Computes the histogram. 
	  * 
	  * @param originalImage The original image.
	  * @param _imgHSHistogram The histogram to be filled.
	  * 
	  * @return YARP_OK
	  */
	int ComputeHSHistogram(
		YARPImageOf<YarpPixelBGR> &inputImage,
		YARPImageOf<YarpPixelMono> &imageHSHistogram
		);

	/** 
	  * Computes the segmented image segmenting those pixels that 
	  * have a significant mutual information value.
	  * The segmented pixels are calculated computing the mean of
	  * that pixel in the images buffer.
	  * 
	  * @param vImagesVector The vector with mono images. 
	  * @param mixelGramImage The mixelgram image.
	  * @param width The width of the images in the buffer.
	  * @param height The height of the images in the buffer.
	  * @param numberOfSamples The number of samples in the images buffer.
	  * @param memoryFactor How much remembers and object.
	  * @param decaingFactor How much decay in the new images.
	  * @param segmentedImage The output segmented image.
	  * 
	  * @return YARP_OK 
	  */
	int GetSegmentedImage (
		YARPListIterator<MonoImage> &imagesIterator,
		YARPImageOf<YarpPixelMono> &mixelGramImage,
		const int width,
		const int height,
		const int numberOfSamples,
		const int memoryFactor,
		const double decaingFactor,
		YARPImageOf<YarpPixelMono> &segmentedImage
		); 
	/** 
	 * Calculates the mixel using the mutual information from a sound stream and an image.
	 * @todo add mutual information formula.
	 * 
	 * @param mXtX The sound covariance matrix.
	 * @param vImgs Array of pointer to the images. 
	 * @param iSamples Number of samples.
	 * @param i     The mixel/pixel 'w' position in the image
	 * @param j     The mixel/pixel 'h' position in the image
	 * 
	 * @return The value of the mixel using the mutual information formula between the sound
	 * and the image.
	 */
	int calculateMixel(YARPCovMatrix &mXtX, 
		YARPImageOf<YarpPixelBGR> * vImgs, 
		int iSamples,
		int i, int j);

	/** 
	 * Calculates the mixel using the mutual information from a sound stream and an image.
	 * In this case we use the Rms of the sound sample.
	 * @todo add mutual information formula.
	 * 
	 * @param vRms A vector with the RMS sound values.
	 * @param vImgs Array of pointer to the images. 
	 * @param iSamples Number of samples.
	 * @param i     The mixel/pixel 'w' position in the image
	 * @param j     The mixel/pixel 'h' position in the image
	 * 
	 * @return The value of the mixel using the mutual information formula between the sound
	 * and the image.
	 */
	int calculateMixel2(
		double * vRms, 
		YARPListIterator<ColorImage> &imagesIterator,
		int iSamples,
		int i, int j,
		double &rmsmean);

};
#endif
