#ifndef __LEARNER__
#define __LEARNER__

#define CHECKANDDESTROY(pointer) if(pointer!=NULL) { delete [] pointer;\
														pointer = NULL;}

#include <YARPBPNNet.h>
#include <YARPThread.h>
#include <YARPList.h>
#include <YARPSemaphore.h>
#include <YARPBottle.h>

class BatchDataSample
{
public:
	BatchDataSample();
	BatchDataSample(const double *in, const double *out, int inS, int outS);
		
	~BatchDataSample();

	double *_inData;
	double *_outData;
};

class BatchData : public YARPList<BatchDataSample *>
{
public:
	BatchData();
	~BatchData();

	void resize(int inS, int outSize);
	void add(double *input, double *target);
	void add(YARPBottle &n);
	void get(double **input, double **output, int *ns);

	int howMany()
	{ return _nSamples; }

	inline void lock()
	{ _mutex.Wait(); }

	inline void unlock()
	{ _mutex.Post(); }
	
private:
	void _destroy();
	int _nSamples;
	int _inSize;
	int _outSize;
	YARPSemaphore _mutex;
};

class Learner: private YARPBPNNet, private YARPThread
{
public:
	// default constructor
	Learner();
	// construct nnet from file
	Learner(const YARPString &file);
	// destructor
	~Learner();

	void add(YARPBottle &n);

	virtual void Body(void);
	
	// start a train sequence
	// if a previous train is still to be finished returns
	// YARP_FAIL, returns YARP_OK otherwise
	int train(int nIt, bool init);
	// before train, load nnet file from disk
	// internally uses train(nIt, init)
	int train(const YARPString &filename, int nIt, bool init);

	int howMany()
	{ return _samples.howMany(); }

	inline void lock()
	{ _mutex.Wait(); }

	inline void unlock()
	{ _mutex.Post(); }

	void setOutputPort(YARPOutputPortOf<YARPBottle> *port, YARPBottle *b)
	{
		_outputBottle = b;
		_outPort = port;
	}

	int send();
		
private:
	BatchData _samples;
	
	// local storage for training
	double *_input;
	double *_target;
	bool _busyTraining;
	YARPSemaphore _mutex;

	YARPBottle *_outputBottle;
	YARPOutputPortOf<YARPBottle> *_outPort;
};

#endif

/*


#ifndef __ARM_BEHAVIOR__
#define __ARM_BEHAVIOR__

#include <YARPBehavior.h>
#include <YARPBottle.h>
#include <YARPConfigFile.h>
#include <./conf/YARPMotorVocab.h>

class RndSharedData: public YARPBehaviorSharedData
{
public:
	RndSharedData(): YARPBehaviorSharedData("/armrandom/behavior/o", YBVMotorLabel)
	{
		char *root = GetYarpRoot();
		ACE_OS::sprintf (_iniFile, "%s/conf/babybot/arm.ini\0", root);

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

		_maxDelta = (_maxFinal-_maxInit)/60;
		_minDelta = (_minFinal-_minInit)/60;
	
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
	void sendShake(const YVector &cmd);
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

	bool input(YARPBottle *in, RndSharedData *d)
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

class RBWaitIdle: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("RBWaitIdle: wait idle\n");
	}
};

class RBInit: public RndBehaviorStateBase
{
public:
	void handle(RndSharedData *d)
	{
		printf("Init rnd generator\n");;
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
		d->sendNext();
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
*/