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
	YARPBehavior<SinkBehavior, Sink>(d, "/headsink/behavior/i", YBVMotorLabel, YBVSinkQuit){}
};

typedef YARPFSMStateBase<SinkBehavior, Sink> SinkBehaviorStateBase;
typedef YARPFSMOutput<Sink> SinkBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<Sink> SinkBehaviorBaseInput;

#define DECLARE_OUTPUT_STATE(CLASS_NAME) class CLASS_NAME: public SinkBehaviorBaseOutput { public: void output(Sink *d); }

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

DECLARE_OUTPUT_STATE(SBOutputInhibitAll);
DECLARE_OUTPUT_STATE(SBOutputEnableAll);
DECLARE_OUTPUT_STATE(SBOutputInhibitVor);
DECLARE_OUTPUT_STATE(SBOutputSuppress);
DECLARE_OUTPUT_STATE(SBOutputRelease);
DECLARE_OUTPUT_STATE(SBOutputInhibitTracker);
DECLARE_OUTPUT_STATE(SBOutputInhibitVergence);

DECLARE_OUTPUT_STATE(SBOutputDisplayStatus);

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