#ifndef __ARMTRIGGER__
#define __ARMTRIGGER__

#include <yarp/YARPFSM.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPBehavior.h>
#include <yarp/YARPConfigRobot.h>

class TBSharedData
{
	public:
		TBSharedData();
		~TBSharedData();
	
	void read();
	bool checkTarget();
	bool checkHand();

	YARPOutputPortOf<YARPBabyBottle> _outBottlePort;
	
	// YARPInputPortOf<YVector> _handTrackingPort;
	// YARPInputPortOf<YVector> _targetTrackingPort;

	private:
};

class TriggerBehavior: public YARPBehavior<TriggerBehavior, TBSharedData>
{
public:
	TriggerBehavior(TBSharedData *d):
	YARPBehavior<TriggerBehavior, TBSharedData>(d, "/armtrigger/behavior/i", YBVMotorLabel, YBVExit){}
};

typedef YARPFSMStateBase<TriggerBehavior, TBSharedData> TBStateBase;
typedef YARPFSMOutput<TBSharedData> TBBaseOutput;
typedef YARPBaseBehaviorInput<TBSharedData> TBBaseInput;

class TBWaitIdle: public TBStateBase
{
public:
	TBWaitIdle(const YARPString &m, double dt=0.0)
	{
		_msg = m;
		_deltaT = dt;
	}
	void handle(TBSharedData *d)
	{
		ACE_OS::printf("Wait state: %s [%lf]\n", _msg.c_str(), _deltaT);
		if (_deltaT != 0)
			YARPTime::DelayInSeconds(_deltaT);
	}

	YARPString _msg;
	double _deltaT;
};

class TBSimpleOutput: public TBBaseOutput
{
public:
	TBSimpleOutput(const YBVocab &v)
	{
		_vocab = v;
		_label = YBVMotorLabel;
	}

	void output(TBSharedData *d)
	{
		d->_outBottlePort.Content().reset();
		d->_outBottlePort.Content().setID(_label);
		d->_outBottlePort.Content().writeVocab(_vocab);
		d->_outBottlePort.Content().display();
		d->_outBottlePort.Write();
	}

	YBVocab _vocab;
	YBVocab _label;
};

class TBArmInput: public TBBaseInput
{
public:
	TBArmInput(const YBVocab &k)
	{
		key = k;
	}

	bool input(YARPBabyBottle *b, TBSharedData *d)
	{
		YBVocab tmpVocab;
		if (b->tryReadVocab(tmpVocab))
		{
			b->moveOn();
			if (tmpVocab == key)
				return true;
			else
			{
				b->rewind();
				return false;
			}
		}
		else
			b->rewind();
			return false;
	}

	YBVocab key;

};

class TBCheckAlmostFixated: public TBBaseInput
{
public:
	bool input(YARPBabyBottle *b, TBSharedData *d)
	{
		ACE_OS::printf("WARNING: This was not supposed to be called\n");
		return false;
	}

};

class TBCheckFixated: public TBBaseInput
{
public:
	bool input(YARPBabyBottle *b, TBSharedData *d)
	{
		YBVocab tmpVocab;
		if (b->tryReadVocab(tmpVocab))
		{
			if (tmpVocab == YBVSaccadeFixated)
				return true;
			else
				return false;
		}
		else
			return false;
	}
};

#endif