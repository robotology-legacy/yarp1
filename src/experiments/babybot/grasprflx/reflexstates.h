#include <YARPBehavior.h>

#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPMatrix.h>
#include <YARPString.h>
#include <YARPTime.h>

const YARPString __inputPortName = "/touch/i";

class ReflexShared: public YARPBehaviorSharedData
{
public:
	ReflexShared():
	  YARPBehaviorSharedData(YBLabelMotor, "/graspreflex/o"),
	  _touchPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
	{
		_touchPort.Register(__inputPortName.c_str());
		_touch.Resize(32);
	}

	void read()
	{
		_touchPort.Read();
		_touch = _touchPort.Content();
	}

	bool checkPalmTouch()
	{
		// check touch on the palm

		double tmp = 0;
		int count = 0;
		for(int i = 1; i <= 5; i++)
		{
			tmp += _touch(i);
			count++;
		}

		if ( (tmp/count) > 0.0 )
			return true;
		else 
			return false;

	}

public:
	YARPInputPortOf<YVector> _touchPort;
	YVector _touch;
};

// BEHAVIOR

class GRBehavior: public YARPBehavior<GRBehavior, ReflexShared>
{
	public:
		GRBehavior(ReflexShared *d):
		YARPBehavior<GRBehavior, ReflexShared>(d, YBLabelMotor, "/graspreflex/i"){};
};

typedef YARPFSMStateBase<GRBehavior, ReflexShared> GRBehaviorBaseState;
typedef YARPFSMOutput<ReflexShared> GRBehaviorBaseOutputState;
typedef YARPBaseBehaviorInput<ReflexShared> GRBehaviorBaseInputState;

class GRBLoopTouch: public GRBehaviorBaseState
{
public:
	void handle(ReflexShared *d)
	{
		d->read();
	}
};

class GRBWaitIdle: public GRBehaviorBaseState
{
public:
	void handle(ReflexShared *d)
	{
		// do nothing
	}
};

class GRBSimpleInput: public GRBehaviorBaseInputState
{
public:
	GRBSimpleInput(int k)
	{
		key = k;
	}

	bool input(YARPBottle *in, ReflexShared *d)
	{
		int k;
		if (!in->tryReadVocab(&k))
			return false;

		if (k != key)
			return false;
	
		in->moveOn();

		return true;
	}
	
	int key;

};

class GRBGrasp: public GRBehaviorBaseState
{
public:
	void handle(ReflexShared *d)
	{
		printf("Grasping...\n");
	}
};

class GRBWaitDeltaT: public GRBehaviorBaseState
{
public:
	GRBWaitDeltaT(double time)
	{
		_deltaT = time;
	}

	void handle(ReflexShared *d)
	{
		YARPTime::DelayInSeconds(_deltaT);
	}

	double _deltaT;
};

class GRBOutputCommand: public GRBehaviorBaseOutputState
{
public:
	GRBOutputCommand(){};
	GRBOutputCommand(const YVector &c);
	
	void output(ReflexShared *d);
	YVector cmd;
};

class GRBInit: public GRBehaviorBaseInputState
{
public:
	bool input(YARPBottle *in, ReflexShared *d);
};

