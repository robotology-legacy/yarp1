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
/// $Id: main.cpp,v 1.2 2004-05-02 09:21:52 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include <YARPValueCanDeviceDriver.h>
#include <YARPControlBoardUtils.h>
#include <YARPScheduler.h>

/// a simple test of the ValueCan device driver.

int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling();

	YARPValueCanDeviceDriver driver;
	ValueCanOpenParameters params;

	params._port_number = 0;
	params._arbitrationID = 0;
	params._destinations[0] = 1;				/// card 1.
	params._my_address = 5;						/// my address
	params._polling_interval = 10;				/// thread polling interval [ms]
	params._timeout = 5;						/// approx this value times the polling interval [ms].

	SingleAxisParameters tmp;
	tmp.axis = 0;
	double value = 0;
	tmp.parameters = (void *)&value;
		
	driver.open ((void *)&params);
	char c = 0;

	for (;;)
	{
		ACE_OS::printf ("-> ");
		scanf ("%c", &c);
		switch (c)
		{
		case 'h':
			ACE_OS::printf ("h: this message\n");
			ACE_OS::printf ("p: get position\n");
			ACE_OS::printf ("s: set position\n");

			ACE_OS::printf ("q: quit\n");
			break;

		case 'p':
			driver.IOCtl(CMDGetPosition, (void *)&tmp);
			ACE_OS::printf ("got position: %lf\n", *((double *)tmp.parameters));
			break;

		case 's':
			value = double(0x12345678);
			driver.IOCtl(CMDSetPosition, (void *)&tmp);
			break;

		case 'q':
			goto SmoothEnd;
			break;
		}

		do
		{
			scanf ("%c", &c);
		}
		while (c != '\n');
	}


SmoothEnd:

	driver.close ();
	return 0;
}