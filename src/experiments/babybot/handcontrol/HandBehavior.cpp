#include "HandBehavior.h"
#include <iostream>
using namespace std;

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

bool HBInputCommand::input(YARPBabyBottle *in, HandBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (tmpK != key)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(d->_cmd))
		return false;

	in->moveOn();
	return true;
}

bool HBInputReset::input(YARPBabyBottle *in, HandBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (tmpK != key)
		return false;

	in->moveOn();

	// reset encoders
	d->_hand.resetEncoders();

	return true;
}

bool HBShakeCmdInput::input(YARPBabyBottle *in, HandBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (tmpK != key)
		return false;

	in->moveOn();

	return true;
}

bool HBCheckMotionDone::input(YARPBabyBottle *in, HandBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;

	if (tmpK != key)
		return false;
	
	in->moveOn();

	return true;
}