#ifndef __SINK_BEHAVIOR__
#define __SINK_BEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
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
DECLARE_OUTPUT_STATE(SBOutputSuppress);
DECLARE_OUTPUT_STATE(SBOutputRelease);
DECLARE_OUTPUT_STATE(SBOutputInhibitTracker);
DECLARE_OUTPUT_STATE(SBOutputInhibitVergence);
DECLARE_OUTPUT_STATE(SBOutputInhibitSaccade);
DECLARE_OUTPUT_STATE(SBOutputInhibitVor);

DECLARE_OUTPUT_STATE(SBOutputEnableTracker);
DECLARE_OUTPUT_STATE(SBOutputEnableVergence);
DECLARE_OUTPUT_STATE(SBOutputEnableVor);
DECLARE_OUTPUT_STATE(SBOutputEnableSaccade);

DECLARE_OUTPUT_STATE(SBOutputDisplayStatus);

class SBSimpleInput: public SinkBehaviorBaseInput
{
public:
	SBSimpleInput(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBabyBottle *in, Sink *d);
	
	YBVocab key;
	YBVocab newK;
};

#endif