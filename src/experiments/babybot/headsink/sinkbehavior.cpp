#include "sinkbehavior.h"

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
	d->inhibitChannel(SinkChVor);
}

void SBOutputInhibitTracker::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit/enable TRACKER channel\n");
	d->inhibitChannel(SinkChTracker);
}

void SBOutputInhibitArm::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit/enable ARM channel\n");
	d->inhibitChannel(SinkChArm);
}

void SBOutputInhibitVergence::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit/enable VERGENCE channel\n");
	d->inhibitChannel(SinkChVergence);
}

void SBOutputDisplayStatus::output(Sink *d)
{
	d->printChannelsStatus();;
}