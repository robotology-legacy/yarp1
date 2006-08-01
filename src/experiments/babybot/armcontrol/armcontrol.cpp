// armcontrol.cpp : Defines the entry point for the console application.
//

// #include "..\..\visualprocessing\parameters.h"

#include "Armthread.h"
#include "RandomThread.h"
#include <yarp/YARPRobotMath.h>
#include <iostream>

///#include "YARPNameClient.h"

#include <yarp/YARPScheduler.h>
#include <yarp/YARPConfigFile.h>

#include <yarp/debug.h>

#include "ArmBehavior.h"

using namespace std;

char __filename[256] = "arm.ini";

int main(int argc, char* argv[])
{
//	set_yarp_debug(100,100);

	YARPScheduler::setHighResScheduling();

	cout << "Starting arm control...\n";
	int _arm_thread_rate;

	YARPConfigFile file;

	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/%s", root, ConfigFilePath); 

	file.set(path, __filename);
	file.get("[THREAD]", "Rate", &_arm_thread_rate, 1);

	ArmThread arm_thread(_arm_thread_rate,
						"arm thread",
						__filename);

	ArmBehavior _arm(&arm_thread);
	
	ABWaitIdle waitIdle("command");
	ABWaitIdle waitMotion("motion");
	ABWaitIdle waitRest("resting");
	ABWaitIdle waitZeroG("zeroG");
	ABWaitIdle waitInhibition("inhibition");
	ABWaitIdle waitHibernated("hibernate");
	ABSimpleInput checkMotionDone(YBVArmDone);
	ABOutputCommand outputCmd;
	ABForceOutputCommand outputCmdForce;
	ABOutputShakeCmd outputShk;
	ABOutputHibernate hibernateOut;
	ABOutputResume resumeOut;
	ABSimpleOutput outputArmIsBusy(YBVArmIsBusy);
	ABInputCommand inputCmd(YBVArmNewCmd);
	ABInputCommand forceCmd(YBVArmForceNewCmd);
	ABInputCommand forceVelCmd(YBVArmForceNewVelCmd);
	ABInputShakeCommand inputShk(YBVArmShake);
	ABSimpleInput inputRest(YBVArmRest);
	ABSimpleInput checkRestDone(YBVArmRestDone);
	ABSimpleInput hibernateCmd(YBVArmHibernate);
	ABSimpleInput resumeCmd(YBVArmResume);
	ABSimpleInput printPids(YBVArmPrintPids);
	ABOutputPrintPids printPidsOutput;

	ABInputSetStiffness setStiffness(YBVArmSetStiffness);
	ABSetStiffnessState setStiffnessState1;
	ABSetStiffnessState setStiffnessState2;

	ABSimpleInput inputForceRest(YBVArmForceResting);
	ABSimpleInput inputInhibitRest(YBVArmInhibitResting);
	ABSimpleInput inputInhibitRestTrue(YBVArmInhibitRestingTrue);
	ABSimpleInput inputForceRestTrue(YBVArmForceRestingTrue);
	ABSimpleInput inputZeroG(YBVArmZeroG);
	ABForceResting		outputForceRest;
	ABInhibitResting	outputInhibitRest;
	ABForceRestingTrue		outputForceRestTrue;
	ABInhibitRestingTrue	outputInhibitRestTrue;
	ABStartZeroG	outputStartZeroG;
	ABStopZeroG		outputStopZeroG;
		
	_arm.setInitialState(&waitIdle);
	/////////////////////////////// config state machine
	// shake and arm command
	_arm.add(&inputCmd, &waitIdle, &waitMotion, &outputCmd);
	_arm.add(&forceCmd, &waitIdle, &waitInhibition , &outputInhibitRestTrue);
	_arm.add(NULL, &waitInhibition, &waitMotion, &outputCmdForce);
	_arm.add(&inputShk, &waitIdle, &waitMotion, &outputShk);
	_arm.add(&checkMotionDone, &waitMotion, &waitIdle);

	//  hibernation
	_arm.add(&hibernateCmd, &waitIdle, &waitHibernated, &hibernateOut);
	_arm.add(&hibernateCmd, &waitMotion, &waitHibernated, &hibernateOut);
	_arm.add(&hibernateCmd, &waitZeroG, &waitHibernated, &hibernateOut);
	_arm.add(&hibernateCmd, &waitRest, &waitHibernated, &hibernateOut);
	_arm.add(&resumeCmd, &waitHibernated, &waitIdle, &resumeOut);
	
	// resting cycle
	_arm.add(&inputRest, &waitMotion, &waitRest);
	_arm.add(&inputRest, &waitIdle, &waitRest);

	_arm.add(&checkRestDone, &waitRest, &waitIdle);
	_arm.add(&inputCmd, &waitRest, &waitRest, &outputArmIsBusy);
	_arm.add(&forceCmd, &waitRest, &waitInhibition , &outputInhibitRestTrue);
	
	// set stiffness
	_arm.add(&setStiffness, &waitIdle, &setStiffnessState1);
	_arm.add(NULL, &setStiffnessState1, &waitIdle);
	_arm.add(&setStiffness, &waitRest, &setStiffnessState2);
	_arm.add(NULL, &setStiffnessState2, &waitRest);
	////////////////////

	// zero G cycle
	_arm.add(&inputZeroG, &waitIdle, &waitZeroG, &outputStartZeroG);
	_arm.add(&inputZeroG, &waitZeroG, &waitIdle, &outputStopZeroG);
	
	// additional functions
	_arm.add(&inputForceRest, &outputForceRest);
	_arm.add(&inputForceRestTrue, &outputForceRestTrue);
	_arm.add(&inputInhibitRest, &outputInhibitRest);
	_arm.add(&inputInhibitRestTrue, &outputInhibitRestTrue);
	_arm.add(&printPids, &printPidsOutput);
	////////////////////

	// start control thread
	arm_thread.start();
	arm_thread.forceResting(true);	// arm starts in resting position
	// start behavior SM
	_arm.Begin();
	_arm.loop();	// block here until quit command is received

	arm_thread.terminate(false);	// no timeout here, important !
	return 0;
}