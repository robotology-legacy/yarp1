#include "HandBehavior.h"

void HBOutputCommand::output(HandBehaviorData *d)
{
	// direct command here
	cout << "Executing direct command !\n";
	d->singleMove(d->_cmd, 35);
}

void HBShakeCmdOutput::output(HandBehaviorData *d)
{
	// direct command here
	cout << "Executing shake command !\n";
	d->shake();
}

bool HBInputCommand::input(YARPBottle *in, HandBehaviorData *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;
	
	if (k != key)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(d->_cmd))
		return false;

	in->moveOn();
	return true;
}

bool HBInputReset::input(YARPBottle *in, HandBehaviorData *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;
	
	if (k != key)
		return false;

	in->moveOn();

	// reset encoders
	d->_hand.resetEncoders();

	return true;
}

bool HBShakeCmdInput::input(YARPBottle *in, HandBehaviorData *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;
	
	if (k != key)
		return false;

	in->moveOn();

	return true;
}

bool HBCheckMotionDone::input(YARPBottle *in, HandBehaviorData *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;

	if (k != key)
		return false;
	
	in->moveOn();

	return true;
}