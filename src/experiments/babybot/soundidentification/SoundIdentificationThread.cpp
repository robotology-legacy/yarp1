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
/// $Id: SoundIdentificationThread.cpp,v 1.8 2004-11-19 13:26:06 beltran Exp $
///

/** 
 * @file SoundIdentificationThread.cpp Implements a soundprocessing front-end.
 * This consist in a sound parametrization throuth a Short Time Fast Fourier
 * Transform procedure. Later, a MFCC (Mel-Frequency Cepstral Coefficients) is
 * implemented to provide a sound representation with a parameter reduced representation.
 * 
 * @author Carlos Beltran
 * @date 2004-08-24
 */

#include "SoundIdentificationThread.h"


//----------------------------------------------------------------------
//  Constructor.
//----------------------------------------------------------------------
SoundIdentificationThread::SoundIdentificationThread():
	__sizex(256),
	__sizey(256),
	__histoWidth(250),
	__histoHeight(100),
	__hsHistoWidth(360),
	__hsHistoHeight(10),
	__iSoundHistogramHeight(100),
	__rmsthreshold(300) {

	_iSValue     = MUTUALINFORMATIONMEMORY;
	_dDecayValue = 0.98;
	learningPhase = 1;
}

//----------------------------------------------------------------------
// SetSValue 
//----------------------------------------------------------------------
void SoundIdentificationThread::setSValue(const int &svalue) {
	LOCAL_TRACE("SoundIdenfication: Entering setSValue");

	if ( _iSValue > MUTUALINFORMATIONMEMORY )
		ACE_OS::fprintf(stdout,"Sorry, max S value superated\n");

	_sema.Wait(1); // Start Semaphore

	ACE_OS::fprintf(stdout,"Setting SValue = %d\n", svalue);
	_iSValue = svalue;

	if ( _imagesList.size() > _iSValue) {
		while ( _imagesList.size() != _iSValue ) {
			_imagesList.pop_front(); 
			_logPolarImagesList.pop_front();
			_rmsList.pop_front();
		}
	}

	_sema.Post(); // End Semaphore
}

//----------------------------------------------------------------------
// setDecaingFactor 
//----------------------------------------------------------------------
void SoundIdentificationThread::setDecaingFactor(const double &dfactor) {
	LOCAL_TRACE("SoundIdentification: Entering setDecaingFactor");

	if( dfactor > 1.0 || dfactor < 0.0)
		ACE_OS::fprintf(stdout,"Sorry, decaing factor must be between 0.0 and 1.0\n");

	_sema.Wait(1); // Start Semaphore 

	ACE_OS::fprintf(stdout,"Setting Decaying factor = %f\n",dfactor);
	_dDecayValue = dfactor;

	_sema.Post(); // End Semaphore
}

//----------------------------------------------------------------------
//  setLearningPhase
//----------------------------------------------------------------------
void SoundIdentificationThread::setLearningState( const int value ) {
	 LOCAL_TRACE("SoundIdentification: Entering setLearningPhase");

	 _sema.Wait(1);
	 ACE_OS::fprintf(stdout,"Setting Learning Phase = %d\n", value);
	 learningPhase = value;
	 _sema.Post();
}

//----------------------------------------------------------------------
// CalculateRMSMean 
//----------------------------------------------------------------------
void SoundIdentificationThread::CalculateRMSMean(
	//const double * rmsVector,
	YARPListIterator<double> &rmsIterator,
	const int vectorSize,
	double &rmsMean
	) {
	
	int i = 0;
	double rmsSum = 0.0; 
	rmsIterator.go_head();

	for ( i = 0; i < vectorSize; i++ ) {
		double &rmsvalue = *rmsIterator;
		//rmsSum += rmsVector[i]; 
		rmsSum += rmsvalue;
		rmsIterator++;
	}

	rmsMean = rmsSum / (double) vectorSize;
}

