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
	void handle(ABSharedData *d)
	{
		// dummy state, do nothing
	}
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

class RBLearnOutputCommand: public RBBaseOutput
{
public:
	void output(ABSharedData *d);
	
};













#endif