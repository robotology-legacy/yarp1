#include "sinkbehavior.h"
#include "sinkconstants.h"

void SBWaitIdle::handle(Sink *d)
{
	ACE_OS::printf("SinkBehavior: waiting on %s\n", _message.c_str());
}

bool SBSimpleInput::input(YARPBabyBottle *in, Sink *d)
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

void SBOutputSuppress::output(Sink *d)
{
	ACE_OS::printf("Suppress head\n");
	d->suppressHead();
}

void SBOutputRelease::output(Sink *d)
{
	ACE_OS::printf("Release head\n");
	d->releaseHead();
}

void SBOutputInhibitTracker::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit TRACKER channel\n");
	d->inhibitChannel(SINK_INHIBIT_SMOOTH);
}

void SBOutputInhibitVergence::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit VERGENCE channel\n");
	d->inhibitChannel(SINK_INHIBIT_VERGENCE);
}

void SBOutputInhibitVor::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit VOR channel\n");
	d->inhibitChannel(SINK_INHIBIT_VOR);
}

void SBOutputInhibitSaccade::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: inhibit SACCADE channel\n");
	d->inhibitChannel(SINK_INHIBIT_SACCADES);
}

void SBOutputEnableTracker::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: enable TRACKER channel\n");
	d->enableChannel(SINK_INHIBIT_SMOOTH);
}

void SBOutputEnableVergence::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: enable VERGENCE channel\n");
	d->enableChannel(SINK_INHIBIT_VERGENCE);
}

void SBOutputEnableVor::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: enable VOR channel\n");
	d->enableChannel(SINK_INHIBIT_VOR);
}

void SBOutputEnableSaccade::output(Sink *d)
{
	ACE_OS::printf("SinkBehavior: enable SACCADE channel\n");
	d->enableChannel(SINK_INHIBIT_SACCADES);
}

void SBOutputDisplayStatus::output(Sink *d)
{
	d->printChannelsStatus();;
}