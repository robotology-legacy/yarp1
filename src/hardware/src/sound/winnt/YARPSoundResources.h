// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundResources.h
// 
//     Description:  This file encapsulates the SoundResources class and the its methods
// 
//         Version:  $Id: YARPSoundResources.h,v 1.7 2004-03-03 15:56:20 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================
#ifndef __YARPSoundResourcesh__
#define __YARPSoundResourcesh__

#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>
#include "YARPSoundDeviceDriver.h"
// =====================================================================================
//        Class: SoundResources 
// 
//  Description:  This class encapsulates all the sound resorces. It is used later by the
//  driver level.
// 
//       Author:  Ing. Carlos Beltran
//      Created:  13/02/2004 15:39:33 W. Europe Standard Time
//     Revision:  none
// =====================================================================================
class SoundResources
{
public:
	//----------------------------------------------------------------------
	//  Constructor/Destructor
	//----------------------------------------------------------------------
	SoundResources (void) : _bmutex(1),
							_new_frame(0),
							_canpost(true),
							numSamples(2048),
							microDistance(0.14)
	{
		//----------------------------------------------------------------------
		// Initialize variables
		// Default: 16-bit, 44KHz, stereo
		//----------------------------------------------------------------------
		m_InRecord     = false;
		dwBufferLength = 8192;
		freqSample     = 44100;
		nBitsSample    = 16;
		channels       = 2;
	}

	~SoundResources () { _uninitialize (); }

	//----------------------------------------------------------------------
	// Variables  
	//----------------------------------------------------------------------
	YARPSemaphore _bmutex;
	YARPSemaphore _new_frame;
	bool          _canpost;

	//Declare usefull variables
    HWAVEIN       m_WaveInHandle;    // Handle to the WAVE In Device
    HMIXER        m_MixerHandle;     // Handle to Mixer for WAVE In Device
    WAVEHDR       m_WaveHeader[3];   // We use two WAVEHDR's for recording (ie, double-buffering) in this example
    bool          m_InRecord;        // Variable used to indicate whether we are in record
    unsigned char m_DoneAll;         // Variable used by recording thread to indicate whether we are in record

    MMRESULT      m_err;
    WAVEFORMATEX  m_waveFormat;
    MIXERLINE     m_mixerLine;
    HANDLE        m_waveInThread;
    unsigned long m_n;
    unsigned long m_numSrc;
	
	// Control structures 
	MIXERCONTROL      m_mixerControlArray;
	MIXERLINECONTROLS m_mixerLineControls;

	//Local lockable buffer
	unsigned char *_rawBuffer;

	//----------------------------------------------------------------------
	//  Parameters
	//----------------------------------------------------------------------
	DWORD        dwBufferLength;
	const DWORD  numSamples;       //dwBufferLength/4
	DWORD        freqSample;
	const double microDistance;    // babybot
	WORD         nBitsSample;
	WORD         channels;

	//----------------------------------------------------------------------
	// Public Method definitions 
	//----------------------------------------------------------------------
	int _initialize (const SoundOpenParameters& params);
	int _uninitialize (void);
	int _select_line(unsigned int type);
	int _select_control(unsigned int control);

protected:
	
	//----------------------------------------------------------------------
	//  Protected method definitions
	//----------------------------------------------------------------------
	int _init (const SoundOpenParameters& params);
	void _prepareBuffers (void);
	void _print_dst_lines();
	void _print_src_lines();
};
#endif
