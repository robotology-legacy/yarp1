// -- nat May 2003 (almost June)

#ifndef __YARPGRAVITYESTIMATOR__
#define __YARPGRAVITYESTIMATOR__

#include <conf/YARPConfig.h>
#include <YARPErrorCodes.h>
#include "debug.h"

#define GRAVITY_VERBOSE
#ifdef GRAVITY_VERBOSE
#define GRAVITY_DEBUG(string) YARP_DEBUG("GRAVITY_DEBUG:", string)
#else  GRAVITY_DEBUG(string) YARP_NULL_DEBUG
#endif

#include "YARPRecursiveLS.h"

const int __gNumOfPar = 6;
const int __querySize = 500;
const int __nSteps = 20;

class YARPGravityEstimator
{
public:
	YARPGravityEstimator(int par);
	~YARPGravityEstimator();

	inline void update(const YVector &in, double out)
	{
		_lock();
		_buildInputVector(in, _input);
		_parameters = _leastSquares.Estimate(out, _input);
		if (_leastSquares.GetSteps() == __nSteps)
		{
			GRAVITY_DEBUG(("Learnt enough points, now active!\n"));
		}
		_unlock();
	}
	
	inline void getP(YVector &out)
	{ 
		_lock();
		out = _parameters;
		_unlock();
	}

	inline int getSteps()
	{
		_lock();
		int ret = _leastSquares.GetSteps();
		_unlock();
		return ret;
	}

	// reduce G in nsteps, returns true when 0% is reached
	inline bool reduce(int nsteps)
	{
		_lock();
		bool ret = false;
		_k2 = _k2-1.0/nsteps;
		if (_k2 <= 0.0)
		{
			_k2 = 0.0;
			ret = true;
			GRAVITY_DEBUG(("disabled!\n"));
		}
		_unlock();
		return ret;
	}

	// increase G in nsteps, returns true when 100% is reached
	inline bool increase(int nsteps)
	{
		_lock();
		bool ret = false;
		_k2 = _k2+1.0/nsteps;
		if (_k2 >= 1.0)
		{
			_k2 = 1.0;
			ret = true;
			GRAVITY_DEBUG(("enabled!\n"));
		}
		_unlock();
		return ret;
	}

	inline void computeG(const YVector &in, double *g)
	{
		_lock();
		
		// check number of steps
		if (_leastSquares.GetSteps() < __nSteps) {
			_unlock();
			*g = 0;
			return;
		}

		_buildInputVector(in, _input);
		double tmp = 0.0;
		for(int i = 1; i <= _parSize; i++)
			tmp += _parameters(i)*_input(i);
		
		_k1 += _deltaK1;
		if (_k1 >= 1.0)
			_k1 = 1.0;
				
		*g = tmp*_k1*_k2;
		_unlock();
	}

	int load(const char *filename);
	int save(const char *filename);

private:
	virtual void _buildInputVector(const YVector &in, YVector &out) = 0;
	
private:
	inline void _lock(){};
	inline void _unlock(){};

	YVector _parameters;
	YVector _input;
	int _parSize;
	double _k1;
	double _k2;
	double _deltaK1;

	YARPRecursiveLS _leastSquares;
};

//
class J1GravityEstimator : public YARPGravityEstimator
{
public:
	J1GravityEstimator(): YARPGravityEstimator(6){}

private:
	void _buildInputVector(const YVector &in, YVector &out)
	{
		double tmp2 = in(2) + 0.5*in(3);	// 2nd and 3rd joint are summed
		out(1) = in(1) * in(1);
		out(2) = tmp2 * tmp2;
		out(3) = in(1) * tmp2;
		out(4) = in(1);
		out(5) = tmp2;
		out(6) = 1.0;
	}
};

//
class J2GravityEstimator : public YARPGravityEstimator
{
public:
	J2GravityEstimator(): YARPGravityEstimator(6){}

private:
	void _buildInputVector(const YVector &in, YVector &out)
	{
		double tmp2 = in(2) + 0.5*in(3);	// 2nd and 3rd joint are summed
		out(1) = in(1) * in(1);
		out(2) = tmp2 * tmp2;
		out(3) = in(1) * tmp2;
		out(4) = in(1);
		out(5) = tmp2;
		out(6) = 1.0;
	}
};

//
class J3GravityEstimator : public YARPGravityEstimator
{
public:
	J3GravityEstimator(): YARPGravityEstimator(6){}

private:
	void _buildInputVector(const YVector &in, YVector &out)
	{
		double tmp2 = in(2) + in(3);	// 2nd and 3rd joint are summed
		out(1) = in(1) * in(1);
		out(2) = tmp2 * tmp2;
		out(3) = in(1) * tmp2;
		out(4) = in(1);
		out(5) = tmp2;
		out(6) = 1.0;
	}
};

//
class J5GravityEstimator : public YARPGravityEstimator
{
public:
	J5GravityEstimator(): YARPGravityEstimator(2)
	{
		_input.Resize(1);
		_input(1) = 0;
	}
	
	void _buildInputVector(const YVector &in, YVector &out)
	{
		out(1) = in(1);
		out(2) = 1.0;
	}

	// update, overload (this is just to allow a double to be passed to the function)
	inline void update(double in, double out)
	{
		// no lock/unlock here, they are already within update
		_input(1) = in;
		YARPGravityEstimator::update(_input, out);
	}

	inline void computeG(double in, double *g)
	{
		// no lock/unlock here, they are already within update
		_input(1) = in;
		YARPGravityEstimator::computeG(_input, g);
	}

private:
	YVector _input;
};

#endif
