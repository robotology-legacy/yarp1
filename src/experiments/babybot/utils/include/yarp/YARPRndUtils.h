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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///
///
/// $Id: YARPRndUtils.h,v 1.1 2004-07-29 13:09:14 babybot Exp $
///
///
/// Useful classes for random number generations.
/// February 2004 -- by nat
/// 

#ifndef __YARPRNDUTILS__
#define __YARPRNDUTILS__

#include <yarp/YARPMatrix.h>
#include <yarp/YARPRnd.h>

// Generate normally distributed random numbers (av = 0, std = 1.0)
// Uses Box-Muller transformation.
class YARPRndNormal: public YARPRnd
{
public:
	YARPRndNormal(long seed = 1);
		
	void init (long aSeed)
	{
		YARPRnd::init(aSeed);
		_last = 2;
	}
	
	double getNumber();

private:
	inline void _boxMuller();
	
	double _y[2];
	int _last;
};

// YARPRndVector: Vector implementation of YARPRnd
class YARPRndVector: public YARPRnd
{
public:
	YARPRndVector(int size = 1, double max = 1.0, double min = 0.0)
	{ resize(YVector(1, &max), YVector(1, &min)); }

	YARPRndVector(const YVector &max, const YVector &min)
	{ resize(max, min); }

	void resize(int s, const double *max, const double *min)
	{ resize(YVector(s, max), YVector(s, min)); }

	void resize(const YVector &max, const YVector &min);
	
	inline const YVector &getVector()
	{
		for(int i = 1; i <= _size; i++)
			_random(i) = (_max(i) - _min(i)) * getNumber() + _min(i);
		
		return _random;
	}

	inline double getScalar()
	{ return (_max(1) - _min(1)) * getNumber() + _min(1); }

protected:
	YVector _max;
	YVector _min;
	YVector _random;
	int _size;
};

// YARPRndGaussVector: vector implementation of YARPRndNormal
class YARPRndGaussVector: public YARPRndNormal
{
public:
	YARPRndGaussVector(int size = 1, double av = 0.0, double std = 1.0)
	{ resize(YVector(1, &av), YVector(1, &std)); }

	YARPRndGaussVector(const YVector &av, const YVector &std)
	{ resize(av, std); }

	void resize(int s, double av, double std)
	{ resize(YVector(1, &av), YVector(1, &std)); }
	void resize(int s, const double *av, const double *std)
	{ resize(YVector(s, av), YVector(s, std)); }

	void resize(const YVector &av, const YVector &std);
	
	inline const YVector &getVector()
	{
		for(int i = 1; i <= _size; i++)
			_random(i) = _std(i)*getNumber() + _average(i);
		
		return _random;
	}

	inline double getScalar()
	{ return _std(1)*getNumber() + _average(1); }

protected:
	YVector _std;
	YVector _average;
	YVector _random;
	int _size;
};

// YARPRndSafeGaussVector
// Safe version of YARPRndGaussVector
// Random vectors are bounded between max and min. Default max = min = 0;
class YARPRndSafeGaussVector: public YARPRndGaussVector
{
public:
	YARPRndSafeGaussVector(int size = 1, double max = 0.0, double min = 0.0, double av = 0.0, double std = 1.0)
	{ resize(YVector(1, &max), YVector(1, &min), YVector(1, &av), YVector(1, &std));}

	YARPRndSafeGaussVector(const YVector &max, const YVector &min, const YVector &av, const YVector &std)
	{ resize(max, min, av, std); }

	void resize(int s, const double *max, const double *min, const double *av, const double *std)
	{ resize(YVector(s, max), YVector(s, min), YVector(s, av), YVector(s, std)); }

	void resize(const YVector &max, const YVector &min, const YVector &av, const YVector &std);
	
	inline const YVector &getVector()
	{
		for(int i = 1; i <= _size; i++)
		{
			double tmp;
			tmp = _std(i)*getNumber() + _average(i);
			if (tmp > _max(i) ) 
				_random(i) = _max(i);
			else if (tmp < _min(i) )
				_random(i) = _min(i);
			else
				_random(i) = tmp;
		}
		
		return _random;
	}

	inline double getScalar()
	{
		double ret;
		ret = _std(1)*getNumber() + _average(1);
		
		if (ret > _max(1))
			return _max(1);
		else if (ret < _min(1) )
			return _min(1);
		else
			return ret;
	}

protected:
	YVector _max;
	YVector _min;
};

#endif