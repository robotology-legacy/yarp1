/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/////////////////////////////////////////////////////////////////////////

///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPFrameGrabberUtils.h,v 1.2 2004-07-20 00:34:25 babybot Exp $
///
///

/**
 * \file YARPFrameGrabberUtils.h contains definition for the frame
 * acquisition classes.
 */

#ifndef __YARPFrameGrabberUtilsh__
#define __YARPFrameGrabberUtilsh__

#include <yarp/YARPConfig.h>

/**
 * The messages accepted by the frame grabber device drivers.
 */
enum FrameGrabberCmd
{
	FCMDAcquireBuffer = 1,
	FCMDReleaseBuffer = 2,
	FCMDWaitNewFrame = 3,
	FCMDGetSizeX = 4,
	FCMDGetSizeY = 5,
	FCMDSetBright = 6,
	FCMDSetHue = 7,
	FCMDSetContrast = 8,
	FCMDSetSatU = 9,
	FCMDSetSatV = 10,
	FCMDSetLNotch = 11,
	FCMDSetLDec = 12,
	FCMDSetPeak = 13,
	FCMDSetCagc = 14,
	FCMDSetCkill = 15,
	FCMDSetRange = 16,
	FCMDSetYsleep = 17,
	FCMDSetCsleep = 18,
	FCMDSetCrush = 19,
	FCMDSetGamma = 20,
	FCMDSetDithFrame = 21,

	FCMDNCmds = 22
};

#endif
