// kfexploration.cpp : Defines the entry point for the console application.
//

#include "reflexstates.h"
#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigFile.h>

const int N = 20;
const int _nJoints = 6;


	// const double pos1[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
	const double pos1[_nJoints] = {5.0 * degToRad, 5.0*degToRad, 25.0*degToRad, 0.0, 0.0, -40.0*degToRad};
	const double pos2[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, -70.0*degToRad};
	const double pos3[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
	const double pos4[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, -20.0*degToRad, 0.0};
	const double pos5[_nJoints] = {10.0 * degToRad, 20.0*degToRad, 5.0*degToRad, 0.0, 20.0*degToRad, 0.0};
	
	const double pos6[_nJoints] = {20.0 * degToRad, 10.0*degToRad, 0.0*degToRad, -20.0*degToRad, 0.0*degToRad, 0.0};
	
	const double pos7[_nJoints] = {0.0 * degToRad, 20.0*degToRad, 20.0*degToRad, -20.0*degToRad, 0.0*degToRad, 0.0};
	const double pos8[_nJoints] = {-10.0 * degToRad, 50.0*degToRad, -30.0*degToRad, 0.0*degToRad, 0.0*degToRad, 0.0};
	const double pos9[_nJoints] = {15.0 * degToRad, 10.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
	
	const double pos10[_nJoints] = {5.0 * degToRad, 0.0*degToRad, 0.0*degToRad, 0.0, 0.0, -120.0*degToRad};


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
	EBWaitIdle endState1("end1");
	EBWaitIdle endState2("end2");

	EBWaitIdle stateInhibitHandTracking("Inhibit hand tracking");
	EBWaitIdle stateEnableHandTracking("Enable hand tracking");

	EBWaitIdle position1Wait("Position1 done");
	EBWaitIdle position2Wait("Position2 done");
	EBWaitIdle position3Wait("Position3 done");
	
	EBWaitDeltaT position2Train(6);
	EBWaitDeltaT position3Train(6);
	EBWaitDeltaT position4Train(6);

	EBWaitIdle position2("Waiting arm done");
	EBWaitIdle position3("Waiting arm done");
	EBWaitIdle position4("Waiting arm done");

	EBWaitIdle waitArmAck("Wait armAck");

	EBWaitDeltaT dT1(6);
	EBWaitDeltaT dT2(0.2);
	EBWaitDeltaT dT3OneSecond(3);

	EBWaitDeltaT dTHandClosing(0.1);
	EBWaitDeltaT waitArmSeemsToRest(5);
	EBBehaviorOutput	startTrain(YBVKFTrainStart);
	EBBehaviorOutput	startSequence(YBVKFStart);
	EBBehaviorOutput    stopTrain(YBVKFTrainStop);
	EBBehaviorOutput    stop(YBVKFStop);
	EBSimpleOutput	forceOpen(YBVGraspRflxForceOpen);
	EBSimpleOutput  parkArm(YBVArmForceRestingTrue);
	
	// output: enable and disable hand tracking system
	EBEnableTracker		enableHandTracking;
	EBInhibitTracker	inhibitHandTracking;
		
	EBOutputCommand cmd1(YBVArmForceNewCmd, YVector(_nJoints, pos1));
	EBOutputCommand cmd2(YBVArmForceNewCmd, YVector(_nJoints, pos2));
	EBOutputCommand cmd3(YBVArmForceNewCmd, YVector(_nJoints, pos3));
	// EBOutputCommand cmd4(YBVArmForceNewCmd, YVector(_nJoints, pos4));
	// EBOutputCommand cmd5(YBVArmForceNewCmd, YVector(_nJoints, pos5));
//	EBOutputCommand cmd6(YBVArmForceNewCmd, YVector(_nJoints, pos6));
	EBOutputCommand cmd7(YBVArmForceNewCmd, YVector(_nJoints, pos7));
	EBOutputCommand cmd8(YBVArmForceNewCmd, YVector(_nJoints, pos8));
	EBOutputCommand cmd9(YBVArmForceNewCmd, YVector(_nJoints, pos9));
	EBOutputCommand cmd10(YBVArmForceNewCmd, YVector(_nJoints, pos10));

	EBSimpleInput armDone(YBVArmDone);
	EBSimpleInput handDone(YBVHandDone);
	EBSimpleInput start(YBVKFExplorationStart);
	EBSimpleInput start2(YBVGraspRflxClutch);
	EBSimpleInput armAck(YBVArmIssuedCmd);
	EBSimpleInput armNAck(YBVArmIsBusy);

	behavior.setInitialState(&waitStart);
	behavior.add(&start, &waitStart, &waitMotion1, &cmd1);
	behavior.add(&start2, &waitStart, &dTHandClosing, &enableHandTracking);
	behavior.add(NULL, &dTHandClosing, &waitArmAck, &cmd1);

	behavior.add(&armAck, &waitArmAck, &stateEnableHandTracking, &startSequence);
	behavior.add(&armNAck, &waitArmAck, &waitArmSeemsToRest, &inhibitHandTracking);
	behavior.add(NULL, &waitArmSeemsToRest, &waitStart, &forceOpen);
	behavior.add(NULL, &stateEnableHandTracking, &waitMotion1, &enableHandTracking);

	behavior.add(&armDone, &waitMotion1, &dT3OneSecond);
	// wait some extra time before issueing the startKF signal
	behavior.add(NULL, &dT3OneSecond, &dT1, &startTrain);
	
	// july 21/04
	behavior.add(NULL, &dT1, &waitMotion6);
	behavior.add(NULL, &waitMotion6, &position1Wait, &stopTrain);
	// position2
	behavior.add(NULL, &position1Wait, &position2, &cmd7);
	behavior.add(&armDone, &position2, &position2Train, &startTrain);
	behavior.add(NULL, &position2Train, &position2Wait, &stopTrain);
	//position3
	behavior.add(NULL, &position2Wait, &position3, &cmd8);
	behavior.add(&armDone, &position3, &position3Train, &startTrain);
	behavior.add(NULL, &position3Train, &position3Wait, &stopTrain);
	//position 4
	behavior.add(NULL, &position3Wait, &position4, &cmd9);
	behavior.add(&armDone, &position4, &position4Train, &startTrain);
	behavior.add(NULL, &position4Train, &stateInhibitHandTracking, &stopTrain);

	behavior.add(NULL, &stateInhibitHandTracking, &dT2);

	/*
	behavior.add(NULL, &dT1, &waitMotion2, &cmd2);
	// behavior.add(&armDone, &waitMotion1, &waitMotion2, &cmd2);
	behavior.add(&armDone, &waitMotion2, &waitMotion3, &cmd3);
	behavior.add(&armDone, &waitMotion3, &waitMotion4, &cmd4);
	behavior.add(&armDone, &waitMotion4, &waitMotion5, &cmd5);
	behavior.add(&armDone, &waitMotion5, &waitMotion6, &cmd6);

	behavior.add(&armDone, &waitMotion6, &dT2, &stopKF);
	*/
	behavior.add(NULL, &dT2, &waitMotion7, &cmd10);
	behavior.add(&armDone, &waitMotion7, &waitMotion8, &forceOpen);
	behavior.add(&handDone, &waitMotion8, &endState1, &parkArm);
	behavior.add(NULL, &endState1, &endState2, &inhibitHandTracking);
	behavior.add(NULL, &endState2, &waitStart, &stop);

	behavior.Begin();
	behavior.loop();

	return 0;
}
