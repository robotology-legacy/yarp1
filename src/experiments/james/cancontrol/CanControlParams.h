/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    # pasa #				                          ///
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
/// $Id: CanControlParams.h,v 1.1 2005-11-23 16:03:28 babybot Exp $
///
///

#ifndef __CanControlParamsh__
#define __CanControlParamsh__


#define CANBUS_DEVICE_NUM			0
#define CANBUS_ARBITRATION_ID		0
#define CANBUS_MY_ADDRESS			0
#define CANBUS_POLLING_INTERVAL		2		/// [ms]
#define CANBUS_TIMEOUT				50		/// 10 * POLLING

#define DEFAULT_NJOINTS				2		/// 1 card connected
#define DEFAULT_DESTINATION			0xf		/// 

#define CANBUS_MAXCARDS				ESD_MAX_CARDS

#define TIMER_ID					666
#define TIMER2_ID					667

#define GUI_REFRESH_INTERVAL		150		/// [ms]

#define CANBUS_INIFILE				"canbus.ini"

#endif
