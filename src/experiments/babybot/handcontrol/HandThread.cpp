// HandThread implementation file
#include "HandThread.h"

HandThread::HandThread(int rate, const char *name, const char *cfgF):
YARPRateThread(name, rate)
{
	_cfgFile = cfgF;

	_fsm = new HandFSM(&_hand);

	_fsm->setInitialState(&_waitState);
	// shake behavior
	_fsm->add(&_startShake, &_waitState, &_shake1);
	_fsm->add(NULL, &_shake1, &_shake2);
	_fsm->add(&_wait1, &_shake2, &_shake3);
	_fsm->add(NULL, &_shake3, &_shake4);
	_fsm->add(&_wait1, &_shake4, &_endMotion);
	_fsm->add(NULL, &_endMotion, &_waitState);

	YVector tmp(6);
	tmp(1) = 0.0;
	tmp(2) = 0.0;
	tmp(3) = 1500;
	tmp(4) = 1500;
	tmp(5) = -1500;
	tmp(6) = -1500;
	_shake1.setCmd(tmp);
	tmp = 0.0;
	_shake3.setCmd(tmp);
	_wait1.setNSteps(35);
	///////
}

HandThread::~HandThread()
{
	delete _fsm;
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

	_hand.output();
}
	
void HandThread::doRelease()
{
	_hand.uninitialize();
}