#include "HandFSMStates.h"
#include "HandThread.h"

void InitMoveState::handle(HandSharedData *t)
{
	HAND_THREAD_DEBUG(("Init move\n"));
	
	t->setPositionsRaw(_cmd.data());
}

void IdleState::handle(HandSharedData *t)
{
/*	// signal idle state
	int msg[2];
	msg[0] = 0;	// hand
	msg[1] = 0;	// idle
	memcpy(t->_behaviorsPort.Content(), msg, sizeof(msg));
	t->_behaviorsPort.Write();
	*/
}

void EndMotion::handle(HandSharedData *t)
{
	t->_data.writeVocab(YBVHandDone);
	t->send();
}