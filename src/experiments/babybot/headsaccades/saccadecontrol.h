#ifndef __saccadecontrolhh__
#define __saccadecontrolhh__

#include <YARPMatrix.h>
#include <YARPBabybotHeadKin.h>
#include <YARPConfigFile.h>
#include <YARPLogFile.h>
#include "SmoothControl.h"

using namespace _logpolarParams;

const int __nj = 5;	// LATER REMOVE THIS

class NeckControl
{
public:
	NeckControl(const YARPString &iniFile, int insize, int outsize)
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
	
	~NeckControl()
	{
		delete [] _pids;
	}

	const YVector & apply(const YVector &in)
	{
		double pred_r = in(4);
		double pred_l = in(5);

		double pred_tilt = in(3);

		_cmd(1) = _pids[0].pid(pred_r-pred_l);
		_cmd(2) = _pids[1].pid(pred_tilt-in(2));
		_cmd(3) = 0.0;
		_cmd(4) = 0.0;
		_cmd(5) = 0.0;

		return _cmd;
	}

private:
	YARPPidFilter *_pids;
	int _nPids;
	YVector _cmd;
	YARPString _iniFile;
	YARPString _path;
	int _inSize;
	int _outSize;
};

class SaccadeControl
{
public:
	SaccadeControl():
		_gaze ( YMatrix (_dh_nrf, __nj, DH_left[0]), YMatrix (_dh_nrf, __nj, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
		_neckControl("headcontrol.ini",5,5),
		_smooth("headcontrol.ini")
		{
			// LATER
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

			_openLoopDone = false;
			_saccadeDone = true;
			_openLoopCounter = 0;
			_closedLoopCounter = 0;

			_openLoopSteps = 1;
			_closedLoopSteps = 1;
			_openLoopError = 2;
			_closedLoopError = 2;

			_retinal.Resize(2);

			readCnfFile("headcontrol.ini");
		}
private:
	YARPBabybotHeadKin _gaze;
	SmoothControl _smooth;
	bool _openLoopDone;
	bool _saccadeDone;
	int _openLoopCounter;
	int _closedLoopCounter;
	
	int _openLoopSteps;
	int _closedLoopSteps;
	int _openLoopError;
	int _closedLoopError;
	YVector _openLoopSpeed;
	YARPString _iniFile;
	YARPString _path;
	YARPLogFile _log;

	NeckControl _neckControl;

	YVector _closedLoop;
	YVector _eyesClosedLoop;
	YVector _neckClosedLoop;
	YVector _command;
	YVector _temp;
	YVector _position;
	YVector _predictedPosition;
	YVector _vRef;
	YVector _stop;
	YVector _retinal;
	int _x;
	int _y;
	
public:
	~SaccadeControl(){}

	void readCnfFile(const YARPString &filename)
	{
		_iniFile = filename;
	
		YARPConfigFile file;
		char *root = GetYarpRoot();
		_path.append(root);
		_path.append("/conf/babybot/"); 
	
		file.set(_path, _iniFile);

		// file.get("[SACCADES]", "SaccadeSteps", &_openLoopSteps);
		file.get("[SACCADES]", "ClosedLoopSteps", &_closedLoopSteps);
		file.get("[SACCADES]", "ClosedLoopError", &_closedLoopError);
		file.get("[SACCADES]", "SaccadeError", &_openLoopError);
		file.get("[SACCADES]", "MaxSpeed", _openLoopSpeed.data(), __nj);
		_openLoopSpeed *= degToRad;

	//	_log.open("c:/temp/log.txt");
	}

	void computeNew(int x, int y)
	{
		_temp = 0.0;
		_command = 0.0;
		_x = x;
		_y = y;
		_vRef = 0;
		_stop = 0;

		_gaze.computeRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _vRef, x, y);

		// integration
		int i = 0;
		int predx;
		int predy;
		_predictedPosition = _position;
		int deltaX = x-_xsize/2;
		int deltaY = y-_ysize/2;
		int error =  deltaX*deltaX + deltaY*deltaY;
		while(error>(_openLoopError*_openLoopError))
		{
			_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _vRef, predx, predy);
			deltaX = (predx-_xsize/2);
			deltaY = (predy-_ysize/2);

			_smooth.apply(deltaX, deltaY, _temp);

			error =  deltaX*deltaX + deltaY*deltaY;

			// accumulate
			_predictedPosition = _predictedPosition + (_temp*=0.04);
			// upadte position
			_gaze.update(_predictedPosition);
		}

		_openLoopSteps = _computeCommand((_predictedPosition-_position), _openLoopSpeed, _command);
		_saccadeDone = false;
		_openLoopDone = false;
		_openLoopCounter = 0;
		_closedLoopCounter = 0;

	}

