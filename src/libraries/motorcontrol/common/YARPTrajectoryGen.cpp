// YARPTrajectoryGen.cpp: implementation of the YARPTrajectoryGen class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPTrajectoryGen.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPTrajectoryGen::YARPTrajectoryGen()
{
	_commands = NULL;
	_lastCommand = NULL;
	_finalCommand = NULL;
	_index = 0;

	_resize(1, 1);

	_busy = false;
}

YARPTrajectoryGen::YARPTrajectoryGen(int size, int npnts)
{
	_commands = NULL;
	_lastCommand = NULL;
	_finalCommand = NULL;
	
	_index = 0;

	_resize(size, npnts);
	_busy = false;
}

YARPTrajectoryGen::~YARPTrajectoryGen()
{
	for (int i = 0; i < _size ; i++)
		delete [] _commands[i];

	delete [] _commands;

	delete [] _lastCommand;
	delete [] _finalCommand;
}

int YARPTrajectoryGen::_resize(int size, int npnts)
{
	if (npnts < 0)
		npnts = 1;
	
	if ( (_steps == npnts) && (_size == size) ) {
		// no need to resize
		return -1;
	}

	int i;
	if (_commands != NULL)
	{
		for (i = 0; i < _steps ; i++)
		{	
			if (_commands[i] != NULL)
				delete [] _commands[i];
		}
		delete [] _commands;
	}

	_steps = npnts;
	_size = size;

	_commands = new double * [_steps];
	for (i = 0; i < npnts; i++)
	{
		_commands[i] = new double [_size];
		for (int j = 0; j < _size; j++)
			_commands[i][j] = 0;
	} 

	_lastCommand = new double [_size];
	_finalCommand = new double [_size];

	for (i = 0; i < _size; i++)
	{
		_finalCommand[i] = 0;
		_lastCommand[i]  = 0;
	}
	_index = 0;
	return -1;
}

bool YARPTrajectoryGen::getNext(double *next)
{
	_lock();
	if (_index < _steps)
	{
		memcpy(next, _commands[_index], sizeof(double)*_size);
		memcpy(_lastCommand, next, sizeof(double)*_size);
		_index ++;
		_busy = true;
		_unlock();
		return true;
	}
	else
	{
		// do nothing but enable new trajectories
		memcpy(next, _lastCommand, sizeof(double)*_size);
		_busy = false;
		_unlock();
		return false;
	}
}

void YARPTrajectoryGen::stop()
{
	_lock();
	_index = _steps;	//signal the end of the trajectory
	_busy = false;		//enable new one
	_unlock();
}

int YARPTrajectoryGen::setFinal(const double *final, int nstp)
{
	_lock();
	int ret;
	double *last;
	last = new double [_size];
	memcpy(last, _lastCommand, sizeof(double) * _size);
	_unlock();
	ret = setFinal(last, final, nstp);
	delete [] last;
	return ret;
}

int YARPTrajectoryGen::setFinal(const double *actual, const double *final, int nstp)
{
	_lock();
	if (_busy)
	{
		// already generating trajectory, do nothing
		_unlock();
		return 0;
	}
	else
	{

		_resize(_size, nstp);

		memcpy(_finalCommand, final, sizeof(double) *_size);

		double *delta;
		delta = new double [_size];
		int i;
		for(i = 0; i < _size; i++)
		{
			delta[i] = (_finalCommand[i] - actual[i])/_steps;
			// first act value...
			_commands[0][i] = actual[i] + delta[i];
		}

		// compute activation vectors (that is: trajectory)
		for (i = 1; i < _steps; i++)
			for (int j = 0; j < _size; j++)
				_commands[i][j] = _commands[i-1][j] + delta[j];
		
		delete [] delta;
		
		_index = 0;
		_busy = true;
		_unlock();
		return -1;
	}
}

YARPTrajectoryGen::setFinal(const double *actual, const double *viaPoint, const double *final, int nstp)
{
	// NOT IMPLEMENTED YET !

	/*
	_lock();
	if (_busy)
	{
		// already generating trajectory, do nothing
		_unlock();
		return 0;
	}
	else
	{

		_resize(_size, nstp);
		
		_finalCommand = final;
		
		int nFirst;
		int nSecond;

		nFirst = int (_steps/2 + 0.5);
		nSecond = _steps - nFirst;

		CVisDVector delta;

		delta = (viaPoint-actual)/nFirst;
		_commands[0] = actual + delta;

		// compute activation vectors (that is: trajectory)
		for (int i = 1; i < nFirst; i++)
			_commands[i] = _commands[i-1] + delta;
		
		delta = (final-_commands[nFirst-1])/nSecond;
		
		for (i = nFirst; i < _steps; i++)
			_commands[i] = _commands[i-1] + delta;
				
		_index = 0;
		_busy = true;
		_unlock();
		return -1;
	}*/

	return 0;
}