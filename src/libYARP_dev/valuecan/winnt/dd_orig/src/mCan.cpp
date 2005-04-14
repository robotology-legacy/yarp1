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
/// $Id: mCan.cpp,v 1.4 2005-04-14 22:10:53 babybot Exp $
///
///

#include "mCan.h"

typedef INT (CALLBACK* LPFOPEN) (int,int,int,int,int,int,unsigned char *,int *);
typedef INT (CALLBACK* LPFCLOSE)(int,int *);
typedef void (CALLBACK* LPFFREEOBJ)(int);
typedef INT (CALLBACK* LPFGETMSG)(int,icsSpyMessage *,int *,int *);
typedef INT (CALLBACK* LPFPUTMSG)(int,icsSpyMessage *,int,int);
typedef INT (CALLBACK* LPFGETDEV)(int,int,int,int,int *,int *,int *,int *);
typedef INT (CALLBACK* LPFSNDCFG)(int,unsigned char *,int);
typedef INT (CALLBACK* LPFGETCFG)(int,unsigned char *,int *);
typedef INT (CALLBACK* LPFGETERR)(int,int *,int *);

static HINSTANCE hDLL = NULL;
static LPFOPEN open_F = NULL;
static LPFCLOSE close_F = NULL;
static LPFFREEOBJ freeObject_F = NULL;
static LPFGETMSG getMessages_F = NULL;
static LPFPUTMSG putMessages_F = NULL;
static LPFGETDEV getDevices_F = NULL;
static LPFSNDCFG sendConfig_F = NULL;
static LPFGETCFG getConfig_F = NULL;
static LPFGETERR getErrors_F = NULL;

static int instance_counter = 0;
static HANDLE mutex = NULL;

bool canInitLibrary(void)
{
	mutex = CreateMutex (NULL, TRUE, "neovidllmutex");
	if (mutex == NULL)
		return false;	/// creation failed.

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		WaitForSingleObject (mutex, INFINITE);

	if (instance_counter == 0)
	{
		hDLL = LoadLibrary("icsneo40.dll");
		if (hDLL < (HINSTANCE)HINSTANCE_ERROR)
		{
			ReleaseMutex (mutex);
			return false;
		}

		open_F = (LPFOPEN)GetProcAddress(hDLL, "icsneoOpenPortEx");
		if (!open_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}
		
		close_F = (LPFCLOSE)GetProcAddress(hDLL, "icsneoClosePort");
		if (!close_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}

		freeObject_F = (LPFFREEOBJ)GetProcAddress(hDLL, "icsneoFreeObject");
		if (!freeObject_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}

		getMessages_F = (LPFGETMSG)GetProcAddress(hDLL, "icsneoGetMessages");
		if (!getMessages_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}

		putMessages_F = (LPFPUTMSG)GetProcAddress(hDLL, "icsneoTxMessages");
		if (!putMessages_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}
		
		getDevices_F = (LPFGETDEV)GetProcAddress(hDLL, "icsneoFindAllCOMDevices");
		if (!getDevices_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}

		sendConfig_F = (LPFSNDCFG)GetProcAddress(hDLL, "icsneoSendConfiguration");
		if (!sendConfig_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}
		
		getConfig_F = (LPFGETCFG)GetProcAddress(hDLL, "icsneoGetConfiguration");
		if (!getConfig_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}
		
		getErrors_F = (LPFGETERR)GetProcAddress(hDLL, "icsneoGetErrorMessages");
		if (!getErrors_F)
		{
			FreeLibrary(hDLL);       
			ReleaseMutex (mutex);
			return false;
		}
	}

	instance_counter ++;

	ReleaseMutex (mutex);
	return true;
}

void canReleaseLibrary(void)
{
	WaitForSingleObject (mutex, INFINITE);

	instance_counter --;
	if (instance_counter <= 0 && hDLL != NULL)
	{
		instance_counter = 0;
		FreeLibrary(hDLL);
	}

	ReleaseMutex (mutex);
	CloseHandle (mutex);
	mutex = NULL;
}

int canOpenPort( int lPortSerialNumber, int lPortType, int lDriverType, int lIPAddressMSB, int lIPAddressLSBOrBaudRate, int bConfigRead, unsigned char * bNetworkID, int * hObject)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*open_F)(lPortSerialNumber, lPortType, lDriverType, lIPAddressMSB, lIPAddressLSBOrBaudRate, bConfigRead, bNetworkID, hObject);	
	ReleaseMutex (mutex);
	return ret;
}

int canClosePort(int hObject, int * pNumberOfErrors)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*close_F)(hObject, pNumberOfErrors);	
	ReleaseMutex (mutex);
	return ret;
}

void canFreeObject(int hObject)
{
	WaitForSingleObject (mutex, INFINITE);
	(*freeObject_F)(hObject);	
	ReleaseMutex (mutex);
}

int canGetMessages(int hObject, icsSpyMessage * pMsg, int * pNumberOfMessages, int * pNumberOfErrors)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*getMessages_F)(hObject, pMsg, pNumberOfMessages, pNumberOfErrors);	
	ReleaseMutex (mutex);
	return ret;
}

int canPutMessages(int hObject, icsSpyMessage * Msg, int lNetworkID, int lNumberOfMessages)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*putMessages_F)(hObject, Msg, lNetworkID, lNumberOfMessages);	
	ReleaseMutex (mutex);
	return ret;
}

int canFindAllDevices(int lDriverType, int lGetSerialNumbers, int lStopAtFirst, int lUSBCommOnly, int *p_lDeviceTypes, int *p_lComPorts, int *p_lSerialNumbers, int *lNumDevices)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*getDevices_F)(lDriverType, lGetSerialNumbers, lStopAtFirst, lUSBCommOnly, p_lDeviceTypes, p_lComPorts, p_lSerialNumbers, lNumDevices);	
	ReleaseMutex (mutex);
	return ret;
}

int canSendConfiguration(int hObject, unsigned char * pData, int lNumBytes)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*sendConfig_F)(hObject, pData, lNumBytes);	
	ReleaseMutex (mutex);
	return ret;
}

int canGetConfiguration(int hObject, unsigned char * pData, int * lNumBytes)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*getConfig_F)(hObject, pData, lNumBytes);	
	ReleaseMutex (mutex);
	return ret;
}

int canGetErrors(int hObject, int * pErrorMsgs, int * pNumberOfErrors)
{
	WaitForSingleObject (mutex, INFINITE);
	int ret = (*getErrors_F)(hObject, pErrorMsgs, pNumberOfErrors);	
	ReleaseMutex (mutex);
	return ret;
}