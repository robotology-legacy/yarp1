// HandThread implementation file


#include "HandThread.h"

HandThread::HandThread(int rate, const char *name):
YARPRateThread(name, rate)
{
	_tempPos1 = new double [6];
	_tempPos2 = new double [6];

	for(int i = 0; i < 6; i++)
	{
		_tempPos1[i] = 0.0;
		_tempPos2[i] = 0.0;
	}

	_shake = false;
}

HandThread::~HandThread()
{
	delete [] _tempPos1;
	delete [] _tempPos2;
}
