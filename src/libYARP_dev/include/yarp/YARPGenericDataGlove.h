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
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPGenericDataGlove.h,v 1.1 2004-09-13 23:22:49 babybot Exp $
///
///

#ifndef __YARPGenericDataGloveh__
#define __YARPGenericDataGloveh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPDataGloveUtils.h>

// TO DO: Handle the sensor calibration.

template <class ADAPTER, class PARAMETERS>
class YARPGenericDataGlove
{
protected:
	ADAPTER _adapter;		/// adapts the hw idiosyncrasies
	PARAMETERS _params;		/// actual grabber specific parameters

public:
	YARPGenericDataGlove () {}
	~YARPGenericDataGlove () {}

	int initialize (unsigned short comPort,	unsigned int baudRate);
	int initialize (const PARAMETERS &param);
	int uninitialize (void);
	int getData(DataGloveData* data);
	int getSwitch(int* switchStatus);
	int getLed(int* ledStatus);
	int setLed(int* ledStatus);
	int resetGlove(void);
	int startStreaming(void);
	int stopStreaming(void);
	
};

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::initialize(unsigned short comPort, unsigned int baudRate)
{
	_params.comPort = comPort;
	_params.baudRate = baudRate;
	/// calls the adapter init that parses the params appropriately.
	/// this is because initialization can vary depending on the specific setup.
	return _adapter.initialize (_params);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::initialize (const PARAMETERS &param)
{
	_params = param;
	
	/// calls the adapter init that parses the params appropriately.
	/// this is because initialization can vary depending on the specific setup.
	return _adapter.initialize (_params);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::uninitialize (void)
{
	return _adapter.uninitialize ();
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::getData (DataGloveData *data)
{
	return _adapter.IOCtl (DGCMDGetData, (void *)data);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::startStreaming (void)
{
	return _adapter.IOCtl (DGCMDStartStreaming, NULL);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::stopStreaming (void)
{
	return _adapter.IOCtl (DGCMDStopStreaming, NULL);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::getLed (int* ledStatus)
{
	return _adapter.IOCtl (DGCMDGetLed, (void *)ledStatus);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::setLed (int* ledStatus)
{
	return _adapter.IOCtl (DGCMDSetLed, (void *)ledStatus);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::getSwitch (int* switchStatus)
{
	return _adapter.IOCtl (DGCMDGetSwitch, (void *)switchStatus);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericDataGlove<ADAPTER, PARAMETERS>::resetGlove (void)
{
	return _adapter.IOCtl (DGCMDResetGlove, NULL);
}

#endif
