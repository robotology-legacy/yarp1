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
//         Version:  $Id: YARPSoundResources.h,v 1.1 2004-02-20 16:58:59 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#define _WINDOWS
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
							Busy(false),
							Ready(true),
							dwBufferLength(8192),
							numSamples(2048),
							freqSample(44100),
							microDistance(0.14),
							nBitsSample(16)
	{
		//Initialize variables
		//Assign memory to variables and structures
		m_InRecord = FALSE;
		//VarMutexID = 2;
	}

	~SoundResources () { _uninitialize (); }

	//----------------------------------------------------------------------
	// Variables  
	//----------------------------------------------------------------------

	enum { _num_buffers = 3 }; //This can be used for the tripe buffer

	YARPSemaphore _bmutex;
	YARPSemaphore _new_frame;

	//Declare usefull variables
	//Declare buffers and buffers handlers
	// i.e. The next code
	//	PICOLOHANDLE _bufHandles[_num_buffers];
	//	PUINT8 _buffer[_num_buffers];
	//	PUINT8 _aligned[_num_buffers];

	HWAVEIN			m_WaveInHandle; // Handle to the WAVE In Device 
	HMIXER			m_MixerHandle; // Handle to Mixer for WAVE In Device
	//HANDLE			m_WaveFileHandle = INVALID_HANDLE_VALUE; // Handle to the disk file where we permanently store 
	// the recorded audio data 
	WAVEHDR			m_WaveHeader[2]; // We use two WAVEHDR's for recording (ie, double-buffering) in this example 
	BOOL			m_InRecord; // Variable used to indicate whether we are in record 
	unsigned char	m_DoneAll;	// Variable used by recording thread to indicate whether we are in record 

	MMRESULT		m_err;
	WAVEFORMATEX	m_waveFormat;
	MIXERLINE		m_mixerLine;
	HANDLE			m_waveInThread;
	unsigned long	m_n, m_numSrc;


	//----------------------------------------------------------------------
	//  Parameters
	//----------------------------------------------------------------------
	const bool Busy, Ready;

	//const int VarMutexID;

	const DWORD dwBufferLength;
	const DWORD numSamples;		//dwBufferLength/4
	const DWORD freqSample;
	const double microDistance; // babybot
	const WORD nBitsSample;	

	//----------------------------------------------------------------------
	// Public Method definitions 
	//----------------------------------------------------------------------
	int _initialize (const SoundOpenParameters& params);
	int _uninitialize (void);

protected:
	
	//----------------------------------------------------------------------
	//  Protected method definitions
	//----------------------------------------------------------------------
	int _init (const SoundOpenParameters& params);
	void _prepareBuffers (void);
};

// ===  FUNCTION  ======================================================================
// 
//         Name: RES  
// 
//  Description: This is a convenient function to recover 
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
SoundResources& RES(void *res) { return *(SoundResources *)res; }
