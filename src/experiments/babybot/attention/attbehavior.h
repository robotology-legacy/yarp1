#ifndef __ATT_BEHAVIOR__
#define __ATT_BEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPPort.h>

class AttSharedData
{
public:
	AttSharedData();
	~AttSharedData();

	YARPInputPortOf<YVector> _handPort;
	YARPInputPortOf<YVector> _handPredictionPort;
	YARPInputPortOf<YVector> _targetPort;
	YARPInputPortOf<YVector> _egoMapPort;
	YARPOutputPortOf<YVector> _out;
};

class AttBehavior: public YARPBehavior<AttBehavior, AttSharedData>
{
public:
	AttBehavior(AttSharedData *d):
	YARPBehavior<AttBehavior, AttSharedData>(d, "/attention/behavior/i", YBVMotorLabel, YBVAttentionQuit){}
};

typedef YARPFSMStateBase<AttBehavior, AttSharedData> AttBStateBase;
typedef YARPFSMOutput<AttSharedData> AttBBaseOutput;
typedef YARPBaseBehaviorInput<AttSharedData> AttBBaseInput;

class AttBHand: public AttBStateBase
{
public:
	void handle(AttSharedData *d);
};

class AttBTarget: public AttBStateBase
{
public:
	void handle(AttSharedData *d);
};

class AttBEgoMap: public AttBStateBase
{
public:
	void handle(AttSharedData *d);
};

class AttBHandPrediction: public AttBStateBase
{
public:
	void handle(AttSharedData *d);
};

class AttBSimpleInput: public AttBBaseInput
{
public:
	AttBSimpleInput(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBabyBottle *in, AttSharedData *d);
	
	YBVocab key;
	YBVocab newK;
};

#endif