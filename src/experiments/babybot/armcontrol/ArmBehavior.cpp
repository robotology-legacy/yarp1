#include "ArmBehavior.h"

void ABOutputCommand::output(ArmBehaviorData *d)
{
	// direct command here
	cout << "Executing direct command !\n";
	d->directCommand(d->_cmd);
}

void ABOutputShakeCmd::output(ArmBehaviorData *d)
{
	cout << "Starting shaking sequence!\n";
	d->shake(d->_shakeCmd);
}

void ABForceResting:: output(ArmBehaviorData *d)
{
	cout << "Force resting!\n";
	d->forceResting();
}

void ABInhibitResting:: output(ArmBehaviorData *d)
{
	cout << "Resting inhibited!\n";
	d->inhibitResting();
}

bool ABInputCommand::input(YARPBottle *in, ArmBehaviorData *d)
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

bool ABInputShakeCommand::input(YARPBottle *in, ArmBehaviorData *d)
{
	int k;
	in->display();
	if (!in->tryReadVocab(&k))
		return false;
	
	if (k != key)
		return false;

	in->moveOn();

	if (!in->tryReadYVector(d->_shakeCmd))
		return false;

	in->moveOn();
	return true;
			
}

bool ABSimpleInput::input(YARPBottle *in, ArmBehaviorData *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;
	
	if (k != key)
		return false;

	in->moveOn();

	return true;
}