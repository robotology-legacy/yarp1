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
_directCmdPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_inertialPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_positionPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	char *root = GetYarpRoot();
	char path[256];

	// register ports
	_directCmdPort.Register("/headcontrol/direct/i");
	_vorPort.Register("/headcontrol/vor/i");

	_inertialPort.Register("/headcontrol/inertial/o");
	_positionPort.Register("/headcontrol/position/o");

	
	#if defined(__WIN32__)
		ACE_OS::sprintf (path, "%s\\conf\\babybot\\\0", root); 
	#elif defined (__QNX6__)
		ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
	#endif
	
	_fsm = new HeadFSM(&_head);
	strcpy(_iniFile, ini_file);
	strcpy(_path, path);
	_head.initialize(_path, _iniFile);

	_vor = new VorControl(_head.nj(), 3, 2);
	_inertial.Resize(3);
	_deltaQ.Resize(_head.nj());
	_vorCmd.Resize(_head.nj());
		
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
}

HeadThread::~HeadThread()
{
	delete _fsm;
	delete _vor;

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
	
	// set accelerations

	/* calibrateInertial(); */
}

void HeadThread::doRelease()
{
/*	// stop the head
	head_status.current_command = 0;
	send_commands();
	write_status();

	// park head
	*/
	park(2);
}

void HeadThread::doLoop()
{
	// read data from MEI board
	read_status();
	
	// gaze
//	_fsm->doYourDuty();

	// printf("%lf\n", _inertial(1));
	// _command = _vor->handle(_inertial);
	
	_deltaQ = _vorCmd;

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

/*
inline void HeadThread::read_status(){
	// MEI
	pHeadControl->Input();
	pInertialSensor->Input();

	pInertialSensor->Ready(&head_status.adcready);

	if (head_status.adcready)
	{
		// inertial sensor is calibrated.
		pInertialSensor->GetSignal (3, head_status.inertial.data() + 2);		
		pInertialSensor->GetSignal (2, head_status.inertial.data() + 1);
		pInertialSensor->GetSignal (1, head_status.inertial.data());
	}
	
	// store old position
	head_status.old_position = head_status.current_position;
	pHeadControl2->GetPosition2 (_head::_nj, head_status.current_position.data());

	p_segmentation1->get(target1);
	p_segmentation2->get(target2);
	// LATER: add joint speed estimation ?
}
*/
/*
inline void HeadThread::write_status(){
	// still not clear what to do...

	// transmit head_status data 
	p_head_sender->set(head_status);
}*/

void HeadThread::calibrateInertial()
{
/*
	int step = 0;
	int flag = 0;
	int nSteps;
	int tmp;
	
	pInertialSensor->GetCalibrationSteps(&nSteps);

	cout << "Wait while the head is being calibrated:\n";
	while (flag == 0)
	{
			tmp = int (step/double(nSteps)*100 + 0.5);

			cout << tmp << "%"<< '\r';
			
			pInertialSensor->Input();
			pInertialSensor->Ready(&flag);

			step++;
			Sleep(30);
	}
	
	cout << "\ndone!\n";
	*/
}