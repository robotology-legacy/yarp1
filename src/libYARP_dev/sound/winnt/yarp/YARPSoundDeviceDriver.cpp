// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundDeviceDriver.cpp
// 
//     Description:  
// 
//         Version:  $Id: YARPSoundDeviceDriver.cpp,v 1.1 2004-07-12 23:40:29 babybot Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#define _WINDOWS

#include <YARPSoundCardUtils.h>
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

//----------------------------------------------------------------------
//  End of the definition of the SoundResources class
//----------------------------------------------------------------------

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
	if ((m_err = waveInStart(m_WaveInHandle))) {
		printf("yarpsounddriver: Error starting record! -- %08X\n", m_err);
	}
	m_InRecord = true;

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
	_bmutex.Wait ();
	
	m_InRecord = false;
    mixerClose(m_MixerHandle);   // Close mixer
    waveInReset(m_WaveInHandle); // Reset the wave input device

    if(_rawBuffer != NULL){
        delete[] _rawBuffer;     // Delete the shared buffer
        _rawBuffer = NULL;
    }
    _bmutex.Post ();

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
	//  Initalize local parameters with the open params. It the open param
	//  is != 0 then the local param is updated if not the default value is
	//  used.
	//  Default configuration:
	//  16-bit, 44KHz, stereo
	//----------------------------------------------------------------------
	if (params.m_Channels      != 0) channels       = params.m_Channels;
	if (params.m_SamplesPerSec != 0) freqSample     = params.m_SamplesPerSec;
	if (params.m_BitsPerSample != 0) nBitsSample    = params.m_BitsPerSample;
	if (params.m_BufferLength  != 0) dwBufferLength = params.m_BufferLength;

	//----------------------------------------------------------------------
	//  Initialize the wave in
	//----------------------------------------------------------------------
	m_waveFormat.wFormatTag 	 = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels 		 = channels;
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

	//----------------------------------------------------------------------
	//  Print all the present lines in the audio interface
	//----------------------------------------------------------------------
	printf("yarpsounddriver: LINES PRESENT\n");
	_print_dst_lines();
	
	//----------------------------------------------------------------------
	// This device should have a WAVEIN destination line. Let's get its ID so
	// that we can determine what source lines are available to record from
	//----------------------------------------------------------------------
	m_mixerLine.cbStruct        = sizeof(MIXERLINE);
	m_mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, 
							 &m_mixerLine, 
							 MIXER_GETLINEINFOF_COMPONENTTYPE);

	if (m_err != MMSYSERR_NOERROR) {
		printf("Device does not have a WAVE recording control! -- %08X\n", m_err);
	}
	
	m_numSrc = m_mixerLine.cConnections; // Get how many source lines are available from which to record. 
	m_err = _select_line(MIXERLINE_COMPONENTTYPE_SRC_LINE); //select default source line

	if (m_err == YARP_FAIL){ //Not line found, trying the auxiliary....
		m_err = _select_line(MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY);
	}

	if (m_err != YARP_OK)
		printf("yarpsounddriver: Atention Source line not found!!!\n");
		
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
// Description: Prepares the internal buffers for the doble buffering. 
//--------------------------------------------------------------------------------------
void 
SoundResources::_prepareBuffers(void)
{
	//----------------------------------------------------------------------
	//  Preparing all memory buffer allocation
	//----------------------------------------------------------------------
	m_WaveHeader[2].dwBufferLength = 
	m_WaveHeader[1].dwBufferLength = 
	m_WaveHeader[0].dwBufferLength = dwBufferLength;

	m_WaveHeader[0].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[0].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	m_WaveHeader[1].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[1].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	m_WaveHeader[2].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[2].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	
	//----------------------------------------------------------------------
	// Initialize dwFlags and dwLoops to 0. This seems to be necesary according to the
	// Microsoft Windows documentation 
	//----------------------------------------------------------------------
	m_WaveHeader[0].dwFlags = m_WaveHeader[1].dwFlags = m_WaveHeader[2].dwFlags = 0L;
	m_WaveHeader[0].dwLoops = m_WaveHeader[1].dwLoops = m_WaveHeader[2].dwFlags = 0L; 

	//----------------------------------------------------------------------
	// Initialize the headers
	//----------------------------------------------------------------------
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[0], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[1], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[2], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);

	//----------------------------------------------------------------------
	//  It is necessary to queue the two buffers.  
	//----------------------------------------------------------------------
	if ((m_err = waveInAddBuffer(m_WaveInHandle, &m_WaveHeader[0], sizeof(WAVEHDR))))
		printf("yarpsounddriver: Error queueing WAVEHDR 1! -- %08X\n", m_err);
	if ((m_err = waveInAddBuffer(m_WaveInHandle, &m_WaveHeader[1], sizeof(WAVEHDR))))
		printf("yarpsounddriver: Error queueing WAVEHDR 2! -- %08X\n", m_err);
	if ((m_err = waveInAddBuffer(m_WaveInHandle, &m_WaveHeader[2], sizeof(WAVEHDR))))
		printf("yarpsounddriver: Error queueing WAVEHDR 2! -- %08X\n", m_err);
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
	for(int i = 0; i < m_numSrc; i++) {
		m_mixerLine.cbStruct = sizeof(MIXERLINE);	
		m_mixerLine.dwSource = i;

		m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle,
								 &m_mixerLine,
								 MIXER_GETLINEINFOF_SOURCE);
		
		if (m_err != MMSYSERR_NOERROR) continue;
		
		if (m_mixerLine.dwComponentType == type)
		{
			printf("yarpsounddriver: source line found\n");
			return YARP_OK;
		}	
	}
	printf("yarpsounddriver: -warning- source line not found\n");
	return YARP_FAIL;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  _print_dst_lines
