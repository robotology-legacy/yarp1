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
//         Version:  $Id: YARPSoundDeviceDriver.cpp,v 1.2 2004-02-23 18:19:19 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#define _WINDOWS

#include "YARPSoundResources.h"
#include <YARPSoundCardUtils.h>

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
	//m_cmds[FCMDAcquireBuffer] = &YARPSoundDeviceDriver::acquireBuffer;
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
	
	//Start the thread first. The identifier is necesary in the lower level	
	Begin ();  //Will see if this is necessary (probably it is)
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
	//PICOLOSTATUS SoundStatus;
	SoundResources& d = RES(system_resources);

	/////const int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	/////SetPriority (prio);

	/// strategy, waits, copy into lockable buffer.
	while (!IsTerminated())	
	{
		//SoundStatus = SoundWaitEvent (d._picoloHandle, PICOLO_EV_END_ACQUISITION);

		/*
		readfro = startbuf;

		for (i = 0; i < d._num_buffers; i++)
		{
			if (d._bmutex.PollingWait () == 1)
			{
				/// buffer acquired.
				/// read from buffer
				memcpy (d._rawBuffer, d._aligned[readfro], d._nImageSize);
					
				if (d._canpost)
				{
					d._canpost = false;
					d._new_frame.Post();
				}

				d._bmutex.Post ();
			}
			else
			{
				/// can't acquire, it means the buffer is still in use.
				/// silently ignores this condition.
				ACE_DEBUG ((LM_DEBUG, "lost a frame, acq thread\n"));
			}

			readfro = ((readfro + 1) % d._num_buffers);

			/// 40 ms delay
			if (i < d._num_buffers-1)
				DelayInSeconds (0.040);
		} /// end for
		*/
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
	/////d._bmutex.Wait ();
	/////(*(unsigned char **)buffer) = d._rawBuffer;

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
	/////d._canpost = true;
	/////d._bmutex.Post ();

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
	/////d._new_frame.Wait ();
	///d._new_frame.acquire ();

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
void 
set_mute(void *cmd)
{
	MIXERCONTROLDETAILS_UNSIGNED	value[2];
	MIXERCONTROLDETAILS				mixerControlDetails;
	MMRESULT						err;
	
	SoundResources& d = RES(system_resources);

	unsigned int * m_mute_state = (unsigned int *) cmd;

	/*
	 * Select the mute control in the selected line
	 */
	d._select_control(MIXERCONTROL_CONTROLTYPE_MUTE);
	
	mixerControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mixerControlDetails.dwControlID = mixerControlArray.dwControlID;
	mixerControlDetails.cChannels = d.m_mixerLine->cChannels;
	
	if (mixerControlDetails.cChannels > 2) mixerControlDetails.cChannels = 2;
	
	if (mixerControlArray.fdwControl & MIXERCONTROL_CONTROLF_UNIFORM) mixerControlDetails.cChannels = 1;

	mixerControlDetails.cMultipleItems = 0;
	mixerControlDetails.paDetails = &value[0];
	mixerControlDetails.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	value[0].dwValue = value[1].dwValue = m_mute_state;

	err = mixerSetControlDetails((HMIXEROBJ)MixerHandle, 
								 &mixerControlDetails, 
								 MIXER_SETCONTROLDETAILSF_VALUE);
	
	if( err != MMSYSERR_NOERROR) printf("yarpsounddriver: Error #%d setting mute for %s!\n", err, mixerLine->szName);
}

//--------------------------------------------------------------------------------------
//       Class:  YARPSoundDeviceDriver
//      Method:  set_volume
// Description:  This function set the volume in both channels
//--------------------------------------------------------------------------------------
void 
set_volume(void *cmd)
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
	
	mixerControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mixerControlDetails.dwControlID = mixerControlArray.dwControlID;
	mixerControlDetails.cChannels = 2;
	mixerControlDetails.cMultipleItems = 0;
	mixerControlDetails.paDetails = &value[0];
	mixerControlDetails.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	value[0].dwValue = value[1].dwValue = m_volume;

	err = mixerSetControlDetails((HMIXEROBJ)MixerHandle, 
								 &mixerControlDetails, 
								 MIXER_SETCONTROLDETAILSF_VALUE);
	
	if( err != MMSYSERR_NOERROR) printf("yarpsounddriver: Error #%d setting mute for %s!\n", err, mixerLine->szName);
}

#undef _WINDOWS
