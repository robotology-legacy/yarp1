// armcontrol.cpp : Defines the entry point for the console application.
//

// #include "..\..\visualprocessing\parameters.h"

#include "Armthread.h"
#include "RandomThread.h"
#include "YARPRobotMath.h"
#include <iostream>

#include "YARPNameClient.h"

#include "YARPScheduler.h"
#include "YARPConfigFile.h"

#include "ArmBehavior.h"

char menu();

using namespace std;

extern int __debug_level;

char __filename[256] = "arm.ini";

int main(int argc, char* argv[])
{
	//__debug_level = 100;

	YARPScheduler::setHighResScheduling();

	cout << "Starting arm control...\n";
	int _arm_thread_rate;

	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];
	
#if defined(__WIN32__)
	ACE_OS::sprintf (path, "%s\\conf\\babybot\\\0", root); 
#elif defined (__QNX6__)
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
#endif

	file.set(path, __filename);
	file.get("[THREAD]", "Rate", &_arm_thread_rate, 1);

	ArmThread arm_thread(_arm_thread_rate,
						"arm thread",
						__filename);

	ArmBehavior _arm(&arm_thread, YBLabelMotor, "/armcontrol/behavior/i");
	arm_thread.start();

	ABWaitIdle waitIdle("command");
	ABWaitIdle waitMotion("motion");
	ABWaitIdle waitRest("resting");
	ABWaitIdle waitZeroG("zeroG");
	ABSimpleInput checkMotionDone(YBVArmDone);
	ABOutputCommand outputCmd;
	ABOutputShakeCmd outputShk;
	ABInputCommand inputCmd(YBVArmNewCmd);
	ABInputShakeCommand inputShk(YBVArmShake);
	ABSimpleInput inputRest(YBVArmRest);
	ABSimpleInput checkRestDone(YBVArmRestDone);

	ABSimpleInput inputForceRest(YBVArmForceResting);
	ABSimpleInput inputInhibitRest(YBVArmInhibitResting);
	ABSimpleInput inputZeroG(YBVArmZeroG);
	ABForceResting		outputForceRest;
	ABInhibitResting	outputInhibitRest;
	ABStartZeroG	outputStartZeroG;
	ABStopZeroG		outputStopZeroG;
		
	_arm.setInitialState(&waitIdle);
	/////////////////////////////// config state machine
	// shake and arm command
	_arm.add(&inputCmd, &waitIdle, &waitMotion, &outputCmd);
	_arm.add(&inputShk, &waitIdle, &waitMotion, &outputShk);
	_arm.add(&checkMotionDone, &waitMotion, &waitIdle);
	
	// resting cycle
	_arm.add(&inputRest, &waitMotion, &waitRest);
	_arm.add(&inputRest, &waitIdle, &waitRest);

	_arm.add(&checkRestDone, &waitRest, &waitIdle);
	////////////////////

	// zero G cyvle
	_arm.add(&inputZeroG, &waitIdle, &waitZeroG, &outputStartZeroG);
	_arm.add(&inputZeroG, &waitZeroG, &waitIdle, &outputStopZeroG);
	
	// additional functions
	_arm.add(&inputForceRest, &outputForceRest);
	_arm.add(&inputInhibitRest, &outputInhibitRest);
	////////////////////

	// start state machine
	_arm.Begin();
	_arm.loop();	// block here until quit command is received

	arm_thread.terminate(false);	// no timeout here, important !
	return 0;
}