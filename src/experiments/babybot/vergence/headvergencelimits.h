#ifndef __headvergencelimitshh__
#define __headvergencelimitshh__

// given max and min vergence angles, compute max and min shifts which are allowed
// for searching the maximum in the crosscorrelation function.
// July 2004 -- by nat

#include <yarp/YARPBabybotHeadKin.h>

const double __maxVerg = 40*degToRad;
const double __minVerg = 3*degToRad;
const int __centerX = 128;
const int __centerY = 128;

class HeadVergenceLimits
{
public:
	HeadVergenceLimits()
	{
		_enable = false;
	}

	void init(const YARPString &path, const YARPString &filename)
	{
		_headkinematics.load(path, filename);
	}

	void update(const YVector &newp)
	{
		_enable = true;
		_currentPosition = newp;
		_headkinematics.update(newp);
	}

	bool computeLimits(float &max, float &min)
	{
		if (!_enable)
			return false;

		YVector tmpV(3);
		_headkinematics.computeRay(YARPBabybotHeadKin::KIN_RIGHT_PERI, tmpV, __centerX, __centerY);

		///// COMPUTE MIN
		YVector tmpPos;
		tmpPos = _currentPosition;
		tmpPos(4) = - (__maxVerg + _currentPosition(5));
		_headkinematics.update(tmpPos);
		int x,y;
		_headkinematics.intersectRay(YARPBabybotHeadKin::KIN_RIGHT_PERI, tmpV, x, y);
		min = __centerX - x;
		
		///// COMPUTE MAX
		tmpPos = _currentPosition;
		tmpPos(4) = - (__minVerg + _currentPosition(5));
		_headkinematics.update(tmpPos);
		_headkinematics.intersectRay(YARPBabybotHeadKin::KIN_RIGHT_PERI, tmpV, x, y);
		max = __centerX - x;
			
		return true;
	}

public:
	YARPBabybotHeadKin	_headkinematics;
	YVector _currentPosition;
	bool _enable;
};

#endif