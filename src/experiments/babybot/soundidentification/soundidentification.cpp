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
/// $Id: soundidentification.cpp,v 1.16 2004-10-05 17:38:39 beltran Exp $
///

/** 
 * @file soundidentification.cpp Implements a soundprocessing front-end.
 * This consist in a sound parametrization throuth a Short Time Fast Fourier
 * Transform procedure. Later, a MFCC (Mel-Frequency Cepstral Coefficients) is
 * implemented to provide a sound representation with a parameter reduced representation.
 * 
 * @author Carlos Beltran
 * @date 2004-08-24
 */
#define ACE_NTRACE 1

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

#include "YARPSoundTemplate.h"
#include "soundidentificationprocessing.h"
#include "YARPExMatrix.h"

#define MIN(a,b) (a>b ? b : a)
#define MAX(a,b) (a>b ? a : b)

using namespace std;

int calculateMixel(YARPCovMatrix &, 
			   YARPImageOf<YarpPixelBGR>*,int,int,int); 
int calculateMixel2(double *,
			   	YARPImageOf<YarpPixelBGR>*,int,int,int); 

const int   __outSize    = 5;
const char *__baseName   = "/soundidentification/";
const char *__configFile = "sound.ini";
int _sizex = 250;
int _sizey = 270; /** @todo Put this as an external parameter?. */

/**
 * Some variable for the network connection.
 */
bool _sharedmem = false;
int  _protocol  = YARP_MCAST;

/** 
 * Runs the main processing loop. 
 */
