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
_vorPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
// _directCmdPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_inertialPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
// _positionPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	char *root = GetYarpRoot();
	char path[256];

	// register ports
	// _directCmdPort.Register("/headcontrol/direct/i");
	_vorPort.Register("/headcontrol/vor/i");

	_inertialPort.Register("/headcontrol/inertial/o");
	// _positionPort.Register("/headcontrol/position/o");

	_directCmdFlag = false;

	
	#if defined(__WIN32__)
		ACE_OS::sprintf (path, "%s\\conf\\babybot\\\0", root); 
	#elif defined (__QNX6__)
		ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
	#endif
	
	_fsm = new HeadFSM(&_head);
	strcpy(_iniFile, ini_file);
	strcpy(_path, path);
	_head.initialize(_path, _iniFile);

	_inertial.Resize(3);
	_deltaQ.Resize(_head.nj());
		
	// now we can create controls
	/*

	p_control_vergence = new Vergence(&head_status, PIDFilter(0.009));
	p_control_saccade = new SaccadeBehavior(&head_status);
	p_control_saccade->load();

	p_avoid = new Avoidance(&head_status); 
	
	p_control_smooth = new GazeShift(&head_status, PIDFilter(0.02),		//eye pan pid
												 PIDFilter(0.02),		//eye tilt pid
												 PIDFilter(0.6),		//neck pan pid
												 PIDFilter(1.0));		//neck tilt pid
											
	p_vor = new Inertial(&head_status, 1.0);
													 
	// transmission
	p_head_sender = new MulticastSender(2,
										head_status,
										__head_thread_rate,
										"head status sender");
	
	p_segmentation1 = new MulticastReceiver(2,
										  target1,
										  0, //__seg_tr_rate,
										  "color sementation receiver 1");

										  
	p_segmentation1->start(seg1_addr);
	
	p_segmentation2 = new MulticastReceiver(2,
										  target2,
										  0, //__seg_tr_rate,
										  "color sementation receiver 2");

										  
	p_segmentation2->start(seg2_addr);

	// initial state
	_init_state = HStateSmooth::Instance();
	_head_state = _init_state;
	*/

	// FSM
	_hsDirectCmd.set(_head._directCmd);
	double tmp[] = {-10.0, -8.0, -10.0, -15.0, -15.0};
	_hsDirectCmd.setPIDs(_head.nj(), tmp);
	_hiDirectCmdStart.set(&_directCmdFlag);
	_hoDirectCmdEnd.set(&_directCmdFlag);
	_hiDirectCmdEnd.set(_head._directCmd);
	
	_fsm->setInitialState(&_hsTrack);
	_fsm->add(&_hiDirectCmdStart, &_hsTrack, &_hsDirectCmd);
	_fsm->add(&_hiDirectCmdEnd, &_hsDirectCmd, &_hsDirectCmdStop);
	_fsm->add(NULL, &_hsDirectCmdStop, &_hsTrack, &_hoDirectCmdEnd);
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
	
	// gaze
	_fsm->doYourDuty();

	// printf("%lf\n", _inertial(1));
	// _command = _vor->handle(_inertial);
	_deltaQ = _head._vorCmd + _head._directCmd;
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
