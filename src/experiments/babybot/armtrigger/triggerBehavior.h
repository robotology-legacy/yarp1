#ifndef __ARMTRIGGER__
#define __ARMTRIGGER__

#include <YARPFSM.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPBottleContent.h>
#include <YARPBehavior.h>
#include <./conf/YARPBabybotVocab.h>

class TBSharedData
{
	public:
		TBSharedData();
		~TBSharedData();
	
	void read();
	bool checkTarget();
	bool checkHand();

	YARPOutputPortOf<YARPBottle> _outPort;
	YARPInputPortOf<YVector> _fixationPort;
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
		ACE_OS::printf("Wait state: %s\n", _msg.c_str());
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
		d->_outPort.Content().reset();
		d->_outPort.Content().setID(_label);
		d->_outPort.Content().writeVocab(_vocab);
		d->_outPort.Content().display();
		d->_outPort.Write();
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

	bool input(YARPBottle *b, TBSharedData *d)
	{
		YBVocab tmpVocab;
		if (b->tryReadVocab(tmpVocab))
		{
			b->moveOn();
		
			ACE_OS::printf("Received %s returning ", tmpVocab.c_str());
			// check command
			if (tmpVocab == key)
			{
				ACE_OS::printf("true\n");
				return true;
			}
			else
			{
				ACE_OS::printf("false\n");
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
	bool input(YARPBottle *b, TBSharedData *d)
	{
		if (d->_fixationPort.Read(0))
			if (d->_fixationPort.Content()(1) > 0)
				return true;
		return false;
	}

};

class TBCheckFixated: public TBBaseInput
{
public:
	bool input(YARPBottle *b, TBSharedData *d)
	{
		if (d->_fixationPort.Read(0))
			if (d->_fixationPort.Content()(1) > 0.5)
				return true;
		return false;
	}

};

#endif