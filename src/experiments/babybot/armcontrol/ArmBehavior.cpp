#include "ArmBehavior.h"

void ABOutputCommand::output(ArmBehaviorData *d)
{
	// direct command here
	d->directCommand(d->_cmd);
}

void ABOutputShakeCmd::output(ArmBehaviorData *d)
{
	d->shake(d->_shakeCmd);
}

void ABForceResting:: output(ArmBehaviorData *d)
{
	d->forceResting();
}

void ABInhibitResting:: output(ArmBehaviorData *d)
{
	d->inhibitResting();
}

bool ABInputCommand::input(YARPBottle *in, ArmBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (key != tmpK)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(d->_cmd))
		return false;

	in->moveOn();
	return true;
			
}

bool ABInputShakeCommand::input(YARPBottle *in, ArmBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (key != tmpK)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(d->_shakeCmd))
		return false;

	in->moveOn();
	return true;
			
}

bool ABSimpleInput::input(YARPBottle *in, ArmBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (key != tmpK)
		return false;

	in->moveOn();

	return true;
}

void ABStartZeroG::output(ArmBehaviorData *d)
{
	d->zeroGMode();
}

void ABStopZeroG::output(ArmBehaviorData *d)
{
	d->directCommandMode();
}