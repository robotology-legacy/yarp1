// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundidentification.cpp
// 
//     Description:  This code is inteded to implement a sound processing front-end. This 
//					 consist in a sound parametrization throuth a Short Time Fast Fourier
//					 Transform procedure. Later a MFCC (Mel-Frequency Cepstral Coefficients)
//					 is implemeted to provide a sound representation with a parameter reducted
//					 representation.
// 
//         Version:  $Id: soundidentification.cpp,v 1.7 2004-08-24 10:51:21 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPString.h>
#include <YARPTime.h>
#include <YARPSound.h>
#include <YARPSoundPortContent.h>
#include <YARPImageDraw.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>

#include "YARPDtw.h"
//#include "YARPVectorBuffer.h"
#include "YARPSoundTemplate.h"
#include "soundidentificationprocessing.h"

const int   __outSize    = 5;
const char *__baseName   = "/soundidentification/";
const char *__configFile = "sound.ini";

// =====================================================================================
//        Class:  mainthread
// 
//  Description:  Runs the main processing loop 
// 
//       Author:  Eng. Carlos Beltran
//      Created:  28/07/2004
//     Revision:  none
// =====================================================================================
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
		
		//----------------------------------------------------------------------
		// Port declarations 
		//----------------------------------------------------------------------
		YARPInputPortOf<YARPSoundBuffer> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
		YARPInputPortOf<YARPBottle> _inAction(YARPInputPort::DOUBLE_BUFFERS, YARP_UDP);
		YARPOutputPortOf<YVector> _outPort_mfcc(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

		YARPScheduler::setHighResScheduling();

		SoundIdentificationProcessing _soundIndprocessor(__configFile,  __outSize);
		size = _soundIndprocessor.GetSize();

		YARPString base1(__baseName);
		YARPString base2(__baseName);

		_inPort.Register(base1.append("i").c_str());
		_outPort_mfcc.Register(base2.append("srmo").c_str());

		time1 = YARPTime::GetTimeAsSeconds();

		//----------------------------------------------------------------------
		// Main loop.
		//----------------------------------------------------------------------
		YARPString file_name("mfcc.txt");

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
        int _sound_order          = 0;
        int _sound_save           = 0;
        int _pre_sound_save       = 0;
        int _sound_identificate   = 0;
        int _pre_sound_idenficate = 0;
        bool _save                = false;
        bool _identify            = false;

		while(!IsTerminated())
		{
			counter++;

			_inPort.Read(); // Read sound stream 
			_soundIndprocessor.apply(_inPort.Content(),_out_mfcc); 

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

			int ret = sound_template.Add(_out_mfcc);

			if (!ret) // The add has failed because no space in the template
			{
				sound_template.Resize();
				sound_template.Add(_out_mfcc);
			}

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

// ===  FUNCTION  ======================================================================
// 
//         Name:  Main
// 
//  Description:  
// 
//    Author:  Carlos Beltran
//  Revision:  28/07/2004 19:10:18 W. Europe Daylight Time
// =====================================================================================
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
