// ArmThread.cpp: implementation of the ArmThread class.
//
//////////////////////////////////////////////////////////////////////

#include "Armthread.h"
#include <yarp/YARPConfigFile.h>

// #include "..\..\visualprocessing\parameters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ArmThread::ArmThread(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
YARPBehaviorSharedData("/armcontrol/behavior/o", YBVMotorLabel),
_tirednessControl(23000.0, 10000.0, rate, 0.5),
_wristPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_armStatusPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)
{
	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/%s/\0",root, ConfigFilePath);
	
	strncpy(_iniFile, ini_file, 80);
	strncpy(_path, path, 256);
	_arm_status.resize(path, ini_file);

	YARPConfigFile file;
	file.set(_path, ini_file);
	file.get("[GENERAL]", "Joints", &_nj, 1);
	file.get("[THREAD]", "TrajSteps", &_nSteps, 1);
	
	_trajectory.resize(_nj, _nSteps);
	_actual_command.Resize(_nj);
	//////// speed and accelerations (used by stiff pid only)
	_speed.Resize(_nj);
	_acc.Resize(_nj);
	_parkSpeed.Resize(_nj);
	_speed = 0.0;	
	_acc = 0.0;
	_parkSpeed = 0.0;
	
	file.get("[THREAD]", "Speeds", _speed.data(), _nj);
	file.get("[THREAD]", "ParkSpeeds", _parkSpeed.data(), _nj);
	file.get("[THREAD]", "Accelerations", _acc.data(), _nj);
	_parkSpeed = _parkSpeed * degToRad;
	_speed = _speed * degToRad;
	_acc = _acc*degToRad;
	////////////////////////////////////////////////////////////

	// LIMITS
	_limitsMax.Resize(_nj);
	_limitsMin.Resize(_nj);
	_limitsMax = 0.0;
	_limitsMin = 0.0;

	file.get("[LIMITS]", "Max", _limitsMax.data(), _nj);
	file.get("[LIMITS]", "Min", _limitsMin.data(), _nj);
	_limitsMax = _limitsMax*degToRad;
	_limitsMin = _limitsMin*degToRad;
	////////////////////////////////////////////////////////////
	
	_cmd.Resize(_nj);
	_shakeCmd.Resize(_nj);
	_shakeCmd = 0.0;
	_cmd = 0.0;

	/////////// Resting states
	YVector tmp1,tmp2;
	tmp1.Resize(_nj);
	tmp2.Resize(_nj);
	file.get("[THREAD]", "RestingWayPoint1", tmp1.data(), _nj);
	file.get("[THREAD]", "RestingFinal", tmp2.data(), _nj);
	ASRestingInit::instance()->setTrajectory(tmp1*degToRad, tmp2*degToRad);
	////////////////////////////////////////////////////////////////////////
	/////////// Wrist
	char wristPortName[255];
	file.getString("[WRIST]", "PortName", wristPortName);
	_wristPort.Register(wristPortName);
	_wristF.Resize(6);
	_wristF = 0.0;
	///////////////////

	// GRAVITY
	int learn;
	file.get("[GRAVITY]", "Learn", &learn);
	ASDirectCommandMove::instance()->_learn = learn;
	_gravityFlags = new int[_nj];
	memset(_gravityFlags, 0, _nj*sizeof(_gravityFlags[0]));
	file.get("[GRAVITY]", "Enable", _gravityFlags, _nj);
	ACE_OS::printf("Gravity vector is: ");
	for(int i = 0; i < _nj; i++)
		ACE_OS::printf("%d\t", _gravityFlags[i]);
	ACE_OS::printf("\n");
	_gravityTerms.Resize(_nj);
	_gravityTerms = 0.0;

	// PORT
	char armStatusPortname[255];
	file.getString("[THREAD]", "ArmStatusPortName", armStatusPortname);
	_armStatusPort.Register(armStatusPortname, "Net0");
	///////////////////////
	
	changeInitState(ASDirectCommand::instance());
	_arm_state = _init_state;
	_restingInhibited = false;
	_shaking = false;

}

ArmThread::~ArmThread()
{/*
	printf("--> ArmThread: Entering ArmThread destructor\n");
	printf("--> ArmThread: Unregistering output behavior port\n\n\n");
	YARPBehaviorSharedData::_outPort.Unregister();
	printf("--> ArmThread: Output behavior port unregistered\n\n\n");*/
}

void ArmThread::resetEncoders(const double *p)
{
	_arm.resetEncoders(p);
}

void ArmThread::doInit()
{
	// wait for power on
	_arm.initialize(_path, _iniFile);

	_gravity1.load(_path, "gravity1.ini");
	_gravity2.load(_path, "gravity2.ini");
	_gravity3.load(_path, "gravity3.ini");

	std::cout << "Ok, setting PID mode\n";
	
	_arm.activatePID();
	park(1);
	_arm.resetEncoders();		// set this new position

	_arm.setVelocities(_speed.data());
	_arm.setAccs(_acc.data());

	_arm.setGainsSmoothly(_arm._parameters._lowPIDs, 200);
	_arm_status._pidStatus = 1;
	_arm.getPositions(_arm_status._current_position.data());

	_trajectory.setFinalSpecSpeed(_arm_status._current_position.data(), _arm_status._current_position.data(), _speed.data());
}

