#ifndef __ARMTRIGGER__
#define __ARMTRIGGER__

#include <YARPBehavior.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPBottle.h>
#include <./conf/YARPVocab.h>

const double __vergenceTh = 2;
const double __trackTh = (128/8 * 128/8)*2;

class TBSharedData
{
	public:
		TBSharedData();
		~TBSharedData();
	
	void read();
	bool checkTarget();
	bool checkHand();

	YARPOutputPortOf<YARPBottle> _outPort;
	YARPInputPortOf<YVector> _vergencePort;
	YARPInputPortOf<YVector> _handTrackingPort;
	YARPInputPortOf<YVector> _targetTrackingPort;

	private:
	bool _checkVergence(const YVector &in)
	{
		if ( fabs(in(1)) <= __vergenceTh )
			return true;

		return false;
	}

	bool _checkTrack(const YVector &in)
	{
		double tmp;
		tmp = in(1)*in(1);
		tmp += in(2)*in(2);

		if (tmp < __trackTh)
			return true;
	
		return false;
	}

	YVector _vergence;
	YVector _hand;
	YVector _target;
};

class TriggerBehavior: public YARPBehavior<TriggerBehavior, TBSharedData>
{
public:
	TriggerBehavior(TBSharedData *d):
	YARPBehavior<TriggerBehavior, TBSharedData>(d, "/trigger/behavior/i", YBVMotorLabel, YBVTriggerQuit){}
};

typedef YARPFSMStateBase<TriggerBehavior, TBSharedData> TBStateBase;
typedef YARPFSMOutput<TBSharedData> TBBaseOutput;
typedef YARPBaseBehaviorInput<TBSharedData> TBBaseInput;

class TBWaitRead: public TBStateBase
{
public:
	TBWaitRead(const YARPString &msg)
	{ 
		_message = msg;
		_count = 0;
	}

	void handle(TBSharedData *d);
	
	YARPString _message;
	int _count;
};

class TBTransition: public TBStateBase
{
public:
	TBTransition(const YARPString &msg)
	{
		_message = msg;
		_count = 0;
	}
	void handle(TBSharedData *d)
	{
		ACE_OS::printf("%s:%d\n", _message.c_str(), _count);
		_count ++;
		Sleep(1000);
	}

	YARPString _message;
	int _count;
};

class TBIsTargetCentered: public TBBaseInput
{
public:
	bool input(YARPBottle *in, TBSharedData *d);
};

class TBIsHandCentered: public TBBaseInput
{
public:
	bool input(YARPBottle *in, TBSharedData *d);
};

class TBSimpleInput: public TBBaseInput
{
public:
	TBSimpleInput(YBVocab k)
	{
		key = k;
	}

	bool input(YARPBottle *in, TBSharedData *d);


	YBVocab key;
	YBVocab tmpKey;
};

class TBOutputCommand: public TBBaseOutput
{
public:
	TBOutputCommand(YBVocab cmd)
	{
		_cmd = cmd;
	}
	void output(TBSharedData *d);
	
	YARPBottle _bottle;
	YBVocab _cmd;
};

#endif