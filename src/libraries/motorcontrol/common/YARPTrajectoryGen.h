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
/// $Id: YARPTrajectoryGen.h,v 1.4 2004-05-05 17:55:36 babybot Exp $
///
///

// YARPTrajectoryGen.h: interface for the YARPTrajectoryGen class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __YARPTrajectoryGenh__
#define __YARPTrajectoryGenh__

// simple class to provide trajectory generation
#include <math.h>
#include "YARPSemaphore.h"

class YARPTrajectoryGen  
{
public:
	YARPTrajectoryGen();
	YARPTrajectoryGen(int nActivation, int npnts);
	virtual ~YARPTrajectoryGen();


	int setFinalSpecSpeed(const double *final, const double *speed)
	{
		_lock();
		int ret;
		double *last;
		last = new double [_size];
		memcpy(last, _lastCommand, sizeof(double) * _size);
		_unlock();
		ret = setFinalSpecSpeed(last, final, speed);
		delete [] last;
		return ret;
	}
	
	int setFinalSpecSpeed(const double *actual, const double *final, const double *speed)
	{
		int k = 0;
		double tmax = 0;
		double t;
		for(k = 0; k < _size; k++)
		{
			t = fabs((actual[k] - final[k]))/speed[k];
			if (t > tmax)
				tmax = t;
		}

		// now tmax is time max
		int n = (int) (tmax/_deltaT + 0.5);
		if (n<=0)
			n = 1;

		setFinal(actual, final, n);

		printf("Steps: %d\n", n);

		return n;
	}

	int setFinal(const double *final, int nstp);
	int setFinal(const double *actual, const double *final, int nstp);
	int setFinal(const double *actual, const double *viaPoint, const double *final, int nstp);	int getCurrent(double *cmd)
	{
		_lock();
		memcpy(cmd, _lastCommand, sizeof(double)*_size);
		_unlock();
		return YARP_OK;
	}
	bool getNext(double *next);
	inline int resize(int size, int npnts)
	{
		int ret;
		_lock();
			ret = _resize(size, npnts);
		_unlock();
		return ret;
	}

	// check if busy
	inline bool checkBusy()
	{ 
		bool temp;
		_lock();
			temp = _busy; 
		_unlock();
		return temp;
	}

	// stop whatever trajectory we are generating
	void stop();
	
private:
	int _resize(int size, int npnts);

	double *_lastCommand;
	double *_finalCommand;
	int _steps;
	int _size;
	int _index;
	double **_commands; 
	bool _busy;

	double _deltaT;

	inline void _lock(void)
	{_mutex.Wait();}	// add timeout ?

	inline void _unlock(void)
	{_mutex.Post();}
	
	YARPSemaphore _mutex;
};

#endif // ,h