void ArmThread::doRelease()
{
	// go to a safe position
	YVector wp;
	wp.Resize(_nj);
	wp = 0.0;
	_directCommand(wp);
	
	while(!checkMotionDone())
	{
		send_commands();
		std::cout << ".";
		ACE_OS::sleep(period);
	}
	/////////////////////////

	_arm.setGainsSmoothly(_arm._parameters._highPIDs,250);
	// park arm
	park(2);
	_arm.uninitialize();
/*
	char *root = GetYarpRoot();
    char path[256];
    char filename[256];

	ACE_OS::sprintf (path, "%s/%s/", root, ConfigFilePath);
    ACE_OS::sprintf (filename, "%s/%s/gravity1.ini", root, ConfigFilePath);
    _gravity1.save(filename);
    ACE_OS::sprintf (filename, "%s/%s/gravity2.ini", root, ConfigFilePath);
    _gravity2.save(filename);
    ACE_OS::sprintf (filename, "%s/%s/gravity3.ini", ConfigFilePath);
    _gravity3.save(filename);
	*/

	//_gravity1.save("Y:\\conf\\babybot\\gravity1.ini");
	//_gravity2.save("Y:\\conf\\babybot\\gravity2.ini");
	//_gravity3.save("Y:\\conf\\babybot\\gravity3.ini");
}

void ArmThread::doLoop()
{
	// read data from MEI board
	read_status();
	
	// check tiredness
	if (_tirednessControl.high() && !_restingInhibited)
	{
		ARM_THREAD_DEBUG(("Arm is tired... I've got to rest for a while!\n"));
		if (_arm_status.isPIDHigh())
			_arm_state = ASRestingInit::instance();
		else
		{
			_arm_state = ASZeroGEnd::instance();
		}
	}
		
	/////////////
	_arm_state->handle(this);
	
	// send commands to MEI board
	send_commands();
}

inline void ArmThread::send_commands()
{
	_trajectory.getNext(_actual_command.data());

	double *g = _gravityTerms.data();
	_gravity1.computeG(_arm_status._current_position, &g[0]);
	_gravity2.computeG(_arm_status._current_position, &g[1]);
	_gravity3.computeG(_arm_status._current_position, &g[2]);

	// check if gravity is enable
	for(int j = 0; j < 0; j++) {
		if (!_gravityFlags[j])
			g[j] = 0.0;
	}

	_arm.setGs(g);

	_armStatusPort.Content() = _arm_status;
	_armStatusPort.Write();
	
	_arm.setCommands(_actual_command.data());
}

inline void ArmThread::read_status()
{
	/// get arm
	_arm.getPositions(_arm_status._current_position.data());
	_arm.getVelocities(_arm_status._velocity.data());
	_arm_status._velocity*=radToDeg/10;		//normalize
	_arm.getTorques(_arm_status._torques.data());
	///
	_tirednessControl.add(_arm_status._torques(1));
	// get from wrist
	if (_wristPort.Read(0))
		_wristF = _wristPort.Content();
}

void ArmThread::park(int index)
{
	// read from file
	YARPConfigFile cfg;
	cfg.set(_path, _iniFile);
	int nj = _arm_status._nj;
	char tmp[80];
	double *pos,*vel,*acc;
	pos = new double [nj];
	vel = new double [nj];
	acc = new double [nj];
	sprintf(tmp, "Park%d", index);
	cfg.get("[THREAD]", tmp, pos, nj);

	for(int i = 0; i<nj; i++)
	{
		vel[i] = 10.0 * degToRad;
		acc[i] = 50.0 * degToRad;
		pos[i] = pos[i] *degToRad;
	}

	_arm.setVelocities(vel);
	_arm.setAccs(acc);
	_arm.setPositionsAll(pos);
	
	std::cout << "Wait while the arm is parking";
	// wait !
	while(!_arm.checkMotionDone())
	{
		std::cout << ".";
		ACE_OS::sleep(ACE_Time_Value(0,200000));
	}

	std::cout << "done ! \n";

	delete [] pos;
	delete [] acc;
	delete [] vel;
}

void ArmThread::_directCommand(const YVector &cmd, int nst)
{
	YVector tmpCmd;
	tmpCmd = cmd;
	if (_checkLimits(cmd, tmpCmd))
	{
		// limits were applied, send a message
		ARM_THREAD_DEBUG(("Sorry, the desired command was out of limits:\n"));
		int j = 0;
		for(j = 1; j <= _nj; j++)
			ACE_OS::printf("%.1lf\t", cmd(j)*radToDeg);

		ARM_THREAD_DEBUG(("Sending:\n"));
		for(j = 1; j <= _nj; j++)
			ACE_OS::printf("%.1lf\t", tmpCmd(j)*radToDeg);
		
		ACE_OS::printf("instead\n");
	}
		
	_trajectory.stop();
	if (nst == 0)
		_trajectory.setFinalSpecSpeed(tmpCmd.data(), _speed.data());	// use default
	else 
		_trajectory.setFinalSpecSpeed(tmpCmd.data(), _speed.data());
	
	_arm.setPositions(tmpCmd.data());

	YARPBehaviorSharedData::_data.writeVocab(YBVArmIssuedCmd);
	YARPBehaviorSharedData::_data.writeYVector(tmpCmd);
	YARPBehaviorSharedData::send();
}

bool ArmThread::_checkLimits(const YVector &inCmd, YVector &outCmd)
{
	int j = 1;
	bool ret = false;
	for(j = 1; j <= _nj; j++)
	{
		// check max
		if (inCmd(j) > _limitsMax(j))
		{
			outCmd(j) = _limitsMax(j);
			ret = ret || true;
		}
		else
			outCmd(j) = inCmd(j);

		// check min
		if (inCmd(j) < _limitsMin(j))
		{
			outCmd(j) = _limitsMin(j);
			ret = ret || true;
		}
		else
			outCmd(j) = inCmd(j);
	}

	return ret;
}