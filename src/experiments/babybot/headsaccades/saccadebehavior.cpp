#include "saccadebehavior.h"
#include "../headsink/sinkconstants.h"

#include <YARPTime.h>

bool SBCheckSaccadeDone::input(SBSharedData *d)
{
	return d->_saccade.isSaccadeDone();
}

bool SBCheckOpenLoopDone::input(SBSharedData *d)
{
	return d->_saccade.isOpenLoopDone();
}

void SBWaitIdle::handle(SBSharedData *d)
{
	if (_dt == 0)
		ACE_OS::printf("State:%s\n", _message.c_str());
	else
	{
		ACE_OS::printf("Waiting %lf sec\n", _dt);
		YARPTime::DelayInSeconds(_dt);
	}
}

bool SBWaitFixation::input (SBSharedData *d)
{
	d->_inFixation.Read();

	_steps++;

	// check boredom
	if (_steps > 300)
	{
		ACE_OS::printf("bored of waiting\n");
		_steps = 0;
		return true;
	}
	
	if (d->_inFixation.Content()(1) == 1)
	{
		_steps = 0;
		return true;
	}
	else
		return false;
}

bool SBNewTarget::input(SBSharedData *d)
{
	if (d->newTarget())
		return true;
	else
		return false;
}

bool SBManualInitSaccade::input (SBSharedData *d)
{
	YBVocab tmp;
	ACE_OS::printf("Entering input state\n");
	d->_inBehavior.Read();
	ACE_OS::printf("done\n");

	if (!d->_inBehavior.Content().tryReadVocab(tmp))
		return false;
	
	if (tmp != YBVSaccadeSetTarget)
		return false;
	
	d->_in.Read();
	d->_saccade.setPosition(d->_in.Content());
	int x;
	int y;
	d->getTarget(x, y);
	d->_saccade.computeNew(x,y);
	
	return true;
}

void SBInitSaccade::handle(SBSharedData *d)
{
	d->_in.Read();
	d->_saccade.setPosition(d->_in.Content());
	int x;
	int y;
	d->getTarget(x, y);
	d->_saccade.computeNew(x,y);
}

void SBOpenLoop::handle(SBSharedData *d)
{
	d->_in.Read();
	d->_saccade.setPosition(d->_in.Content());
	d->_out.Content().reset();
	d->_out.Content().writeYVector(d->_saccade.openLoopStep());
	d->_out.Content().writeInt(SINK_INHIBIT_SMOOTH|SINK_INHIBIT_VERGENCE|SINK_INHIBIT_VOR|SINK_INHIBIT_NECK);
	d->_out.Write();

	//
	d->_outFixation.Content() = d->_saccade.getError();
	d->_outFixation.Write();
}

void SBClosedLoop::handle(SBSharedData *d)
{
	d->_in.Read();
	d->_saccade.setPosition(d->_in.Content());
	d->_out.Content().reset();
	d->_out.Content().writeYVector(d->_saccade.closedLoopStep());
	d->_out.Content().writeInt(SINK_INHIBIT_SMOOTH|SINK_INHIBIT_NECK|SINK_INHIBIT_VOR);
	d->_out.Write();

	// 
	d->_outFixation.Content() = d->_saccade.getError();
	d->_outFixation.Write();
}

void SBStop:: handle(SBSharedData *d)
{
	d->_in.Read();
	d->_saccade.setPosition(d->_in.Content());

	ACE_OS::printf("State:%s\n", _message.c_str());

	d->_out.Content().reset();
	d->_out.Content().writeYVector(d->_saccade.stopStep());
	d->_out.Content().writeInt(SINK_INHIBIT_NONE);

	d->_out.Write();

	// signal the fixation program we have done (error should be zero, now)
	d->_outFixation.Content() = d->_saccade.getError();
	d->_outFixation.Write();

	// send update to the attentional system
	d->updateIORTable();
}

void SBSimpleOutput::output(SBSharedData *d)
{
	/*
	int x;
	int y;

	d->_saccade.getPosition(&x, &y);

	YARPBottle bottle;
	bottle.writeVocab(YBVEgoMapAdd);
	bottle.writeText("Saccades");
	bottle.writeInt(x);	// LATER
	bottle.writeInt(y);

	d->_outEgoMap.Content() = bottle;
	d->_outEgoMap.Write(0);
	ACE_OS::printf("Waiting half a seconds\n");
	Sleep(500);	// LATER REMOVE IT!
	*/
}
