
#include "RandomThread.h"
#include "YARPConfigFile.h"

// generate a random number within max and min
void getRand(const double *, const double *, double *, int);

void getRand(const double *max, const double *min, double *rnd, int nj)
{
	for (int i = 0 ; i < nj; i++)
	{
		double x = (double) rand()/RAND_MAX;
		int a = (max[i] - min[i]);
		int b = min[i];

		rnd[i] = a*x+b;
	}
}


RandomThread::RandomThread(ArmThread *pArm, int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate)
{
	strncpy(_iniFile, ini_file, 80);

	YARPConfigFile file;
	file.set("", ini_file);
	file.get("[GENERAL]", "Joints", &_nj, 1);
	_maxInit.Resize(_nj);
	_minInit.Resize(_nj);
	_maxFinal.Resize(_nj);
	_minFinal.Resize(_nj);
	_maxDelta.Resize(_nj);
	_minDelta.Resize(_nj);

	_maxCurrent.Resize(_nj);
	_minCurrent.Resize(_nj);

	_command = new double [_nj];
	_torques = new double [_nj];
	_positions = new double [_nj];

	pArmThread = pArm;
}

RandomThread::~RandomThread()
{
	delete [] _command;
	delete [] _torques;
	delete [] _positions;

}

void RandomThread::doInit()
{
	// read ini file now; parameters are read every time the thread starts
	YARPConfigFile file;
	file.set("", _iniFile);
	file.get("[RANDOMTHREAD]", "MAXINIT", _maxInit.data(), _nj);
	file.get("[RANDOMTHREAD]", "MININIT", _minInit.data(), _nj);

	file.get("[RANDOMTHREAD]", "MAXFINAL", _maxFinal.data(), _nj);
	file.get("[RANDOMTHREAD]", "MINFINAL", _minFinal.data(), _nj);

	_maxDelta = (_maxFinal-_maxInit)/20;
	_minDelta = (_minFinal-_minInit)/20;
	
	_minCurrent = _minInit;
	_maxCurrent = _maxInit;
	
	// initialize random generator
	srand( (unsigned)time( NULL ) );

	// open file
	// pFile = fopen("Q:\\Ini Files\\gravity.txt","a");

}

void RandomThread::doRelease()
{
//	fclose(pFile);
}

void RandomThread::doLoop()
{
	if (pArmThread->_arm_status._state == _armThread::directCommand)
	{
		cout << "New random move\n";
		int ns = pArmThread->_gravity1.getSteps();
		_maxCurrent = _maxCurrent + _maxDelta;
		_minCurrent = _minCurrent + _minDelta;

		int i = 0;
		for(i = 1; i <=_nj; i++)
		{
			if (_maxCurrent(i) >= _maxFinal(i))
				_maxCurrent(i) = _maxFinal(i);
			if (_minCurrent(i) <= _minFinal(i))
				_minCurrent(i) = _minFinal(i);
		}

		getRand(_maxCurrent.data(), _minCurrent.data(), _command, _nj);
		
		pArmThread->directCommand(YVector (_nj, _command)*degToRad);
	}
}