	int _computeCommand(const YVector &s, const YVector &olSpeed, YVector &command)
	{
		// LATER: deltaT and nj
		int max = 0;
		
		double deltaS1 = s(4); 
		double direction1 = fabs(s(4))/s(4);
		double deltaS2 = s(3);
		double direction2 = fabs(s(3))/s(3);

		double tmpSpeed1 = olSpeed(1)+olSpeed(4);
		int steps1 = int (fabs(deltaS1)/(0.04*tmpSpeed1) + 1.0);

		double tmpSpeed2 = olSpeed(2)+olSpeed(3);
		int steps2 = int (fabs(deltaS2)/(0.04*tmpSpeed2) + 1.0);

		if (steps2>steps1)
			max = steps2;
		else
			max = steps1;
		
		// now I have max number of steps
		// compute how much the neck can do in these steps;
		double tmp1 = direction1*olSpeed(1)*max*0.04;
		double tmp2 = direction2*olSpeed(2)*max*0.04;

		if (tmp1 >= deltaS1*0.5)
		{
			// too much, just move the eyes
			tmp1 = 0;
		}
		if (tmp2 >= deltaS2*0.5)
		{
			// too much, just move the eyes
			tmp2 = 0;
		}

		command(1) = direction1*olSpeed(1);
		command(2) = direction2*olSpeed(2);
				
		// recompute command considering nsteps and how much the neck is going to move
		command(3) = (s(3)-tmp2)/(max*0.04);
		command(4) = (s(4)-tmp1)/(max*0.04);
		command(5) = (s(5)+tmp1)/(max*0.04);

		return max;
	}
	
	inline bool isOpenLoopDone()
	{
		return _openLoopDone;
	}

	inline bool isSaccadeDone()
	{
		return _saccadeDone;
	}

	void setPosition(const YVector &p)
	{
		_position = p;
		_gaze.update(_position);
	}

	const YVector &getError()
	{ return _retinal; }

	inline const YVector &closedLoopStep()
	{
		_closedLoop = 0.0;
		
		int predx, predy;
		int deltaX, deltaY;

		_closedLoopCounter++;
		
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _vRef, predx, predy);
		deltaX = predx-_xsize/2;
		deltaY = predy-_ysize/2;

		_retinal(1) = deltaX;
		_retinal(2) = deltaY;

		double error;
		error = sqrt(deltaX*deltaX + deltaY*deltaY);

		if (_closedLoopCounter > (_closedLoopSteps-1) ||
			(error<_closedLoopError) )
			_saccadeDone = true;

		_smooth.apply(deltaX, deltaY, _eyesClosedLoop);

		const YVector &neck = _neckControl.apply(_position);

		_closedLoop = _eyesClosedLoop + neck;

		_closedLoop(3) = _eyesClosedLoop(3) - neck(2);
		_closedLoop(4) = _eyesClosedLoop(4) - neck(1);
		_closedLoop(5) = _eyesClosedLoop(5) + neck(1);

		///////// DUMP
		/*
		_log.dump(_position);		// position
		_log.dump(_closedLoop);		// command
		_log.dump(deltaX);
		_log.dump(deltaY);
		_log.newLine();
		*/
		////////////////////////////

	//	cout << "CL_FLAG\n";
		return _closedLoop;
	}

	inline const YVector &openLoopStep()
	{
		// modify _openLoopDone
		int predx, predy;
		int deltaX, deltaY;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _vRef, predx, predy);
		deltaX = predx-_xsize/2;
		deltaY = predy-_ysize/2;

		_retinal(1) = deltaX;
		_retinal(2) = deltaY;

		_openLoopCounter++;
		if (_openLoopCounter>(_openLoopSteps-1))
			_openLoopDone = true;

		///////// DUMP
		/*
		_log.dump(_position);		// position
		_log.dump(_command);		// command
		_log.dump(deltaX);
		_log.dump(deltaY);
		_log.newLine();*/
		////////////////////////////

	//	cout << "OL_FLAG\n";

		return _command;
	}

	inline const YVector &stopStep()
	{
		_log.flush();
		return _stop;
	}

	void getPosition(int *x, int *y)
	{
		*x = _x;
		*y = _y;
	}

};

#endif