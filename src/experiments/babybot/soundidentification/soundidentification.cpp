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
/// $Id: soundidentification.cpp,v 1.10 2004-09-06 16:40:27 beltran Exp $
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

#include "YARPDtw.h"
#include "YARPSoundTemplate.h"
#include "soundidentificationprocessing.h"

using namespace std;

const int   __outSize    = 5;
const char *__baseName   = "/soundidentification/";
const char *__configFile = "sound.ini";
int _sizex = 256;
int _sizey = 256; /** @todo Put this as an external parameter?. */

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
		//YARPVectorBuffer vector_buff;
		YARPSoundTemplate sound_template;
        YARPBottle m_bottle;              // This bottle is used to control externally the
                                          // action to be taken.
        YVector _out_mfcc(L_VECTOR_MFCC); // The vector for the MFCC coefficients

        YARPImageOf<YarpPixelBGR> _out_mix;   /** The mixelgram image.    */
        YARPImageOf<YarpPixelBGR> _input_img; /** The normal input image. */

        _out_img.Resize  (_sizex,_sizey);
        _input_img.Resize(_sizex,_sizey);

		_out_img.Zero();
		
		YARPScheduler::setHighResScheduling();
		
		//----------------------------------------------------------------------
		// Port declarations 
		//----------------------------------------------------------------------
        YARPInputPortOf<YARPSoundBuffer>   _inpSound (YARPInputPort::DEFAULT_BUFFERS ,_protocol);
        YARPInputPortOf<YARPBottle>        _inpAction(YARPInputPort::DEFAULT_BUFFERS ,_protocol);
        YARPInputPortOf<YARPGenericImage>  _inpImg   (YARPInputPort::DEFAULT_BUFFERS ,_protocol);
        YARPOutputPortOf<YVector>          _outpMfcc (YARPOutputPort::DEFAULT_OUTPUTS,_protocol);
        YARPOutputPortOf<YARPGenericImage> _outpImg  (YARPOutputPort::DEFAULT_OUTPUTS,_protocol);

        _inpSound.SetAllowShmem (_sharedmem);
        _inpAction.SetAllowShmem(_sharedmem);
        _inpImg.SetAllowShmem   (_sharedmem);
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
        int  mixel_value           = 0;

		while(!IsTerminated())
		{
			counter++;

			_inpSound.Read(); // Read sound stream 
			_soundIndprocessor.apply(_inpSound.Content(),_out_mfcc); 
			
			int ret = sound_template.Add(_out_mfcc, 2); // Adds new vector; bufferize if necessary

			if (!ret) // The add has failed because no space in the template
			{
				sound_template.Resize();
				sound_template.Add(_out_mfcc);
			}
			
			//Read the image from the image port
			_input_img.Refer(_inpImg.Content());

			//----------------------------------------------------------------------
			//  Go to calculate the mixel for each pixel in the image.
			//----------------------------------------------------------------------
			for(i = 0; i < _input_img.GetHeight(); i++ )
				for( j = 0; j < _input_img.GetWidth(); j++)
				{
					mixel_value = CalculateMixel(YARPSoundTemplate &stemp, YARPGenericImage &img);
					//somehow put the value of the mixel in the output image
					/////_output_mix(i,j) = mixel_value;
					//send the mixelgram image into the network
					//
					_outpImg.Content().Refer(_output_mix);
					_outpImg.Write();
				}

#if 0
			_inAction.Read(); // Read what to do with the sound than now is beeing received
			m_bottle = _inAction.Content();

			//Read and Proccess the action
			_pre_sound_save = _sound_save;
			_pre_sound_identification = _sound_indentification;
			m_bottle.readInt((int *)&_sound_order);
			m_bottle.readInt((int *)&_sound_save);
			m_bottle.readInt((int *)&_sound_identificate);

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
			////_outPort_mfcc.Content() = _out_mfcc;
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
		sound_template.Save(name);
		sound_template.Destroy();

		return ;
	}
};

/** 
  * Calculates the mixel using the mutual information from a sound stream and an image.
  * @todo add mutual information formula.
  * 
  * @param stemp The sound template containing the sound parametrization.
  * @param img   The image with the pixels to be analized.
  * @param i     The mixel/pixel 'w' position in the image
  * @param j     The mixel/pixel 'h' position in the image
  * 
  * @return The value of the mixel using the mutual information formula between the sound
  * and the image.
  */
int 
CalculateMixel(YARPSoundTemplate &stemp, YARPGenericImage &img, int i, int j)
{

    YarpPixelBGR pix(0,0,0);
	YARPMatrix soundcovariance;
	YARPMatrix pixelcovariance;
	YARPMatrix mutualcovariance;

	// Calculate sound covariance matrix
	stemp.CovarianceMatrix(soundcovariance);	

	// Caculate pixel covariance matrix
	pix = img.SafePixel(i,j);
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
