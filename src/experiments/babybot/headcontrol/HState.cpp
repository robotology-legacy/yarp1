#define __NO_COMPILE__
#ifndef __NO_COMPILE__

#include "HeadThread.h"
#include "HState.h"

// initialize static instances...
State1					State1::_instance;
State2					State2::_instance;
State3					State3::_instance;
State4					State4::_instance;
HStateSmooth			HStateSmooth::_instance;
HStateSaccadeInit		HStateSaccadeInit::_instance;
HStateSaccade			HStateSaccade::_instance;
HStateSmoothNoSaccade	HStateSmoothNoSaccade::_instance;

void HState::changeState (HeadThread *t, HState *s)
{
	t->changeHeadState(s);
}

void State1:: handle(HeadThread *t)
{
	t->head_status.flags.state = 1;

	if (t->target1.valid && !t->target1.is_in_fovea())
	{
		deltaQ = t->p_control_smooth->handle(t->target1);
	}
	else 
	{
		count++;
		deltaQ = t->p_control_smooth->handle(t->target1);
		if (count > _head::_wait_time)
		{
			changeState(t, State1::Instance());		
			count = 0;
		}
	}

	t->head_status.current_command += deltaQ;
}

void State2:: handle(HeadThread* t)
{
	t->head_status.flags.state = 2;
	
	if (t->target2.valid && /*!t->target2.is_in_fovea()*/!t->head_status.is_still())
	{
		deltaQ = t->p_control_smooth->handle(t->target2);
	}
	else if (!t->target2.valid)
	{
		changeState(t, State1::Instance());		
	}
	else
	{
		count++;
		deltaQ = t->p_control_smooth->handle(t->target2);
		if (count > _head::_wait_time)
		{
			changeState(t, State3::Instance());		
			count = 0;
		}
	}
	
	t->head_status.current_command += deltaQ;
	
}

void State3:: handle(HeadThread* t)
{
	t->head_status.flags.state = 3;
	
	if (t->target1.valid && !t->target1.is_in_fovea())
	{
		deltaQ = t->p_control_smooth->handle(t->target1);
	}
	else if (!t->target1.valid)
	{
		changeState(t, State1::Instance());		
		deltaQ = 0.7*deltaQ;
	}
	else
	{
		count++;
		count++;
		deltaQ = t->p_control_smooth->handle(t->target1);
		if (count > _head::_wait_time)
		{
			changeState(t, State4::Instance());		
			count = 0;
		}
	}
	
	t->head_status.current_command += deltaQ;
}
	
void State4:: handle(HeadThread *t)
{
	// keep fixating target1, and perform a pushing 
	t->head_status.flags.state = 4;
	
	cout << t->head_status.time << "\t: State" << 4 << '\n';

	// for now just wait until the target is out of fovea, then start again ...
	
	if (t->target1.is_in_fovea())
	{
		deltaQ = t->p_control_smooth->handle(t->target1);
	}
	else
	{
		changeState(t, State1::Instance());
		deltaQ = 0.7*deltaQ;
	}
	
	t->head_status.current_command += deltaQ;
}

void HStateSmooth:: handle(HeadThread *t)
{
	HEAD_STATE_DEBUG("HStateSmooth\n");

	t->head_status.flags.state = _head::smooth;
	
	if (t->target1.valid && t->target1.is_in_fovea())
	{
		// keep smooth control
		deltaQ = t->p_control_smooth->handle(t->target1);
		changeState(t, HStateSmooth::Instance());
	}
	else if (t->target1.valid && !t->target1.is_in_fovea())
	{
		// init saccade
		deltaQ = t->p_control_saccade->init(t->target1);

		// add smooth control
		deltaQ += t->p_control_smooth->handle(t->target1);

		changeState(t, HStateSaccade::Instance());
		
	}
	else
	{
		// don't change state
		deltaQ = 0.7*deltaQ;
	}

	t->head_status.current_command += deltaQ;
}

void HStateSaccadeInit:: handle(HeadThread *t)
{
	HEAD_STATE_DEBUG("HStateSaccadeInit\n");

	// really useless !! REMOVE IT !!!

	t->head_status.flags.state = _head::saccade_init;
		
	t->head_status.current_command += deltaQ;
}

void HStateSaccade:: handle(HeadThread *t)
{
	HEAD_STATE_DEBUG("HStateSaccade\n");

	t->head_status.flags.state = _head::saccade;
	count++;

	// init saccade
	deltaQ = t->p_control_saccade->handle();
	// add smooth control
	deltaQ += t->p_control_smooth->handle(t->target1);

	if (count >= n_steps)
	{
		count = 0;
		changeState(t, HStateSmoothNoSaccade::Instance());
	}
	
	t->head_status.current_command += deltaQ;
}

void HStateSmoothNoSaccade:: handle(HeadThread *t)
{
	HEAD_STATE_DEBUG("HStateSmoothNoSaccade\n");

	t->head_status.flags.state = _head::smooth_no_saccade;
	count++;

	// smooth control
	if (t->target1.valid)
		deltaQ = t->p_control_smooth->handle(t->target1);
	else
		deltaQ = 0.0;
	
	if (count >= n_steps)
	{
		count = 0;
		changeState(t, HStateSmooth::Instance());
	}
	
	t->head_status.current_command += deltaQ;
}

#endif