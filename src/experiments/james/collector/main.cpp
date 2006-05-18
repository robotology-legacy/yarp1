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
/// $Id: main.cpp,v 1.8 2006-05-18 14:11:35 babybot Exp $
///		Collects broadcast messages and dump them to file.
///
///


#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/log_msg.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>

#include <yarp/YARPControlBoardUtils.h>
#include <yarp/YARPADCUtils.h>
#include <yarp/YARPEsdDeviceDriver.h>
#include <yarp/YARPEsdDaqDeviceDriver.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPTime.h>

const char *const HEAD_INI_FILE = "head.ini";
const char *const ARM_INI_FILE = "arm.ini";

static FILE *	__file_handle = NULL;

static BOOL init_log (void)
{
	if (__file_handle == NULL)
	{
        __file_handle = ACE_OS::fopen ("log.txt", "w");
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
        ACE_OS::fclose (__file_handle);
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

    ACE_OS::fprintf (__file_handle, "%s", s);
	return(cnt);
}

static int flush_log (void)
{
	if (__file_handle == NULL)
		return -1;

    ACE_OS::fflush (__file_handle);
	return 0;
}

static int init_null (void) { return YARP_OK; }
static int close_null (void) { return YARP_OK; }
static int flush_null (void) { return YARP_OK; }

//#define INITLOG init_log
//#define CLOSELOG close_log
//#define PRINTLOG xfprintf
//#define FLUSHLOG flush_log

#define INITLOG init_null
#define CLOSELOG close_null
#define PRINTLOG (ACE_OS::printf)
#define FLUSHLOG flush_null

#define MAX_ARM_JNTS 15
#define MAX_HEAD_JNTS 8


const int CANBUS_DEVICE_NUM			= 0;
const int CANBUS_ARBITRATION_ID		= 0;
const int CANBUS_MY_ADDRESS			= 0;
const int CANBUS_POLLING_INTERVAL	= 4;			/// [ms]
const int CANBUS_TIMEOUT			= 10;			/// 10 * POLLING
const int CANBUS_MAXCARDS			= 16;

const unsigned char _destinations[CANBUS_MAXCARDS] = { 0x0e, 0x0d, 0x0c, 0x0b, 
													   0x0a, 0x09, 0x08, 0x07,
													   0x80, 0x80, 0x80, 0x80,
													   0x80, 0x80, 0x80, 0x80 };

YARPEsdDeviceDriver head;
YARPEsdDeviceDriver arm;
YARPEsdDaqDeviceDriver touch;


///
///
///
int main (int argc, char *argv[])
{
    YARPScheduler::setHighResScheduling();

    YARPString filename ("localdump.txt");
    int max_steps = 50;
    int sampling_period = 10; /// ms.
    bool verbose = false;

    if (argc >= 2)
    {
        YARPParseParameters::parse (argc, argv, "-filename", filename);
        YARPParseParameters::parse (argc, argv, "-steps", &max_steps);
        YARPParseParameters::parse (argc, argv, "-period", &sampling_period);
        if (sampling_period < CANBUS_POLLING_INTERVAL)
        {
            sampling_period = CANBUS_POLLING_INTERVAL;
            ACE_OS::printf ("Setting period to the minimun allowed %d\n", sampling_period);
        }
        verbose = YARPParseParameters::parse (argc, argv, "-verbose");
    }

    ACE_OS::printf("Running with:\n");
    ACE_OS::printf("filename: %s\n", filename.c_str());
    ACE_OS::printf("number of steps: %d\n", max_steps);
    ACE_OS::printf("period: %d\n", sampling_period);
    ACE_OS::printf("verbose: %d\n", verbose);

	bool _headinitialized = false;
	bool _arminitialized = false;
    bool _touchinitialized = false;

	double **_headjointstore = NULL;
    double **_headcurrentstore = NULL;
    double **_headerrorstore = NULL;
	double **_armjointstore = NULL;
    double **_armcurrentstore = NULL;
    double **_armerrorstore = NULL;
    short **_analogstore = NULL;
	int _maxanalogchannels = 0;

	/// init robot.
	INITLOG ();
	PRINTLOG ("Log started\n");
	FLUSHLOG();

	/// head (bus 1).
	EsdOpenParameters op_par1;
	ACE_OS::memcpy (op_par1._destinations, _destinations, sizeof(unsigned char) * CANBUS_MAXCARDS);
	op_par1._my_address = CANBUS_MY_ADDRESS;				/// my address.
	op_par1._polling_interval = CANBUS_POLLING_INTERVAL;	/// thread polling interval [ms].
	op_par1._timeout = CANBUS_TIMEOUT;						/// approx this value times the polling interval [ms].
	op_par1._networkN = 0;									/// first can controller, attached to the head.

	op_par1._njoints = MAX_HEAD_JNTS;
	op_par1._p = NULL;

	if (head.open ((void *)&op_par1) < 0)
	{
		head.close();
		ACE_OS::printf ("head driver open failed\n");
	}
	else
    {
        _headinitialized = true;
    }

	/// arm (bus 2).
	EsdOpenParameters op_par2;
	op_par2._njoints = MAX_ARM_JNTS;
	op_par2._p = PRINTLOG;
	ACE_OS::memcpy (op_par2._destinations, _destinations, sizeof(unsigned char) * CANBUS_MAXCARDS);
	op_par2._my_address = CANBUS_MY_ADDRESS;					/// my address.
	op_par2._polling_interval = CANBUS_POLLING_INTERVAL;		/// thread polling interval [ms].
	op_par2._timeout = CANBUS_TIMEOUT;							/// approx this value times the polling interval [ms].
	op_par2._networkN = 1;										/// second can controller, attached to the arm/hand.

	if (arm.open ((void *)&op_par2) < 0)
	{
		arm.close();
		ACE_OS::printf ("arm driver open failed\n");
	}
    else
    {
        _arminitialized = true;
    }

	EsdDaqOpenParameters op_par3;
	op_par3._networkN = 1;						// same address of the arm/hand.
	op_par3._remote_address = 5;				// address of the daq card.
	op_par3._my_address = CANBUS_MY_ADDRESS;	// this is the second instance to the same driver (we can even use a different ID).
	op_par3._polling_interval = CANBUS_POLLING_INTERVAL;
	op_par3._timeout = CANBUS_TIMEOUT;			
    op_par3._broadcast = true;

	if (touch.open ((void *)&op_par3) < 0)
	{
		touch.close();
		ACE_OS::printf ("daq driver open failed\n");
	}
	else
	{
		ACE_OS::printf ("daq driver opened correctly (hopefully)\n");
		_touchinitialized = true;

		int ret = touch.IOCtl (CMDGetMaxChannels, (void *)&_maxanalogchannels);
		if (_maxanalogchannels)
		{
			_analogstore = new short *[max_steps];
            ACE_ASSERT (_analogstore != NULL);
            _analogstore[0] = new short[_maxanalogchannels*max_steps];
			ACE_ASSERT (_analogstore[0] != NULL);

            for (int j = 1; j < max_steps; j++)
            {
                _analogstore[j] = _analogstore[j-1] + _maxanalogchannels;
            }

			ACE_OS::memset (_analogstore[0], 0, sizeof(short) * max_steps * _maxanalogchannels);
		}

        // ret = touch.IOCtl (CMDBroadcastSetup, &op_par3._scanSequence);
        // deferred to TestControl.
	}

    // just pause a bit before proceeding with sensors' reading.
    YARPTime::DelayInSeconds(1.0);

	if (!_headinitialized || !_arminitialized || !_touchinitialized)
	{
		ACE_OS::printf ("Some device failure\n");
		return -1;
	}

	ACE_OS::printf ("The robot is initialized\n");
	if (_headinitialized)
	{
        _headjointstore = new double *[max_steps];
        _headcurrentstore = new double *[max_steps];
        _headerrorstore = new double *[max_steps];
        ACE_ASSERT (_headjointstore != NULL && 
                    _headcurrentstore != NULL && 
                    _headerrorstore != NULL);
		_headjointstore[0] = new double[MAX_HEAD_JNTS*max_steps];
        _headcurrentstore[0] = new double[MAX_HEAD_JNTS*max_steps];
        _headerrorstore[0] = new double[MAX_HEAD_JNTS*max_steps];
		ACE_ASSERT (_headjointstore[0] != NULL && 
                    _headcurrentstore[0] != NULL &&
                    _headerrorstore[0] != NULL);

        for (int j = 1; j < max_steps; j++)
        {
            _headjointstore[j] = _headjointstore[j-1]+MAX_HEAD_JNTS;
            _headcurrentstore[j] = _headcurrentstore[j-1]+MAX_HEAD_JNTS;
            _headerrorstore[j] = _headerrorstore[j-1]+MAX_HEAD_JNTS;
        }
	}

	if (_arminitialized)
	{
        _armjointstore = new double *[max_steps];
        _armcurrentstore = new double *[max_steps];
        _armerrorstore = new double *[max_steps];
        ACE_ASSERT (_armjointstore != NULL && 
                    _armcurrentstore != NULL &&
                    _armerrorstore != NULL);
		_armjointstore[0] = new double[MAX_ARM_JNTS*max_steps];
		_armcurrentstore[0] = new double[MAX_ARM_JNTS*max_steps];
		_armerrorstore[0] = new double[MAX_ARM_JNTS*max_steps];
		ACE_ASSERT (_armjointstore[0] != NULL && 
                    _armcurrentstore[0] != NULL &&
                    _armerrorstore[0] != NULL);

        for (int j = 1; j < max_steps; j++)
        {
            _armjointstore[j] = _armjointstore[j-1]+MAX_ARM_JNTS;
            _armcurrentstore[j] = _armcurrentstore[j-1]+MAX_ARM_JNTS;
            _armerrorstore[j] = _armerrorstore[j-1]+MAX_ARM_JNTS;
        }
	}

	ACE_OS::printf ("Running!\n");

    FILE *fp = ACE_OS::fopen (filename.c_str(), "w");
    if (fp == NULL)
    {
        ACE_OS::printf ("Can't open filename %s\n", filename.c_str());
	    if (_touchinitialized)
        {
            touch.close();
            if (_analogstore != NULL && _analogstore[0] != NULL) 
            {
                delete[] _analogstore[0];
                delete[] _analogstore;
            }
        }

	    if (_headinitialized)
	    {
		    head.close();
		    if (_headjointstore != NULL && _headjointstore[0] != NULL) 
            {
                delete[] _headjointstore[0];
                delete[] _headjointstore;
            }
		    
            if (_headcurrentstore != NULL && _headcurrentstore[0] != NULL) 
            {
                delete[] _headcurrentstore[0];
                delete[] _headcurrentstore;
            }

            if (_headerrorstore != NULL && _headerrorstore[0] != NULL) 
            {
                delete[] _headerrorstore[0];
                delete[] _headerrorstore;
            }
	    }

	    if (_arminitialized)
	    {
		    arm.close();
		    if (_armjointstore != NULL && _armjointstore[0] != NULL) 
            {
                delete[] _armjointstore[0];
                delete[] _armjointstore;
            }

            if (_armcurrentstore != NULL && _armcurrentstore[0] != NULL) 
            {
                delete[] _armcurrentstore[0];
                delete[] _armcurrentstore;
            }

            if (_armerrorstore != NULL && _armerrorstore[0] != NULL) 
            {
                delete[] _armerrorstore[0];
                delete[] _armerrorstore;
            }
	    }

        ACE_OS::exit(-1);
    }

    //
    //
    //
    int i;
    for (i = 0; i < MAX_HEAD_JNTS; i++)
        ACE_OS::fprintf (fp, "p_head%d ", i);
    for (i = 0; i < MAX_HEAD_JNTS; i++)
        ACE_OS::fprintf (fp, "i_head%d ", i);
    for (i = 0; i < MAX_HEAD_JNTS; i++)
        ACE_OS::fprintf (fp, "e_head%d ", i);
    for (i = 0; i < MAX_ARM_JNTS; i++)
        ACE_OS::fprintf (fp, "p_arm%d ", i);
    for (i = 0; i < MAX_ARM_JNTS; i++)
        ACE_OS::fprintf (fp, "i_arm%d ", i);
    for (i = 0; i < MAX_ARM_JNTS; i++)
        ACE_OS::fprintf (fp, "e_arm%d ", i);
    for (i = 0; i < _maxanalogchannels; i++)
        ACE_OS::fprintf (fp, "analog%d ", i);
    ACE_OS::fprintf (fp, "\n");

    int head_axis_map[MAX_HEAD_JNTS];
    int arm_axis_map[MAX_ARM_JNTS];

    char *root = GetYarpRoot();
	char path[512];
	ACE_OS::sprintf (path, "%s/%s\0", root, ConfigFilePath); 

	YARPConfigFile cfgFile;
	cfgFile.set(path, "head.ini");
	if (cfgFile.get("[GENERAL]", "AxisMap", head_axis_map, MAX_HEAD_JNTS) == YARP_FAIL)
    {
        ACE_OS::printf ("troubles reading from the head config file\n");
    }

	cfgFile.set(path, "arm.ini");
	if (cfgFile.get("[GENERAL]", "AxisMap", arm_axis_map, MAX_ARM_JNTS) == YARP_FAIL)
    {
        ACE_OS::printf ("troubles reading from the arm config file\n");
    }


    double before = YARPTime::GetTimeAsSecondsHr(), now = 0.0;
    double beginning = before;

	int cycle;
    for (cycle = 0; cycle < max_steps; cycle++)
	{
		/// read from the head.
		int ret;
		
        if (!(cycle % 100))
        {
            ACE_OS::printf ("cycle; %d\n", cycle);
        }

		ret = head.IOCtl(CMDGetPositions, _headjointstore[cycle]);
		if (ret != YARP_OK)
			ACE_OS::printf ("troubles reading head joint pos\n");

		ret = head.IOCtl(CMDGetTorques, _headcurrentstore[cycle]);
		if (ret != YARP_OK)
			ACE_OS::printf ("troubles reading head joint torques\n");

		ret = head.IOCtl(CMDGetPIDErrors, _headerrorstore[cycle]);
		if (ret != YARP_OK)
			ACE_OS::printf ("troubles reading head joint errors\n");

        ret = arm.IOCtl(CMDGetPositions, _armjointstore[cycle]);
		if (ret != YARP_OK)
			ACE_OS::printf ("troubles reading arm joint pos\n");

        ret = arm.IOCtl(CMDGetTorques, _armcurrentstore[cycle]);
		if (ret != YARP_OK)
			ACE_OS::printf ("troubles reading arm torques\n");

        ret = arm.IOCtl(CMDGetPIDErrors, _armerrorstore[cycle]);
		if (ret != YARP_OK)
			ACE_OS::printf ("troubles reading arm errors\n");

        ret = touch.IOCtl(CMDAIReadScan, (void *)_analogstore[cycle]);
	    if (ret != YARP_OK)
		    ACE_OS::printf ("troubles reading from daq card\n");

        if (verbose)
        {
            for (int k = 0; k < _maxanalogchannels; k++)
                ACE_OS::printf ("%d ", _analogstore[cycle][k]);
            ACE_OS::printf ("\n");
        }

		/// wait.
		now = YARPTime::GetTimeAsSecondsHr();
        if ((now - before)*1000 < sampling_period)
		{
            double k = double(sampling_period)/1000.0-(now-before);
			YARPTime::DelayInSeconds(k);
            before = now + k;
		}
		else 
		{
            ACE_OS::printf("%d: Thread can't poll fast enough (time: %f)\n", cycle, now-before);
            before = now;
		}

		FLUSHLOG();
	}

    // saving file
    for (cycle = 0; cycle < max_steps; cycle++)
    {
		for (i = 0; i < MAX_HEAD_JNTS; i++)
		{
            int j = head_axis_map[i];
			ACE_OS::fprintf (fp, "%.2f ", _headjointstore[cycle][j]);
		}

		for (i = 0; i < MAX_HEAD_JNTS; i++)
		{
            int j = head_axis_map[i];
			ACE_OS::fprintf (fp, "%.2f ", _headcurrentstore[cycle][j]);
		}

        for (i = 0; i < MAX_HEAD_JNTS; i++)
		{
            int j = head_axis_map[i];
			ACE_OS::fprintf (fp, "%.2f ", _headerrorstore[cycle][j]);
		}

		for (i = 0; i < MAX_ARM_JNTS; i++)
		{
            int j = arm_axis_map[i];
			ACE_OS::fprintf (fp, "%.2f ", _armjointstore[cycle][j]);
		}

		for (i = 0; i < MAX_ARM_JNTS; i++)
		{
            int j = arm_axis_map[i];
			ACE_OS::fprintf (fp, "%.2f ", _armcurrentstore[cycle][j]);
		}

		for (i = 0; i < MAX_ARM_JNTS; i++)
		{
            int j = arm_axis_map[i];
			ACE_OS::fprintf (fp, "%.2f ", _armerrorstore[cycle][j]);
		}

        for (i = 0; i < _maxanalogchannels; i++)
			ACE_OS::fprintf (fp, "%.2f ", double(_analogstore[cycle][i]));

        ACE_OS::fprintf (fp, "\n");
    }

    if (fp != NULL)
        ACE_OS::fclose (fp);

	if (_touchinitialized)
    {
        touch.close();
        if (_analogstore != NULL && _analogstore[0] != NULL) 
        {
            delete[] _analogstore[0];
            delete[] _analogstore;
        }
        
    }

	if (_headinitialized)
	{
		head.close();
		if (_headjointstore != NULL && _headjointstore[0] != NULL) 
        {
            delete[] _headjointstore[0];
            delete[] _headjointstore;
        }

        if (_headcurrentstore != NULL && _headcurrentstore[0] != NULL) 
        {
            delete[] _headcurrentstore[0];
            delete[] _headcurrentstore;
        }

        if (_headerrorstore != NULL && _headerrorstore[0] != NULL) 
        {
            delete[] _headerrorstore[0];
            delete[] _headerrorstore;
        }
	}

	if (_arminitialized)
	{
		arm.close();
		if (_armjointstore != NULL && _armjointstore[0] != NULL) 
        {
            delete[] _armjointstore[0];
            delete[] _armjointstore;
        }

        if (_armcurrentstore != NULL && _armcurrentstore[0] != NULL) 
        {
            delete[] _armcurrentstore[0];
            delete[] _armcurrentstore;
        }

        if (_armerrorstore != NULL && _armerrorstore[0] != NULL) 
        {
            delete[] _armerrorstore[0];
            delete[] _armerrorstore;
        }
	}

    CLOSELOG();

	return 0;
}
