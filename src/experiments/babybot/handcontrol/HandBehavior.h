#ifndef __HAND_BEHAVEIORS__
#define __HAND_BEHAVEIORS__

#include <YARPBehavior.h>
#include "HandThread.h"

typedef HandThread HandBehaviorData;

class HandBehavior: public YARPBehavior<HandBehavior, HandThread>
{
	public:
		HandBehavior(HandBehaviorData *d, int k, const std::string &pName):
		YARPBehavior<HandBehavior, HandBehaviorData>(d, k, pName){};
};

typedef YARPFSMStateBase<HandBehavior, HandBehaviorData> HandBehaviorStateBase;
typedef YARPFSMOutput<HandBehaviorData> HandBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<HandBehaviorData> HandBehaviorBaseInput;


class HBWaitIdle: public HandBehaviorStateBase
{
public:
	void handle(HandBehaviorData *d)
	{
		printf("Wait idle\n");
	}
};

class HBWaitMotion: public HandBehaviorStateBase
{
public:
	void handle(HandBehaviorData *d)
	{
		printf("Wait for motion\n");
	}
};

class HBOutputCommand: public HandBehaviorBaseOutput
{
public:
	void output(HandBehaviorData *d);
};

class HBShakeCmdOutput: public HandBehaviorBaseOutput
{
public:
	void output(HandBehaviorData *d);
};

class HBInputCommand: public HandBehaviorBaseInput
{
public:
	HBInputCommand()
	{
		key = YBVHandNewCmd;
	}
	bool input(YARPBottle *in, HandBehaviorData *d);
	
	int key;
};


class HBInputReset: public HandBehaviorBaseInput
{
public:
	HBInputReset()
	{
		key = YBVHandResetEncoders;
	}
	bool input(YARPBottle *in, HandBehaviorData *d);
	
	int key;
};

class HBShakeCmdInput: public HandBehaviorBaseInput
{
public:
	HBShakeCmdInput()
	{
		key = YBVHandShake;
	}
	bool input(YARPBottle *in, HandBehaviorData *d);
		
	int key;
};

class HBCheckMotionDone: public HandBehaviorBaseInput
{
public:
	HBCheckMotionDone()
	{
		key = YBVHandDone;
	}

	bool input(YARPBottle *in, HandBehaviorData *d);
	
	int key;

};

#endif
