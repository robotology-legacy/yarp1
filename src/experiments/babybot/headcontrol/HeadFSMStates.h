#ifndef __HEADFSMSTATES__
#define __HEADFSMSTATES__

#include "HeadFSM.h"
#include <YARPPidFilter.h>

#include <math.h>

class HIDirectCmdStart: public HeadFSMInputs
{
public:
	HIDirectCmdStart()
	{
		_flag = NULL;	
	}

	void set(bool *f)
	{
		_flag = f;
	}

	bool input(HeadSharedData *t)
	{
		if (_flag==NULL)
			return false;
		
		return *_flag;
	}
	bool *_flag;
};

class HIDirectCmdEnd: public HeadFSMInputs
{
public:
	HIDirectCmdEnd()
	{}

	void set(const YVector &cmd)
	{
		_command = cmd;
	}
	
	bool input(HeadSharedData *t)
	{
		double tmp = 0.0;
		int i;
		for(i = 1; i <= _command.Length(); i++)
			tmp += fabs((t->_status._current_position(i) - (_command)(i)));

		if (tmp < 0.1)
			return true;
		else
			return false;
	}

	YVector _command;
};

class HODirectCmdEnd: public HeadFSMOutputs
{
public:
	HODirectCmdEnd()
	{
		_flag = NULL;
	}

	void set(bool *f)
	{
		_flag = f;
	}

	void output (HeadSharedData *t)
	{
		if (_flag==NULL)
			return;

		*_flag = false;
	}

	bool *_flag;

};

class HSDirectCmd: public HeadFSMStates
{
public:
	HSDirectCmd()
	{}
	void set(const YVector &cmd)
	{
		_command = cmd;
	}

	void setPIDs(int nj, const YARPPidFilter *p)
	{
		_nj = nj;
		_pids = new YARPPidFilter[nj];
		int i;
		for(i = 0; i < _nj; i++)
		{
			_pids[i] = p[i];
		}
		
	}
	void handle (HeadSharedData *t)
	{
		int i;
		for(i = 1; i<=_nj; i++)
		{
			t->_directCmd(i) = _pids[i-1].pid(t->_status._current_position(i) - _command(i));
		}
	}

	YVector _command;
	YARPPidFilter *_pids;
	int _nj;
};

class HSDirectCmdStop: public HeadFSMStates
{
public:
	HSDirectCmdStop()
	{}
	void handle (HeadSharedData *t)
	{
		t->_directCmd = 0.0;
	}
};

class HSTrack: public HeadFSMStates
{
public:
	HSTrack()
	{
		
	}
	
	void handle (HeadSharedData *t)
	{
		// do nothing		
	}
};

#endif
