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
/// $Id: YARPNIDAQDeviceDriver.cpp,v 1.1 2004-07-13 13:21:09 babybot Exp $
///

#include "YARPNIDAQDeviceDriver.h"
#include "../dd_orig/include/nidaqex.h"


int YARPNIDAQDeviceDriver::open(void *d)
{
	NIDAQOpenParameters *tmp = (NIDAQOpenParameters *) d;

	m_handle = (void *) tmp->device_id;

	return 0;	// no error
}

int YARPNIDAQDeviceDriver::close(void)
{
	m_handle = 0;

	// TODO: check whether we need a SCAN stop or something like that ...
	return 0;	// no error
}

int YARPNIDAQDeviceDriver::ai_configure(void *p)
{
	i16 st = 0;
	NIDAQAIConfigureParameters *tmp = (NIDAQAIConfigureParameters *) p;

	st = AI_Configure((i16) m_handle,tmp->chan,
						tmp->inputMode,
						tmp->inputRange,
						tmp->polarity,
						tmp->driveAIS);
	return st;
}

int YARPNIDAQDeviceDriver::scan_setup(void *p)
{
	i16 st = 0;
	NIDAQScanSetupParameters *tmp = (NIDAQScanSetupParameters *) p;

	st = SCAN_Setup((i16) m_handle, tmp->nCh,
					tmp->ch_sequence,
					tmp->ch_gains);

	return st;
}

int YARPNIDAQDeviceDriver::ai_vread_scan(void *p)
{
	i16 st = 0;

	st = AI_VRead_Scan((i16) m_handle, (f64 *) p);

	return st;
}

int YARPNIDAQDeviceDriver::ai_read_scan(void *p)
{
	i16 st = 0;

	st = AI_Read_Scan((i16) m_handle, (i16 *) p);

	return st;
}