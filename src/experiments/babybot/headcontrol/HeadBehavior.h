#ifndef __HEAD_BEHAVIOR__
#define __HEAD_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include "HeadThread.h"

class HeadThread;

class HeadBehavior: public YARPBehavior<HeadBehavior, HeadThread>
{
public:
	HeadBehavior(HeadThread *d, int k, const std::string &pName, int exitCode):
	YARPBehavior<HeadBehavior, HeadThread>(d, k, pName, exitCode){}

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

class HBOutputCommand: public HeadBehaviorBaseOutput
{
public:
	void output(HeadThread *d);
};

class HBOutputStop: public HeadBehaviorBaseOutput
{
public:
	void output(HeadThread *d);
};

class HBSimpleInput: public HeadBehaviorBaseInput
{
public:
	HBSimpleInput(int k)
	{
		key = k;
	}
	bool input(YARPBottle *in, HeadThread *d);
	
	int key;
};

class HBDirectCommandInput: public HeadBehaviorBaseInput
{
public:
	HBDirectCommandInput(int k)
	{
		key = k;
	}
	bool input(YARPBottle *in, HeadThread *d);
	
	int key;
	YVector _cmd;
};

#endif