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
//         Version:  $Id: YARPSoundResources.cpp,v 1.1 2004-02-20 16:58:59 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "YARPSoundResources.h"

//----------------------------------------------------------------------
//  Useful functions. But, this should be declared in another place..
//----------------------------------------------------------------------
double
GetTimeAsSeconds(void)
{
	ACE_Time_Value timev = ACE_OS::gettimeofday ();
	return double(timev.sec()) + timev.usec() * 1e-6; 
}

void
DelayInSeconds(double delay_in_seconds)
{
	ACE_Time_Value tv;
	tv.sec (int(delay_in_seconds));
	tv.usec ((delay_in_seconds-int(delay_in_seconds)) * 1e6);

	ACE_OS::sleep(tv);
}

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
	//_prepareBuffers ();

	//SoundStatus = SooundSetBufferList (_picoloHandle, BufferList, _num_buffers);
	/////ACE_ASSERT (SoundStatus == PICOLO_OK);

	// select initial buffer.

	// starts continuous acquisition.

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class: SoundResources 
//      Method: _uninitialize
// Description:  
//--------------------------------------------------------------------------------------
int
SoundResources::_uninitialize (void)
{
	/////_bmutex.Wait ();

	//Insert some class of stop function if exist in the hardware

	//Release memory (buffer, variables...etc)	
	/////_bmutex.Post ();

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (private) _init
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
	if (m_err) {
		//PrintWaveErrorMsg(err, "Can't open WAVE In Device!");
		printf("Can't open WAVE In Device! %d",m_err);
		return(-2);
	} 

	//----------------------------------------------------------------------
	//  Initialize Mixter
	//----------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------
	// Initialize the local buffers 
	//----------------------------------------------------------------------
	/*
	_bmutex.Wait ();
	_rawBuffer = new unsigned char [_nImageSize];
	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.Post ();
	*/

	/// all ok, store the handle.

	//return ret;
	return 0; //This should return an error/success value
}

//--------------------------------------------------------------------------------------
//       Class: SoundResources 
//      Method: (Private) _prepareBuffer 
// Description:  
//--------------------------------------------------------------------------------------
void 
SoundResources::_prepareBuffers(void)
{
	//Here I should prepare all the buffer memory allocation
}



