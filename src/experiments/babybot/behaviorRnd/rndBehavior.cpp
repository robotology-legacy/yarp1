#include "rndBehavior.h"
#include <iostream>
using namespace std;

void RndSharedData::getRand(const double *max, const double *min, double *rnd, int nj)
{
	for (int i = 0 ; i < nj; i++)
	{
		double x = (double) rand()/RAND_MAX;
		int a = (max[i] - min[i]);
		int b = min[i];

		rnd[i] = a*x+b;
	}
}

void RndSharedData::sendNext()
{
	cout << "New random move\n";
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

	YVector cmd;
	cmd.Resize(6);
	for(i = 0; i < _nj; i++)
	{
		cmd(i+1) = _command[i]*degToRad;
	}

	_stored = cmd;	// store old command

	_data.reset();
	_data.writeVocab(YBVArmNewCmd);
	_data.writeYVector(cmd);
	send();
}

void RndSharedData::rnd()
{
	cout << "New random move\n";
	
	YVector cmd;
	cmd.Resize(_nj);

	cmd = _rnd.getVector();

	int i;
	for(i = 1; i <= _nj; i++)
	{
		cout << cmd(i) << '\t';
		cmd(i) = cmd(i)*degToRad;
	}

	cout << "\n";

	_data.reset();
	_data.writeVocab(YBVArmNewCmd);
	_data.writeYVector(cmd);

	_stored = cmd;

	send();
}

void RndSharedData::gauss()
{
	cout << "New gaussian move\n";
	
	YVector cmd;
	cmd.Resize(_nj);

	cmd = _gauss.getVector();

	int i;
	for(i = 1; i <= _nj; i++)
	{
		cout << cmd(i) << '\t';
		cmd(i) = cmd(i)*degToRad;
	}
	cout << "\n";

	cmd = cmd + _stored; // add new command
	_stored = cmd;		 // save current

	_data.reset();
	_data.writeVocab(YBVArmNewCmd);
	_data.writeYVector(cmd);

	send();
}

void RndSharedData::sendNextGauss()
{
	if (_first)
	{
		rnd();
		_first = false;
	}
	else
		gauss();

}

void RndSharedData::sendShake(const YVector &cmd)
{
	_data.reset();
	_data.writeVocab(YBVArmShake);
	_data.writeYVector(cmd);
	send();
}