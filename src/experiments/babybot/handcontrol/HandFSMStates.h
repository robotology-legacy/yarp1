#ifndef __HANDFSMSTATES__
#define __HANDFSMSTATES__

#include "HandFSM.h"

class HandThread;

class WaitNSteps: public HandFSMInputs
{
public:
	WaitNSteps()
	{
		_nMax = 0;
		_n = 0;
	}

	bool input(HandSharedData *t)
	{
		_n++;

		if (_n >= _nMax)
		{
			_n = 0;
			return true;
		}
		
		return false;
	}

	void setNSteps(int n)
	{
		_nMax = n;
	}

	int _nMax;
	int _n;
};

class Pulse: public HandFSMInputs
{
public:
	Pulse()
	{
		_do = false;
	}

	bool input(HandSharedData *t)
	{
		if (_do)
		{
			_do = false;
			return true;
		}
		return false;
	}

	void set()
	{
		_do = true;
	}

	bool _do;
};


class IdleState: public HandFSMStates
{
public:
	void handle(HandSharedData *t);
};

class EndMotion: public HandFSMStates
{
public:
	void handle(HandSharedData *t);
};

class InitMoveState: public HandFSMStates
{
public:
	InitMoveState()
	{
		_cmd.Resize(6);
		_cmd = 0.0;
	}
	void handle(HandSharedData *t);

	void setCmd(const YVector &c)
	{ _cmd = c; }

	YVector _cmd;
};

#endif