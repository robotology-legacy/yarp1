#ifndef __ARM_BEHAVIOR__
#define __ARM_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include "Armthread.h"

class ArmThread;

typedef ArmThread ArmBehaviorData;

class ArmBehavior: public YARPBehavior<ArmBehavior, ArmBehaviorData>
{
public:
	ArmBehavior(ArmBehaviorData *d, int k, const std::string &pName):
	YARPBehavior<ArmBehavior, ArmBehaviorData>(d, k, pName){}

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
	ABInputCommand(int k)
	{
		key = k;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	int key;
};

class ABSimpleInput: public ArmBehaviorBaseInput
{
public:
	ABSimpleInput(int k)
	{
		key = k;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	int key;
};

#endif