// Description:  This function prints in the screen the source lines present
//--------------------------------------------------------------------------------------
void
SoundResources::_print_dst_lines()
{
	MIXERCAPS mixerCaps;

	mixerGetDevCaps(0,&mixerCaps, sizeof(mixerCaps));

	for (int i = 0; i < mixerCaps.cDestinations; i++)
	{
		m_mixerLine.cbStruct      = sizeof(MIXERLINE);
		m_mixerLine.dwSource      = 0;
		m_mixerLine.dwDestination = i;

		if (!(m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, &m_mixerLine, MIXER_GETLINEINFOF_DESTINATION))) {
				printf("\t#%lu: %s\n", i, m_mixerLine.szName);
		}
		m_numSrc = m_mixerLine.cConnections;
		_print_src_lines();
	}
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  _print_src_lines
// Description:  This function prints in the screen the source lines present
//--------------------------------------------------------------------------------------
void
SoundResources::_print_src_lines()
{
	for (int i = 0; i < m_numSrc; i++)
	{
		m_mixerLine.cbStruct = sizeof(MIXERLINE);
		m_mixerLine.dwSource = i;

		if (!(m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, &m_mixerLine, MIXER_GETLINEINFOF_SOURCE)))
		{
			if (m_mixerLine.dwComponentType != MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER)
				printf("\t\t#%lu: %s\n", i, m_mixerLine.szName);
		}
	}
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (public)_select_control
// Description:  This methos allows to select the type of control in the selected line
//--------------------------------------------------------------------------------------
int
SoundResources::_select_control(unsigned int control_type)
{
	//----------------------------------------------------------------------
	//  Fill the mixerLineControls structure
	//----------------------------------------------------------------------
	m_mixerLineControls.cbStruct      = sizeof(MIXERLINECONTROLS);
	m_mixerLineControls.dwLineID      = m_mixerLine.dwLineID;
	m_mixerLineControls.cControls     = 1;
	m_mixerLineControls.dwControlType = control_type;
	m_mixerLineControls.pamxctrl      = &m_mixerControlArray;
	m_mixerLineControls.cbmxctrl      = sizeof(MIXERCONTROL);

	m_err = mixerGetLineControls((HMIXEROBJ)m_MixerHandle, 
								 &m_mixerLineControls, 
								 MIXER_GETLINECONTROLSF_ONEBYTYPE);
	
	if (m_err != MMSYSERR_NOERROR) 
		printf("yarpsounddriver: %s has no %s control!\n", 
			   m_mixerLine.szName, 
			   m_mixerLineControls.pamxctrl->szName);
	
	return YARP_OK;
}

// ===  FUNCTION  ======================================================================
// 
//         Name: RES  
// 
//  Description: This is a convenient function to recover the pointer to the SoundResources
//  object  
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
inline SoundResources& RES(void *res) { return *(SoundResources *)res; }

