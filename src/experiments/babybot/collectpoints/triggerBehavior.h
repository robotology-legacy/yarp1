#ifndef __collectpoints__
#define __collectpoints__

#include <YARPBehavior.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPBottle.h>
#include <YARPControlBoardNetworkData.h>
#include <./conf/YARPVocab.h>
#include <YARPLogFile.h>
#include <YARPBabybotHeadKin.h>

const double __vergenceTh = 2;
const double __trackTh = (128/13 * 128/13)*2;
const char __outFile1[]= "c:/collector.dat";

class TBSharedData
{
	public:
		TBSharedData();
		~TBSharedData();
	
	void read();
	
	const Y3DVector &getCartesian()
	{
		_headKinematics.update(_head);
		return _headKinematics.fixation();
	}

	const Y3DVector &getPolar()
	{
		_headKinematics.update(_head);
		return _headKinematics.fixationPolar();
	}

	bool checkTarget();
	
	YARPOutputPortOf<YARPBottle> _outPort;
	YARPInputPortOf<YVector> _vergencePort;
	YARPInputPortOf<YVector> _targetTrackingPort;
	YARPInputPortOf<YVector> _headPort;
	YARPInputPortOf<YARPControlBoardNetworkData> _armPort;

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

	void _printStatus()
	{
		if (_armFrame%500 == 0)
			ACE_OS::printf("Arm frames %d head frames %d target %d vergence %d\n", _armFrame, _headFrame, _targetFrame, _vergenceFrame);
	}

	public:
	YARPBabybotHeadKin _headKinematics;
	YVector _vergence;
	YVector _target;
	YVector _arm;
	YVector _head;

	int _vergenceFrame;
	int _targetFrame;
	int _armFrame;
	int _headFrame;
};

class TriggerBehavior: public YARPBehavior<TriggerBehavior, TBSharedData>
{
public:
	TriggerBehavior(TBSharedData *d):
	YARPBehavior<TriggerBehavior, TBSharedData>(d, "/collect/behavior/i", YBVMotorLabel, YBVTriggerQuit){}
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

class TBIsTargetCentered: public TBBaseInput
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

class TBOutput: public TBBaseOutput
{
public:
	TBOutput(const YARPString &name)
	{
		_name = name;
		_file.open(__outFile1);
	}

	~TBOutput()
	{
		_file.close();
	}
	
	void output(TBSharedData *d);
	
	YARPBottle _bottle;
	YARPString _name;
	
	YARPLogFile _file;
};

#endif