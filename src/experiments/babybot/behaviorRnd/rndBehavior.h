#include <YARPBehavior.h>

#ifndef __ARM_BEHAVIOR__
#define __ARM_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include <YARPConfigFile.h>

class RndSharedData: public YARPBehaviorSharedData
{
public:
	RndSharedData(): YARPBehaviorSharedData(YBLabelMotor, "/armrandom/behavior/o")
	{
		strncpy(_iniFile, "y:\\conf\\babybot\\arm.ini", 80);

		YARPConfigFile file;
		file.set("", _iniFile);
		file.get("[GENERAL]", "Joints", &_nj, 1);
		_maxInit.Resize(_nj);
		_minInit.Resize(_nj);
		_maxFinal.Resize(_nj);
		_minFinal.Resize(_nj);
		_maxDelta.Resize(_nj);
		_minDelta.Resize(_nj);
	
		_maxCurrent.Resize(_nj);
		_minCurrent.Resize(_nj);

		_command = new double [_nj];
		_torques = new double [_nj];
		_positions = new double [_nj];

		// read ini file now; parameters are read every time the thread starts
		file.get("[RANDOMTHREAD]", "MAXINIT", _maxInit.data(), _nj);
		file.get("[RANDOMTHREAD]", "MININIT", _minInit.data(), _nj);

		file.get("[RANDOMTHREAD]", "MAXFINAL", _maxFinal.data(), _nj);
		file.get("[RANDOMTHREAD]", "MINFINAL", _minFinal.data(), _nj);

		_maxDelta = (_maxFinal-_maxInit)/20;
		_minDelta = (_minFinal-_minInit)/20;
	
		_minCurrent = _minInit;
		_maxCurrent = _maxInit;
	
		// initialize random generator
		srand( (unsigned)time( NULL ) );
	}

	~RndSharedData()
	{
		delete [] _command;
		delete [] _torques;
		delete [] _positions;
	}

	int _nj;
	YVector _maxInit;
	YVector _minInit;
	YVector _maxFinal;
	YVector _minFinal;
	YVector _maxDelta;
	YVector _minDelta;

	YVector _maxCurrent;
	YVector _minCurrent;

	char _iniFile[80];

	double *_command;
	double *_torques;
	double *_positions;

	// generate a random number within max and min
	void getRand(const double *, const double *, double *, int);

	void sendNext();
};

class RndBehavior: public YARPBehavior<RndBehavior, RndSharedData>
{
public:
	RndBehavior(RndSharedData *d):
	YARPBehavior<RndBehavior, RndSharedData>(d, YBLabelMotor, "/armrandom/behavior/i"){}

};

typedef YARPFSMStateBase<RndBehavior, RndSharedData> RndBehaviorStateBase;
typedef YARPFSMOutput<RndSharedData> RndBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<RndSharedData> RndBehaviorBaseInput;

class RBCheckMotionDone: public RndBehaviorBaseInput
{
public:
	RBCheckMotionDone()
	{
		key = YBVArmDone;
	}

	bool input(YARPBottle *in, RndSharedData *d)
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

class RBStart: public RndBehaviorBaseInput
{
public:
	RBStart()
	{
		key = YBVArmRndStart;
	}

	bool input(YARPBottle *in, RndSharedData *d)
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

class RBStop: public RndBehaviorBaseInput
{
public:
	RBStop()
	{
		key = YBVArmRndStop;
	}

	bool input(YARPBottle *in, RndSharedData *d)
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

class RBWaitIdle: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("RBWaitIdle: wait idle\n");
	}
};

class RBWaitMotion: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("RBWaitMotion: wait for arm motion to be done\n");
	}
};

class RBInitMotion: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("RBInitMotion: initialize new random movement\n");
		d->sendNext();
	}
};

#endif
