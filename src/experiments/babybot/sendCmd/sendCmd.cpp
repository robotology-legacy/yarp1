// sendCmd.cpp : Defines the entry point for the console application.
//

#include <YARPPort.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <./conf/YARPVocab.h>
#include "functionList.h"

YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
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
	REG_VOCAB(sendArmJointCommand, YBVArmNewCmd, "send arm position command");
	REG_VOCAB(sendArmJointCommand, YBVArmShake, "send arm shake command");
	REG_VOCAB(sendSimpleVocab, YBVArmHibernate, "hibernate arm control");
	REG_VOCAB(sendSimpleVocab, YBVArmResume, "resume arm control");
	REG_VOCAB(sendSimpleVocab, YBVArmQuit, "quit arm control");

	// head
	REG_VOCAB(sendHeadJointCommand, YBVHeadNewCmd, "send head new position command");
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
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitAll, "inhibit ALL sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkEnableAll, "enable ALL sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitVor, "inhibit VOR sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitTracker, "inhibit TRACKER sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkInhibitVergence, "inhibit VERGENCE sink input");
	REG_VOCAB(sendSimpleVocab, YBVSinkDisplayStatus, "display channel status");

	// hand kinematics.m
	REG_VOCAB(sendSimpleVocab, YBVHandKinQuit, "quit handkinematics.m");
	REG_VOCAB(sendHandKinematicsView, YBVHandKinView, "handkinematics.m change viewpoint");
	REG_VOCAB(sendSimpleVocab, YBVHandKinFreeze, "freeze/resume handkinematics.m");
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

bool _parse(const YARPString &c, YARPBottle &b)
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
	YARPBottle tmp;
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
