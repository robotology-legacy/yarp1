// HeadThread.cpp: implementation of the HeadThread class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadThread.h"

#include <iostream>
#include <iomanip>

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
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
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

	_count = 0;

	_stopFlag = false;

	delete [] tmp;
	delete [] tmpPid;
}

HeadThread::~HeadThread()
{
	delete _fsm;
}

void HeadThread::doInit()
{
	// calibration 
	_head.idleMode();
	_head._status._pidStatus = 0;
	std::cout << "--> Head is idle. Calibrate it manually then press any key + return\n";
	char c;
	std::cin >> c;
	std::cout << "Ok, setting velocity mode\n";
	
	_head.activatePID();
	_head._status._pidStatus = 1;

	park(1);

	_head.calibrate();
	_count = 0;
}

void HeadThread::doRelease()
{
	// park head
	park(2);

	// be sure to stop the head, in principle this is not needed.
	_head.idleMode();
}

void HeadThread::doLoop()
{
	// read data from MEI board
	read_status();

	_count++;
	
	// this is the position control
	_fsm->doYourDuty();

	if (_directCmdFlag || _stopFlag)
		_deltaQ = _head._directCmd;
	else
		_deltaQ = _head._inCmd;
	/////////////
	// vergence
	write_status();
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
	sprintf(tmp, "Park%d", flag);
	cfg.get("[THREAD]", tmp, pos.data(), nj);
	cfg.get("[THREAD]", "Speeds", vel.data(), nj);
	cfg.get("[THREAD]", "Accelerations", acc.data(), nj);

	pos = pos * degToRad;
	vel = vel * degToRad;
	acc = acc * degToRad;

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
