#ifndef __AStateh__
#define __AStateh__

#include "debug.h"

#define ARM_STATE_VERBOSE

#ifdef ARM_STATE_VERBOSE
#define ARM_STATE_DEBUG(string) YARP_DEBUG("ARM_STATE_DEBUG:", string)
#else  ARM_STATE_DEBUG(string) YARP_NULL_DEBUG
#endif

#include "Armthread.h"
#include <YARPLogFile.h>

//forward declarations
class ArmThread;

// abstract class to implement reaching  --> see state pattern
class AState
{
protected:
	AState(){}
	virtual ~AState(){}
	
public:
	// virtual function
	virtual void handle(ArmThread*){}
	void changeState (ArmThread*, AState *);
};

class ASDirectCommand: public AState
{
public:
	ASDirectCommand()
	{
		cmd.Resize(6);
		cmd = 0.0;
		newCmdFlag = false;
	}
	virtual void handle(ArmThread *t);
	
	static ASDirectCommand _instance;

	inline static ASDirectCommand* instance()
		{return &_instance;}

	YVector cmd;
	bool newCmdFlag;
};

class ASDirectCommandMove: public AState
{
public:
	ASDirectCommandMove()
	{
		_nSteps = 25;	// almost 1 sec 
		_steps = _nSteps;
		_finalCmd.Resize(6);
		_error.Resize(6);
		_finalCmd = 0.0;
		_error = 0.0;

		_lsparameters = new YVector[3];
		for(int i = 0; i < 3; i++)
		{
			_lsparameters[i].Resize(6);
			_lsparameters[i] = 0.0;
		}

		_learn = 0;
		_j5Ls.Resize(2);
		
		char *root = GetYarpRoot();

#if defined(__QNXEurobot__)
		ACE_OS::sprintf (filename, "%s/conf/eurobot/errors.txt", root);
		_errorFile.open(filename);
        ACE_OS::sprintf (filename, "%s/conf/eurobot/leastSquares.txt", root);
		_lsFile.open(filename);
        ACE_OS::sprintf (filename, "%s/conf/eurobot/points.txt", root);
		_pointsFile.open(filename);
#else
		_errorFile.open("Y:\\conf\\babybot\\errors.txt");
		_lsFile.open("Y:\\conf\\babybot\\leastSquares.txt");
		_pointsFile.open("Y:\\conf\\babybot\\points.txt");
#endif
	}
	~ASDirectCommandMove()
	{
		_errorFile.close();
		_lsFile.close();
		_pointsFile.close();

		delete [] _lsparameters;
	}

	inline void setFinalCmd(const YVector &final)
	{
		_finalCmd = final;
	}

	virtual void handle(ArmThread *t);
	
	static ASDirectCommandMove _instance;

	inline static ASDirectCommandMove* instance()
		{return &_instance;}

	YVector _finalCmd;
	YVector _error;
	YVector *_lsparameters;
	YVector _j5Ls;
	int _nSteps;
	int _steps;

	int _learn;

	// dump
	YARPLogFile _errorFile;
	YARPLogFile _lsFile;
	YARPLogFile _pointsFile;
};

class ASRestingInit: public AState
{
	public:
	ASRestingInit() {
		firstPosition.Resize(6);
		secondPosition.Resize(6);
		firstPosition = 0.0;
		secondPosition = 0.0;
	}
	static ASRestingInit _instance;

	virtual void handle(ArmThread *t);

	inline static ASRestingInit* instance()
		{return &_instance;}

	inline void setTrajectory(const YVector &first, const YVector &second)
	{
		firstPosition = first;
		secondPosition = second;
	}

	YVector firstPosition;
	YVector secondPosition;
};

class ASShake: public AState
{
	public:
	ASShake() {
		firstPosition.Resize(6);
		secondPosition.Resize(6);
		currentPosition.Resize(6);

		cmd.Resize(6);
		firstPosition = 0.0;
		secondPosition = 0.0;
		currentPosition = 0.0;
		cmd = 0.0;
		n = 16;
	}
	static ASShake _instance;

	virtual void handle(ArmThread *t);

	inline static ASShake* instance()
		{return &_instance;}

	inline void setTrajectory(const YVector &first, const YVector &second)
	{
		firstPosition = first;
		secondPosition = second;
	}

	inline void setActualPosition(const YVector &pos)
	{
		currentPosition = pos;
	}

	YVector firstPosition;
	YVector secondPosition;
	YVector currentPosition;
	YVector cmd;
	int n;
};

class ASZeroGInit: public AState
{
	public:
	ASZeroGInit() {}
	static ASZeroGInit _instance;

	virtual void handle(ArmThread *t);

	inline static ASZeroGInit* instance()
		{return &_instance;}
};

class ASWaitForHand: public AState
{
	public:
	ASWaitForHand() {
		_nSteps = 0;
		_timeout = 500;
	}
	static ASWaitForHand _instance;

	virtual void handle(ArmThread *t);

	inline static ASWaitForHand* instance()
		{return &_instance;}

	int _nSteps;
	int _timeout;
};

class ASZeroGWait: public AState
{
	public:
	ASZeroGWait() {}
	static ASZeroGWait _instance;

	virtual void handle(ArmThread *t);

	inline static ASZeroGWait* instance()
		{return &_instance;}
};

class ASZeroGEnd: public AState
{
	public:
	ASZeroGEnd() {}
	static ASZeroGEnd _instance;

	virtual void handle(ArmThread *t);

	inline static ASZeroGEnd* instance()
		{return &_instance;}
};

class ASMove: public AState
{
	public:
	ASMove() {
		cmd.Resize(6);
		cmd = 0.0;
	}
	static ASMove _instance;

	virtual void handle(ArmThread *t);

	inline static ASMove* instance()
		{return &_instance;}

	// next in this case is the state after the wait
	inline void setNext(AState *n)
		{ _next = n; }

	inline void setCmd(const YVector &n)
		{ cmd = n; }

	YVector cmd;
	AState *_next;
};

class ASWaitForMotion: public AState
{
	public:
	ASWaitForMotion() {
		_next = ASWaitForMotion::instance();
	}
	virtual void handle(ArmThread *t);
	
	static ASWaitForMotion _instance;
	AState *_next;

	inline static ASWaitForMotion* instance()
		{return &_instance;}
	inline void setNext(AState *n)
		{ _next = n; }
};

class ASRestingLowerGains: public AState
{
	public:
	ASRestingLowerGains() {
		}
	static ASRestingLowerGains _instance;

	virtual void handle(ArmThread *t);

	inline static ASRestingLowerGains* instance()
		{return &_instance;}

};

class ASRestingWaitIdle: public AState
{
	public:
	ASRestingWaitIdle() {
		}
	static ASRestingWaitIdle _instance;

	virtual void handle(ArmThread *t);

	inline static ASRestingWaitIdle* instance()
		{return &_instance;}

};

class ASRestingRaiseGains: public AState
{
	public:
	ASRestingRaiseGains() {
		}
	static ASRestingRaiseGains _instance;

	virtual void handle(ArmThread *t);

	inline static ASRestingRaiseGains* instance()
		{return &_instance;}
};


#endif	//.h
