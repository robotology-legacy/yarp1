// ArmThread.cpp: implementation of the ArmThread class.
//
//////////////////////////////////////////////////////////////////////

#include "ArmThread.h"
#include "YARPConfigFile.h"

// #include "..\..\visualprocessing\parameters.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ArmThread::ArmThread(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
_tirednessControl(23000.0, 10000.0, rate, 0.5),
_arm_status(ini_file)
{
	strncpy(_iniFile, ini_file, 80);

	YARPConfigFile file;
	file.set("", ini_file);
	file.get("[GENERAL]", "Joints", &_nj, 1);
	file.get("[THREAD]", "TrajSteps", &_nSteps, 1);
	
	_trajectory.resize(_nj, _nSteps);
	_actual_command.Resize(_nj);

	/////////// Resting states
	YVector tmp1,tmp2;
	tmp1.Resize(_nj);
	tmp2.Resize(_nj);
	file.get("[THREAD]", "RestingWayPoint1", tmp1.data(), _nj);
	file.get("[THREAD]", "RestingFinal", tmp2.data(), _nj);
	ASRestingInit::instance()->setTrajectory(tmp1*degToRad, tmp2*degToRad);
	////////////////////////////////////////////////////////////////////////
	
	changeInitState(ASDirectCommand::instance());
	_arm_state = _init_state;
	_restingInhibited = false;
}

ArmThread::~ArmThread()
{

}

void ArmThread::doInit()
{
	// wait for power on
	_arm.initialize(_iniFile);

	_gravity1.load("Y:\\conf\\babybot\\gravity1.ini");
	_gravity2.load("Y:\\conf\\babybot\\gravity2.ini");
	_gravity3.load("Y:\\conf\\babybot\\gravity3.ini");

	std::cout << "Ok, setting PID mode\n";
	
	_arm.activatePID();
	park(1);
	_arm.resetEncoders();		// set this new position

	_arm._parameters._lowPIDs[0].KP = -5.0;
	_arm._parameters._lowPIDs[0].KD = -600.0;
	// _arm._parameters._lowPIDs[0].OFFSET = g[0];	//the first value for g has to be applied smoothly
	_arm.setGainsSmoothly(_arm._parameters._lowPIDs,200);
	_arm_status._pidStatus = _armThread::high;
	_arm.getPositions(_arm_status._current_position.data());
	
	_trajectory.setFinal(_arm_status._current_position.data(), _arm_status._current_position.data(), _nSteps);
}

void ArmThread::doRelease()
{
	// go to a safe position
	YVector wp;
	wp.Resize(6);
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

	_gravity1.save("Y:\\conf\\babybot\\gravity1.ini");
	_gravity2.save("Y:\\conf\\babybot\\gravity2.ini");
	_gravity3.save("Y:\\conf\\babybot\\gravity3.ini");
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
	// 
	double g[3];
	_gravity1.computeG(_arm_status._current_position, &g[0]);
	_gravity2.computeG(_arm_status._current_position, &g[1]);
	_gravity3.computeG(_arm_status._current_position, &g[2]);

	for(int i = 0; i < 3; i++)
		_arm.setG(i,g[i]);

	_arm.setCommands(_actual_command.data());
}

inline void ArmThread::read_status()
{
	/// get
	_arm.getPositions(_arm_status._current_position.data());
	_arm.getTorques(_arm_status._torques.data());
	_tirednessControl.add(_arm_status._torques(1));
}

void ArmThread::park(int index)
{
	// read from file
	YARPConfigFile cfg;
	cfg.set("", _iniFile);
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