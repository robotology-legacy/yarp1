#ifndef __REACHINGBEHAVIOR__
#define __REACHINGBEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
#include "armmap.h"

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
		ACE_OS::printf("Waiting %.1lf[s]\n", _deltaT);
		YARPTime::DelayInSeconds(_deltaT);
	}
private:
	YARPString _message;
	double _deltaT;
};

class RBInputCommand: public RBBaseInput
{
public:
	RBInputCommand(const YBVocab &k)
	{
		key = k;
	}
	bool input(YARPBabyBottle *in, ABSharedData *d);
	
	YBVocab key;
	YBVocab tmpK;
};

class RBInputString: public RBBaseInput
{
public:
	RBInputString(const YARPString &s)
	{
		key = s;
	}
	bool input(YARPBabyBottle *in, ABSharedData *d);
	
	YARPString key;
	YARPString tmpK;
};

class RBOutputCommand: public RBBaseOutput
{
public:
	RBOutputCommand(const YBVocab &v1, const YBVocab &v2, const YVector &hand)
	{
		_bottle1.setID(YBVMotorLabel);
		_bottle2.setID(YBVMotorLabel);
		_bottle3.setID(YBVMotorLabel);
		
		// bottle1 will be filled "on the fly"
		_bottle2.writeVocab(v2);
		_bottle2.writeYVector(hand);
		/////
		_bottle3.writeVocab(v1);
	}
	void output(ABSharedData *d);

private:
	void _sendAbort(ABSharedData *d);
	void _sendReachingCommand(ABSharedData *d, const YVector &cmd);

	YARPBabyBottle _bottle1;
	YARPBabyBottle _bottle2;
	YARPBabyBottle _bottle3;
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

class RBOutputMessage: public RBBaseOutput
{
public:
	RBOutputMessage(const YBVocab &v)
	{
		_bottle.setID(YBVMotorLabel);
		_bottle.writeVocab(v);
	}
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
};

class RB4Output: public RBBaseOutput
{
public:
	RB4Output(const YBVocab &v1, const YBVocab &v2, const YBVocab &v3, const YBVocab &v4, const YVector &cmd)
	{
		_bottle1.setID(YBVMotorLabel);
		_bottle2.setID(YBVMotorLabel);
		_bottle3.setID(YBVMotorLabel);
		_bottle4.setID(YBVMotorLabel);

		_bottle1.writeVocab(v1);
		_bottle2.writeVocab(v2);
		_bottle3.writeVocab(v3);
		
		_bottle4.writeVocab(v4);
		_bottle4.writeYVector(cmd);

	}
	void output(ABSharedData *d);

	YARPBabyBottle _bottle1;
	YARPBabyBottle _bottle2;
	YARPBabyBottle _bottle3;
	YARPBabyBottle _bottle4;
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

class RBOutputReachingCL: public RBBaseOutput
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

class RBMotorCommand: public RBBaseOutput
{
public:
	RBMotorCommand(const YBVocab &voc, const YVector &cmd)
	{
		_bottle.setID(YBVMotorLabel);
		_bottle.writeVocab(voc);
		_bottle.writeYVector(cmd);
	}
	void output(ABSharedData *d);

	YARPBabyBottle _bottle;
	
};

#endif