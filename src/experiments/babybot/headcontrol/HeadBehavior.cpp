#include "HeadBehavior.h"

void HBWaitIdle::handle(HeadThread *d)
{
	printf("HEAD_BEHAVIOR: waiting on %s\n", _message.c_str());
}

bool HBSimpleInput::input(YARPBottle *in, HeadThread *d)
{
	if (!in->tryReadVocab(newK))
		return false;
	
	if (newK != key)
		return false;

	in->moveOn();

	return true;
}

void HBHibernateCommand::output(HeadThread *d)
{
	d->askHibernation();
	d->terminate(false);	// terminate the thread
}

void HBResumeCommand::output(HeadThread *d)
{
	d->start();				// start the thread
}

bool HBDirectCommandInput::input(YARPBottle *in, HeadThread *d)
{
	if (!in->tryReadVocab(newK))
		return false;
	
	if (newK != key)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(_cmd))
		return false;

	in->moveOn();

	d->directCommand(_cmd);
	return true;
}