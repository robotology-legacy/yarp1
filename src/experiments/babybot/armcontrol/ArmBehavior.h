#ifndef __ARM_BEHAVIOR__
#define __ARM_BEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
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

class ABSetStiffness: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABForceOutputCommand: public ArmBehaviorBaseOutput
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

class ABForceRestingTrue: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABForceRestingFalse: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABInhibitResting: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABInhibitRestingTrue: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABInhibitRestingFalse: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABOutputPrintPids: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABOutputHibernate: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABOutputResume: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABOutputShakeCmd: public ArmBehaviorBaseOutput
{
public:
	void output(ArmBehaviorData *d);
};

class ABSimpleOutput: public ArmBehaviorBaseOutput
{
public:
	ABSimpleOutput(const YBVocab &v)
	{
		vocab = v;
	}
	void output(ArmBehaviorData *d);

private:
	YBVocab vocab;
};

class ABInputCommand: public ArmBehaviorBaseInput
{
public:
	ABInputCommand(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBabyBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;

};

class ABInputShakeCommand: public ArmBehaviorBaseInput
{
public:
	ABInputShakeCommand(const YBVocab &k)
	{
		key = k;
	}

	bool input(YARPBabyBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

class ABSimpleInput: public ArmBehaviorBaseInput
{
public:
	ABSimpleInput(const YBVocab &k)
	{
		key = k;
	}

	bool input(YARPBabyBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

class ABInputSetStiffness: public ArmBehaviorBaseInput
{
public:
	ABInputSetStiffness(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBabyBottle *in, ArmBehaviorData *d);
	
	YBVocab key;
	YBVocab tmpK;

};

class ABSetStiffnessState:public ArmBehaviorStateBase
{
public:
	void handle(ArmBehaviorData *d);
};

#endif
