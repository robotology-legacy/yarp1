/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #pasa#				                          ///
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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: main.cpp,v 1.1 2005-04-15 00:17:44 babybot Exp $
///
///


#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/log_msg.h>

#include <yarp/YARPControlBoardUtils.h>
#include <yarp/YARPValueCanDeviceDriver.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPRobotMath.h>



static FILE *	__file_handle = NULL;

static BOOL init_log (void)
{
	if (__file_handle == NULL)
	{
		__file_handle = fopen ("log.txt", "w");
		if (__file_handle != NULL)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

static BOOL close_log (void)
{
	if (__file_handle != NULL)
		fclose (__file_handle);
	__file_handle = NULL;

	return TRUE;
}

/// Use wprintf like TRACE0, TRACE1, ... (The arguments are the same as printf)
static int xfprintf(char *fmt, ...)
{
	if (__file_handle == NULL)
		return -1;

	char s[300];
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsprintf(s, fmt, argptr);
	va_end(argptr);

	fprintf (__file_handle, "%s", s);
	return(cnt);
}

static int flush_log (void)
{
	if (__file_handle == NULL)
		return -1;

	fflush (__file_handle);
	return 0;
}

static int init_null (void) { return YARP_OK; }
static int close_null (void) { return YARP_OK; }

#define INITLOG init_log
#define CLOSELOG close_log
#define PRINTLOG xfprintf
#define FLUSHLOG flush_log

#define MAX_ARM_JNTS 12
#define MAX_HEAD_JNTS 8


const int CANBUS_DEVICE_NUM			= 0;
const int CANBUS_ARBITRATION_ID		= 0;
const int CANBUS_MY_ADDRESS			= 0;
const int CANBUS_POLLING_INTERVAL	= 20;			/// [ms]
const int CANBUS_TIMEOUT			= 10;			/// 10 * POLLING
const int CANBUS_MAXCARDS			= MAX_CARDS;

const unsigned char _destinations[CANBUS_MAXCARDS] = { 0x0f, 0x0e, 0x0d, 0x0c, 
													   0x0b, 0x0a, 0x09, 0x08,
													   0x80, 0x80, 0x80, 0x80,
													   0x80, 0x80, 0x80, 0x80 };

YARPValueCanDeviceDriver head;
YARPValueCanDeviceDriver arm;


///
///
///
int main (int argc, char *argv[])
{
	bool _headinitialized = false;
	bool _arminitialized = false;
	bool _headrunning = false;
	bool _armrunning = false;
	
	double *_headjointstore = NULL;
	double *_armjointstore = NULL;

	/// init robot.
	INITLOG ();
	PRINTLOG ("Log started\n");
	FLUSHLOG();

	/// head (bus 1).
	ValueCanOpenParameters op_par;
	op_par._port_number = CANBUS_DEVICE_NUM;
	op_par._arbitrationID = CANBUS_ARBITRATION_ID;
	memcpy (op_par._destinations, _destinations, sizeof(unsigned char) * CANBUS_MAXCARDS);
	op_par._my_address = CANBUS_MY_ADDRESS;					/// my address.
	op_par._polling_interval = CANBUS_POLLING_INTERVAL;		/// thread polling interval [ms].
	op_par._timeout = CANBUS_TIMEOUT;						/// approx this value times the polling interval [ms].

	op_par._njoints = MAX_HEAD_JNTS;
	op_par._p = NULL;

	if (head.open ((void *)&op_par) < 0)
	{
		head.close();
		ACE_OS::printf ("head driver open failed\n");
	}
	else
	{
		// filters out certain messages.
		int msg = 20;
		head.IOCtl(CMDSetDebugMessageFilter, (void *)&msg);
		_headinitialized = true;
	}
	
	/// arm (bus 2).
	op_par._njoints = MAX_ARM_JNTS;
	op_par._p = PRINTLOG;

	if (arm.open ((void *)&op_par) < 0)
	{
		arm.close();
		ACE_OS::printf ("arm driver open failed\n");
	}
	else
	{
		// filters out certain messages.
		int msg = 20;
		arm.IOCtl(CMDSetDebugMessageFilter, (void *)&msg);
		_arminitialized = true;
	}

	if (!_headinitialized && !_arminitialized)
	{
		ACE_OS::printf ("No device found\n");
		return -1;
	}

	ACE_OS::printf ("The robot is initialized\n");
	if (_headinitialized)
	{
		_headjointstore = new double[MAX_HEAD_JNTS];
		ACE_ASSERT (_headjointstore != NULL);
	}
	if (_arminitialized)
	{
		_armjointstore = new double[MAX_ARM_JNTS];
		ACE_ASSERT (_armjointstore != NULL);
	}

	ACE_OS::printf ("Running!");
	
	int cycle;
	for (cycle = 0; ; cycle++)
	{
		/// read from head.
		int ret;
		
		if (_headinitialized)
		{
			ret = head.IOCtl(CMDGetPositions, _headjointstore);
			if (ret != YARP_OK)
				ACE_OS::printf ("troubles reading head joint pos\n");

			ACE_OS::printf ("head: ");
			for (int i = 0; i < MAX_HEAD_JNTS; i++)
			{
				ACE_OS::printf ("%.2f ", _headjointstore[i]);
			}
			ACE_OS::printf ("\n");
		}
		
		if (_arminitialized)
		{
			ret = arm.IOCtl(CMDGetPositions, _armjointstore);
			if (ret != YARP_OK)
				ACE_OS::printf ("troubles reading arm joint pos\n");

			ACE_OS::printf ("arm: ");
			for (int i = 0; i < MAX_ARM_JNTS; i++)
			{
				ACE_OS::printf ("%.2f ", _armjointstore[i]);
			}
			ACE_OS::printf ("\n");
		}

		Sleep (150);
		FLUSHLOG();
	}


	if (_headinitialized)
	{
		head.close();
		if (_headjointstore != NULL) delete[] _headjointstore;
	}

	if (_arminitialized)
	{
		arm.close();
		if (_armjointstore != NULL) delete[] _armjointstore;
	}

	CLOSELOG();

	return 0;
}
