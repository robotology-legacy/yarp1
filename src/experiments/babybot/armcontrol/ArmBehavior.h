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
	void handle(ArmBehaviorData *d)
	{
		printf("Wait idle\n");
	}
};

class ABWaitMotion: public ArmBehaviorStateBase
{
public:
	void handle(ArmBehaviorData *d)
	{
		printf("Wait for motion\n");
	}
};

class ABOutputCommand: public ArmBehaviorBaseOutput
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
	ABInputCommand()
	{
		key = YBVArmNewCmd;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	int key;
};

class ABInputShakeCmd: public ArmBehaviorBaseInput
{
public:
	ABInputShakeCmd()
	{
		key = YBVArmShake;
	}
	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	int key;
};

class ABCheckMotionDone: public ArmBehaviorBaseInput
{
public:
	ABCheckMotionDone()
	{
		key = YBVArmDone;
	}

	bool input(YARPBottle *in, ArmBehaviorData *d);
	
	int key;

};

#endif
