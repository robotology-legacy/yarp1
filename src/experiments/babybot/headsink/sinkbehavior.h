#ifndef __SINK_BEHAVIOR__
#define __SINK_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include <./conf/YARPVocab.h>
#include "sink.h"

class SinkBehavior: public YARPBehavior<SinkBehavior, Sink>
{
public:
	SinkBehavior(Sink *d):
	YARPBehavior<SinkBehavior, Sink>(d, "/sink/behavior/i", YBVMotorLabel, YBVSinkQuit){}
};

typedef YARPFSMStateBase<SinkBehavior, Sink> SinkBehaviorStateBase;
typedef YARPFSMOutput<Sink> SinkBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<Sink> SinkBehaviorBaseInput;

class SBWaitIdle: public SinkBehaviorStateBase
{
public:
	SBWaitIdle(const YARPString &msg)
	{
		_message = msg;
	}

	void handle(Sink *d);
	
	YARPString _message;
};

class SBOutputInhibitAll: public SinkBehaviorBaseOutput
{
public:
	void output(Sink *d);
};

class SBSimpleInput: public SinkBehaviorBaseInput
{
public:
	SBSimpleInput(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBottle *in, Sink *d);
	
	YBVocab key;
	YBVocab newK;
};

#endif