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
/// $Id: soundidentification.cpp,v 1.12 2004-09-13 17:54:25 beltran Exp $
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

#include <iostream>
#include <ace/Vector_T.h>
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

typedef YARPImageOf<YarpPixelBGR> BGRImage_Type;
typedef ACE_Vector<BGRImage_Type> vImages_Type;
//typedef ACE_Vector<YARPGenericImage>::iterator vImages_Iterator;

int 
calculateMixel(YARPCovMatrix &mXtX, vImages_Type &vImgs, int i, int j); 

const int   __outSize    = 5;
const char *__baseName   = "/soundidentification/";
const char *__configFile = "sound.ini";
int _sizex = 256;
int _sizey = 256; /** @todo Put this as an external parameter?. */

/**
 * Some variable for the network connection.
 */
bool _sharedmem = true;
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
		//YARPVectorBuffer vector_buff;
		YARPSoundTemplate _soundTemplate;
        YARPBottle _botActions;            // This bottle is used to control externally the
                                           // action to be taken.
        YVector _vOutMfcc(L_VECTOR_MFCC);  // The vector for the MFCC coefficients
		YARPCovMatrix _mSoundCov;
		YMatrix _mLocVar;

        BGRImage_Type _imgMixelgram;      /** The mixelgram image.    */
        BGRImage_Type _imgInput;          /** The normal input image. */

        vImages_Type _vImages(_soundTemplate.Size()); /** Vector of images.       */

        _imgMixelgram.Resize(_sizex,_sizey);
        _imgInput.Resize(_sizex,_sizey);

		_imgMixelgram.Zero();
		
		YARPScheduler::setHighResScheduling();
		
		//----------------------------------------------------------------------
		// Port declarations 
		//----------------------------------------------------------------------
        YARPInputPortOf<YARPSoundBuffer>   _inpSound (YARPInputPort::DEFAULT_BUFFERS ,YARP_UDP);
        YARPInputPortOf<YARPBottle>        _inpAction(YARPInputPort::DEFAULT_BUFFERS ,YARP_UDP);
        YARPInputPortOf<YARPGenericImage>  _inpImg   (YARPInputPort::DEFAULT_BUFFERS ,YARP_UDP);
        YARPOutputPortOf<YVector>          _outpMfcc (YARPOutputPort::DEFAULT_OUTPUTS,YARP_UDP);
        YARPOutputPortOf<YARPGenericImage> _outpImg  (YARPOutputPort::DEFAULT_OUTPUTS,YARP_MCAST);

        //_inpSound.SetAllowShmem (_sharedmem);
        //_inpAction.SetAllowShmem(_sharedmem);
        //_inpImg.SetAllowShmem   (_sharedmem);
        _outpMfcc.SetAllowShmem (_sharedmem);
        _outpImg.SetAllowShmem  (_sharedmem);

		SoundIdentificationProcessing _soundIndprocessor(__configFile,  __outSize);
		size = _soundIndprocessor.GetSize();

		YARPString file_name("mfcc.txt"); // Name of the file where to save the mfcc if necessary
		YARPString base1(__baseName); YARPString base2(__baseName);
		YARPString base3(__baseName); YARPString base4(__baseName); YARPString base5(__baseName);

        _inpSound.Register (base1.append("i"     ).c_str());
        _inpAction.Register(base2.append("action").c_str());
        _inpImg.Register   (base3.append("action").c_str());
        _outpMfcc.Register (base4.append("mfcc"  ).c_str());
        _outpImg.Register  (base5.append("mfcc"  ).c_str());

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
        int  _sound_order          = 0;
        int  _sound_save           = 0;
        int  _pre_sound_save       = 0;
        int  _sound_identificate   = 0;
        int  _pre_sound_idenficate = 0;
        bool _save                 = false;
        bool _identify             = false;
        int _dMixelValue           = 0;

		while(!IsTerminated())
		{
			int i,j;
			counter++;

			_inpSound.Read(); // Read sound stream 
			_soundIndprocessor.apply(_inpSound.Content(),_vOutMfcc); 
			
            int ret = _soundTemplate.Add(_vOutMfcc, 2);  // Adds new vector; bufferize if necessary
            ACE_ASSERT(ret != 0);
            _soundTemplate.CovarianceMatrix(_mSoundCov); // Calculate the sound covariance

            _imgInput.Refer(_inpImg.Content());          // Read the image from the image port
            _vImages.push_back(_imgInput);               // Add new image to the images vector

			//----------------------------------------------------------------------
			//  Go to calculate the mixel for each pixel in the image.
			//----------------------------------------------------------------------
			for(i = 0; i < _imgInput.GetHeight(); i++ )
				for( j = 0; j < _imgInput.GetWidth(); j++)
				{
					_dMixelValue = calculateMixel(_mSoundCov, _vImages, i, j);
					_auxpix.r = _dMixelValue;
					_auxpix.g = 0.0;
					_auxpix.b = 0.0;
					_imgMixelgram.SafePixel(i,j) = _auxpix;
				}
			
			//send the mixelgram image into the network
			_outpImg.Content().Refer(_imgMixelgram);
			_outpImg.Write();
#if 0
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

			// Sends the mfcc coefficients
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
  * @param vImgs   The image with the pixels to be analized.
  * @param i     The mixel/pixel 'w' position in the image
  * @param j     The mixel/pixel 'h' position in the image
  * 
  * @return The value of the mixel using the mutual information formula between the sound
  * and the image.
  */
int 
calculateMixel(YARPCovMatrix &mXtX, 
			   vImages_Type &vImgs, 
			   int i, int j)
{
    int n         = 0;        /** Sound number of componects.                           */
    int m         = 0;        /** Image number of components.                           */
    int _iSamples = 0;        /** Number of samples in the temporal dimension.          */
    int ret;                  /** Return variable.                                      */

    YarpPixelBGR _pix(0,0,0); /** Temporal pixel.                                       */

    YARPExMatrix  _mV;        /** Matrix with the pixel samples (in the time domain).   */
    YARPCovMatrix _mYtY;      /** Covariance matrix of _mV.                             */
    YARPCovMatrix _mCtC;      /** Mutual covariance matrix.                             */
    YMatrix _mY;              /** The local variances matrix of mYtY.                   */
    YMatrix _mX;              /** The local variances matrix of mXtX.                   */
    YMatrix _mC;              /** The local variances matrix for the mutual covariance. */


    n = mXtX.NRows();         // The soundcovariance matrix determinates the
                              // number of parameters used to parametrice the sound
    m = 3;                    // The pixel contains 3 bytes for RGB data
    _iSamples = vImgs.size(); // The size of the images vector dictates the number of
                              // samples that are being used. It should be the same
                              // for the sound.

    _mV.Resize(_iSamples,3);
	_mC.Resize(_iSamples,n+m);

	ret = mXtX.getOriginalVariancesMatrix(_mX);
	ACE_ASSERT(ret != YARP_FAIL);

	// Fill PixelSamples matrix
	for (int z = 0; z < _iSamples; z++)
	{
		_pix = vImgs[z].SafePixel(i,j);
		_mV(z,1) = _pix.r;
		_mV(z,2) = _pix.g;
		_mV(z,3) = _pix.b;
	}
		
	// Caculate pixel covariance matrix
	_mV.covarianceMatrix(_mYtY);
	ret = _mYtY.getOriginalVariancesMatrix(_mY);
	ACE_ASSERT(ret != YARP_FAIL);

	// Calculate local mutual variances matrix
	for (int r = 1; r <= _iSamples; r++)
		for (int c = 1; c <= _mC.NCols(); c++)
		{
			if ( c <= n)
				_mC(r,c) = _mX(r,c); 
			else
				_mC(r,c) = _mY(r,c-n);
		}

	// Calculate mutual covariance
	_mCtC.setOriginalVariancesMatrix(_mC);
	//_mCtC.calculateCovariance();
	//
	double _dDetCtC = 0.0;
	double _dDetXtX = 0.0;
	double _dDetYtY = 0.0;

	_mCtC.determinant(_dDetCtC);
	mXtX.determinant(_dDetXtX);
	_mYtY.determinant(_dDetYtY);

	double value = 1/2 * ACE::log2((_dDetXtX * _dDetYtY) / _dDetCtC);

	//pixel = min(255,max(0,255*mixel)) from Vuppla
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
