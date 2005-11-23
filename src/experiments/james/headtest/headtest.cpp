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
/// $Id: headtest.cpp,v 1.1 2005-11-23 16:10:54 babybot Exp $
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


///
///
///
const char *DEFAULT_NAME = "/headtest";
const char *INI_FILE = "head.ini";


///
///
///
///
int main(int argc, char *argv[])
{
	YARPRobotcubHead head;
	
	char *root = GetYarpRoot();
	char path[256];

	ACE_OS::sprintf (path, "%s/%s/\0", root, ConfigFilePath); 

	head.initialize(YARPString(path), YARPString(INI_FILE));
	YVector v(head.nj());
	int i;

	ACE_OS::printf ("[type h for help]\n");
	while(1)
	{
		ACE_OS::printf ("-> ");
		char c;
		c = ACE_OS::fgetc(stdin);

		switch (c)
		{
		case 'q':
			goto CleanExit;
			break;

		case 'h':
		case '?':
			ACE_OS::printf ("h, ?: prints this message\n");
			ACE_OS::printf ("q: quits the application\n");
			ACE_OS::printf ("e: reads encoders\n");
			ACE_OS::printf ("g j, position, speed\n");
			break;

		case 'g':
			{
				int j;
				double where;
				double speed;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &j);
				ACE_OS::printf ("go to: ");
				scanf ("%lf", &where);
				ACE_OS::printf ("with speed: ");
				scanf ("%lf", &speed);
				ACE_OS::printf ("moving joint %d to %f with speed %f\n", j, where, speed);


			}
			break;

		case 'e':
			head.getPositions(v.data());
			for (i = 1; i <= head.nj(); i++)
			{
				ACE_OS::printf ("%f ", v(i));
			}
			ACE_OS::printf ("\n");
			break;
		}

		while (ACE_OS::fgetc(stdin) != '\n');
	}

CleanExit:
	head.idleMode();

	return 0;
}
