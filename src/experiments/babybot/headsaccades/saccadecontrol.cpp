#include "saccadecontrol.h"
// May 2004 -- by nat

//////
// NeckControl class
NeckControl::NeckControl(const YARPString &iniFile, int insize, int outsize)
{
	_iniFile = iniFile;
	_inSize = insize;
	_outSize = outsize;

	YARPConfigFile file;
	char *root = GetYarpRoot();
	_path.append(root);
	_path.append("/conf/babybot/"); 
	
	file.set(_path, _iniFile);
	file.get("[NECKCONTROL]", "nPids", &_nPids);

	_pids = new YARPPidFilter[_nPids];
	double temp[3];
	int i;
	for(i = 0; i < _nPids; i++)
	{
		char tmpField[80];
		sprintf(tmpField, "Pid%d", i);
		file.get("[NECKCONTROL]", tmpField, temp, 3);
		_pids[i].setKs(temp[0], temp[1], temp[2]);
	}

	_cmd.Resize(outsize);
	_cmd = 0.0;
}

//////
// SaccadeControl class

void SaccadeControl::init(const YARPString &filename);
{
	_iniFile = filename;
	
	YARPConfigFile file;
	char *root = GetYarpRoot();
	_path.append(root);
	_path.append("/conf/babybot/"); 
	
	file.set(_path, _iniFile);

	file.get("[HEAD]", "Joints", &_nj);

	_allocVectors();

	// file.get("[SACCADES]", "SaccadeSteps", &_openLoopSteps);
	file.get("[SACCADES]", "ClosedLoopSteps", &_closedLoopSteps);
	file.get("[SACCADES]", "ClosedLoopError", &_closedLoopError);
	file.get("[SACCADES]", "SaccadeError", &_openLoopError);
	file.get("[SACCADES]", "MaxSpeed", _openLoopSpeed.data(), _nj);
	_openLoopSpeed *= degToRad;

	//	_log.open("c:/temp/log.txt");
}

void SaccadeControl::_allocVectors()
{
	_temp.Resize(__nj);
	_command.Resize(__nj);
	_vRef.Resize(3);
	_predictedPosition.Resize(__nj);
	_position.Resize(__nj);
	_stop.Resize(__nj);
	_closedLoop.Resize(__nj);
	_eyesClosedLoop.Resize(__nj);
	_openLoopSpeed.Resize(__nj);
	_neckClosedLoop.Resize(__nj);
	_zeroError.Resize(__nj);
	_zeroError = 0.0;	

	_openLoopDone = false;
	_saccadeDone = true;
	_openLoopCounter = 0;
	_closedLoopCounter = 0;

	_openLoopSteps = 1;
	_closedLoopSteps = 1;
	_openLoopError = 2;
	_closedLoopError = 2;

	_retinal.Resize(2);
}