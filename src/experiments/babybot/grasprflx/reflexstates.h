#include <yarp/YARPBehavior.h>
#include <yarp/YARPConfigRobot.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPMatrix.h>
#include <yarp/YARPString.h>
#include <yarp/YARPTime.h>

const YARPString __inputPortName = "/touch/i";

class ReflexShared: public YARPBehaviorSharedData
{
public:
	ReflexShared():
	  YARPBehaviorSharedData("/graspreflex/o", YBVMotorLabel),
	  _touchPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
	{
		_touchPort.Register(__inputPortName.c_str());
		_touch.Resize(17);

		_openCmds = NULL;
		_closeCmds = NULL;

		_initialized = false;
		_rndIndex = 0;
		_count = 0;
		_inhibited = false;
	}

	void read()
	{
		if (_count%200 == 0)
			ACE_OS::printf("Read %d\n frames", _count);

		_touchPort.Read();
		_touch = _touchPort.Content();
		_count++;
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

		if ( ((tmp/count) > 0.0 ) && (!_inhibited) )
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
		if (!_initialized)
			_initRnd();

		double tmp = (_nSynergies-1)*((double) rand() )/RAND_MAX;
		_rndIndex = (int) (tmp+0.5);

		ACE_OS::printf("Selected action:%d\n", _rndIndex);
	}

	YVector getOpen()
	{
		// returns an open position
		return _openCmds[_rndIndex];
	}

	YVector getClose()
	{
		// returns a close position
		return _closeCmds[_rndIndex];
	}

	void _initRnd()
	{
		srand((unsigned) time(NULL));
		_initialized = true;
	}

	void inhibit()
	{
		ACE_OS::printf("grasp reflex inhibited\n");
		_inhibited = true;
	}

	void release()
	{
		ACE_OS::printf("grasp reflex released\n");
		_inhibited = false;
	}

public:
	YARPInputPortOf<YVector> _touchPort;
	YVector _touch;

	int _nSynergies;
	YVector *_openCmds;
	YVector *_closeCmds;

	bool _initialized;

	int _rndIndex;

	int _count;
	bool _inhibited;
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
	GRBWaitIdle()
	{
		_msg = YARPString("Waiting idle");
	}
	GRBWaitIdle(const YARPString &message)
	{
		_msg = message;
	}
	void handle(ReflexShared *d)
	{
		ACE_OS::printf("%s\n", _msg.c_str());
	}

	YARPString _msg;
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

	bool input(YARPBabyBottle *in, ReflexShared *d)
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

class GRBOutputSignal: public GRBehaviorBaseOutputState
{
public:
	GRBOutputSignal(const YBVocab &k){ _signal = k; };
		
	void output(ReflexShared *d);
	YBVocab _signal;
};

class GRBInit: public GRBehaviorBaseInputState
{
public:
	bool input(YARPBabyBottle *in, ReflexShared *d);
};

class GRBInhibitCommand: public GRBehaviorBaseOutputState
{
public:
	void output(ReflexShared *d)
	{
		d->inhibit();
	}
};

class GRBReleaseCommand: public GRBehaviorBaseOutputState
{
public:
	void output(ReflexShared *d)
	{
		d->release();
	}

};