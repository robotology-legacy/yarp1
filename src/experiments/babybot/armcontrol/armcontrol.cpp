// armcontrol.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// #include "..\..\visualprocessing\parameters.h"

#include "ArmThread.h"
#include "RandomThread.h"
#include "YARPRobotMath.h"
#include <iostream>

#include "YARPNameClient.h"

#include "YARPScheduler.h"
#include "YARPConfigFile.h"

#include "ArmBehavior.h"

char menu();

using namespace std;

int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	cout << "Starting arm control...\n";
	int _arm_thread_rate;
	int _random_thread_rate;

	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];
	char filename[256];

#if defined(__WIN32__)
	ACE_OS::sprintf (path, "%s\\conf\\babybot\\\0", root); 
	ACE_OS::sprintf (filename, "%s\\conf\\babybot\\arm.ini\0", root); 
#elif defined (__QNX6__)
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
	ACE_OS::sprintf (filename, "%s/conf/babybot/arm.ini\0", root); 
#endif

	file.set(path, "arm.ini");
	file.get("[THREAD]", "Rate", &_arm_thread_rate, 1);

	// file.get("[RANDOMTHREAD]", "Rate", &_random_thread_rate, 1);
	
	ArmThread arm_thread(_arm_thread_rate,
						"arm thread",
						filename);

	ArmBehavior _arm(&arm_thread, YBLabelMotor, "/armcontrol/behavior/i");
	arm_thread.start();

	ABWaitIdle waitIdle;
	ABWaitMotion waitMotion;
	ABInputCommand inputCmd;
	ABCheckMotionDone checkMotionDone;
	ABOutputCommand outputCmd;
	
	_arm.setInitialState(&waitIdle);
	_arm.Begin();
	_arm.add(&inputCmd, &waitIdle, &waitMotion, &outputCmd);
	_arm.add(&checkMotionDone, &waitMotion, &waitIdle);

	_arm.loop();

	_arm.End();
	// stop arm
	arm_thread.terminate(false);	// no timeout here, important !
	return 0;
}

  /*
int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	cout << "Starting arm control...\n";
	int _arm_thread_rate;
	int _random_thread_rate;

	YARPConfigFile file;
	file.set("Y:\\conf\\babybot\\", "arm.ini");
	file.get("[THREAD]", "Rate", &_arm_thread_rate, 1);
	file.get("[RANDOMTHREAD]", "Rate", &_random_thread_rate, 1);
	
	ArmThread arm_thread(_arm_thread_rate, "arm thread", "Y:\\conf\\babybot\\arm.ini");

	RandomThread rnd_thread(&arm_thread, _random_thread_rate, "rnd thread", "Y:\\conf\\babybot\\arm.ini");
	
	arm_thread.start();
 			
	for(;;)
	{
		bool loop = true;
		char c = menu();
		switch (c)
		{
			case 'd':
			{
				loop = true;
				YVector pos;
				pos.Resize(6);
				for(int i = 1; i <= 6; i++)
					cin >> pos(i);

				pos = pos*degToRad;
				arm_thread.directCommandMode();
				arm_thread.directCommand(pos);
				break;
			}
			case 'r':
				{
					arm_thread.directCommandMode();
					rnd_thread.start();
					loop = true;
					break;
				}
			case 's':
				{
					rnd_thread.terminate();
					loop = true;
					break;
				}
			case 'f':
				{
					arm_thread.forceResting(true);
					loop = true;
					break;
				}
			case 'c':
				{
					arm_thread.forceResting(false);
					loop = true;
					break;
				}
			case 'z':
				{
					arm_thread.zeroGMode();
					loop = true;
					break;
				}
			case 'e':
				loop = false;
				break;
		}

		if (!loop)
			break;
	}

	rnd_thread.terminate();

	arm_thread.terminate(false);	// no timeout here, important !

	cout << "exiting...\n";

	return 0;
}

char menu()
{
	char c;
	cout << "--------------------\n";
	cout << "What ?\n";
	cout << "d j1 j2 j3 j4 j5 j6: direct command, move all joins\n";
	cout << "r: start random movement\n";
	cout << "s: stop random movement\n";
	cout << "f: force resting on the arm\n";
	cout << "c: cancel resting on the arm\n";
	cout << "z: zero g mode\n";
	cout << "e: exit :( \n";
	cout << "--------------------\n";

	cin >> c;
	return c;
}

*/