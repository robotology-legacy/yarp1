#ifndef __ARM_BEHAVIOR__
#define __ARM_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include <./conf/YARPMotorVocab.h>
#include "Armthread.h"

class ArmThread;

typedef ArmThread ArmBehaviorData;

class ArmBehavior: public YARPBehavior<ArmBehavior, ArmBehaviorData>
{
public:
	ArmBehavior(ArmBehaviorData *d):
	YARPBehavior<ArmBehavior, ArmBehaviorData>(d, "/armcontrol/behavior/i", YBVMotorLabel, YBVArmQuit){}

};

typedef YARPFSMStateBase<ArmBehavior, ArmBehaviorData> ArmBehaviorStateBase;
typedef YARPFSMOutput<ArmBehaviorData> ArmBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<ArmBehaviorData> ArmBehaviorBaseInput;

class ABWaitIdle: public ArmBehaviorStateBase
{
public:
	ABWaitIdle(const YARPString &msg)
	{
		_message = msg;
	}

	void handle(ArmBehaviorData *d)
	{
		printf("ARMBehavior: waiting on %s\n", _message.c_str());
	}

	YARPString _message;
};

class ABOutputCommand: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABStartZeroG: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABStopZeroG: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABForceResting: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABInhibitResting: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABOutputShakeCmd: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABInputCommand: public ArmBehaviorBaseInput
{
public:
	ABInputCommand(YBVocab k)
	{
		key = k;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;

};

class ABInputShakeCommand: public ArmBehaviorBaseInput
{
public:
	ABInputShakeCommand(YBVocab k)
	{
		key = k;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

class ABSimpleInput: public ArmBehaviorBaseInput
{
public:
	ABSimpleInput(YBVocab k)
	{
		key = k;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

#endif
