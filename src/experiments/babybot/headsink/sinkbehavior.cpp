#include "sinkbehavior.h"
#include "sinkconstants.h"

void SBWaitIdle::handle(Sink *d)
{
	ACE_OS::printf("SinkBehavior: waiting on %s\n", _message.c_str());
}

bool SBSimpleInput::input(YARPBottle *in, Sink *d)
{
	if (!in->tryReadVocab(newK))
		return false;
	
	if (newK != key)
		return false;

	in->moveOn();

	return true;
}

void SBOutputInhibitAll::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit ALL channels\n");
	d->inhibitAll();
}

void SBOutputEnableAll::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: enable ALL channels\n");
	d->enableAll();
}

void SBOutputInhibitVor::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit/enable VOR channel\n");
	d->inhibitChannel(SINK_INHIBIT_VOR);
}

void SBOutputInhibitTracker::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit/enable TRACKER channel\n");
	d->inhibitChannel(SINK_INHIBIT_SMOOTH);
}

void SBOutputInhibitVergence::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit/enable VERGENCE channel\n");
	d->inhibitChannel(SINK_INHIBIT_VERGENCE);
}

void SBOutputDisplayStatus::output(Sink *d)
{
	d->printChannelsStatus();;
}