#ifndef __saccadecontrolhh__
#define __saccadecontrolhh__

// saccades
// May 2004, by nat

#include <yarp/YARPMatrix.h>
#include <yarp/YARPBabybotHeadKin.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPLogFile.h>
#include "SmoothControl.h"

using namespace _logpolarParams;

// closed loop control of the neck (copied from smoothcontrol)
// this is used by SaccadeControl class to simulate neck motion within the computeNew loop 
class NeckControl
{
public:
	NeckControl(const YARPString &iniFile, int insize, int outsize);
		
	~NeckControl()
	{ delete [] _pids; }

	inline const YVector & apply(const YVector &in)
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
		// _gaze ( YMatrix (_dh_nrf, __nj, DH_left[0]), YMatrix (_dh_nrf, __nj, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
		_neckControl("headcontrol.ini",5,5),
		_smooth("headcontrol.ini")
		{
			init("headcontrol.ini", "head.ini");
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
	// this stores a vector of zeros and it is returned by getError()
	// when the the saccade is done
	YVector _zeroError;
	int _x;
	int _y;

	int _nj;

	// read parameters from file and call _allocVectors()
	void init(const YARPString &filename, const YARPString &headfilename);
	// perform allocation of vectors and general initialization
	void _allocVectors();

public:
	~SaccadeControl(){}

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
	{ 
		if (_saccadeDone)
			return _zeroError;
		else 
			return _retinal; 
	}

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
//		_log.flush();
		return _stop;
	}

	void getPosition(int *x, int *y)
	{
		*x = _x;
		*y = _y;
	}

};

#endif