//----------------------------------------------------------------------
//  RGBtoHSV
//----------------------------------------------------------------------
void SoundIdentificationThread::RGBtoHSV( 
	float r, 
	float g, 
	float b, 
	float &h, 
	float &s, 
	float &v 
	) {
	float min = MIN3( r, g, b);
	float max = MAX3( r, g, b);
	float delta = max - min;

	v = max;			

	if( max != 0 ) {
		s = delta / max;		
	}
	else {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = -1;
		return;
	}
	if( r == max )
		h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		h = 4 + ( r - g ) / delta;	// between magenta & cyan
	h *= 60;				// degrees
	if( h < 0 )
		h += 360;
}

//----------------------------------------------------------------------
//  BackProjectHSHistogram
//----------------------------------------------------------------------
int SoundIdentificationThread::BackProjectHSHistogram(
	const int &hsMaximumWidth,
	const int &hsMaximumHeight,
	YARPImageOf<YarpPixelMono> &imageHSHistogram,
	YARPImageOf<YarpPixelBGR> &coloredImage,
	YARPImageOf<YarpPixelMono> &backProjectedImage
	) {

	int imageWidth  = backProjectedImage.GetWidth();
	int imageHeight = backProjectedImage.GetHeight();
	int histogramHeight = imageHSHistogram.GetHeight();
	int iHueWidthCoordinate = 0;
	int iSaturationHeightCoordinate = 0;
	float dHue        = 0.0;
	float dSaturation = 0.0;
	float dValue      = 0.0;
	double integerPart    = 0.0;
	double fractionalPart = 0.0;
	YarpPixelBGR pixelBGR;

	 
	for( int i = 0; i < imageWidth; i++) {
		for ( int j = 0; j < imageHeight; j++){

			pixelBGR.b = coloredImage.SafePixel(i,j).b;
			pixelBGR.g = coloredImage.SafePixel(i,j).g;
			pixelBGR.r = coloredImage.SafePixel(i,j).r;
				 
			RGBtoHSV(
				pixelBGR.r / (float) 255,
				pixelBGR.g / (float) 255,
				pixelBGR.b / (float) 255,
				dHue,
				dSaturation,
				dValue
				);

			iHueWidthCoordinate = dHue;
			dSaturation *= 10.0f;
			fractionalPart = modf( dSaturation, &integerPart);
			iSaturationHeightCoordinate = ( fractionalPart < 0.5) ? integerPart : integerPart +1;


			YarpPixelMono &hspixel = imageHSHistogram.SafePixel( 
				iHueWidthCoordinate, 
				histogramHeight - iSaturationHeightCoordinate
				);

			// Backproject pixel is HS values more than a threshold
			if ( hspixel < 250 ) {
				backProjectedImage(i,j) = 0;
			}

		}

	}
	return YARP_OK;
}

