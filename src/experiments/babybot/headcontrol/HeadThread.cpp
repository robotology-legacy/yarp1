// HeadThread.cpp: implementation of the HeadThread class.
//
//////////////////////////////////////////////////////////////////////

//#include <>
#include "HeadThread.h"

#include <iostream>
#include <iomanip>

const int __inPortDelay = 5;
const int __outPortDelay = 100;		// 4 seconds

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HeadThread::HeadThread(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
_inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
// _directCmdPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_inertialPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_positionPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)
{
	char *root = GetYarpRoot();
	char path[256];

	ACE_OS::sprintf (path, "%s/%s", root, ConfigFilePath); 

	_iniFile = YARPString(ini_file);
	_path = YARPString(path);

	// register ports
	// _directCmdPort.Register("/headcontrol/direct/i");
	_inPort.Register("/headcontrol/i");

	_inertialPort.Register("/headcontrol/inertial/o");
	_positionPort.Register("/headcontrol/position/o");
	// _statusPort.Register("/headcontrol/status/o");

	_directCmdFlag = false;
		
	YARPConfigFile file;
	file.set(_path, _iniFile);
	_fsm = new HeadFSM(&_head);
	_head.initialize(_path, _iniFile);

	_inertial.Resize(3);
	_deltaQ.Resize(_head.nj());

	_deltaT = rate/1000.0;

	// FSM
	_hsDirectCmd.set(_head._directCmd);
	double *tmp = new double [3];
	YARPPidFilter *tmpPid = new YARPPidFilter [_head.nj()];
	int i;
	for(i = 0; i < _head.nj(); i++)
	{
		char k[80];
		sprintf(k, "Pid%d", i);
		file.get("[POSITIONCONTROL]", k, tmp, 3);
		tmpPid[i].setKs(tmp[0], tmp[1], tmp[2]);
	}
		
	_hsDirectCmd.setPIDs(_head.nj(), tmpPid);
	_hiDirectCmdStart.set(&_directCmdFlag);
	_hoDirectCmdEnd.set(&_directCmdFlag);
	_hiDirectCmdEnd.set(_head._directCmd);
	
	_fsm->setInitialState(&_hsTrack);
	_fsm->add(&_hiDirectCmdStart, &_hsTrack, &_hsDirectCmd);
	_fsm->add(&_hiDirectCmdEnd, &_hsDirectCmd, &_hsDirectCmdStop);
	_fsm->add(NULL, &_hsDirectCmdStop, &_hsTrack, &_hoDirectCmdEnd);

	// counters and flags
	_threadCounter = 0;
	_inPortCounter = __inPortDelay;

	_stopFlag = false;
	_askHib = false;;

	delete [] tmp;
	delete [] tmpPid;
}

HeadThread::~HeadThread()
{
	delete _fsm;
}

void HeadThread::absCalibrate()
{

	// this routine sends all joints, in turn, off to their hard limits;
	// gathers the extremum positions; and then sets the mean positions
	// and resets the system, so that the zero position is always the same.

	// gather admissible accelerations from cfg file
	YARPConfigFile cfg;
	cfg.set(_path, _iniFile);
	YVector cfg_vel(_head.nj()), cfg_acc(_head.nj());
	cfg.get("[THREAD]", "Accelerations", cfg_acc.data(), _head.nj());
	cfg.get("[THREAD]", "Speeds", cfg_vel.data(), _head.nj());

	std::cout << "Doing absolute calibration." << std::endl;

	YVector old_pos(_head.nj()), vel(_head.nj()), pos(_head.nj());
	YVector start_vel(_head.nj());
	YVector meanPosition(_head.nj());
	vel = 0.0; meanPosition = 0.0;

	for ( int i=1; i<=_head.nj(); ++i ) {

		// cannot calibrate the eyes - the motor slips.
		if ( i==4 || i==5 ) continue;
		
		std::cout << "Joint " << i << "... ";
		double lowerLimit, upperLimit;

		_head.getPositions(pos.data());
		lowerLimit = pos(i);

		// gather lower limit
		old_pos = 0.0;
		vel(i) = -35.0;
		_head.velocityMove(vel.data());
		while(true) {
			YARPTime::DelayInSeconds(.2);
			_head.getPositions(pos.data());
			if ( fabs(lowerLimit-pos(i))<1.0 ) continue;
//			std::cout << ".";
			if ( pos(i) == old_pos(i) ) {
				// stop the motion
				vel(i) = 0.0;
				_head.velocityMove(vel.data());
				break;
			}
			old_pos = pos;
		}
		lowerLimit = pos(i);
//		std::cout << "o";

		// gather upper limit
		old_pos = 0.0;
		vel(i) = 35.0;
		_head.velocityMove(vel.data());
		while(true) {
			YARPTime::DelayInSeconds(.2);
			_head.getPositions(pos.data());
			if ( fabs(lowerLimit-pos(i))<1.0 ) continue;
//			std::cout << ".";
			if ( pos(i) == old_pos(i) ) {
				vel(i) = 0.0;
				_head.velocityMove(vel.data());
				break;
			}
			old_pos = pos;
		}
		upperLimit = pos(i);
//		std::cout << "o";

		// evaluate mean position, wrt current zero
		meanPosition(i) = __min(upperLimit,lowerLimit)+fabs(upperLimit-lowerLimit)/2.0;
		
		// now send the joint to its mean position
		std::cout << ".done. Now going to newhome... ";
//		std::cout << "Now sending joint to " << meanPosition(i);
		vel(i) = -35.0;
		_head.setVelocities(vel.data());
		_head.setAccs(cfg_acc.data());
		_head.setPositions(meanPosition.data());
		while(!_head.checkMotionDone()) {
//			std::cout << ".";
			ACE_OS::sleep(ACE_Time_Value(0,200000));
		}
		std::cout << "done." << std::endl;
		vel = 0.0;
	}

	std::cout << "Done." << std::endl << std::endl;
//	std::cout << "mean positions: " << meanPosition << std::endl;

}

void HeadThread::doInit()
{
	if (!_askHib)
	{
		// start head
		_head.idleMode();
		_head._status._pidStatus = 0;

		std::cout << "--> Head is idle. Turn it on then press any key.\n";
		std::cout.flush(); char c; std::cin >> c;
	
		_head.activatePID();
		_head._status._pidStatus = 1;
		absCalibrate();
		_head.activatePID();

		park(1);
		_head.calibrate();
	}
	else
	{
		// resume from hibernated state
		_head.idleMode();
		HEAD_THREAD_DEBUG(("resuming from hibernation\n"));
		_head.activateLowPID(false); // don't reset encoders
		_head._status._pidStatus = 0;
		_head.setGainsSmoothly(_head._parameters._highPIDs, 200);
		_head._status._pidStatus = 1;

		park(1);	// go to starting position
	}

	_askHib = false;	// reset hibernation state

	// reset counters
	_threadCounter = 0;
	_inPortCounter = __inPortDelay;
}

void HeadThread::doRelease()
{
	// stop the head
	_deltaQ = 0.0;
	_head.velocityMove(_deltaQ.data());
	_head.waitForMotionDone(50000);	// poll with sleep(50ms)

	if (!_askHib)
	{
		park(2);			// park the head
		_head.idleMode();	// disable ampli
	}
	else
	{
		// hibernate code
		// do we need to park the head ?
		HEAD_THREAD_DEBUG(("starting hibernation procedure\n"));
		_head.setGainsSmoothly(_head._parameters._lowPIDs,200);
		_head._status._pidStatus = 0;	
		_head.idleMode();	// disable ampli
		HEAD_THREAD_DEBUG(("it is now safe to turn off the amplifiers\n"));
	}
}

void HeadThread::doLoop()
{
	///////////////// READ
	// read data from MEI board
	/// get head
	_head.getPositions(_head._status._current_position.data());
	_head._status._current_position *= degToRad;

	_head.getVelocities(_head._status._velocity.data());
	_head._status._velocity /= 10;		// normalize (hell, WHY!!!!)

	_head.readAnalogs(_inertial.data());

//	std::cout << "inertial: "
//			  << _inertial(1) << " "
//			  << _inertial(2) << " "
//			  << _inertial(3) << "             \r";

	// poll input port
	if (_inPort.Read(0))
	{
		_head._inCmd = _inPort.Content();
		_inPortCounter = 0;
	}
	/////////////////////////////////////////////

	/////////// check _inPort delay
	if (_inPortCounter>=__inPortDelay)
	{
	//	HEAD_THREAD_DEBUG(("#%u Input port timed out !\n", _threadCounter));
		_stopFlag = true;
	}
	else
		_stopFlag = false;
	/////////////////////////////////////
	
	// this is the position control; it writes on _head._directCmd
	_fsm->doYourDuty();

	/////////// check stop flag
	if (_directCmdFlag || _stopFlag)
		_deltaQ = _head._directCmd;
	else
		_deltaQ = _head._inCmd;
	/////////////

	////// CHECK LIMITS
	_head._predictedPos = _head._status._current_position + _deltaT*_deltaQ;
	// check limits
	_head.checkLimits(_head._predictedPos.data(), _deltaQ.data());
	
	//////// SEND STATUS
	// send inertial info
	_inertialPort.Content() = _inertial;
	_inertialPort.Write();

	_positionPort.Content() = _head._status._current_position;
	_positionPort.Write();
	
	// SET VELOCITY
	// wait a few control loop, in order to let the position propagate
	if (_threadCounter > __outPortDelay)
	{
		_deltaQ *= radToDeg;
		_head.safeVelocityMove(_deltaQ.data());
	}

	// increase counters
	_threadCounter++;
	_inPortCounter++;
}

void HeadThread::park(int flag)
{
	// read from file
	YARPConfigFile cfg;
	cfg.set(_path, _iniFile);
	int nj = _head.nj();
	char tmp[80];
	YVector pos(nj);
	YVector vel(nj);
	YVector acc(nj);
	sprintf(tmp, "Park%d", flag);		// use flag to retrieve different parking positions
	cfg.get("[THREAD]", tmp, pos.data(), nj);
	cfg.get("[THREAD]", "Speeds", vel.data(), nj);
	cfg.get("[THREAD]", "Accelerations", acc.data(), nj);

//	pos = pos * degToRad;
//	vel = vel * degToRad;
//	acc = acc * degToRad;

	_head.setVelocities(vel.data());
	_head.setAccs(acc.data());
	_head.setPositions(pos.data());

	std::cout << "Wait while the head is parking";
	// wait !
	while(!_head.checkMotionDone())
	{
		std::cout << ".";
		ACE_OS::sleep(ACE_Time_Value(0,200000));
	}

	std::cout << "done ! \n";
}
