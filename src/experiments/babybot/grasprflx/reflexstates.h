#include <YARPBehavior.h>
#include <./conf/YARPVocab.h>

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
	  YARPBehaviorSharedData("/graspreflex/o", YBVMotorLabel),
	  _touchPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
	{
		_touchPort.Register(__inputPortName.c_str());
		_touch.Resize(32);

		_openCmds = NULL;
		_closeCmds = NULL;
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
		{
			printf("DEBUG: activating graps, level %lf\n", tmp/count);
			return true;
		}
		else 
			return false;

	}

	void set(int n, YVector *open, YVector *close)
	{
		_nSynergies = n;
		if (_openCmds != NULL)
			delete [] _openCmds;

		if (_closeCmds != NULL)
			delete [] _closeCmds;

		_openCmds = new YVector[n];
		_closeCmds = new YVector[n];

		int i = 0;
		for(i = 0; i < n; i++)
		{
			_openCmds[i] = open[i];
			_closeCmds[i] = close[i];
		}
	}

	void updateRnd()
	{
		// pick rnd open, close couple
		// ACE_ASSERT(0);
	}

	YVector getOpen()
	{
		// returns an open position
		// ACE_ASSERT(0);
		return _openCmds[0];
	}

	YVector getClose()
	{
		// returns a close position
		// ACE_ASSERT(0);
		return _closeCmds[0];
	}

public:
	YARPInputPortOf<YVector> _touchPort;
	YVector _touch;

	int _nSynergies;
	YVector *_openCmds;
	YVector *_closeCmds;

};

// BEHAVIOR

class GRBehavior: public YARPBehavior<GRBehavior, ReflexShared>
{
	public:
		GRBehavior(ReflexShared *d):
		YARPBehavior<GRBehavior, ReflexShared>(d, "/graspreflex/i", YBVMotorLabel, YBVGraspRflxQuit){};
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

class GRBPickRndAction: public GRBehaviorBaseState
{
public:
	void handle(ReflexShared *d)
	{
		d->updateRnd();
	}
};

class GRBSimpleInput: public GRBehaviorBaseInputState
{
public:
	GRBSimpleInput(const YBVocab &k)
	{
		key = k;
	}

	bool input(YARPBottle *in, ReflexShared *d)
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

class GRBOpenOutputCommand: public GRBehaviorBaseOutputState
{
public:
	GRBOpenOutputCommand(){};
		
	void output(ReflexShared *d);
	YVector cmd;
};

class GRBCloseOutputCommand: public GRBehaviorBaseOutputState
{
public:
	GRBCloseOutputCommand(){};
		
	void output(ReflexShared *d);
	YVector cmd;
};

class GRBInit: public GRBehaviorBaseInputState
{
public:
	bool input(YARPBottle *in, ReflexShared *d);
};

