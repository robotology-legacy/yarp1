#ifndef __HLBEHAVIORHH__
#define __HLBEHAVIORHH__

#include <YARPBehavior.h>
#include <./conf/YARPMotorVocab.h>

#include "FindHand.h"

class HLBehavior: public YARPBehavior<HLBehavior, FindHand>
{
	public:
		HLBehavior(FindHand *d):
		YARPBehavior<HLBehavior, FindHand>(d, "/handlocalization/behavior/i", YBVMotorLabel, YBVHandLocQuit){};
};

typedef YARPFSMStateBase<HLBehavior, FindHand> HLBehaviorStateBase;
typedef YARPFSMOutput<FindHand> HLBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<FindHand> HLBehaviorBaseInput;

class HLDoMotion: public HLBehaviorStateBase
{
public:
	void handle(FindHand *d)
	{
		printf("Computing motion\n");
	}
};

class HLFind: public HLBehaviorStateBase
{
public:
	void handle(FindHand *d)
	{
		printf("End detection sequence\n");
	}
};

class HLShakeStart: public HLBehaviorBaseInput
{
public:
	HLShakeStart(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBottle *in, FindHand *d)
	{
		if (!in->tryReadVocab(tmpK))
			return false;

		if (tmpK != key)
			return false;
	
		in->moveOn();

		d->startShake();

		return true;
	}
	YBVocab key;
	YBVocab tmpK;
};

class HLShakeStop: public HLBehaviorBaseInput
{
public:
	HLShakeStop(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBottle *in, FindHand *d)
	{
		if (!in->tryReadVocab(tmpK))
			return false;

		if (tmpK != key)
			return false;
	
		in->moveOn();

		d->stopShake();

		return true;
	}

	YBVocab key;
	YBVocab tmpK;
};

#endif