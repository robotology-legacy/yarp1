// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundResources.cpp
// 
//     Description:  This files implements the SoundResources methods
// 
//         Version:  $Id: YARPSoundResources.cpp,v 1.4 2004-02-26 16:21:35 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "YARPSoundResources.h"

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (public) _initialize
// Description:  This method is called from the driver level to initialize the resorces.
// Notice that some "open paramenters" can be pases as the method parameters. This is
// so to set the parameters to some setup dependent setting
//--------------------------------------------------------------------------------------
int 
SoundResources::_initialize (const SoundOpenParameters& params)
{
	_init (params);
	_prepareBuffers ();
	//start continuous acquisition
	if ((m_err = waveInStart(m_WaveInHandle)))
	{
		printf("yarpsounddriver: Error starting record! -- %08X\n", m_err);
	}

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class: SoundResources 
//      Method: (public)_uninitialize
// Description:  
//--------------------------------------------------------------------------------------
int
SoundResources::_uninitialize (void)
{
	/////_bmutex.Wait ();

	//Insert some class of stop function if exist in the hardware

	//Release memory (buffer, variables...etc)	
	/////_bmutex.Post ();
	mixerClose(m_MixerHandle);
	waveInReset(m_WaveInHandle);

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (protected) _init
// Description:  This is the internal class init method 
//--------------------------------------------------------------------------------------
int 
SoundResources::_init (const SoundOpenParameters& params)
{
	//----------------------------------------------------------------------
	//  Initialize the wave in
	//----------------------------------------------------------------------
	
	// Initialize the WAVEFORMATEX for 16-bit, 44KHz, stereo. That's what I want to record 
	m_waveFormat.wFormatTag 	 = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels 		 = 2;
	m_waveFormat.nSamplesPerSec  = freqSample;
	m_waveFormat.wBitsPerSample  = nBitsSample;
	m_waveFormat.nBlockAlign     = m_waveFormat.nChannels * (m_waveFormat.wBitsPerSample/8);
	m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;
	m_waveFormat.cbSize          = 0;

	m_err = waveInOpen(&m_WaveInHandle, 
					   WAVE_MAPPER, 
					   &m_waveFormat, 
					   (DWORD)params.m_callbackthread_identifier, //Here I have to add the thread ID 
					   (DWORD)this, 
					   CALLBACK_THREAD);

	if (m_err != MMSYSERR_NOERROR) {
		//PrintWaveErrorMsg(m_err, "Can't open WAVE In Device!");
		printf("Can't open WAVE In Device! %d",m_err);
		return(-2);
	} 

	//----------------------------------------------------------------------
	//  Initialize Mixter
	//----------------------------------------------------------------------
	m_err = mixerOpen(&m_MixerHandle, 
					(DWORD)m_WaveInHandle, 
					0, 
					0, 
					MIXER_OBJECTF_HWAVEIN);

	if (m_err != MMSYSERR_NOERROR) {
		printf("yarpsounddriver: Device does not have mixer support! -- %08X\n", m_err);
	}

	/* 
	 * This device should have a WAVEIN destination line. Let's get its ID so
	 * that we can determine what source lines are available to record from
	 */
	m_mixerLine.cbStruct = sizeof(MIXERLINE);
	m_mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, &m_mixerLine, MIXER_GETLINEINFOF_COMPONENTTYPE);

	if (m_err != MMSYSERR_NOERROR) {
		printf("Device does not have a WAVE recording control! -- %08X\n", m_err);
	}
	
	// Get how many source lines are available from which to record. 
	m_numSrc = m_mixerLine.cConnections;
	//select default source line
	_select_line(MIXERLINE_COMPONENTTYPE_SRC_LINE);

	//----------------------------------------------------------------------
	// Initialize the local buffers 
	//----------------------------------------------------------------------
	_bmutex.Wait ();
	_rawBuffer = new unsigned char [dwBufferLength];
	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.Post ();

	return YARP_OK; 
}

//--------------------------------------------------------------------------------------
//       Class: SoundResources 
//      Method: (protected) _prepareBuffer 
// Description: Prepares the internal buffers for the doble buffering. Later this could
// be done with three buffers.
//--------------------------------------------------------------------------------------
void 
SoundResources::_prepareBuffers(void)
{
	//Here I should prepare all the buffer memory allocation
	m_WaveHeader[1].dwBufferLength = m_WaveHeader[0].dwBufferLength = dwBufferLength;
	

	m_WaveHeader[0].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[0].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	m_WaveHeader[1].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[1].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	//Initialize dwFlags and dwLoops to 0. This seems to be necesary according to the
	//window documentation
	m_WaveHeader[0].dwFlags = m_WaveHeader[1].dwFlags = 0L;
	m_WaveHeader[0].dwLoops = m_WaveHeader[1].dwLoops = 0L; 

	//Lets initialize the headers
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[0], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);

	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[1], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (public)_select_line
// Description:  This method sets the active source line in the m_mixer_line variable. The idea
// is that externally one can chose the source line (microfone, in line...etc). Then, by
// chosing the control type (volume, mute, on/off) one can control the selected line.
//--------------------------------------------------------------------------------------
int
SoundResources::_select_line(unsigned int type)
{
	for(int i = 0; i < m_mixerLine.cConnections; i++) {
		m_mixerLine.cbStruct = sizeof(MIXERLINE);	
		m_mixerLine.dwSource = i;

		m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle,
								 &m_mixerLine,
								 MIXER_GETLINEINFOF_SOURCE);
		
		if (m_err != MMSYSERR_NOERROR) continue;
		
		if (m_mixerLine.dwComponentType == type)
			printf("yarpsounddriver: source line found\n");
	}
	printf("yarpsounddriver: -warning- source line not found");
	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (public)_select_control
// Description:  This methos allows to select the type of control in the selected line
//--------------------------------------------------------------------------------------
int
SoundResources::_select_control(unsigned int control_type)
{
	/*
	 * Fill the mixerLineControls structure
	 */
	m_mixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	m_mixerLineControls.dwLineID = m_mixerLine.dwLineID;
	m_mixerLineControls.cControls = 1;
	//m_mixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	m_mixerLineControls.dwControlType = control_type;
	m_mixerLineControls.pamxctrl = &m_mixerControlArray;
	m_mixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);

	m_err = mixerGetLineControls((HMIXEROBJ)m_MixerHandle, 
							   &m_mixerLineControls, 
							   MIXER_GETLINECONTROLSF_ONEBYTYPE);
	
	if (m_err != MMSYSERR_NOERROR) printf("yarpsounddriver: %s has no mute control!\n", m_mixerLine.szName);
	
	return YARP_OK;
}
