// HandThread implementation file
#include "HandThread.h"

HandThread::HandThread(int rate, const char *name, const char *cfgF):
YARPRateThread(name, rate),
_handStatusOut(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_handForceOut(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/conf/babybot/\0", root); 
	
	_cfgFile = YARPString(cfgF);
	_path = YARPString(path);
		

	_fsm = new HandFSM(&_hand);

	int n = 16;
	_shakeMove = new InitMoveState[n];
	_shakeWait = new IdleState[n];

	_fsm->setInitialState(&_waitState);
	// shake behavior
	int i;
	_fsm->add(&_startShake, &_waitState, &_shakeMove[0]);
	for(i = 0; i<=(n-2);)
	{
		_fsm->add(NULL, &_shakeMove[i], &_shakeWait[i]);
		_fsm->add(&_wait1, &_shakeWait[i], &_shakeMove[i+1]);
		i++;
	}
	_fsm->add(NULL, &_shakeMove[n-1], &_shakeWait[n-1]);
	_fsm->add(&_wait1, &_shakeWait[n-1], &_endMotion);
	_fsm->add(NULL, &_endMotion, &_waitState, &_endShakeOutput);
	
	////////////////////////
	YVector tmp1(6),tmp2(6);
	tmp1(1) = 2000;
	tmp1(2) = 0.0;
	tmp1(3) = 1500;
	tmp1(4) = 0;
	tmp1(5) = -1500;
	tmp1(6) = 0;
	tmp2 = 0.0;
	for(i = 0; i<=(n-1);)
	{
		_shakeMove[i].setCmd(tmp1);
		i++;
		_shakeMove[i].setCmd(tmp2);
		i++;
	}
	_wait1.setNSteps(10);
	///////

	// single movement behavior
	_fsm->add(&_startMove, &_waitState, &_move1State);
	_fsm->add(NULL, &_move1State, &_move2State);
	_fsm->add(&_waitMove, &_move2State, &_endMotionState);
	_fsm->add(NULL, &_endMotionState, &_waitState);

	///////////////
	_cmd.Resize(6);
	_cmd = 0.0;
	/////////////
	_handStatusOut.Register("/handcontrol/o:status");
	_handForceOut.Register("/handcontrol/o:force");
	_posture.Resize(16);
	_motorJoints.Resize(6);
	_hallEncodersRaw.Resize(15);
	_hallDeltaTheta.Resize(15);
	
	///////
	for(i = 0; i < 15; i++)
		_hallPoly[i].setParameters(__hallParameters[i]);
	
	_hand._shaking = false;
}

HandThread::~HandThread()
{
	delete _fsm;
	delete [] _shakeMove;
	delete [] _shakeWait;
}

void HandThread::doInit()
{
	_hand.initialize(_path, _cfgFile);
	_hand.idleMode();
	_hand.resetEncoders();

	cout << "Turn on the hand and press any key...\n";
	int c = getc(stdin);
	_hand.activatePID();
}

void HandThread::doLoop()
{
	_hand.input();

	_fsm->doYourDuty();

	_hand.getPositions(_motorJoints.data());
	_hand.getHallRaw(_hallEncodersRaw.data());
	
	_computeHandPosture();

	_handStatusOut.Content() = _posture;
	_handStatusOut.Write();

	_handForceOut.Content() = _hallDeltaTheta;
	_handForceOut.Write();

	_hand.output();
}
	
void HandThread::doRelease()
{
	_hand.uninitialize();
}

void HandThread::singleMove(const YVector &pos, int time)
{
	if (time<0)
		time = 1;

	_waitMove.setNSteps(time);
	_move1State.setCmd(pos);
	_startMove.set();
}

void HandThread::shake()
{
	_hand._shaking = true;
	_startShake.set();
}

void HandThread::_computeHandPosture()
{
	// convert hall raw measurements into angles
	int i = 0;
	for (i = 0; i < 15; i++)
		_hallDeltaTheta(i+1) = _hallPoly[i](_hallEncodersRaw(i+1));
//	_hallDeltaTheta = 0.0;

	// thumb
	// CHECK OFFSETS for the thumb
	_posture(1) = _motorJoints(1) + __offsets[0];
	_posture(2) = _motorJoints(2) - _hallDeltaTheta(1) + __offsets[1];
	_posture(3) = -_posture(2) - _hallDeltaTheta(2) + __offsets[2];
	_posture(4) = _posture(3) - _hallDeltaTheta(3) + __offsets[3];

	// index
	_posture(5) = _motorJoints(3) + _hallDeltaTheta(4) + __offsets[4];
	_posture(6) = _motorJoints(4) - _hallDeltaTheta(4) + _hallDeltaTheta(5) + __offsets[5];
	_posture(7) = 0.5*_posture(6) + _hallDeltaTheta(6) + __offsets[6];

	// medium
	_posture(8) = _motorJoints(5) + _hallDeltaTheta(7) + __offsets[7];
	_posture(9) = _motorJoints(6) - _hallDeltaTheta(7) + _hallDeltaTheta(8) + __offsets[8];
	_posture(10) = 0.5*_posture(9) + _hallDeltaTheta(9) + __offsets[9];

	// ring
	_posture(11) = _motorJoints(5) + _hallDeltaTheta(10) + __offsets[10];
	_posture(12) = _motorJoints(6) - _hallDeltaTheta(10) + _hallDeltaTheta(11) + __offsets[11];
	_posture(13) = 0.5*_posture(12) + _hallDeltaTheta(12) + __offsets[12];

	// small finger
	_posture(14) = _motorJoints(5) + _hallDeltaTheta(13) + __offsets[13];
	_posture(15) = _motorJoints(6) - _hallDeltaTheta(13) + _hallDeltaTheta(14) + __offsets[14];
	_posture(16) = 0.5*_posture(15) + _hallDeltaTheta(15) + __offsets[15];
}