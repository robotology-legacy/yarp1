#ifndef __REACHINGBEHAVIOR__
#define __REACHINGBEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
#include "armmap.h"

#ifndef __INHIBITGRASPING__
	const double __handOpen[] = {20.0*degToRad, 0.0, 0.0, 0.0, 0.0, 0.0};
	const double __handClose[] = {90.0*degToRad, 70.0*degToRad, -80.0*degToRad, -80.0*degToRad, -80.0*degToRad, -80.0*degToRad};
#else
	const double __handOpen[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	const double __handClose[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif


class ABSharedData
{
	public:
		ABSharedData();
		~ABSharedData(){};
		
	YARPOutputPortOf<YARPBabyBottle> _outPort;
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
	RBWaitIdle(const YARPString &msg)
	{
		_message = msg;	
	}
	void handle(ABSharedData *d)
	{
		ACE_OS::printf("%s\n", _message.c_str());
	}

private:
	YARPString _message;
};

class RBWaitDeltaT: public RBStateBase
{
public:
	RBWaitDeltaT(const YARPString &msg, double time)
	{
		_message = msg;
		_deltaT = time;
	}

	void handle(ABSharedData *d)
	{
		ACE_OS::printf("%s\n", _message.c_str());
		ACE_OS::printf("Waiting %ld[s]\n", _deltaT);
		YARPTime::DelayInSeconds(_deltaT);
	}
private:
	YARPString _message;
	double _deltaT;
};

class RBInputCommand: public RBBaseInput
{
public:
	RBInputCommand(YBVocab k)
	{
		key = k;
	}
	bool input(YARPBabyBottle *in, ABSharedData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

class RBOutputCommand: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

class RB2Output: public RBBaseOutput
{
public:
	RB2Output(const YBVocab &v1, const YBVocab &v2)
	{
		_bottle1.setID(YBVMotorLabel);
		_bottle2.setID(YBVMotorLabel);

		_bottle1.writeVocab(v1);
		_bottle2.writeVocab(v2);

	}
	void output(ABSharedData *d);

	YARPBabyBottle _bottle1;
	YARPBabyBottle _bottle2;
};

class RBSimpleOutput: public RBBaseOutput
{
public:
	RBSimpleOutput(YBVocab k)
	{
		_key = k;
	}
	void output(ABSharedData *d);

	YBVocab _key;
	YARPBabyBottle _bottle;
};

class RBOutputReaching1: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

class RBOutputReaching2: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

class RBOutputReaching3: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

class RBOutputBack: public RBBaseOutput
{
public:
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};


class RBLearnOutputCommand: public RBBaseOutput
{
public:
	void output(ABSharedData *d);
	
};

class RBHandClose: public RBBaseOutput
{
public:
	RBHandClose()
	{
		_bottle.setID(YBVMotorLabel);
		_bottle.writeVocab(YBVHandNewCmd);
		_bottle.writeYVector(YVector(6, __handClose));
	}
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

class RBHandOpen: public RBBaseOutput
{
public:
	RBHandOpen()
	{
		_bottle.setID(YBVMotorLabel);
		_bottle.writeVocab(YBVHandNewCmd);
		_bottle.writeYVector(YVector(6, __handOpen));
	}
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

#endif