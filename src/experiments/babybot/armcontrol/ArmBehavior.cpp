#include "ArmBehavior.h"

void ABOutputCommand::output(ArmBehaviorData *d)
{
	// direct command here
	cout << "Executing direct command !\n";
	d->directCommand(d->_cmd);
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

bool ABCheckMotionDone::input(YARPBottle *in, ArmBehaviorData *d)
{
	int k;
	if (!in->tryReadVocab(&k))
		return false;

	if (k != key)
		return false;
	
	in->moveOn();

	return true;
}