#ifndef __ATT_BEHAVIOR__
#define __ATT_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include <./conf/YARPVocab.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>

class AttSharedData
{
public:
	AttSharedData();
	~AttSharedData();

	YARPInputPortOf<YVector> _positionPort;
	YARPInputPortOf<YVector> _predictionPort;
	YARPOutputPortOf<YARPBottle> _out;
};

class AttBehavior: public YARPBehavior<AttBehavior, AttSharedData>
{
public:
	AttBehavior(AttSharedData *d):
	YARPBehavior<AttBehavior, AttSharedData>(d, "/hts/i", YBVMotorLabel, YBVExit){}
};

typedef YARPFSMStateBase<AttBehavior, AttSharedData> AttBStateBase;
typedef YARPFSMOutput<AttSharedData> AttBBaseOutput;
typedef YARPBaseBehaviorInput<AttSharedData> AttBBaseInput;

class AttBWait: public AttBStateBase
{
public:
	AttBWait(const YARPString &m)
	{
		_msg = m;
	}

	void handle(AttSharedData *d);

private:
	YARPString _msg;
};

class AttBPollCurrent: public AttBStateBase
{
public:
	void handle(AttSharedData *d);
};

class AttBPollPrediction: public AttBStateBase
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
	bool input(YARPBottle *in, AttSharedData *d);
	
	YBVocab key;
	YBVocab newK;
};

class AttBSimpleOutput: public AttBBaseOutput
{
public:
	AttBSimpleOutput(const YBVocab &m)
	{
		_bottle.setID(YBVMotorLabel);
		_bottle.writeVocab(m);
		_msg = m;
	}
	void output(AttSharedData *d);
	
	YARPBottle _bottle;
	YBVocab _msg;
};

#endif