//----------------------------------------------------------------------
//  ComputeHSHistogram
//----------------------------------------------------------------------
int SoundIdentificationThread::ComputeHSHistogram (
	YARPImageOf<YarpPixelBGR> &inputImage,
	YARPImageOf<YarpPixelMono> &imageHSHistogram,
	int &hsMaximumWidth,
	int &hsMaximumHeight,
	int paintMax
	) {

	int imageWidth  = inputImage.GetWidth();
	int imageHeight = inputImage.GetHeight();
	int histogramHeight = imageHSHistogram.GetHeight();
	int iHueWidthCoordinate = 0;
	int iSaturationHeightCoordinate = 0;
	float dHue        = 0.0;
	float dSaturation = 0.0;
	float dValue      = 0.0;
	double integerPart    = 0.0;
	double fractionalPart = 0.0;
	int hsmaxValue = 0;
	YarpPixelBGR pixelBGR;

	imageHSHistogram.Zero();
	hsMaximumWidth = 0;
	hsMaximumHeight = 0;

	for( int i = 0; i < imageWidth; i++) {
		for ( int j = 0; j < imageHeight; j++){

			pixelBGR.b = inputImage.SafePixel(i,j).b;
			pixelBGR.g = inputImage.SafePixel(i,j).g;
			pixelBGR.r = inputImage.SafePixel(i,j).r;

			// Checking if is a valid pixel
			if ( ( pixelBGR.b != 0 ) 
				&& ( pixelBGR.g != 0 ) 
				&& ( pixelBGR.r != 0 )) {
				 
				RGBtoHSV(
					pixelBGR.r / (float) 255,
					pixelBGR.g / (float) 255,
					pixelBGR.b / (float) 255,
					dHue,
					dSaturation,
					dValue
					);

				iHueWidthCoordinate = dHue;
				dSaturation *= 10.0f;
				fractionalPart = modf( dSaturation, &integerPart);
				iSaturationHeightCoordinate = ( fractionalPart < 0.5) ? integerPart : integerPart +1;

				YarpPixelMono & hspixel = imageHSHistogram.SafePixel( 
					iHueWidthCoordinate, 
					histogramHeight - iSaturationHeightCoordinate
					); 

				hspixel++;

				if ( hspixel > hsmaxValue) {
					 hsmaxValue = hspixel;
					 hsMaximumWidth = iHueWidthCoordinate;
					 hsMaximumHeight = histogramHeight - iSaturationHeightCoordinate;
				}
			}
		}
	}

	if ( paintMax ) {
		YarpPixelMono whitepixel = 255;

		AddRectangle( 
			imageHSHistogram, 
			whitepixel, 
			hsMaximumWidth, 
			hsMaximumHeight, 
			7,
			2
			); 
		AddCircle( 
			imageHSHistogram, 
			whitepixel, 
			hsMaximumWidth, 
			hsMaximumHeight, 
			4
			); 
	}

	return YARP_OK;
}

//----------------------------------------------------------------------
//  GetSegmentedImage
//----------------------------------------------------------------------
int SoundIdentificationThread::GetSegmentedImage (
	YARPListIterator<MonoImage> &imagesIterator,
	YARPImageOf<YarpPixelMono> &mixelGramImage,
	const int width,
	const int height,
	const int numberOfSamples,
	const int memoryFactor,
	const double decaingFactor,
	YARPImageOf<YarpPixelMono> &segmentedImage
	) {

	int i = 0;
	int j = 0;
	int mixelValue   = 0;
	double pixelsSum = 0.0;

	ACE_ASSERT( numberOfSamples != 0);

	for(i = 2; i < width-2; i++ )
		for( j = 2; j < height-3; j++) {
			mixelValue = mixelGramImage(i,j);
			if ( mixelValue > MIXELTHRESHOLD) {
				imagesIterator.go_head();
				for (int z = 1; z <= numberOfSamples; z++) {
					YARPImageOf<YarpPixelMono>& pimg = *imagesIterator;
					pixelsSum += pimg(i,j);
					imagesIterator++;
				}
				segmentedImage(i,j) = pixelsSum / (double) numberOfSamples;
				pixelsSum = 0.0;
			}
			else {
				if ( memoryFactor > MEMORYMAX) {
					segmentedImage(i,j) = 0;
				}
				else {
					segmentedImage(i,j) = decaingFactor * segmentedImage(i,j);
				}
			}
		}
	return YARP_OK;
}
 
