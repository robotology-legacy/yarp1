#ifndef __ARM_BEHAVIOR__
#define __ARM_BEHAVIOR__

#include <yarp/YARPBehavior.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPConfigRobot.h>

#include <yarp/YARPRndUtils.h>

class RndSharedData: public YARPBehaviorSharedData
{
public:
	RndSharedData(): YARPBehaviorSharedData("/armrandom/behavior/o", YBVMotorLabel)
	{
		char *root = GetYarpRoot();
		ACE_OS::sprintf (_iniFile, "%s/%sarm.ini\0", root, ConfigFilePath);

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
		_stored.Resize(_nj);
		_stored = 0;

		// read ini file now; parameters are read every time the thread starts
		file.get("[RANDOMTHREAD]", "MAXINIT", _maxInit.data(), _nj);
		file.get("[RANDOMTHREAD]", "MININIT", _minInit.data(), _nj);

		file.get("[RANDOMTHREAD]", "MAXFINAL", _maxFinal.data(), _nj);
		file.get("[RANDOMTHREAD]", "MINFINAL", _minFinal.data(), _nj);

		_maxDelta = (_maxFinal-_maxInit)/60;
		_minDelta = (_minFinal-_minInit)/60;
	
		_minCurrent = _minInit;
		_maxCurrent = _maxInit;

		YVector av(_nj);
		YVector std(_nj);
		YVector max(_nj);
		YVector min(_nj);
		
		file.get("[RANDOMTHREAD]", "GAUSS_AVERAGE", av.data(), _nj);
		file.get("[RANDOMTHREAD]", "GAUSS_STD", std.data(), _nj);
		file.get("[RANDOMTHREAD]", "GAUSS_MAX", max.data(), _nj);
		file.get("[RANDOMTHREAD]", "GAUSS_MIN", min.data(), _nj);

		_gauss.resize(max, min, av, std);
		_rnd.resize(_maxFinal, _minFinal);

		_first = true;
			
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

	YVector _stored;
	YARPRndVector _rnd;
	YARPRndSafeGaussVector _gauss;

	bool _first;

	// generate a random number within max and min
	void getRand(const double *, const double *, double *, int);

	void sendNext();
	void sendShake(const YVector &cmd);

	void reset()
	{
		_first = true;
	}

	void sendNextGauss();
	void gauss();
	void rnd();
};

class RndBehavior: public YARPBehavior<RndBehavior, RndSharedData>
{
public:
	RndBehavior(RndSharedData *d):
	YARPBehavior<RndBehavior, RndSharedData>(d, "/armrandom/behavior/i", YBVMotorLabel, YBVArmRndQuit){}

};

typedef YARPFSMStateBase<RndBehavior, RndSharedData> RndBehaviorStateBase;
typedef YARPFSMOutput<RndSharedData> RndBehaviorBaseOutput;
typedef YARPBaseBehaviorInput<RndSharedData> RndBehaviorBaseInput;

class RBSimpleInput: public RndBehaviorBaseInput
{
public:
	RBSimpleInput(const YARPString &k)
	{
		key = k;
	}

	bool input(YARPBabyBottle *in, RndSharedData *d)
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

class RBSimpleOutput: public RndBehaviorBaseOutput
{
public:
	RBSimpleOutput(const YBVocab &k)
	{
		_key = k;
	}

	void output(RndSharedData *d)
	{
		d->writeAndSend(_key);
	}

	YBVocab _key;
};

class RBRndReset: public RndBehaviorBaseOutput
{
public:
	void output(RndSharedData *d)
	{
		d->reset();
	}
};

class RBWaitIdle: public RndBehaviorStateBase
{
public:
	RBWaitIdle(const YARPString &msg)
	{
		_message = msg;
	}

	void handle(RndSharedData *d)
	{
		printf(_message.c_str());
		d->reset();
	}

	YARPString _message;
};

class RBInit: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("Init rnd generator\n");;
		d->reset();
		srand(time(NULL));
	}
};

class RBWaitMotion: public RndBehaviorStateBase
{
public:
	RBWaitMotion(const YARPString &msg)
	{
		_message = msg;
	}
	void handle(RndSharedData *d)
	{
		printf("RBWaitMotion: %s\n", _message.c_str());
	}
	
	YARPString _message;
};

class RBInitMotion: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("RBInitMotion: initialize new random movement\n");
		// d->sendNext();
		d->sendNextGauss();
	}
};

class RBInitShake: public RndBehaviorStateBase
{
public:
	RBInitShake(const YVector cmd, const YARPString &msg)
	{
		command = cmd;
		message = msg;
	}
	void handle(RndSharedData *d)
	{
		printf("RBInitShake: %s\n", message.c_str());
		d->sendShake(command);
	}

	YARPString message;
	YVector command;
};


class RBWaitDeltaT: public RndBehaviorStateBase
{
public:
	RBWaitDeltaT(double time)
	{
		_deltaT = time;
	}

	void handle(RndSharedData *d)
	{
		YARPTime::DelayInSeconds(_deltaT);
	}

	double _deltaT;
};

#endif
