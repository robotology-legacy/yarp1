#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPBehavior.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPMatrix.h>
#include <yarp/YARPString.h>
#include <yarp/YARPTime.h>

const YARPString __inputPortName = "/touch/i";

class ExplorationShared: public YARPBehaviorSharedData
{
public:
	ExplorationShared():
	  YARPBehaviorSharedData("/kfexploration/o", YBVMotorLabel),
	  _outBehavior(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP)
	{
		_outBehavior.Register("/kfexploration/behavior/o", "default");
	}

public:

	YARPOutputPortOf<YARPBabyBottle> _outBehavior;

};

// BEHAVIOR

class EBehavior: public YARPBehavior<EBehavior, ExplorationShared>
{
	public:
		EBehavior(ExplorationShared *d):
		YARPBehavior<EBehavior, ExplorationShared>(d, "/kfexploration/i", YBVMotorLabel, YBVKFExplorationQuit){};
};

typedef YARPFSMStateBase<EBehavior, ExplorationShared> EBehaviorBaseState;
typedef YARPFSMOutput<ExplorationShared> EBehaviorBaseOutputState;
typedef YARPBaseBehaviorInput<ExplorationShared> EBehaviorBaseInputState;

class EBWaitIdle: public EBehaviorBaseState
{
public:
	EBWaitIdle(const char *m)
	{
		msg = YARPString(m);
	}
	void handle(ExplorationShared *d)
	{
		ACE_OS::printf("Waiting:%s\n", msg.c_str());
	}

	YARPString msg;
};

class EBSimpleInput: public EBehaviorBaseInputState
{
public:
	EBSimpleInput(const YBVocab &k)
	{
		key = k;
	}

	bool input(YARPBabyBottle *in, ExplorationShared *d)
	{
		if (!in->tryReadVocab(tmpK))
			return false;

		if (tmpK != key)
			return false;
	
		in->moveOn();

		return true;
	}
	
	YBVocab key;
	YBVocab tmpK;

};

class EBGrasp: public EBehaviorBaseState
{
public:
	void handle(ExplorationShared *d)
	{
		printf("Grasping...\n");
	}
};

class EBWaitDeltaT: public EBehaviorBaseState
{
public:
	EBWaitDeltaT(double time)
	{
		_deltaT = time;
	}

	void handle(ExplorationShared *d)
	{
		YARPTime::DelayInSeconds(_deltaT);
	}

	double _deltaT;
};

class EBOutputCommand: public EBehaviorBaseOutputState
{
public:
	EBOutputCommand(const YBVocab &v, const YVector &cmd)
	{
		_cmd = cmd;
		_signal = v;
	};
		
	void output(ExplorationShared *d);
	YVector _cmd;
	YBVocab _signal;
};

class EBStartKF: public EBehaviorBaseOutputState
{
public:
	EBStartKF()
	{
		_bottle.writeVocab(YBVKFStart);
	}
	void output(ExplorationShared *d)
	{
		d->_outBehavior.Content() = _bottle;
		d->_outBehavior.Write(0);
	}

	YARPBabyBottle _bottle;
};

class EBEnableTracker: public EBehaviorBaseOutputState
{
public:
	EBEnableTracker()
	{
		_bottleVor.setID(YBVMotorLabel);
		_bottleVor.writeVocab(YBVSinkEnableVor);

		_bottleTracker.setID(YBVMotorLabel);
		_bottleTracker.writeVocab(YBVSinkEnableTracker);
	}
	void output(ExplorationShared *d)
	{
		d->_data = _bottleVor;
		d->send();

		d->_data = _bottleTracker;
		d->send();
	}

	YARPBabyBottle _bottleVor;
	YARPBabyBottle _bottleTracker;
};

class EBInhibitTracker: public EBehaviorBaseOutputState
{
public:
	EBInhibitTracker()
	{
		_bottleVor.setID(YBVMotorLabel);
		_bottleVor.writeVocab(YBVSinkInhibitVor);

		_bottleTracker.setID(YBVMotorLabel);
		_bottleTracker.writeVocab(YBVSinkInhibitTracker);
	}
	void output(ExplorationShared *d)
	{
		d->_data = _bottleVor;
		d->send();

		d->_data = _bottleTracker;
		d->send();
	}

	YARPBabyBottle _bottleVor;
	YARPBabyBottle _bottleTracker;
};

class EBStopKF: public EBehaviorBaseOutputState
{
public:
	EBStopKF()
	{
		_bottle.writeVocab(YBVKFStop);
	}

	void output(ExplorationShared *d)
	{
		d->_outBehavior.Content() = _bottle;
		d->_outBehavior.Write(0);
	}

	YARPBabyBottle _bottle;
};

class EBSimpleOutput: public EBehaviorBaseOutputState
{
public:
	EBSimpleOutput(const YBVocab &cmd)
	{
		_signal = cmd;
	};
		
	void output(ExplorationShared *d);
	YBVocab _signal;
};