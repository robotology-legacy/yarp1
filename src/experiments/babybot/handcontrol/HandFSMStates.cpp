#include "HandFSMStates.h"
#include "HandThread.h"

void InitMoveState::handle(HandSharedData *t)
{
	HAND_THREAD_DEBUG(("Init move\n"));
	
	t->setPositions(_cmd.data());
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
	HAND_THREAD_DEBUG(("End Motion\n"));
	t->_data.writeVocab(YBVHandDone);
	t->send();
	HAND_THREAD_DEBUG(("Sent Hand Done\n"));
}