//----------------------------------------------------------------------
//  calculateMixel
//----------------------------------------------------------------------
int SoundIdentificationThread::calculateMixel(
	YARPListIterator<double> &rmsIterator,
	YARPListIterator<ColorImage> &imagesIterator,
	int iSamples,
	int i, 
	int j,
	double &rmsmean
	) {
	//LOCAL_TRACE("SoundIdentification: CalculateMixel2");

    int n         = 0;  /** Sound number of componects.                          */
    int m         = 0;  /** Image number of components.                          */
    double _dSX   = 0.0;
    double _dSX2  = 0.0;
    double _dSY   = 0.0;
    double _dSY2  = 0.0;
    double _dSXY  = 0.0;
    double _dR2   = 0.0; /* * Pearson correlation coeffiecient. */
    double _dSSxy = 0.0;
    double _dSSxx = 0.0;
    double _dSSyy = 0.0;
	double _dMaxRms = 0.0;
	double _dDampeningfactor = 0.0;
	const double _dalpha = 100.0;

	ACE_ASSERT( iSamples != 0 );

	YVector _vV;        /** Vector containing the gray values of pixels samples. */

    _vV.Resize(iSamples);
	
	imagesIterator.go_head();
	rmsIterator.go_head();
	//----------------------------------------------------------------------
	//  Fill PixelSamples matrix
	//----------------------------------------------------------------------
	for (int z = 1; z <= iSamples; z++)
	{
		//----------------------------------------------------------------------
		//  Compute sound associated values
		//----------------------------------------------------------------------
		double &prmsvalue = *rmsIterator;
		_dSX  += prmsvalue;
		_dSX2 += powf(prmsvalue,2);
		if ( prmsvalue > _dMaxRms)
			_dMaxRms = prmsvalue;
		
		//----------------------------------------------------------------------
		//  Compute image associated values.
		//----------------------------------------------------------------------
		YARPImageOf<YarpPixelBGR>& pimg = *imagesIterator;

		_vV(z) = 0.299 * pimg.SafePixel(i,j).r 
			+ 0.587 * pimg.SafePixel(i,j).g 
			+ 0.114 * pimg.SafePixel(i,j).b;

		imagesIterator++;
		rmsIterator++;

		_dSY  += _vV(z);
		_dSY2 += powf(_vV(z),2);

		//----------------------------------------------------------------------
		//  Compute common values.
		//----------------------------------------------------------------------
		_dSXY += ( prmsvalue * _vV(z) );
	}

	//Return rmsmean
	rmsmean = _dSX/(double)iSamples;
	if (rmsmean < __rmsthreshold) return 0;

	//----------------------------------------------------------------------
	//  Compute Pearson correlation coefficient.
	//----------------------------------------------------------------------
    _dSSxy = _dSXY  - ((_dSX * _dSY)/(double)iSamples);
    _dSSxx = _dSX2  - (powf(_dSX,2) /(double)iSamples);
    _dSSyy = _dSY2  - (powf(_dSY,2) /(double)iSamples);
    _dR2   = powf(_dSSxy,2)         /(double)(_dSSxx * _dSSyy);
	_dDampeningfactor = (1.0 - (1.0/(double)powf(2.0,(double)_dMaxRms*_dalpha)));

	//----------------------------------------------------------------------
	//  Compute mutual information
	//---------------------------------------------------------------------- 
	double value = (-1.0)*(0.5 * (logf((1.0 - _dR2))/(double)logf(2)));

	return(MIN(255,MAX(0,255*value)));
}

