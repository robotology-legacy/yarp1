/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPGenericGrabber.h,v 1.9 2004-01-29 10:06:46 beltran Exp $
///
///

#ifndef __YARPGenericGrabberh__
#define __YARPGenericGrabberh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPFrameGrabberUtils.h>
// #include <YARPPicoloOnBabybotAdapter.h>

template <class ADAPTER, class PARAMETERS>
class YARPGenericGrabber
{
protected:
	ADAPTER _adapter;		/// adapts the hw idiosyncrasies
	PARAMETERS _params;		/// actual grabber specific parameters

public:
	YARPGenericGrabber () {}
	~YARPGenericGrabber () {}

	int initialize (int board, int sizex, int sizey = -1);
	int initialize (const PARAMETERS &param);
	int uninitialize (void);
	int acquireBuffer (unsigned char **buffer);
	int releaseBuffer (void);
	int waitOnNewFrame (void);
	int getWidth (int *w);
	int getHeight (int *h);
	int setBright(unsigned int);
	int setHue(unsigned int);
	int setContrast(unsigned int);
	int setSatU(unsigned int);
	int setSatV(unsigned int);
	int setLNotch(int);
	int setLDec(int);
	int setPeak(int);
	int setCagc(int);
	int setCkill(int);
	int setRange(int);
	int setYsleep(int);
	int setCsleep(int);
	int setCrush(int);
	int setGamma(int);
	int setDithFrame(int);
};

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::initialize (int board, int sizex, int sizey /* = -1 */)
{
	_params._unit_number = board;
	_params._video_type = 0;
	_params._size_x = sizex;
	if (sizey > 0)
		_params._size_y = sizey;
	else
		_params._size_y = sizex;

	/// calls the adapter init that parses the params appropriately.
	/// this is because initialization can vary depending on the specific setup.
	return _adapter.initialize (_params);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::initialize (const PARAMETERS &param)
{
	_params = param;
	
	/// calls the adapter init that parses the params appropriately.
	/// this is because initialization can vary depending on the specific setup.
	return _adapter.initialize (_params);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::uninitialize (void)
{
	return _adapter.uninitialize ();
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::acquireBuffer (unsigned char **buffer)
{
	return _adapter.IOCtl (FCMDAcquireBuffer, (void *)buffer);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::releaseBuffer (void)
{
	return _adapter.IOCtl (FCMDReleaseBuffer, NULL);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::waitOnNewFrame (void)
{
	return _adapter.IOCtl (FCMDWaitNewFrame, NULL);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::getWidth (int *w)
{
	return _adapter.IOCtl (FCMDGetSizeX, w);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::getHeight (int *h)
{
	return _adapter.IOCtl (FCMDGetSizeY, h);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setBright(unsigned int bright)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetBright, &bright);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setHue(unsigned int hue)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetHue, &hue);
	return ret; 	
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setContrast(unsigned int contrast)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetContrast, &contrast);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setSatU(unsigned int satu)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetSatU, &satu);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setSatV(unsigned int satv)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetSatV,&satv);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setLNotch(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetLNotch,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setLDec(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetLDec,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setPeak(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetPeak,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCagc(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCagc,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCkill(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCkill,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setRange(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetRange,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setYsleep(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetYsleep,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCsleep(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCsleep,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCrush(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCrush,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setGamma(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetGamma,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setDithFrame(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetDithFrame,&state);
	return ret;
}
#endif
