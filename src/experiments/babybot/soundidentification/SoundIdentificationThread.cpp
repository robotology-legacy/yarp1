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
/// $Id: SoundIdentificationThread.cpp,v 1.1 2004-10-28 14:28:53 beltran Exp $
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

	_iSValue     = 15;
	_dDecayValue = 0.98;
}

//----------------------------------------------------------------------
// SetSValue 
//----------------------------------------------------------------------
void SoundIdentificationThread::setSValue(const int &svalue) {
	LOCAL_TRACE("SoundIdenfication: Entering setSValue");

	if ( _iSValue > ARRAY_MAX )
		ACE_OS::fprintf(stdout,"Sorry, max S value superated\n");

	_sema.Wait(1); // Start Semaphore

	ACE_OS::fprintf(stdout,"Setting SValue = %d\n", svalue);
	_iSValue = svalue;
	_soundTemplate.Resize(_iSValue);

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
//  getSValue
//----------------------------------------------------------------------
int SoundIdentificationThread::getSValue() { 
	LOCAL_TRACE("SoundIdentification: Entering getSValue");
	return _iSValue;
}

//----------------------------------------------------------------------
//  getDecaingFactor
//----------------------------------------------------------------------
double SoundIdentificationThread::getDecaingFactor() { 
	LOCAL_TRACE("SoundIdentification: Entering getDecaingFactor");
	return _dDecayValue; 
}

//----------------------------------------------------------------------
//  RGBtoHSV
//----------------------------------------------------------------------
void SoundIdentificationThread::RGBtoHSV ( 
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
//  ComputeHSHistogram
//----------------------------------------------------------------------
int SoundIdentificationThread::ComputeHSHistogram (
	YARPImageOf<YarpPixelBGR> &inputImage,
	YARPImageOf<YarpPixelMono> &imageHSHistogram
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
	YarpPixelBGR pixelBGR;

	imageHSHistogram.Zero();

	for( int i = 1; i <= imageWidth; i++) {
		for ( int j = 1; j <= imageHeight; j++){

			pixelBGR.b = inputImage.SafePixel(i,j).b;
			pixelBGR.g = inputImage.SafePixel(i,j).g;
			pixelBGR.r = inputImage.SafePixel(i,j).r;

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

			imageHSHistogram.SafePixel( iHueWidthCoordinate, 
				histogramHeight - iSaturationHeightCoordinate)++; 
		}
	}
	return YARP_OK;
}

//----------------------------------------------------------------------
//  GetSegmentedImage
//----------------------------------------------------------------------
int SoundIdentificationThread::GetSegmentedImage (
	YARPImageOf<YarpPixelMono> *vImagesVector, 
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
	int pixelValue        = 0;
	double pixelMeanValue = 0.0;
	double pixelsSum      = 0.0;

	for(i = 2; i < width-2; i++ )
		for( j = 2; j < height-3; j++) {
			pixelValue = mixelGramImage(i,j);
			if ( pixelValue > MIXELTHRESHOLD) {
				for (int z = 1; z <= numberOfSamples; z++) {
					YARPImageOf<YarpPixelMono>& pimg = vImagesVector[z-1];
					pixelsSum += pimg(i,j);
				}
				pixelMeanValue = pixelsSum / (double) numberOfSamples;
				segmentedImage(i,j) = pixelMeanValue;
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
int SoundIdentificationThread::calculateMixel (
	YARPCovMatrix &mXtX, 
	YARPImageOf<YarpPixelBGR> * vImgs, 
	int iSamples,
	int i, 
	int j
	) {

	//LOCAL_TRACE("SoundIdentification: Entering calculateMixel");

    int n   = 0; /** Sound number of componects. */
    int m   = 0; /** Image number of components. */
    int ret = 0; /** Return variable.            */
    
	//YARPExMatrix  _mV;        /** Matrix with the pixel samples (in the time domain).   */
    //YARPCovMatrix _mYtY;      /** Covariance matrix of _mV.                             */
    YMatrix _mV;         /** Vector containing the gray values of pixels samples.  */
    YMatrix _mVtV;       /** Variance of V.                                        */
    YARPCovMatrix _mCtC; /** Mutual covariance matrix.                             */
    YMatrix _mC;         /** The local variances matrix for the mutual covariance. */

	YMatrix& _mX = mXtX.getOriginalVariancesMatrix(); /** The local variances matrix of mXtX. */

    n = _mX.NCols();          // The soundcovariance matrix determinates the
                              // number of parameters used to parametrice the sound
    m = 1;                    // The pixel contains 3 bytes for RGB data

    _mV.Resize(iSamples,1);
	_mVtV.Resize(1,1);
	_mC.Resize(iSamples,n+m);
	
	//----------------------------------------------------------------------
	//  Fill PixelSamples matrix
	//----------------------------------------------------------------------
	double mean = 0.0;
	for (int z = 1; z <= iSamples; z++)
	{
		YARPImageOf<YarpPixelBGR>& pimg = vImgs[z-1];
		_mV(z,1) = 0.299 * pimg.SafePixel(i,j).r + 0.587 * pimg.SafePixel(i,j).g + 0.114 * pimg.SafePixel(i,j).b;
		//_mV(z,1) = pimg.SafePixel(i,j).r;
		//_mV(z,2) = pimg.SafePixel(i,j).g;
		//_mV(z,3) = pimg.SafePixel(i,j).b;
		mean += _mV(z,1);
	}
	mean /= (double)_mV.NRows();
		
	//----------------------------------------------------------------------
	//  Calculate pixel covariance matrix.
	//----------------------------------------------------------------------
	////_mV.covarianceMatrix(_mYtY,0); //Calculate only variance matrix.

	////YMatrix& _mY = _mYtY.getOriginalVariancesMatrix(); /** The local variances matrix of mYtY. */
	_mV  -= mean;
	_mVtV = _mV.Transposed() * _mV;

	//----------------------------------------------------------------------
	//  Calculate local mutual variances matrix.
	//----------------------------------------------------------------------
	for (int r = 1; r <= iSamples; r++)
		for (int c = 1; c <= _mC.NCols(); c++)
		{
			if ( c <= n)
				_mC(r,c) = _mX(r,c); 
			else
				_mC(r,c) = _mV(r,1);
		}

	//----------------------------------------------------------------------
	//  Compute mutual covariance
	//----------------------------------------------------------------------
	_mCtC.setOriginalVariancesMatrix(_mC);
	
	//----------------------------------------------------------------------
	//  Compute determinants
	//----------------------------------------------------------------------
	double _dDetCtC = 0.0;
	double _dDetXtX = 0.0;
	double _dDetVtV = 0.0;
	////double _dDetYtY = 0.0;
    _mCtC.determinant(_dDetCtC);
    mXtX.determinant (_dDetXtX);
    ////_mYtY.determinant(_dDetYtY);
	_dDetVtV = _mVtV(1,1);

	//----------------------------------------------------------------------
	//  Compute mutual information
	//----------------------------------------------------------------------
	//double value = (1/2.0) * ACE::log2((_dDetXtX * _dDetVtV) / (double)_dDetCtC);
	double value = 0.5 * ((logf((_dDetXtX * _dDetVtV) / (double)_dDetCtC))/(double)logf(2));

	////int pixel = min(255,max(0,255*mixel)) /////from Vuppla
	return(MIN(255,MAX(0,255*value)));
	//return(value);
}
 
//----------------------------------------------------------------------
//  calculateMixel2
//----------------------------------------------------------------------
int SoundIdentificationThread::calculateMixel2 (
	double * vRms, 
	YARPImageOf<YarpPixelBGR> * vImgs, 
	int iSamples,
	int i, 
	int j,
	double &rmsmean
	) {
	//LOCAL_TRACE("SoundIdentification: CalculateMixel2");

    int n         = 0;  /** Sound number of componects.                          */
    int m         = 0;  /** Image number of components.                          */
    int ret;            /** Return variable.                                     */
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
	YVector _vV;        /** Vector containing the gray values of pixels samples. */

    _vV.Resize(iSamples);
	
	//----------------------------------------------------------------------
	//  Fill PixelSamples matrix
	//----------------------------------------------------------------------
	for (int z = 1; z <= iSamples; z++)
	{
		//----------------------------------------------------------------------
		//  Compute sound associated values
		//----------------------------------------------------------------------
		_dSX  += vRms[z-1];
		_dSX2 += powf(vRms[z-1],2);
		if ( vRms[z-1] > _dMaxRms)
			_dMaxRms = vRms[z-1];

		//----------------------------------------------------------------------
		//  Compute image associated values.
		//----------------------------------------------------------------------
		YARPImageOf<YarpPixelBGR>& pimg = vImgs[z-1];
		_vV(z) = 0.299 * pimg.SafePixel(i,j).r + 0.587 * pimg.SafePixel(i,j).g + 0.114 * pimg.SafePixel(i,j).b;
		_dSY  += _vV(z);
		_dSY2 += powf(_vV(z),2);

		//----------------------------------------------------------------------
		//  Compute common values.
		//----------------------------------------------------------------------
		_dSXY += (vRms[z-1] * _vV(z));
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
	YarpPixelBGR _auxpix(0,0,0); /** Temporal pixel.  */
	char * ppixel;
	YARPBottle _botActions;            

	YVector _vOutMfcc(L_VECTOR_MFCC);
	YARPCovMatrix _mSoundCov;
	YMatrix _mLocVar;

	/** Vector of pointers to images. */
	//YARPImageOf<YarpPixelBGR> ** _vImages = new YARPImageOf<YarpPixelBGR> *[_soundTemplate.Size()]; 
	YARPImageOf<YarpPixelBGR> _vImages[ARRAY_MAX]; 
	YARPImageOf<YarpPixelMono> _vLogPolarImages[ARRAY_MAX]; 

	//----------------------------------------------------------------------
	//  Resize some of the images.
	//----------------------------------------------------------------------
	_imgInput.Resize(_stheta,_srho);
	_coloredImage.Resize(_stheta, _srho);
	_imgSoundHistogram.Resize(__histoWidth, __histoHeight);
	_imgHSHistogram.Resize(__hsHistoWidth, __hsHistoHeight);

	YARPScheduler::setHighResScheduling();

	//----------------------------------------------------------------------
	//  Initialize sound template.
	//----------------------------------------------------------------------
	_soundTemplate.Resize(_iSValue);

	//----------------------------------------------------------------------
	// Port declarations 
	//----------------------------------------------------------------------
	//LOCAL_TRACE("SoundIdentification: Declaring ports");
	YARPInputPortOf<YARPSoundBuffer>   _inpSound(YARPInputPort::NO_BUFFERS ,YARP_TCP);
	//YARPInputPortOf<YARPBottle>        _inpAction(YARPInputPort::DEFAULT_BUFFERS ,YARP_UDP);
	YARPInputPortOf<YARPGenericImage>  _inpImg(YARPInputPort::NO_BUFFERS, YARP_TCP);
	//YARPOutputPortOf<YVector>          _outpMfcc (YARPOutputPort::DEFAULT_OUTPUTS,YARP_UDP);
	YARPOutputPortOf<YARPGenericImage> _outpImg;
	YARPOutputPortOf<YARPGenericImage> _outprecImg;
	YARPOutputPortOf<YARPGenericImage> _outSoundHistogramPort;
	YARPOutputPortOf<YARPGenericImage> _outHSHistogramPort;

	//_inpSound.SetAllowShmem (_sharedmem);
	//_inpAction.SetAllowShmem(_sharedmem);
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
	//_inpAction.Register(base2.append("action").c_str());
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

	//----------------------------------------------------------------------
	//  The next variables control the action to be done by the front end. They
	//	are received from an input port and supposed to be controlled by a higher 
	//	level sound processing software (i.e. that deciding what to memorice, to
	//	identify...etc.)
	//  ---	'_sound_order' tells the position of the sound beeing sent, this can
	//		be used in the case the sound are following a temporal sequence.
	//	---	'_sound_save' tells the front end to save the current buffer in 
	//		a sound template. When to start and end a template is controlled by
	//		this variable in a "level-triggered" fasion (i.e. a level change from
	//		0->1 produce the front-end to start saving a template; a level change
	//		from 1-> says the front-end to stop saving the current template)
	//	--- '_sound_idenficate' works in a similar ways as the previous variable, but
	//		it goal is that to detect the start-end of a sound template that needs to
	//		be identified.
	//----------------------------------------------------------------------
	int    _sound_order          = 0;
	int    _sound_save           = 0;
	int    _pre_sound_save       = 0;
	int    _sound_identificate   = 0;
	int    _pre_sound_idenficate = 0;
	bool   _save                 = false;
	bool   _identify             = false;
	int    _dMixelValue          = 0;
	double _dSoundRms            = 0.0;
	double _vRms[ARRAY_MAX];
	int    _iMemoryFactor         = 0;
	bool   _first                = true;
	double _rmsmean              = __rmsthreshold + 1;
	double _rmsmeansum           = 0.0;
	int    _circleradius         = 0;
	double _imgtimeacquisition   = 0.0;
	double _soundtimeacquisition = 0.0;
	double _imgtimeacquisitionold = 0.0;
	double _soundtimeacquisitionold = 0.0;

	while(!IsTerminated())
	{
		int i,j,y;
		counter++;

		_sema.Wait(1); // Start Semaphore

		//----------------------------------------------------------------------
		//  Read sound stream  and image from the network.
		//----------------------------------------------------------------------
		//LOCAL_TRACE("SoundIdentification: Reading from ports");
		_inpSound.Read(1); 
		_inpImg.Read(1);  
		//_imgInput.Refer(_inpImg.Content()); 
		_inputLogPolarImage.Refer(_inpImg.Content());
		_logPolarMapper.ReconstructColor (
			(const YARPImageOf<YarpPixelMono>&)_inputLogPolarImage, 
			_imgInput
			);



		//----------------------------------------------------------------------
		//  Get acquisition time for the image. This double is travelling in the 
		//  first bytes of the image.
		//----------------------------------------------------------------------
		//double * _pimgtimeacq = (double *)_imgInput.GetRawBuffer();
		//_imgtimeacquisition   =  *_pimgtimeacq;

		//
		//Check image sequence correctness
		//
		//if ( _imgtimeacquisition < _imgtimeacquisitionold)
		// 	ACE_OS::fprintf(stdout,"ALERT: error in image sequence time order\n");

		//_imgtimeacquisitionold = _imgtimeacquisition; //Refresh the old variable

		//----------------------------------------------------------------------
		//  Calculate MFCC vector and add it into the sound template.
		//----------------------------------------------------------------------
		_soundIndprocessor.apply(_inpSound.Content(),_vOutMfcc, _dSoundRms); 
		int ret = _soundTemplate.Add(_vOutMfcc, 2);  // Adds new vector; bufferize if necessary
		ACE_ASSERT(ret != 0);

		//
		//Check sound sequence correctness
		//
		//if ( _soundtimeacquisition < _soundtimeacquisitionold)
		//	ACE_OS::fprintf(stdout,"ALERT: error in sound sequence time order\n");

		//_soundtimeacquisitionold = _soundtimeacquisition; //Refresh the old variable

		//
		//Chenk syncronization
		//
		//if (abs(_soundtimeacquisition - _imgtimeacquisition) > 0.010)
		//	ACE_OS::fprintf(stdout,"ALERT: Unacceptable image/sound desynchronization.\n");

		//----------------------------------------------------------------------
		// Check images sizes.
		//----------------------------------------------------------------------
		/*
		   int _iRealMixelImgHeight = _imgMixelgram.GetHeight() - __iSoundHistogramHeight;
		   if ( _imgInput.GetWidth() != _imgMixelgram.GetWidth() || _imgInput.GetHeight() != _iRealMixelImgHeight)
		   _imgMixelgram.Resize(_imgInput.GetWidth(),_imgInput.GetHeight() + __iSoundHistogramHeight);
		   if (_first)
		   {
		   newimg.Resize(_imgInput.GetWidth(), _imgInput.GetHeight());
		   _first = false;
		   }
		 */
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
		//  Introduce image into the images vectors.
		//----------------------------------------------------------------------
		///@todo study how to do this operation more eficiently.
		YARPImageOf<YarpPixelBGR>& tmpimg4 = _vImages[_soundTemplate.Length()-1];
		YARPImageOf<YarpPixelMono>& tempLogPolarImage = _vLogPolarImages[_soundTemplate.Length()-1];
		tmpimg4.CastCopy(_imgInput);
		tempLogPolarImage.CastCopy(_inputLogPolarImage);

		_vRms[_soundTemplate.Length()-1] = _dSoundRms;

		if( _soundTemplate.isFull()) 
		{ 
			for (i = 1; i < _soundTemplate.Length();i++)
			{
				//Get color images shift to make room for the new arrival
				YARPImageOf<YarpPixelBGR>& tmpimg  = _vImages[i-1];
				YARPImageOf<YarpPixelBGR>& tmpimg2 = _vImages[i];
				tmpimg.CastCopy(tmpimg2);

				// Get Log Polar mono images make room for the new image
				YARPImageOf<YarpPixelMono>& previousImage = _vLogPolarImages[i-1];
				YARPImageOf<YarpPixelMono>& currentImage  = _vLogPolarImages[i];
				previousImage.CastCopy(currentImage);

				// Get the space for the new sound sample
				_vRms[i-1] = _vRms[i];
			}
		}

		//----------------------------------------------------------------------
		//  Go to calculate the mixel for each pixel in the image.
		//----------------------------------------------------------------------
		if (_soundTemplate.Length() > 3)
		{
			//  Calculate sound covariance matrix.
			_soundTemplate.CovarianceMatrix(_mSoundCov,0); 

			int w = _imgInput.GetWidth();
			int h = _imgInput.GetHeight();
			int numberMixels = 0;
			//----------------------------------------------------------------------
			//  Navigate through the pixels and calculate the Mixel for each one. 
			//----------------------------------------------------------------------
			for(i = 2; i < w-2; i+=4 )
				for( j = 2; j < h-2; j+=4)
				{
					///////_dMixelValue = calculateMixel(_mSoundCov, _vImages,_soundTemplate.Length(), i, j);
					_dMixelValue = calculateMixel2(_vRms, _vImages,_soundTemplate.Length(), i, j, _rmsmean);
					if (_dMixelValue > MIXELTHRESHOLD) 
					{
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
			_soundIndprocessor.TruncateSoundToVector (
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
			if ( _iMemoryFactor > MEMORYMAX)
				_newLogPolarImage.Zero();
			_iMemoryFactor++;

			if (numberMixels > MIXELSMAX)
			{
				GetSegmentedImage (
					_vLogPolarImages, 
					_imgMixelgram,
					w,
					h,
					_soundTemplate.Length(),
					_iMemoryFactor,
					_dDecayValue,
					_newLogPolarImage
					);

				_iMemoryFactor = 0;
			}

			//----------------------------------------------------------------------
			//  Compute the HS histogram.
			//----------------------------------------------------------------------
			if ( numberMixels > MIXELSMAX || _iMemoryFactor < MEMORYMAX) {
				_logPolarMapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)_newLogPolarImage, 
					_coloredImage);
				ComputeHSHistogram( _coloredImage, _imgHSHistogram);
			}
			else {
				ComputeHSHistogram( _imgInput, _imgHSHistogram); 	
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
			if ( numberMixels > MIXELSMAX || _iMemoryFactor < MEMORYMAX)
			{
				_outprecImg.Content().SetID(YARP_PIXEL_MONO);
				_outprecImg.Content().Refer(_newLogPolarImage); ////Here I have to send logpolar
				_outprecImg.Write(1);
			}
			else {
				_outprecImg.Content().SetID(YARP_PIXEL_MONO);
				_outprecImg.Content().Refer(_inputLogPolarImage); ////Here I have to sent orig logpolar
				_outprecImg.Write(1);
			}

			//----------------------------------------------------------------------
			//  Send sound the histogram to the network
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
		/*
		//----------------------------------------------------------------------
		//  This just resends the received images with some delay.
		//----------------------------------------------------------------------
		if (_soundTemplate.Length() > 18)
		{
		YARPImageOf<YarpPixelBGR>& tempp = _vImages[0];
		_outpImg.Content().SetID(YARP_PIXEL_BGR);
		_outpImg.Content().Refer(tempp);
		_outpImg.Write();
		}
		 */
#if 0
		//----------------------------------------------------------------------
		//  Actions stuff. This need to be written better.
		//----------------------------------------------------------------------
		_inAction.Read(); // Read what to do with the sound than now is beeing received
		_botActions = _inAction.Content();

		//Read and Proccess the action
		_pre_sound_save = _sound_save;
		_pre_sound_identification = _sound_indentification;
		_botActions.readInt((int *)&_sound_order);
		_botActions.readInt((int *)&_sound_save);
		_botActions.readInt((int *)&_sound_identificate);

		// Start recording a sound template
		if ( _pre_sound_save == 0 && _sound_save == 1) 
		{
			_save = true;
			//Create a new template and start saving

		}
		// End recording the courrent template
		if ( _pre_sound_save == 1 && _sound_save == 0) 
		{
			_save = false;
		}
		// Start recording sound template to be identified
		if ( _pre_sound_identification == 0 && _sound_identification == 1) {}
		// End recording sound template and identify it
		if ( _pre_sound_identification == 1 && _sound_identification == 0) {}
#endif

		//----------------------------------------------------------------------
		//  Sends the mfcc coefficients
		//----------------------------------------------------------------------
		////_outPort_mfcc.Content() = _vOutMfcc;
		////_outPort_mfcc.Write();

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

		_sema.Post(); // End Semaphore
	}


	YARPString name(file_name);
	_soundTemplate.Save(name);
	_soundTemplate.Destroy();

	return ;
}