//----------------------------------------------------------------------
//  Body
//----------------------------------------------------------------------
void SoundIdentificationThread::Body (void)
{
	//LOCAL_TRACE("SoundIdenfication: Entering Body");
	const int N  = 200;
	const int N2 = 10 ;

	int    counter = 0;
	int    size    = 0;
	double time1   = 0.0;
	double time2   = 0.0;
	double period  = 0.0;

	YVector _vOutMfcc(L_VECTOR_MFCC);
	YVector _vShorterOutMfcc( L_VECTOR_MFCC - 1 );

	YARPListIterator<ColorImage> _imagesListIterator(_imagesList);
	YARPListIterator<MonoImage> _logPolarImagesListIterator(_logPolarImagesList);
	YARPListIterator<SoundImagePair> _pairListIterator( _pairList );
	YARPListIterator<double> _rmsListIterator( _rmsList );

	//----------------------------------------------------------------------
	//  Resize some of the images.
	//----------------------------------------------------------------------
	_imgInput.Resize(_stheta,_srho);
	_coloredImage.Resize(_stheta, _srho);
	_imgSoundHistogram.Resize(__histoWidth, __histoHeight);
	_imgHSHistogram.Resize(__hsHistoWidth, __hsHistoHeight);

	YARPScheduler::setHighResScheduling();

	//----------------------------------------------------------------------
	// Port declarations 
	//----------------------------------------------------------------------
	//LOCAL_TRACE("SoundIdentification: Declaring ports");
	YARPInputPortOf<YARPSoundBuffer>   _inpSound(YARPInputPort::NO_BUFFERS ,YARP_TCP);
	YARPInputPortOf<YARPGenericImage>  _inpImg(YARPInputPort::NO_BUFFERS, YARP_TCP);
	//YARPOutputPortOf<YVector>          _outpMfcc (YARPOutputPort::DEFAULT_OUTPUTS,YARP_UDP);
	YARPOutputPortOf<YARPGenericImage> _outpImg;
	YARPOutputPortOf<YARPGenericImage> _outprecImg;
	YARPOutputPortOf<YARPGenericImage> _outSoundHistogramPort;
	YARPOutputPortOf<YARPGenericImage> _outHSHistogramPort;

	//_inpSound.SetAllowShmem (_sharedmem);
	//_inpImg.SetAllowShmem   (_sharedmem);
	//_outpMfcc.SetAllowShmem (_sharedmem);
	_outpImg.SetAllowShmem  (_sharedmem);
	_outprecImg.SetAllowShmem(_sharedmem);
	_outSoundHistogramPort.SetAllowShmem(_sharedmem);
	_outHSHistogramPort.SetAllowShmem(_sharedmem);

	SoundIdentificationProcessing _soundIndprocessor(__configFile,  __outSize);
	size = _soundIndprocessor.GetSize();

	//----------------------------------------------------------------------
	//  Register ports.
	//----------------------------------------------------------------------
	YARPString file_name("mfcc.txt"); // Name of the file where to save the mfcc if necessary
	YARPString base1(__baseName); YARPString base2(__baseName);
	YARPString base3(__baseName); YARPString base4(__baseName); 
	YARPString base5(__baseName); YARPString base6(__baseName);
	YARPString base7(__baseName); YARPString base8(__baseName);

	//LOCAL_TRACE("SoundIdentification: Registering ports");
	_inpSound.Register (base1.append("i"     ).c_str());
	_inpImg.Register   (base3.append("i:img").c_str());
	//_outpMfcc.Register (base4.append("mfcc"  ).c_str());
	_outpImg.Register  (base5.append("o:img"  ).c_str());
	_outprecImg.Register(base6.append("o:img2"  ).c_str());
	_outSoundHistogramPort.Register(base7.append("o:soundhisto"  ).c_str());
	_outHSHistogramPort.Register(base8.append("o:hshisto"  ).c_str());

	time1 = YARPTime::GetTimeAsSeconds();

	//----------------------------------------------------------------------
	// Main loop.
	//----------------------------------------------------------------------

	int    _dMixelValue     = 0;
	double _dSoundRms       = 0.0;
	int    _iMemoryFactor   = 0;
	bool   _first           = true;
	bool   _bContinuityFlag = false;
	double _rmsmean         = __rmsthreshold + 1;
	double _rmsmeansum      = 0.0;
	double _minDtwValue     = HUGE;
	bool _bFoundTemplate = false;
	int _hsMaximumWidth = 0;
	int _hsMaximumHeight = 0;
	SoundImagePair soundImagePair;

	while(!IsTerminated())
	{
		int i,j;
		counter++;

		_sema.Wait(1); // Start Semaphore

		//----------------------------------------------------------------------
		//  Read sound stream  and image from the network.
		//----------------------------------------------------------------------
		//LOCAL_TRACE("SoundIdentification: Reading from ports");
		_inpSound.Read(1); 
		_inpImg.Read(1);  
		//_inputLogPolarImage.Refer(_inpImg.Content());
		_inputLogPolarImage.CastCopy(_inpImg.Content());

		_logPolarMapper.ReconstructColor(
			(const YARPImageOf<YarpPixelMono>&)_inputLogPolarImage, 
			_imgInput
			);

		//----------------------------------------------------------------------
		//  Calculate MFCC vector and add it into the sound template.
		//----------------------------------------------------------------------
		_soundIndprocessor.apply(_inpSound.Content(),_vOutMfcc, _dSoundRms); 
		// Create shorter mfcc vector without first element
		for ( i = 1; i < L_VECTOR_MFCC; i++) {
			 _vShorterOutMfcc[i-1] = _vOutMfcc[i];
		}

		//----------------------------------------------------------------------
		//  Check size correctness.
		//----------------------------------------------------------------------
		if ( _inputLogPolarImage.GetWidth() != _imgMixelgram.GetWidth() || 
			_inputLogPolarImage.GetHeight() != _imgMixelgram.GetHeight()) {

			_imgMixelgram.Resize(
				_inputLogPolarImage.GetWidth(),
				_inputLogPolarImage.GetHeight()
				);
		}

		if (_first) {
			_newLogPolarImage.Resize(
				_inputLogPolarImage.GetWidth(),
				_inputLogPolarImage.GetHeight()
				); 

			_first = false;
		}

		//----------------------------------------------------------------------
		//  Introduce image into the images lists.
		//----------------------------------------------------------------------
		_imagesList.push_back(_imgInput);
		_logPolarImagesList.push_back(_inputLogPolarImage);
		_rmsList.push_back( _dSoundRms );


		if ( _imagesList.size() > _iSValue) {
			_imagesList.pop_front(); 
			_logPolarImagesList.pop_front();
			_rmsList.pop_front();
		}

		//----------------------------------------------------------------------
		//  Go to calculate the mixel for each pixel in the image.
		//----------------------------------------------------------------------
		if ( _imagesList.size() > 3 && learningPhase == 1) {

			int w = _imgInput.GetWidth();
			int h = _imgInput.GetHeight();
			int numberMixels = 0;
			//----------------------------------------------------------------------
			//  Navigate through the pixels and calculate the Mixel for each one. 
			//----------------------------------------------------------------------
			for(i = 2; i < w-2; i+=4 )
				for( j = 2; j < h-2; j+=4)
				{
					_dMixelValue = calculateMixel(
						_rmsListIterator, 
						_imagesListIterator,
						_imagesList.size(),
						i, 
						j, 
						_rmsmean
						);

					if (_dMixelValue > MIXELTHRESHOLD) {
						//----------------------------------------------------------------------
						//  Paint the pixel and all its sorrounding pixels
						//----------------------------------------------------------------------
						for ( int ni = i-2; ni < i+2; ni++ )
							for (int nj = j-2; nj < j+2; nj++)
								_imgMixelgram(ni,nj) = _dMixelValue;

						numberMixels++;
					}
					else
					{
						for ( int ni = i-2; ni < i+2; ni++ )
							for (int nj = j-2; nj < j+2; nj++)
								_imgMixelgram(ni,nj) = 0;
					}
				}

			_rmsmeansum += _rmsmean;

			//  Clean histogram
			_imgSoundHistogram.Zero();

			//----------------------------------------------------------------------
			//  Compute the normalized and truncated sound wave form. 
			//----------------------------------------------------------------------
			YVector _vNormalizedSoundSamples;
			_soundIndprocessor.NormalizeSoundArray ( 32000 );
			_soundIndprocessor.TruncateSoundToVector(
				__histoWidth, 
				_vNormalizedSoundSamples
				);

			//----------------------------------------------------------------------
			//  Paint the sound wave form. 
			//----------------------------------------------------------------------
			for( i = 1; i <= _vNormalizedSoundSamples.Length(); i++){
				int value = _vNormalizedSoundSamples[i] * _imgSoundHistogram.GetHeight();
				value = fabs(value);
				int valuerawmaxposition = _imgSoundHistogram.GetHeight() - value;
				if ( valuerawmaxposition < 0){
					valuerawmaxposition = 0;
				}
				// Fill all the pixels in the column until the value
				for( int j = valuerawmaxposition; j < _imgSoundHistogram.GetHeight(); j++){
					_imgSoundHistogram.SafePixel(i,j) = 100;
				}
			}

			//----------------------------------------------------------------------
			//  Keep a detected object in the image for some time.
			//  When the number of mixel is over a particular threshold I asume
			//  that an object has been detected.
			//----------------------------------------------------------------------
			if ( _iMemoryFactor > MEMORYMAX) {
				_newLogPolarImage.Zero();
			}
			_iMemoryFactor++;

			if (numberMixels > MIXELSMAX) {

				// Get the segmented image
				GetSegmentedImage(
					_logPolarImagesListIterator, 
					_imgMixelgram,
					w,
					h,
					_imagesList.size(),
					_iMemoryFactor,
					_dDecayValue,
					_newLogPolarImage
					);

				if ( _bContinuityFlag == true ) {

					// Where are continually detecting sound image
					// correlations; therefore, lets store the sound
					// vector in the template 
					int ret = _soundTemplate.Add(_vShorterOutMfcc, 1);
					ACE_ASSERT(ret != YARP_FAIL);
				}

				_iMemoryFactor = 0;
				_bContinuityFlag = true;
			}
			else {

				if ( _bContinuityFlag == true ) {

					soundImagePair.soundTemplate = _soundTemplate;
					soundImagePair.image.CastCopy( _newLogPolarImage ); 

					ACE_OS::fprintf(stdout," Memorizing a pair sound-image!! \n");
					_pairList.push_back( soundImagePair );  

					_soundTemplate.Destroy();
				}

				_bContinuityFlag = false; 
			}

			//----------------------------------------------------------------------
			//  Compute the HS histogram.
			//----------------------------------------------------------------------
			if ( numberMixels > MIXELSMAX || _iMemoryFactor < MEMORYMAX) {

				_logPolarMapper.ReconstructColor (
					(const YARPImageOf<YarpPixelMono>&)_newLogPolarImage, 
					_coloredImage
					);

				if ( numberMixels > MIXELSMAX ) {
					ComputeHSHistogram ( 
						_coloredImage, 
						_imgHSHistogram,
						_hsMaximumWidth,
						_hsMaximumHeight,
						1
						);
				}

				BackProjectHSHistogram(
					_hsMaximumWidth,
					_hsMaximumHeight,
					_imgHSHistogram,
					_imgInput,
					_inputLogPolarImage
					);
			}
			else {
				ComputeHSHistogram( 
					_imgInput, 
					_imgHSHistogram,
					_hsMaximumWidth,
					_hsMaximumHeight,
					0
					); 	
			}

			//----------------------------------------------------------------------
			//  Send the mixelgram image into the network
			//----------------------------------------------------------------------
			_outpImg.Content().SetID(YARP_PIXEL_MONO);
			_outpImg.Content().Refer(_imgMixelgram);
			_outpImg.Write(1);

			//----------------------------------------------------------------------
			//  Send the procesed image back to the network (the image remains
			//  as received) or send modified image with object segmented.
			//----------------------------------------------------------------------
			//if ( numberMixels > MIXELSMAX || _iMemoryFactor < MEMORYMAX)
			//{
			//	_outprecImg.Content().SetID(YARP_PIXEL_MONO);
			//	_outprecImg.Content().Refer(_newLogPolarImage); 
			//	_outprecImg.Write(1);
			//}
			//else {
				_outprecImg.Content().SetID(YARP_PIXEL_MONO);
				_outprecImg.Content().Refer(_inputLogPolarImage); 
				_outprecImg.Write(1);
			//}

			//----------------------------------------------------------------------
			//  Send sound histogram to the network
			//----------------------------------------------------------------------
			_outSoundHistogramPort.Content().SetID(YARP_PIXEL_MONO);
			_outSoundHistogramPort.Content().Refer(_imgSoundHistogram); 
			_outSoundHistogramPort.Write();

			//----------------------------------------------------------------------
			//  Send the HS histogram to the network
			//----------------------------------------------------------------------
			YARPImageOf<YarpPixelMono> scaledimage;	
			scaledimage.ScaledCopy( _imgHSHistogram, 200, 100);
			_outHSHistogramPort.Content().SetID(YARP_PIXEL_MONO);
			_outHSHistogramPort.Content().Refer(scaledimage);
			_outHSHistogramPort.Write();
		}

		if ( learningPhase == 0 ) {
			 
			double _rmsMean     = 0.0;
			int    _machingPair = 0;
			int    _result      = 0;
			double _dtwValue    = 0.0;
			int _vectorSize = _imagesList.size(); 

			// Get the sound mean
			CalculateRMSMean( 
				_rmsListIterator,
				_vectorSize,
				_rmsMean
				);
			
			_pairListIterator.go_head();
			
			// If there is an strong sound try to identified it
			if ( _rmsMean > __rmsthreshold ) {

				if ( _bContinuityFlag == true ) {

					// Where are continually detecting sound image
					// correlations; therefore, lets store the sound
					// vector in the template 
					int ret = _soundTemplate.Add(_vShorterOutMfcc, 1);
					ACE_ASSERT(ret != 0);
				}

				_bContinuityFlag = true;

				// Send the received image into the network
				_outprecImg.Content().SetID(YARP_PIXEL_MONO);
				_outprecImg.Content().Refer(_inputLogPolarImage); 
				_outprecImg.Write(1);
			}
			else {

				// We have been detecting a sound and sudently
				// it has stoped; therefore lets identify it.
				if ( _bContinuityFlag == true && 
				  _soundTemplate.Length() > 3 ) {
					 
					ACE_OS::fprintf(stdout,"SEARCHING FOR A SIMILAR TEMPLATE\n");
					for ( i = 0; i < _pairList.size(); i++ ) {

						SoundImagePair &pair = *_pairListIterator;

						_dtwValue = pair.soundTemplate.Dtw( 
							_soundTemplate,
							&_result
							); 

						ACE_OS::fprintf(stdout," DTW value = %f \n", _dtwValue);

						// This pair correlates more
						if ( _dtwValue < _minDtwValue) {
							_minDtwValue = _dtwValue;

							_newLogPolarImage.CastCopy( pair.image );
							_iMemoryFactor = 0;
						}
						_pairListIterator++;
					}

					_soundTemplate.Destroy();
				}

				_bContinuityFlag = false;

				if ( _iMemoryFactor < MEMORYMAX ) {

					_outprecImg.Content().SetID(YARP_PIXEL_MONO);
					_outprecImg.Content().Refer(_newLogPolarImage); 
					_outprecImg.Write(1);

					_iMemoryFactor++;
				}
				else {
					// Send the received image into the network
					_outprecImg.Content().SetID(YARP_PIXEL_MONO);
					_outprecImg.Content().Refer(_inputLogPolarImage); 
					_outprecImg.Write(1);

					_minDtwValue = HUGE;
				}

			}

		}
		
		//----------------------------------------------------------------------
		//  This just resends the received images with some delay.
		//----------------------------------------------------------------------
		
		//if (_soundTemplate.Length() > 10)
		//{
		//	_logPolarImagesListIterator.go_head();
		//	MonoImage& tempp = *_logPolarImagesListIterator;
		//	_outpImg.Content().SetID(YARP_PIXEL_MONO);
		//	_outpImg.Content().Refer(tempp);
		//	_outpImg.Write();
		//}

		//----------------------------------------------------------------------
		//  Sends the mfcc coefficients
		//----------------------------------------------------------------------
		//_outPort_mfcc.Content() = _vOutMfcc;
		//_outPort_mfcc.Write();

		//----------------------------------------------------------------------
		//  Time calculation stuff
		//----------------------------------------------------------------------
		time2 = time1;
		time1 = YARPTime::GetTimeAsSeconds();

		period += (time1-time2);
		if (counter == N)
		{
			printf("average= %lf \n", period/N);
			printf("rms average= %lf \n", _rmsmeansum/N);
			period = 0.0;
			_rmsmeansum = 0.0;
			counter = 0;
		}

		_sema.Post(); 
	}


	YARPString name(file_name);
	_soundTemplate.Save(name);
	_soundTemplate.Destroy();

	return ;
}