class mainthread : public YARPThread
{
public:
	virtual void Body (void)
	{
		const int N   = 200;

		int    counter = 0;
		int    size    = 0;
		double time1   = 0.0;
		double time2   = 0.0;
		double period  = 0.0;
		YarpPixelBGR _auxpix(0,0,0); /** Temporal pixel.  */
		char * ppixel;
		//YARPVectorBuffer vector_buff;
		YARPSoundTemplate _soundTemplate;
        YARPBottle _botActions;            // This bottle is used to control externally the
                                           // action to be taken.
        YVector _vOutMfcc(L_VECTOR_MFCC);  // The vector for the MFCC coefficients
		YARPCovMatrix _mSoundCov;
		YMatrix _mLocVar;

        YARPImageOf<YarpPixelBGR> _imgMixelgram; /** The mixelgram image.    */
        YARPImageOf<YarpPixelBGR> _imgInput;    /** The normal input image. */

        /** Vector of pointers to images. */
		//YARPImageOf<YarpPixelBGR> ** _vImages = new YARPImageOf<YarpPixelBGR> *[_soundTemplate.Size()]; 
		YARPImageOf<YarpPixelBGR> _vImages[ARRAY_MAX]; 

        _imgMixelgram.Resize(_sizex,_sizey);
        //_imgInput.Resize(_sizex,_sizey);

		YARPScheduler::setHighResScheduling();
		
		//----------------------------------------------------------------------
		// Port declarations 
		//----------------------------------------------------------------------
        YARPInputPortOf<YARPSoundBuffer>   _inpSound(YARPInputPort::NO_BUFFERS ,YARP_TCP);
        //YARPInputPortOf<YARPBottle>        _inpAction(YARPInputPort::DEFAULT_BUFFERS ,YARP_UDP);
		YARPInputPortOf<YARPGenericImage>  _inpImg(YARPInputPort::NO_BUFFERS, YARP_TCP);
        //YARPOutputPortOf<YVector>          _outpMfcc (YARPOutputPort::DEFAULT_OUTPUTS,YARP_UDP);
        YARPOutputPortOf<YARPGenericImage> _outpImg;
        YARPOutputPortOf<YARPGenericImage> _outprecImg;

        //_inpSound.SetAllowShmem (_sharedmem);
        //_inpAction.SetAllowShmem(_sharedmem);
        //_inpImg.SetAllowShmem   (_sharedmem);
        //_outpMfcc.SetAllowShmem (_sharedmem);
        _outpImg.SetAllowShmem  (_sharedmem);
        _outprecImg.SetAllowShmem  (_sharedmem);

		SoundIdentificationProcessing _soundIndprocessor(__configFile,  __outSize);
		size = _soundIndprocessor.GetSize();

		YARPString file_name("mfcc.txt"); // Name of the file where to save the mfcc if necessary
		YARPString base1(__baseName); YARPString base2(__baseName);
		YARPString base3(__baseName); YARPString base4(__baseName); 
		YARPString base5(__baseName); YARPString base6(__baseName);

        _inpSound.Register (base1.append("i"     ).c_str());
        //_inpAction.Register(base2.append("action").c_str());
        _inpImg.Register   (base3.append("i:img").c_str());
        //_outpMfcc.Register (base4.append("mfcc"  ).c_str());
        _outpImg.Register  (base5.append("o:img"  ).c_str());
        _outprecImg.Register(base6.append("o:img2"  ).c_str());

		time1 = YARPTime::GetTimeAsSeconds();
		
		//test
		double valor = 0.55;
		ACE_OS::fprintf(stdout,"value of log2 = %f\n",logf(1.0 - valor)/(double)logf(2));

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
        int  _sound_order          = 0;
        int  _sound_save           = 0;
        int  _pre_sound_save       = 0;
        int  _sound_identificate   = 0;
        int  _pre_sound_idenficate = 0;
        bool _save                 = false;
        bool _identify             = false;
        int _dMixelValue           = 0;
		double _dSoundRms = 0.0;
		double _vRms[ARRAY_MAX];

		while(!IsTerminated())
		{
			int i,j,y;
			counter++;

			//----------------------------------------------------------------------
			//  Read sound stream  and image from the network.
			//----------------------------------------------------------------------
            _inpSound.Read(1); // Read sound stream
            _inpImg.Read(1);   // Read the image
            _imgInput.Refer(_inpImg.Content()); 

			//----------------------------------------------------------------------
			//  Calculate MFCC vector and add it into the sound template.
			//----------------------------------------------------------------------
			_soundIndprocessor.apply(_inpSound.Content(),_vOutMfcc, _dSoundRms); 
            int ret = _soundTemplate.Add(_vOutMfcc, 2);  // Adds new vector; bufferize if necessary
            ACE_ASSERT(ret != 0);
			
			//----------------------------------------------------------------------
			// Check images sizes.
			//----------------------------------------------------------------------
			if ( _imgInput.GetWidth() != _imgMixelgram.GetWidth() || _imgInput.GetHeight() != _imgMixelgram.GetHeight())
				_imgMixelgram.Resize(_imgInput.GetWidth(),_imgInput.GetHeight());

			//----------------------------------------------------------------------
			//  Introduce image into the images vector.
			//----------------------------------------------------------------------
			///@todo study how to do this operation more eficiently.
			YARPImageOf<YarpPixelBGR>& tmpimg4 = _vImages[_soundTemplate.Length()-1];
			tmpimg4.CastCopy(_imgInput);
			_vRms[_soundTemplate.Length()-1] = _dSoundRms;

			if( _soundTemplate.isFull()) 
			{ 
				for (i = 1; i < _soundTemplate.Length();i++)
				{
					YARPImageOf<YarpPixelBGR>& tmpimg  = _vImages[i-1];
					YARPImageOf<YarpPixelBGR>& tmpimg2 = _vImages[i];
					tmpimg.CastCopy(tmpimg2);
					_vRms[i-1] = _vRms[i];
				}
			}

			//----------------------------------------------------------------------
			//  Go to calculate the mixel for each pixel in the image.
			//----------------------------------------------------------------------
			if (_soundTemplate.Length() > 3)
			{
				//----------------------------------------------------------------------
				//  Calculate sound covariance matrix.
				//----------------------------------------------------------------------
				//////_soundTemplate.CovarianceMatrix(_mSoundCov,0); 
				
				int w = _imgInput.GetWidth();
				int h = _imgInput.GetHeight();
				int wstart = w - (w * 2/3);
				int wend   = w - (w * 1/3);
				int hstart = h - (h * 2/3);
				int hend   = h - (h * 1/3);
				//----------------------------------------------------------------------
				//  Navigate through the pixels and calculate the Mixel for each one. 
				//----------------------------------------------------------------------
				for(i = wstart; i < wend; i++ )
					for( j = hstart; j < hend; j++)
				//for(i = 0; i < w; i++ )
				//	for( j = 0; j < h; j++)
					{
						///////_dMixelValue = calculateMixel(_mSoundCov, _vImages,_soundTemplate.Length(), i, j);
						_dMixelValue = calculateMixel2(_vRms, _vImages,_soundTemplate.Length(), i, j);
						//_dMixelValue = counter;
						if (_dMixelValue > 100) //Threshold
							_imgMixelgram.SafePixel(i,j).r = _dMixelValue;
						else
							_imgMixelgram.SafePixel(i,j).r = 0;
						_imgMixelgram.SafePixel(i,j).g = 0;
						_imgMixelgram.SafePixel(i,j).b = 0;
					}

				//----------------------------------------------------------------------
				//  Paint a little green mark to see that images are being received.
				//----------------------------------------------------------------------
				if (counter < _imgInput.GetWidth())
				{
					int r,c;
					r = _imgInput.GetHeight()-10;
					c = counter;
					_imgMixelgram.SafePixel(c,r).g = 255;
				}

				//----------------------------------------------------------------------
				//  Send the mixelgram image into the network
				//----------------------------------------------------------------------
				_outpImg.Content().SetID(YARP_PIXEL_BGR);
				_outpImg.Content().Refer(_imgMixelgram);
				_outpImg.Write(1);
				
				//----------------------------------------------------------------------
				//  Send the procesed image back to the network (the image remains
				//  as received)
				//----------------------------------------------------------------------
				YARPImageOf<YarpPixelBGR>& tempp = _vImages[_soundTemplate.Length()-1];
				_outprecImg.Content().SetID(YARP_PIXEL_BGR);
				_outprecImg.Content().Refer(tempp);
				_outprecImg.Write(1);
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
				period = 0.0;
				counter = 0;
			}
		}


		YARPString name(file_name);
		_soundTemplate.Save(name);
		_soundTemplate.Destroy();

		return ;
	}
};

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
int 
calculateMixel(YARPCovMatrix &mXtX, 
			   YARPImageOf<YarpPixelBGR> * vImgs, 
			   int iSamples,
			   int i, int j)
{
	ACE_TRACE("calculateMixel");

    int n         = 0;        /** Sound number of componects.                           */
    int m         = 0;        /** Image number of components.                           */
    int ret;                  /** Return variable.                                      */
    
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
	mean /= _mV.NRows();
		
	//----------------------------------------------------------------------
	//  Calculate pixel covariance matrix.
	//----------------------------------------------------------------------
	////_mV.covarianceMatrix(_mYtY,0); //Calculate only variance matrix.

	////YMatrix& _mY = _mYtY.getOriginalVariancesMatrix(); /** The local variances matrix of mYtY. */
	_mV -= mean;
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
	double value = (1/2.0) * ACE::log2((_dDetXtX * _dDetVtV) / (double)_dDetCtC);

	////int pixel = min(255,max(0,255*mixel)) /////from Vuppla
	return(MIN(255,MAX(0,255*value)));
	//return(value);
}

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
int 
calculateMixel2(double * vRms, 
			   YARPImageOf<YarpPixelBGR> * vImgs, 
			   int iSamples,
			   int i, int j)
{
	ACE_TRACE("calculateMixel2");

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
	const double _dalpha = 50.0;

	YVector _vV;        /** Vector containing the gray values of pixels samples. */

    _vV.Resize(iSamples);
	
	//----------------------------------------------------------------------
	//  Fill PixelSamples matrix
	//----------------------------------------------------------------------
	double mean = 0.0;
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
	//double value = (0.5 * (ACE::log2(1.0 - _dR2)));
	double value = (-1.0)*(0.5 * (logf((1.0 - _dR2)* _dDampeningfactor)/(double)logf(2)));

	return(MIN(255,MAX(0,255*value)));
}

/** 
 * The main function.
 * 
 * @param argc The number of parameters
 * @param argv The buffer of parameters
 * 
 * @return 0 always
 */
int 
main(int argc, char* argv[])
{
	mainthread _thread;
	_thread.Begin();

	char c = 0;

	do {
		cout << "Type q+return to quit" << endl;
		cin >> c;
	}
	while (c != 'q');

	_thread.End(-1);
	
	return 0;
}
