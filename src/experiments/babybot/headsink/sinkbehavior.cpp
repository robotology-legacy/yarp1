#include "sinkbehavior.h"

void SBWaitIdle::handle(Sink *d)
{
	printf("SinkBehavior: waiting on %s\n", _message.c_str());
}

void SBOutputInhibitAll::output(Sink *d)
{
	d->inhibitAll();
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