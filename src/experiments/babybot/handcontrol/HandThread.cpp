// HandThread implementation file
#include "HandThread.h"

HandThread::HandThread(int rate, const char *name, const char *cfgF):
YARPRateThread(name, rate),
_handStatusOut(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	_cfgFile = cfgF;

	_fsm = new HandFSM(&_hand);

	int n = 10;
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
	_fsm->add(NULL, &_endMotion, &_waitState);

	////////////////////////
	YVector tmp1(6),tmp2(6);
	tmp1(1) = 0.0;
	tmp1(2) = 0.0;
	tmp1(3) = 1500;
	tmp1(4) = 1500;
	tmp1(5) = -1500;
	tmp1(6) = -1500;
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
	_status.Resize(12);

}

HandThread::~HandThread()
{
	delete _fsm;
	delete [] _shakeMove;
	delete [] _shakeWait;
}

void HandThread::doInit()
{
	_hand.initialize(_cfgFile);
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

	_hand.getPositionsRaw(_status.data());
	_hand.getSpeedsRaw(_status.data()+6);

	_handStatusOut.Content() = _status;
	_handStatusOut.Write();
	
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
	_startShake.set();
}