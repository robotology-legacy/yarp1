/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
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
/// $Id: mCan.h,v 1.1 2004-07-12 23:40:29 babybot Exp $
///
///

#ifndef __MCAN_HEADER__
#define __MCAN_HEADER__

#include <windows.h>
#include "icsnVC40.h"


/// prototypes

bool canInitLibrary(void);
void canReleaseLibrary(void);
// int icsneoOpenPortEx(	int lPortSerialNumber,int lPortType, int lDriverType, int lIPAddressMSB, int lIPAddressLSBOrBaudRate, int bConfigRead, unsigned char * bNetworkID, int * hObject);
int canOpenPort( int lPortSerialNumber, int lPortType, int lDriverType, int lIPAddressMSB, int lIPAddressLSBOrBaudRate, int bConfigRead, unsigned char * bNetworkID, int * hObject);
// int icsneoClosePort(int hObject, int * pNumberOfErrors);
int canClosePort(int hObject, int * pNumberOfErrors);
// void icsneoFreeObject(int hObject);
void canFreeObject(int hObject);
// int icsneoGetMessages(int hObject, icsSpyMessage * pMsg, int * pNumberOfMessages, int * pNumberOfErrors);
int canGetMessages(int hObject, icsSpyMessage * pMsg, int * pNumberOfMessages, int * pNumberOfErrors);
// int icsneoTxMessages(int hObject, icsSpyMessage * pMsg, int lNetworkID, int lNumMessages);
int canPutMessages(int hObject, icsSpyMessage * Msg, int lNetworkID, int lNumMessages);
// int icsneoFindAllCOMDevices(int lDriverType, int lGetSerialNumbers, int lStopAtFirst, int lUSBCommOnly, int *p_lDeviceTypes, int *p_lComPorts, int *p_lSerialNumbers, int *lNumDevices);
int canFindAllDevices(int lDriverType, int lGetSerialNumbers, int lStopAtFirst, int lUSBCommOnly, int *p_lDeviceTypes, int *p_lComPorts, int *p_lSerialNumbers, int *lNumDevices);
// int icsneoSendConfiguration(int hObject, unsigned char * pData, int lNumBytes);
int canSendConfiguration(int hObject, unsigned char * pData, int lNumBytes);
// int icsneoGetConfiguration(int hObject, unsigned char * pData, int * p_lNumBytes);
int canGetConfiguration(int hObject, unsigned char * pData, int * p_lNumBytes);
// int icsneoGetErrorMessages(int hObject, int * pErrorMsgs, int * pNumberOfErrors);
int canGetErrors(int hObject, int * pErrorMsgs, int * pNumberOfErrors);

#endif


