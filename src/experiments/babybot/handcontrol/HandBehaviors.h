#ifndef __HAND_BEHAVEIORS__
#define __HAND_BEHAVEIORS__

#include <YARPBehavior.h>
#include "HandThread.h"

class HandBehavior: public YARPBehavior<HandBehavior, HandThread>
{
	public:
		HandBehavior(HandThread *s): YARPBehavior<HandBehavior, HandThread>(s){};
};

typedef YARPFSMStateBase<HandBehavior, HandThread> HandBehaviorBaseStates;

typedef YARPBehaviorMsgHandler<HandBehavior, HandThread> HandMsgHandler;


void sigFunction(HandBehavior *fsm, HandBehaviorBaseStates *s)
{
	fsm->pulse(s);
};

class MoveState: public HandBehaviorBaseStates
{
	public:
		void handle(HandThread *t)
		{
			t->_startShake.set();
		}
};

class WaitState: public HandBehaviorBaseStates
{
	public:
		void handle(HandThread*t)
		{
			printf("Wait... !\n");
		}
};

#endif
