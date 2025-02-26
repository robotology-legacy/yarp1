// sendCmd.cpp : Defines the entry point for the console application.
//
//

#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
#include <iostream>
using namespace std;

#include "functionList.h"

YARPOutputPortOf<YARPBabyBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
COMMAND_TABLE _commands;

void _fillTable()
{
	// global commands
	REG_VOCAB(sendSimpleVocab, YBVIsAlive, "broadcast alive message");
	REG_VOCAB(sendSimpleVocab, YBVExit, "broadcast exit message (obsolete)");

	// arm rnd
	REG_VOCAB(sendSimpleVocab, YBVArmRndStart, "start arm random thread");
	REG_VOCAB(sendSimpleVocab, YBVArmRndStop, "stop arm random thread");
	
	// arm control
	REG_VOCAB(sendSimpleVocab, YBVArmZeroG, "arm goes/exit to/from zero g mode");
	REG_VOCAB(sendSimpleVocab, YBVArmInhibitResting, "inhibit arm resting (dangerous)");
	REG_VOCAB(sendSimpleVocab, YBVArmForceResting, "force arm resting");
	REG_VOCAB(sendArmJointCommand, YBVArmForceNewCmd, "send arm position command");
	REG_VOCAB(sendArmJointCommand, YBVArmNewCmd, "send arm position command");
	REG_VOCAB(sendArmJointCommand, YBVArmShake, "send arm shake command");
	REG_VOCAB(sendSimpleVocab, YBVArmHibernate, "hibernate arm control");
	REG_VOCAB(sendSimpleVocab, YBVArmResume, "resume arm control");
	REG_VOCAB(sendSimpleVocab, YBVArmQuit, "quit arm control");

	// head
	REG_VOCAB(sendHeadJointCommand, YBVHeadNewCmd, "send head new position command");
	REG_VOCAB(sendHeadJointCommand, YBVHeadNewAcc, "set head new acceleration");
	REG_VOCAB(sendSimpleVocab, YBVHeadQuit, "quit head control");
	REG_VOCAB(sendSimpleVocab, YBVHeadHibernate, "hibernate head");
	REG_VOCAB(sendSimpleVocab, YBVHeadResume, "resume head from hibernation");
		
	// hand
	REG_VOCAB(sendHandJointCommand, YBVHandNewCmd, "send hand new position command");
	REG_VOCAB(sendSimpleVocab, YBVHandQuit, "quit hand control");
	REG_VOCAB(sendSimpleVocab, YBVHandShake, "shake hand");
	REG_VOCAB(sendSimpleVocab, YBVHandResetEncoders, "reset hand encoders");

	// sink
	REG_VOCAB(sendSimpleVocab, YBVSinkQuit, "quit sink behavior");
	REG_VOCAB(sendSimpleVocab, YBVSinkSuppress, "pause head");
	REG_VOCAB(sendSimpleVocab, YBVSinkRelease, "release head");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitAll, "inhibit ALL sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkEnableAll, "enable ALL sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitVor, "inhibit VOR sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitTracker, "inhibit TRACKER sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitVergence, "inhibit VERGENCE sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitSaccade, "inhibit SACCADE sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkEnableVor, "enable VOR sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkEnableTracker, "enable TRACKER sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkEnableVergence, "enable VERGENCE sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkEnableSaccade, "enable SACCADE sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkDisplayStatus, "display channel status");
	
	REG_VOCAB(sendSimpleVocab, YBVSinkSaccadeMode, "goes in saccade mode");
	REG_VOCAB(sendSimpleVocab, YBVSinkTrackingMode, "goes in tracking mode");

	// attention
	REG_VOCAB(sendSimpleVocab, YBVAttentionQuit, "attention: quit module");
	REG_VOCAB(sendSimpleVocab, YBVAttentionLookHand, "attention: switch to hand");
	REG_VOCAB(sendSimpleVocab, YBVAttentionLookTarget, "attention: switch to target");
	REG_VOCAB(sendSimpleVocab, YBVAttentionLookPrediction, "attention: switch to hand prediction");

	// hand tracker switch
	REG_VOCAB(sendSimpleVocab, YBVHTSPrediction, "hand tracker switch: prediction");
	REG_VOCAB(sendSimpleVocab, YBVHTSCurrent, "hand tracker switch: current position");
	REG_VOCAB(sendSimpleVocab, YBVHTSAuto, "hand tracker switch: auto");

	// reaching
	REG_VOCAB(sendSimpleVocab, YBVReachingQuit, "reaching: quit module");
	REG_VOCAB(sendSimpleVocab, YBVReachingReach, "reaching: reach out");
	REG_VOCAB(sendSimpleVocab, YBVReachingLearn, "reaching: learn current point");

	// hand kinematics.m
	REG_VOCAB(sendSimpleVocab, YBVHandKinQuit, "quit handkinematics.m");
	REG_VOCAB(sendHandKinematicsView, YBVHandKinView, "handkinematics.m change viewpoint");
	REG_VOCAB(sendSimpleVocab, YBVHandKinFreeze, "freeze/resume handkinematics.m");
	REG_VOCAB(sendString, YBVHandKinSetFile, "handkinematics.m set filename");
	REG_VOCAB(sendSimpleVocab, YBVHandKinSavePosture, "handkinematics.m save current posture");

	// grasp rflx
	REG_VOCAB(sendSimpleVocab, YBVGraspRflxForceOpen, "grasp reflex force open");
	REG_VOCAB(sendSimpleVocab, YBVGraspRflxClutch, "grasp reflex force close");

	// kfexploration
	REG_VOCAB(sendSimpleVocab, YBVKFExplorationStart, "kf exploration start");
	REG_VOCAB(sendSimpleVocab, YBVKFExplorationQuit, "kf exploration quit");
}

void _help()
{
	COMMAND_TABLE_IT it(_commands);
	cout << "--------- MENU ---------\n";
	cout << "- Program commands:\n";
	cout << "enter: this menu\n";
	cout << "q!: quit program\n";
	cout << "- Currently registered robot commands:\n";
	while(!it.done())
	{
		cout << (*it).verbose << ": " << (*it).help << "\n";
		it++;
	}
	cout << "------------------------\n";
}

bool _parse(const YARPString &c, YARPBabyBottle &b)
{
	COMMAND_TABLE_IT it(_commands);
	while(!it.done())
	{
		COMMAND_TABLE_ENTRY &tmp = (*it);

		if (tmp.verbose == c)
		{
			// command registered
			return tmp.f(tmp.command, b);
		}
		it++;
	}

	cout << "Sorry, " << c << " command not supported\n";
	return false;
}

int main(int argc, char* argv[])
{
	_fillTable();
	
	_outPort.Register("/motorcmd/o");
	YARPBabyBottle tmp;
	tmp.setID(YBVMotorLabel);	// set bottle label
	YARPString c;
	bool quit = false;
	
	_help();
	while(cin>> c)
	{
		tmp.reset();
		
		if (c=="")
			_help();
		else if (c=="q!")
			break;
		else
		{
			if (_parse(c, tmp))
			{
				_outPort.Content() = tmp;
				_outPort.Write();
			}
		}
	}

	return 0;
}
