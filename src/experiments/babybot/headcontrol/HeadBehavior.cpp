#include "Headbehavior.h"

void HBWaitIdle::handle(HeadThread *d)
{
	printf("HeadBehavior: waiting on %s\n", _message.c_str());
}

bool HBSimpleInput::input(YARPBottle *in, HeadThread *d)
{
	return false;
}

void HBOutputCommand::output(HeadThread *d)
{
	return;
}

bool HBDirectCommandInput::input(YARPBottle *in, HeadThread *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;
	
	if (k != key)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(_cmd))
		return false;

	in->moveOn();

	d->directCommand(_cmd);
	return true;
}