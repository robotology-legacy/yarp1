#ifndef __ZERO_CROSSING__
#define __ZERO_CROSSING__

#include <yarp/YARPRobotmath.h>
#include <yarp/YARPIterativeStats.h>
#include <vector>

class ZeroCrossing
{
public:
	ZeroCrossing(double t = 0);
	~ZeroCrossing();

	void reset()
	{
		_idle = true;
		_rise = false;

		_index = 0;
		_last = 0;
		
		_measure.reset();
	}

	void setThreshold(double t)
	{
		_threshold = t;
	}
	
	int result(double *mean, double *std)
	{
		*mean = _measure.get_mean();
		*std = _measure.get_std();
		return _measure.elem();
	}

	void dump();

	void find(double in, double *out);

	double _threshold;
	bool _idle;
	bool _rise;
	
	int _index;
	int _last;
	IterativeStats _measure;
};

#endif