/*************************************************
 * Implementation of class YARPSoundDeviceDriver *
 *************************************************/

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  (Constructor) YARSoundDeviceDriver
// Description:  Initializes and sets the IOCtl functions mapping
//--------------------------------------------------------------------------------------
YARPSoundDeviceDriver::YARPSoundDeviceDriver(void) : 
YARPDeviceDriver<YARPNullSemaphore, YARPSoundDeviceDriver>(SCMDNCmds)
{
	system_resources = (void *) new SoundResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call. Here the mapping for the IOCtl should be included
	m_cmds[SCMDAcquireBuffer] = &YARPSoundDeviceDriver::acquireBuffer;
	m_cmds[SCMDReleaseBuffer] = &YARPSoundDeviceDriver::releaseBuffer;
	m_cmds[SCMDWaitNewFrame]  = &YARPSoundDeviceDriver::waitOnNewFrame;
	m_cmds[SCMDSetMute]       = &YARPSoundDeviceDriver::set_mute;
	m_cmds[SCMDSetVolume]     = &YARPSoundDeviceDriver::set_volume;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  ~YARPSoundDeviceDriver 
// Description:  (Destructor)
//--------------------------------------------------------------------------------------
YARPSoundDeviceDriver::~YARPSoundDeviceDriver()
{
	if (system_resources != NULL)
		delete (SoundResources *)system_resources;
	system_resources = NULL;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  open 
// Description:  (public) this is one of the external visible method. It supports the 
// abstraction (open,close,IOCtl). 
//--------------------------------------------------------------------------------------
int 
YARPSoundDeviceDriver::open (void *res)
{
	int ret;
	SoundResources& d = RES(system_resources);
	
	Begin ();  //Start the thread first. The identifier is necesary in the lower level	
	
	/***************************************************************************
	 * The thread ID is necesary to run the waveIn call that uses it to assing *
	 * the callback message to this thread. Therefore, the Body part of the    *
	 * thread will receive the messages (linked internally to hardware         *
	 * interrupts) generated by the low level sound driver.                    *
	 ***************************************************************************/
	((SoundOpenParameters *)res)->m_callbackthread_identifier = this->identifier;
	
	ret = d._initialize (*(SoundOpenParameters *)res);

	return ret;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  close
// Description:  (public) This is the close of the abstraction (open, close, IOCtl..)
//--------------------------------------------------------------------------------------
int 
YARPSoundDeviceDriver::close (void)
{
	SoundResources& d = RES(system_resources);

	End (); //This ends the thread. Will see if is really necessary in the sound driver	

	int ret = d._uninitialize ();

	return ret;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  Body()
// Description:  Here, the body of the thread should be implemented
//--------------------------------------------------------------------------------------
void 
YARPSoundDeviceDriver::Body (void)
{
	SoundResources& d = RES(system_resources);
	MSG		msg;

	const int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	SetPriority (prio);
	
	//----------------------------------------------------------------------
	//  Wait for a message sent by the audio driver
	//----------------------------------------------------------------------
	while (GetMessage(&msg, 0, 0, 0) == 1 && !IsTerminated()) {

		switch (msg.message) {
			case MM_WIM_DATA: //Buffer filled 
				/*********************************************************************************
				 * the msg.lParam contains a pointer to the WAVEHDR structure for the filled buffer. *
				 *********************************************************************************/
				if (((WAVEHDR *)msg.lParam)->dwBytesRecorded) {
					//----------------------------------------------------------------------
					//  Here write in the local buffer using the syncronization mutexes
					//----------------------------------------------------------------------
					if (d._bmutex.PollingWait () == 1) { // buffer acquired. Reading from the buffer
						if (d.m_InRecord)
							memcpy (d._rawBuffer, 
									((WAVEHDR *)msg.lParam)->lpData, 
                                    ((WAVEHDR *)msg.lParam)->dwBytesRecorded); // Note: is this right?
                                                                               // The buffer could not be full...
						if (d._canpost) {
							d._canpost = false;
							d._new_frame.Post();
						}
						d._bmutex.Post ();
					}
					else
					{
						//----------------------------------------------------------------------
						//  can't acquire, it means the buffer is still in use.
						//  silently ignores this condition.
						//----------------------------------------------------------------------
						ACE_DEBUG ((LM_DEBUG, "lost a frame, acq thread\n"));
					}
				}

				//----------------------------------------------------------------------
				//  Requeue the used buffer	
				//----------------------------------------------------------------------
				waveInAddBuffer(d.m_WaveInHandle, 
								(WAVEHDR *)msg.lParam, 
								sizeof(WAVEHDR));

				break;
				/* Our main thread is opening the WAVE device */
			case MM_WIM_OPEN:
				ACE_DEBUG ((LM_DEBUG, "yarpsounddriver: sound device opened\n"));
				break;
				/* Our main thread is closing the WAVE device */
			case MM_WIM_CLOSE:
				break;
			default:
				//ACE_DEBUG ((LM_DEBUG, "yarpsounddriver: received an unknown message\n"));
				break;
		}
	}

	ACE_DEBUG ((LM_DEBUG, "acquisition thread returning...\n"));
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  acquireBuffer
// Description:  This method blocks the access to the lockable buffer. This allows to
// obtain access to a clear copy without DMA interferences
//--------------------------------------------------------------------------------------
int 
YARPSoundDeviceDriver::acquireBuffer (void *buffer)
{
	SoundResources& d = RES(system_resources);
	d._bmutex.Wait ();
	(*(unsigned char **)buffer) = d._rawBuffer;

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  releaseBuffer
// Description:  This method release the mutex controlling the exclusive access to the 
// buffer containing the data, thus the thread can write in the common buffer getting the
// data from the memory used by the DMA.
//--------------------------------------------------------------------------------------
int
YARPSoundDeviceDriver::releaseBuffer (void *cmd)
{
	SoundResources& d = RES(system_resources);
	d._canpost = true;
	d._bmutex.Post ();

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  YARSoundDeviceDriver
//      Method:  waitOnNewFrame
// Description:  This call blocks until new data is available in the common buffer
//--------------------------------------------------------------------------------------
int 
YARPSoundDeviceDriver::waitOnNewFrame (void *cmd)
{
	SoundResources& d = RES(system_resources);
	d._new_frame.Wait ();

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  set_mute
// Description:  This function set the mute state in the already selected line.
// Use:
// 	0 -> mute
// 	1 -> No mute
//--------------------------------------------------------------------------------------
int 
YARPSoundDeviceDriver::set_mute(void *cmd)
{
	MIXERCONTROLDETAILS_UNSIGNED	value[2];
	MIXERCONTROLDETAILS				mixerControlDetails;
	MMRESULT						err;
	
	SoundResources& d = RES(system_resources);

	unsigned int * m_mute_state = (unsigned int *) cmd;
	
	//----------------------------------------------------------------------
	//  Select the mute control in the selected line
	//----------------------------------------------------------------------
	//d._select_control(MIXERCONTROL_CONTROLTYPE_MUTE);
	d._select_control(MIXERCONTROL_CONTROLTYPE_ONOFF);
	
	mixerControlDetails.cbStruct    = sizeof(MIXERCONTROLDETAILS);
	mixerControlDetails.dwControlID = d.m_mixerControlArray.dwControlID;
	mixerControlDetails.cChannels   = d.m_mixerLine.cChannels;
	
	if (mixerControlDetails.cChannels > 2) mixerControlDetails.cChannels = 2;
	
	if (d.m_mixerControlArray.fdwControl & MIXERCONTROL_CONTROLF_UNIFORM) mixerControlDetails.cChannels = 1;

	mixerControlDetails.cMultipleItems = 0;
	mixerControlDetails.paDetails      = &value[0];
	mixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_UNSIGNED);

	value[0].dwValue = value[1].dwValue = (*m_mute_state);

	err = mixerSetControlDetails((HMIXEROBJ)d.m_MixerHandle, 
								 &mixerControlDetails, 
								 MIXER_SETCONTROLDETAILSF_VALUE);
	
	if( err != MMSYSERR_NOERROR) printf("yarpsounddriver: Error #%d setting mute for %s!\n", err, d.m_mixerLine.szName);

	return YARP_OK;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  set_volume
// Description:  This function set the volume in both channels
//--------------------------------------------------------------------------------------
int 
YARPSoundDeviceDriver::set_volume(void *cmd)
{
	MIXERCONTROLDETAILS_UNSIGNED	value[2];
	MIXERCONTROLDETAILS				mixerControlDetails;
	MMRESULT						err;
	
	SoundResources& d = RES(system_resources);

	unsigned int * m_volume = (unsigned int *) cmd;

	/*
	 * Select the control type in the current line
	 */
	d._select_control(MIXERCONTROL_CONTROLTYPE_VOLUME);
	
	mixerControlDetails.cbStruct       = sizeof(MIXERCONTROLDETAILS);
	mixerControlDetails.dwControlID    = d.m_mixerControlArray.dwControlID;
	mixerControlDetails.cChannels      = 2;
	mixerControlDetails.cMultipleItems = 0;
	mixerControlDetails.paDetails      = &value[0];
	mixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_UNSIGNED);

	value[0].dwValue = value[1].dwValue = (*m_volume);

	err = mixerSetControlDetails((HMIXEROBJ)d.m_MixerHandle, 
								 &mixerControlDetails, 
								 MIXER_SETCONTROLDETAILSF_VALUE);
	
	if( err != MMSYSERR_NOERROR) printf("yarpsounddriver: Error #%d setting mute for %s!\n", err, d.m_mixerLine.szName);
	
	return YARP_OK;
}
#undef _WINDOWS
