//

#include <YARPBehavior.h>

#include "FindHand.h"

class HLBehavior: public YARPBehavior<HLBehavior, FindHand>
{
	public:
		HLBehavior(FindHand *d, int k, const std::string &pName):
		YARPBehavior<HLBehavior, FindHand>(d, k, pName){};
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
	HLShakeStart(int k)
	{
		key = k;
	}
	bool input(YARPBottle *in, FindHand *d)
	{
		int k;
		if (!in->tryReadVocab(&k))
			return false;

		if (k != key)
			return false;
	
		in->moveOn();

		d->startShake();

		return true;
	}
	int key;
};

class HLShakeStop: public HLBehaviorBaseInput
{
public:
	HLShakeStop(int k)
	{
		key = k;
	}
	bool input(YARPBottle *in, FindHand *d)
	{
		int k;
		if (!in->tryReadVocab(&k))
			return false;

		if (k != key)
			return false;
	
		in->moveOn();

		d->stopShake();

		return true;
	}
	int key;
};