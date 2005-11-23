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
/// $Id: main.cpp,v 1.1 2005-11-23 16:10:54 babybot Exp $
///
///

#include <yarp/YARPConfig.h>
#include <yarp/YARPRateThread.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPRobotHardware.h>

#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPRobotcubHead.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPRobotMath.h>


/// global variables.
///
YARPHead head;
YARPArm arm;


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

#define GUI_REFRESH_INTERVAL		150		// [ms]
#define N_POSTURES					10		// what can be stored (limit).

const char *const HEAD_INI_FILE = "head.ini";
const char *const ARM_INI_FILE = "arm.ini";

#define MAX_ARM_JNTS 15
#define MAX_HEAD_JNTS 8

///
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

	char *root = GetYarpRoot();
	char path[512];

	ACE_OS::sprintf (path, "%s/%s\0", root, ConfigFilePath); 

	// initialize head controller on bus 1.
	YARPRobotcubHeadParameters parameters;
	parameters.load (YARPString(path), YARPString(HEAD_INI_FILE));
	parameters._message_filter = 20;
	parameters._p = NULL; //xprintf;

	int ret = head.initialize(parameters);
	if (ret != YARP_OK)
	{
		_headinitialized = false;
		ACE_OS::printf ("Can't start the interface with the robot head, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, HEAD_INI_FILE);
	}
	else
	{
		_headinitialized = true;
		_headrunning = false;
		ACE_ASSERT (head.nj() == MAX_HEAD_JNTS); // not other size is allowed.
	}

	// initialize arm controller on bus 2.
	YARPRobotcubArmParameters aparameters;
	aparameters.load (YARPString(path), YARPString(ARM_INI_FILE));
	aparameters._message_filter = 20;
	aparameters._p = (int (*) (char *, ...)) PRINTLOG;

	ret = arm.initialize(aparameters);
	if (ret != YARP_OK)
	{
		_arminitialized = false;
		ACE_OS::printf ("Can't start the interface with the robot arm/hand, please make sure the hardware and control cards are on.\nThe ini file was: %s%s", path, ARM_INI_FILE);
	}
	else
	{
		_arminitialized = true;
		_armrunning = false;
		ACE_ASSERT (arm.nj() == MAX_ARM_JNTS);
	}

	if (!_headinitialized && !_arminitialized)
	{
		ACE_OS::printf ("No device found\n");
		return -1;
	}

	ACE_OS::printf ("The robot is initialized\n");
	if (_headinitialized)
	{
		_headjointstore = new double[head.nj()];
		ACE_ASSERT (_headjointstore != NULL);
	}
	if (_arminitialized)
	{
		_armjointstore = new double[arm.nj()];
		ACE_ASSERT (_armjointstore != NULL);
	}

	ACE_OS::printf ("Running!\n");

	int cycle;
	for (cycle = 0; ; cycle++)
	{
		if (_headinitialized)
		{
			head.getPositions (_headjointstore);			
			ACE_OS::printf ("head: ");
			for (int i = 0; i < MAX_HEAD_JNTS; i++)
			{
				ACE_OS::printf ("%.2f ", _headjointstore[i]);
			}
			ACE_OS::printf ("\n");
		}
		
		if (_arminitialized)
		{
			arm.getPositions (_armjointstore);
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


	/// close devices.
	///
	if (_headinitialized)
	{
		head.idleMode ();
		_headrunning = false;
		head.uninitialize ();	
		_headinitialized = false;
		if (_headjointstore != NULL) delete[] _headjointstore;
	}

	if (_arminitialized)
	{
		arm.idleMode ();
		_armrunning = false;
		arm.uninitialize ();	
		_arminitialized = false;
		if (_armjointstore != NULL) delete[] _armjointstore;
	}

	CLOSELOG();

	return 0;
}
