#ifndef __rndthreadh__
#define __rndthreadh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPRateThread.h>
#include "armstatus.h"
#include "Armthread.h"

class RandomThread: public YARPRateThread
{
public:
	RandomThread(ArmThread *pArm,
				int rate,
				const char *name = "arm random control thread",
				const char *ini_file = NULL);
	virtual ~RandomThread();

	void doLoop();
	void doInit();
	void doRelease();

private:
	int _nj;
	ArmThread *pArmThread;
	YVector _maxInit;
	YVector _minInit;
	YVector _maxFinal;
	YVector _minFinal;
	YVector _maxDelta;
	YVector _minDelta;

	YVector _maxCurrent;
	YVector _minCurrent;

	char _iniFile[80];

	FILE *pFile;
	double *_command;
	double *_torques;
	double *_positions;
};

#endif //.h