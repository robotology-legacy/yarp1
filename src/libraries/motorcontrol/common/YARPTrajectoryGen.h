// YARPTrajectoryGen.h: interface for the YARPTrajectoryGen class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __yarptrajectoryh__
#define __yarptrajectoryh__

// simple class to provide trajectory generation
#include <math.h>
#include "YARPSemaphore.h"

class YARPTrajectoryGen  
{
public:
	YARPTrajectoryGen();
	YARPTrajectoryGen(int nActivation, int npnts);
	virtual ~YARPTrajectoryGen();

	int setFinal(const double *final, int nstp);
	int setFinal(const double *actual, const double *final, int nstp);
	int setFinal(const double *actual, const double *viaPoint, const double *final, int nstp);
	int getCurrent(double *cmd)
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

	inline void _lock(void)
	{_mutex.Wait();}	// add timeout ?

	inline void _unlock(void)
	{_mutex.Post();}
	
	YARPSemaphore _mutex;
};

#endif // ,h
