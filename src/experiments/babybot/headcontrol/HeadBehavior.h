#ifndef __HEAD_BEHAVIOR__
#define __HEAD_BEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPVocab.h>
#include "HeadThread.h"

class HeadBehavior: public YARPBehavior<HeadBehavior, HeadThread>
{
public:
	HeadBehavior(HeadThread *d):
	YARPBehavior<HeadBehavior, HeadThread>(d, "/headcontrol/behavior/i", YBVMotorLabel, YBVHeadQuit){}
};

typedef YARPFSMStateBase<HeadBehavior, HeadThread> HeadBehaviorStateBase;
typedef YARPFSMOutput<HeadThread> HeadBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<HeadThread> HeadBehaviorBaseInput;

class HBWaitIdle: public HeadBehaviorStateBase
{
public:
	HBWaitIdle(const YARPString &msg)
	{
		_message = msg;
	}

	void handle(HeadThread *d);
	
	YARPString _message;
};

class HBHibernateCommand: public HeadBehaviorBaseOutput
{
public:
	void output(HeadThread *d);
};

class HBResumeCommand: public HeadBehaviorBaseOutput
{
public:
	void output(HeadThread *d);
};

class HBSimpleInput: public HeadBehaviorBaseInput
{
public:
	HBSimpleInput(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBottle *in, HeadThread *d);
	
	YBVocab key;
	YBVocab newK;
};

class HBDirectCommandInput: public HeadBehaviorBaseInput
{
public:
	HBDirectCommandInput(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBottle *in, HeadThread *d);
	
	YBVocab key;
	YBVocab newK;
	YVector _cmd;
};

class HBAccInput: public HeadBehaviorBaseInput
{
public:
	HBAccInput(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBottle *in, HeadThread *d);
	
	YBVocab key;
	YBVocab newK;
	YVector _cmd;
};

#endif