// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPGenericSoundGrabber.h
// 
//     Description:  
// 
//         Version:  $Id: YARPGenericSoundGrabber.h,v 1.4 2004-05-24 09:17:36 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __YARPGenericSoundGrabberh__
#define __YARPGenericSoundGrabberh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPSoundCardUtils.h>

// =====================================================================================
//        Class:  YARPGenericSoundGrabber
// 
//  Description:  
// 
//       Author:  Carlos Beltran
//      Created:  25/02/2004 16:02:18 
//     Revision:  none
// =====================================================================================
template <class ADAPTER, class PARAMETERS>
class YARPGenericSoundGrabber
{
protected:
	ADAPTER    _adapter;   /// adapts the hw idiosyncrasies
	PARAMETERS _params;    /// actual grabber specific parameters

public:
	YARPGenericSoundGrabber () {}
	~YARPGenericSoundGrabber () {}

	int initialize (int, int, int, int, int);
	int initialize (const PARAMETERS &param);
	int uninitialize (void);
	int acquireBuffer (unsigned char **buffer);
	int releaseBuffer (void);
	int waitOnNewFrame (void);
	int setVolume(int);
	int setMute(int);
};

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  initialize
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::initialize (int board, 
															  int channels, 
															  int samplespersecond,
															  int bitspersample,
															  int bufferlength)
{
	//_params._unit_number  = board;
	_params.m_Channels 		= channels;
	_params.m_SamplesPerSec = samplespersecond;
	_params.m_BitsPerSample = bitspersample;
	_params.m_BufferLength  = bufferlength;

	return _adapter.initialize (_params);
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  initialize
// Description:  Overloaded version
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::initialize (const PARAMETERS &param)
{
	_params = param;
	return _adapter.initialize (_params);
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::uninitialize (void)
{
	return _adapter.uninitialize ();
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGraber
//      Method:  acquireBuffer
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::acquireBuffer (unsigned char **buffer)
{
	return _adapter.IOCtl (SCMDAcquireBuffer, (void *)buffer);
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  releaseBuffer
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::releaseBuffer (void)
{
	return _adapter.IOCtl (SCMDReleaseBuffer, NULL);
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  waitOnNewFrame
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::waitOnNewFrame (void)
{
	return _adapter.IOCtl (SCMDWaitNewFrame, NULL);
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  setVolume
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::setVolume(int volume)
{
	int ret;
	ret = _adapter.IOCtl(SCMDSetVolume,&volume);
	return ret;
}

//--------------------------------------------------------------------------------------
//       Class:  YARPGenericSoundGrabber
//      Method:  setMute
// Description:  
//--------------------------------------------------------------------------------------
template <class ADAPTER, class PARAMETERS>
int YARPGenericSoundGrabber<ADAPTER, PARAMETERS>::setMute(int mute_value)
{
	int ret;
	ret = _adapter.IOCtl(SCMDSetMute,&mute_value);
	return ret;
}
#endif
