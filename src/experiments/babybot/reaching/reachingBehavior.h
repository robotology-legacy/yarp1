#ifndef __REACHINGBEHAVIOR__
#define __REACHINGBEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include <./conf/YARPVocab.h>
#include "armmap.h"

class ABSharedData
{
	public:
		ABSharedData();
		~ABSharedData(){};
		
	YARPOutputPortOf<YARPBottle> _outPort;
	YARPInputPortOf<YVector> _headPort;
	YARPInputPortOf<YARPControlBoardNetworkData> _armPort;
	ArmMap _map;
};

class ReachingBehavior: public YARPBehavior<ReachingBehavior, ABSharedData>
{
public:
	ReachingBehavior(ABSharedData *d):
	YARPBehavior<ReachingBehavior, ABSharedData>(d, "/reaching/behavior/i", YBVMotorLabel, YBVReachingQuit){}
};

typedef YARPFSMStateBase<ReachingBehavior, ABSharedData> RBStateBase;
typedef YARPFSMOutput<ABSharedData> RBBaseOutput;
typedef YARPBaseBehaviorInput<ABSharedData> RBBaseInput;

class RBWaitIdle: public RBStateBase
{
public:
	RBWaitIdle(const YARPString &msg)
	{
		_message = msg;	
	}
	void handle(ABSharedData *d)
	{
		ACE_OS::printf("%s\n", _message.c_str());
	}

private:
	YARPString _message;
};

class RBWaitDeltaT: public RBStateBase
{
public:
	RBWaitDeltaT(const YARPString &msg, double time)
	{
		_message = msg;
		_deltaT = time;
	}

	void handle(ABSharedData *d)
	{
		ACE_OS::printf("%s\n", _message.c_str());
		ACE_OS::printf("Waiting %ld[s]\n", _deltaT);
		YARPTime::DelayInSeconds(_deltaT);
	}
private:
	YARPString _message;
	double _deltaT;
};

class RBInputCommand: public RBBaseInput
{
public:
	RBInputCommand(YBVocab k)
	{
		key = k;
	}
	bool input(YARPBottle *in, ABSharedData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

class RBOutputCommand: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBottle _bottle;
	
};

class RBSimpleOutput: public RBBaseOutput
{
public:
	RBSimpleOutput(YBVocab k)
	{
		_key = k;
	}
	void output(ABSharedData *d);

	YBVocab _key;
	YARPBottle _bottle;
};

class RBOutputReaching: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBottle _bottle;
	
};

class RBOutputBack: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBottle _bottle;
	
};


class RBLearnOutputCommand: public RBBaseOutput
{
public:
	void output(ABSharedData *d);
	
};













#endif