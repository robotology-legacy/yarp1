#ifndef __saccadebehaviorhh__
#define __saccadebehaviorhh__

#include <YARPBehavior.h>
#include <conf/YARPVocab.h>
#include <YARPPort.h>
#include <YARPBottleContent.h>
#include <YARPVectorPortContent.h>
#include "SaccadeControl.h"

#include "SmoothControl.h"

class SBSharedData: public YARPInputPortOf<YARPBottle>
{
	public:
		SBSharedData()
		{ 
			_out.Register("/headsaccades/cmd/o", "Net0");
			_in.Register("/headsaccades/position/i", "Net0");
//			_outEgoMap.Register("/headsaccades/egomap/o", "Net0");
			_outAtt.Register("/headsaccades/attention/o", "Net0");
			_inBehavior.Register("/headsaccades/behavior/i", "Net0");
			_outFixation.Register("/headsaccades/fixation/o", "Net0");
			_inFixation.Register("/headsaccades/fixation/i", "Net0");
			Register("/headsaccades/target/i", "Net0");

			targetX = 128;
			targetY = 128;

			_newTarget = false;
		}
		~SBSharedData()
		{}

	public:
		virtual void OnRead()
		{
			int x,y;
			YARPInputPortOf<YARPBottle>::Read();
			YARPInputPortOf<YARPBottle>::Content().readInt(&x);
			YARPInputPortOf<YARPBottle>::Content().readInt(&y);
			
			if ( (x<=0) || (y<=0) )
			{
				ACE_OS::printf("Warning received an invalid target %d\t%d\n", x, y);			
			}
			else
			{
				targetX = x;
				targetY = y;
				_newTarget = true;
			}
		}

		void updateIORTable()
		{
			_outAtt.Content().reset();
			_outAtt.Content().writeVocab(YBVVAUpdateIORTable);
			_outAtt.Write();
		}

		bool getTarget(int &x, int &y)
		{
			if (_newTarget)
			{ 
				x = targetX;
				y = targetY;
				_newTarget = false;
				return true;
			}
			else
				return false;
		}

		bool newTarget()
		{ return _newTarget; }
	
	YARPOutputPortOf<YARPBottle> _out;
	YARPOutputPortOf<YARPBottle> _outAtt;
	YARPOutputPortOf<YVector> _outFixation;
	YARPInputPortOf<YVector> _in;
	YARPInputPortOf<YARPBottle> _inBehavior;
	YARPInputPortOf<YVector> _inFixation;


	int targetX;
	int targetY;
	bool _newTarget;
	SaccadeControl _saccade;
};

class SaccadeBehavior : public YARPFSM<SaccadeBehavior, SBSharedData>
{
public:
	SaccadeBehavior(SBSharedData *d):
	  YARPFSM<SaccadeBehavior, SBSharedData>(d){};
};

typedef YARPFSMStateBase<SaccadeBehavior, SBSharedData> SBStateBase;
typedef YARPFSMOutput<SBSharedData> SBBaseOutput;
typedef YARPFSMInput<SBSharedData> SBBaseInput;

class SBWaitIdle: public SBStateBase
{
public:
	SBWaitIdle(const YARPString &msg, double deltaT = 0.0)
	{
		_message = msg;
		_dt = deltaT;
	}

	void handle(SBSharedData *d);

	YARPString _message;
	double _dt;
};

class SBOpenLoop: public SBStateBase
{
public:
	void handle(SBSharedData *d);

};

class SBClosedLoop: public SBStateBase
{
public:
	void handle(SBSharedData *d);
};

class SBStop: public SBStateBase
{
public:
	void handle(SBSharedData *d);

	YARPString _message;
};

class SBManualInitSaccade: public SBBaseInput
{
public:
	SBManualInitSaccade(const YARPString &msg)
	{
		_message = msg;
	}

	bool input(SBSharedData *d);

	YARPString _message;
};

class SBInitSaccade: public SBStateBase
{
public:
	void handle(SBSharedData *d);
};

class SBWaitFixation: public SBBaseInput
{
public:
	SBWaitFixation(const YARPString &msg)
	{
		_message = msg;
	}

	bool input(SBSharedData *d);

	YARPString _message;
	int _steps;
};

class SBNewTarget: public SBBaseInput
{
public:
	bool input(SBSharedData *d);
};

class SBCheckSaccadeDone: public SBBaseInput
{
public:
	bool input(SBSharedData *d);
};

class SBCheckOpenLoopDone: public SBBaseInput
{
public:
	bool input(SBSharedData *d);
};

class SBSimpleOutput: public SBBaseOutput
{
public:
	void output(SBSharedData *d);
};

#endif