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
/// $Id: main.cpp,v 1.6 2004-05-09 00:30:14 babybot Exp $
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
const int NJOINTS = 2;

int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling();

	YARPValueCanDeviceDriver driver;
	ValueCanOpenParameters params;

	params._port_number = 0;
	params._arbitrationID = 0;
	params._destinations[0] = 1;				/// card 1.
	params._my_address = 5;						/// my address.
	params._polling_interval = 2;				/// thread polling interval [ms].
	params._timeout = 10;						/// approx this value times the polling interval [ms].
	params._njoints = NJOINTS;					/// number of joints.

	if (driver.open ((void *)&params) < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "Failed to initialize\n"));
		return -1;
	}

	char c = 0;

	for (;;)
	{
		ACE_OS::printf ("-> ");
		scanf ("%c", &c);
		switch (c)
		{
		case 'h':
		case 'H':
		case '?':
			ACE_OS::printf ("h,H,?: this message\n");
			ACE_OS::printf ("p: display reference position\n");
			ACE_OS::printf ("e: display encoder position and speed\n");
			
			ACE_OS::printf ("s: move to position\n");
			ACE_OS::printf ("t: accelerate to given speed\n");
			ACE_OS::printf ("u: stop movement\n");

			ACE_OS::printf ("v: set desired speed\n");
			ACE_OS::printf ("a: set desired acceleration\n");
			ACE_OS::printf ("f: print reference values\n");

			ACE_OS::printf ("k: prepare for a movement\n");
			ACE_OS::printf ("z: zero encoders\n");

			ACE_OS::printf ("x: set PID control params\n");
			ACE_OS::printf ("y: display PID control params\n");

			ACE_OS::printf ("i: controlled idle\n");
			ACE_OS::printf ("r: controller run\n");
			ACE_OS::printf ("w: enable PWM generation [amp enable]\n");
			ACE_OS::printf ("d: disable PWM generation [amp disable]\n");

			ACE_OS::printf ("1: read boot data from flash mem\n");
			ACE_OS::printf ("2: save boot dato to flash mem\n");

			ACE_OS::printf ("m: set max position\n");
			ACE_OS::printf ("n: set min position\n");

			ACE_OS::printf ("q: quit\n");
			break;

		case 'm':
			{
				ACE_OS::printf ("axis: ");
				SingleAxisParameters x;
				scanf ("%d", &x.axis);
				double param = 0;
				x.parameters = &param;

				ACE_OS::printf ("max position: ");
				scanf ("%lf", &param);
				ACE_OS::printf ("setting max position to: %lf\n", param);
				driver.IOCtl(CMDSetPositiveLimit, (void *)&x);
			}
			break;

		case 'n':
			{
				ACE_OS::printf ("axis: ");
				SingleAxisParameters x;
				scanf ("%d", &x.axis);
				double param = 0;
				x.parameters = &param;

				ACE_OS::printf ("min position: ");
				scanf ("%lf", &param);
				ACE_OS::printf ("setting min position to: %lf\n", param);
				driver.IOCtl(CMDSetNegativeLimit, (void *)&x);
			}
			break;

		case '1':
				driver.IOCtl(CMDLoadBootMemory, NULL);
				ACE_OS::printf ("new control values loaded\n");
			break;

		case '2':
				driver.IOCtl(CMDSaveBootMemory, NULL);
				ACE_OS::printf ("boot memory saved\n");
			break;

		case 't':
			{
				double cmd[4] = { 0, 0, 0, 0 };
				ACE_OS::printf ("MOVES only axis 0\n");
				ACE_OS::printf ("desired speed: ");
				scanf ("%lf", cmd);
				ACE_OS::printf ("desired acceleration: ");
				scanf ("%lf", cmd+1);
				ACE_OS::printf ("going to: %lf with acceleration %lf\n", cmd[0], cmd[1]);
				driver.IOCtl(CMDVMove, (void *)cmd);
			}
			break;

		case 'u':
			{
				double cmd[4] = { 0, 1, 0, 1 };
				driver.IOCtl(CMDVMove, (void *)cmd);
				ACE_OS::printf ("axes should be stopped\n");
			}
			break;

		case 'f':
			{
				int i = 0;
				double values[NJOINTS];
				memset (values, 0, sizeof(double) * NJOINTS);				

				driver.IOCtl(CMDGetRefPositions, (void *)values);
				ACE_OS::printf ("position: ");
				for (i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", values[i]);
				ACE_OS::printf ("\n");

				driver.IOCtl(CMDGetRefSpeeds, (void *)values);
				ACE_OS::printf ("speed: ");
				for (i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", values[i]);
				ACE_OS::printf ("\n");

				driver.IOCtl(CMDGetRefAccelerations, (void *)values);
				ACE_OS::printf ("acceleration: ");
				for (i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", values[i]);
				ACE_OS::printf ("\n");
			}
			break;

		case 'w':
			{
				int axis = 0;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &axis);
				driver.IOCtl(CMDEnableAmp, &axis);
				ACE_OS::printf ("PWM is now enabled\n");
			}
			break;

		case 'd':
			{
				int axis = 0;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &axis);
				driver.IOCtl(CMDDisableAmp, &axis);
				ACE_OS::printf ("PWM is now disabled\n");
			}
			break;

		case 'i':
			{
				int axis = 0;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &axis);
				driver.IOCtl(CMDControllerIdle, &axis);
				ACE_OS::printf ("controller is now idle\n");
			}
			break;

		case 'r':
			{
				int axis = 0;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &axis);
				driver.IOCtl(CMDControllerRun, &axis);
				ACE_OS::printf ("controller is now running\n");
			}
			break;

		case 'e':
			{
				double ar[NJOINTS];
				memset (ar, 0, sizeof(double) * NJOINTS);
				driver.IOCtl(CMDGetPositions, (void *)ar);
				ACE_OS::printf ("got position: ");
				int i;
				for (i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", ar[i]);
				ACE_OS::printf ("\n");
				driver.IOCtl(CMDGetSpeeds, (void *)ar);
				ACE_OS::printf ("got speed: ");
				for (i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", ar[i]);
				ACE_OS::printf ("\n");
			}
			break;

		case 'k':
			{
				double pos[NJOINTS];
				memset (pos, 0, sizeof(double) * NJOINTS);

				/// resets encoders.
				driver.IOCtl(CMDDefinePositions, (void *)pos);

				/// must reset the internal ref position.
				driver.IOCtl(CMDSetCommands, (void *)pos);

				/// reads back encoders.
				driver.IOCtl(CMDGetPositions, (void *)pos);
				ACE_OS::printf ("ref positions: ");
				for (int i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", pos[i]);
				ACE_OS::printf ("\n");

				/// enables the PWM.
				int axis = 0;
				driver.IOCtl(CMDEnableAmp, &axis);
				axis = 1;
				driver.IOCtl(CMDEnableAmp, &axis);

				/// starts the PID up.
				axis = 0;
				driver.IOCtl(CMDControllerRun, &axis);
				axis = 1;
				driver.IOCtl(CMDControllerRun, &axis);

				ACE_OS::printf ("the controller is now running\n");
			}
			break;

		case 'z':
			{
				double pos[NJOINTS];
				memset (pos, 0, sizeof(double) * NJOINTS);

				/// disables the PWM.
				int axis = 0;
				driver.IOCtl(CMDDisableAmp, &axis);
				axis = 1;
				driver.IOCtl(CMDDisableAmp, &axis);
	
				/// stops the controller.
				axis = 0;
				driver.IOCtl(CMDControllerIdle, &axis);
				axis = 1;
				driver.IOCtl(CMDControllerIdle, &axis);

				/// resets encoders.
				driver.IOCtl(CMDDefinePositions, (void *)pos);

				/// must reset the internal ref position.
				driver.IOCtl(CMDSetCommands, (void *)pos);

				/// reads back encoders.
				driver.IOCtl(CMDGetPositions, (void *)pos);
				ACE_OS::printf ("ref positions: ");
				for (int i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", pos[i]);
				ACE_OS::printf ("\n");

				ACE_OS::printf ("the controller is idle now\n");
			}
			break;

		case 'p':
			{
				double ar[NJOINTS];
				memset (ar, 0, sizeof(double) * NJOINTS);
				driver.IOCtl(CMDGetRefPositions, (void *)ar);
				ACE_OS::printf ("got positions: ");
				for (int i = 0; i < NJOINTS; i++)
					ACE_OS::printf ("%lf ", ar[i]);
				ACE_OS::printf ("\n");
			}
			break;

		case 's':
			{
				ACE_OS::printf ("axis: ");
				SingleAxisParameters x;
				scanf ("%d", &x.axis);
				double params[2] = { 0, 0 };
				x.parameters = (void *)params;
				ACE_OS::printf ("desired position: ");
				scanf ("%lf", params);
				ACE_OS::printf ("desired speed: ");
				scanf ("%lf", params+1);
				ACE_OS::printf ("going to: %lf with speed %lf\n", params[0], params[1]);
				driver.IOCtl(CMDSetPosition, (void *)&x);
			}
			break;

		case 'v':
			{
				ACE_OS::printf ("axis: ");
				SingleAxisParameters x;
				scanf ("%d", &x.axis);
				double param = 0;
				x.parameters = &param;

				ACE_OS::printf ("desired speed: ");
				scanf ("%lf", &param);
				ACE_OS::printf ("setting desired speed to: %lf\n", param);
				driver.IOCtl(CMDSetSpeed, (void *)&x);
			}
			break;

		case 'a':
			{
				ACE_OS::printf ("axis: ");
				SingleAxisParameters x;
				scanf ("%d", &x.axis);
				double param = 0;
				x.parameters = &param;

				ACE_OS::printf ("desired acceleration: ");
				scanf ("%lf", &param);
				ACE_OS::printf ("setting desired acceleration to: %lf\n", param);
				driver.IOCtl(CMDSetAcceleration, (void *)&x);
			}
			break;

		case 'x':
			{
				LowLevelPID pid;
				SingleAxisParameters par;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &par.axis);
				par.parameters = (void *)&pid;
				ACE_OS::printf ("setting new controller params\n");
				ACE_OS::printf ("p: ");
				scanf ("%lf", &pid.KP);
				ACE_OS::printf ("d: ");
				scanf ("%lf", &pid.KD);
				ACE_OS::printf ("shift: ");
				scanf ("%lf", &pid.SHIFT);
				pid.OFFSET = 0;
				pid.I_LIMIT = 0;
				pid.KI = 0;
				driver.IOCtl(CMDSetPID, (void *)&par);
			}
			break;

		case 'y':
			{
				LowLevelPID pid;
				SingleAxisParameters par;
				ACE_OS::printf ("axis: ");
				scanf ("%d", &par.axis);
				par.parameters = (void *)&pid;
				driver.IOCtl(CMDGetPID, (void *)&par);
				
				ACE_OS::printf ("params for axis %d\n", par.axis);
				ACE_OS::printf ("p: %lf\n", pid.KP);
				ACE_OS::printf ("d: %lf\n", pid.KD);
				ACE_OS::printf ("shift: %lf\n", pid.SHIFT);
				ACE_OS::printf ("offset: %lf\n", pid.OFFSET);
				ACE_OS::printf ("int limit: %lf\n", pid.I_LIMIT);
				ACE_OS::printf ("i: %lf\n", pid.KI);
			}
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