// kfexploration.cpp : Defines the entry point for the console application.
//

#include "reflexstates.h"
#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigFile.h>

const int N = 20;
const int _nJoints = 6;


	const double pos1[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
	const double pos2[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, -70.0*degToRad};
	const double pos3[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
	const double pos4[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, -20.0*degToRad, 0.0};
	const double pos5[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 20.0*degToRad, 0.0};
	const double pos6[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
	const double pos7[_nJoints] = {5.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, -70.0*degToRad};


#if 0
	const double pos1[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, 0.0};
	const double pos2[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, -70.0*degToRad};
	const double pos3[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, 0.0};
	const double pos4[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, -20.0*degToRad, 0.0};
	const double pos5[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 20.0*degToRad, 0.0};
	const double pos6[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, 0.0};
	const double pos7[_nJoints] = {10.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, -70.0*degToRad};
#endif


int main(int argc, char* argv[])
{
	ExplorationShared shared;
	EBehavior behavior(&shared);
	EBWaitIdle waitStart("Start");
	EBWaitIdle waitMotion1("ArmPositioning");
	EBWaitIdle waitMotion2("Turn1");
	EBWaitIdle waitMotion3("Turn2");
	EBWaitIdle waitMotion4("Turn3");
	EBWaitIdle waitMotion5("Turn4");
	EBWaitIdle waitMotion6("Turn5");
	EBWaitIdle waitMotion7("Final pos");
	EBWaitIdle waitMotion8("Going back to rest");

	EBWaitIdle stateInhibitHandTracking("Inhibit hand tracking");
	EBWaitIdle stateEnableHandTracking("Enable hand tracking");
	EBWaitIdle waitArmAck("Wait armAck");

	EBWaitDeltaT dT1(3);
	EBWaitDeltaT dT2(0.2);
	EBWaitDeltaT dT3OneSecond(1);

	EBWaitDeltaT dTHandClosing(3);
	EBWaitDeltaT waitArmSeemsToRest(5);
	EBStartKF	startKF;
	EBStopKF	stopKF;
	EBSimpleOutput	forceOpen(YBVGraspRflxForceOpen);
	EBSimpleOutput  parkArm(YBVArmForceRestingTrue);
	
	// output: enaqble and disable hand tracking system
	EBEnableTracker enableHandTracking;
	EBInhibitTracker inhibitHandTracking;
		
	EBOutputCommand cmd1(YBVArmForceNewCmd, YVector(_nJoints, pos1));
	EBOutputCommand cmd2(YBVArmForceNewCmd, YVector(_nJoints, pos2));
	EBOutputCommand cmd3(YBVArmForceNewCmd, YVector(_nJoints, pos3));
	EBOutputCommand cmd4(YBVArmForceNewCmd, YVector(_nJoints, pos4));
	EBOutputCommand cmd5(YBVArmForceNewCmd, YVector(_nJoints, pos5));
	EBOutputCommand cmd6(YBVArmForceNewCmd, YVector(_nJoints, pos6));
	EBOutputCommand cmd7(YBVArmForceNewCmd, YVector(_nJoints, pos7));

	EBSimpleInput motionDone(YBVArmDone);
	EBSimpleInput start(YBVKFExplorationStart);
	EBSimpleInput start2(YBVGraspRflxClutch);
	EBSimpleInput armAck(YBVArmIssuedCmd);
	EBSimpleInput armNAck(YBVArmIsBusy);

	behavior.setInitialState(&waitStart);
	behavior.add(&start, &waitStart, &waitMotion1, &cmd1);
	behavior.add(&start2, &waitStart, &dTHandClosing);
	behavior.add(NULL, &dTHandClosing, &waitArmAck, &cmd1);

	behavior.add(&armAck, &waitArmAck, &stateEnableHandTracking);
	behavior.add(&armNAck, &waitArmAck, &waitArmSeemsToRest);
	behavior.add(NULL, &waitArmSeemsToRest, &waitStart, &forceOpen);
	behavior.add(NULL, &stateEnableHandTracking, &waitMotion1, &enableHandTracking);

	behavior.add(&motionDone, &waitMotion1, &dT3OneSecond);
	// wait some extra time before issueing the startKF signal
	behavior.add(NULL, &dT3OneSecond, &dT1, &startKF);
	
	// july 21/04
	behavior.add(NULL, &dT1, &waitMotion6);
	behavior.add(NULL, &waitMotion6, &stateInhibitHandTracking, &stopKF);
	behavior.add(NULL, &stateInhibitHandTracking, &dT2, &inhibitHandTracking);

	/*
	behavior.add(NULL, &dT1, &waitMotion2, &cmd2);
	// behavior.add(&motionDone, &waitMotion1, &waitMotion2, &cmd2);
	behavior.add(&motionDone, &waitMotion2, &waitMotion3, &cmd3);
	behavior.add(&motionDone, &waitMotion3, &waitMotion4, &cmd4);
	behavior.add(&motionDone, &waitMotion4, &waitMotion5, &cmd5);
	behavior.add(&motionDone, &waitMotion5, &waitMotion6, &cmd6);

	behavior.add(&motionDone, &waitMotion6, &dT2, &stopKF);
	*/
	behavior.add(NULL, &dT2, &waitMotion7, &cmd7);
	behavior.add(&motionDone, &waitMotion7, &waitMotion8, &forceOpen);
	behavior.add(NULL, &waitMotion8, &waitStart, &parkArm);

	behavior.Begin();
	behavior.loop();

	return 0;
}
