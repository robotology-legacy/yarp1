/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPTrajectoryGen.cpp,v 1.6 2004-01-17 00:15:15 gmetta Exp $
///
///

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

int YARPTrajectoryGen::setFinal(const double *actual, const double *viaPoint, const double *final, int nstp)
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
