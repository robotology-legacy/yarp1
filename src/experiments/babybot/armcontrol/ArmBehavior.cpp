#include "ArmBehavior.h"

void ABOutputCommand::output(ArmBehaviorData *d)
{
	// direct command here
	d->directCommand(d->_cmd);
}

void ABForceOutputCommand::output(ArmBehaviorData *d)
{
	// direct command here
	d->directCommand(d->_cmd, true);
}

void ABOutputShakeCmd::output(ArmBehaviorData *d)
{
	d->shake(d->_shakeCmd);
}

void ABForceResting:: output(ArmBehaviorData *d)
{
	d->forceResting();
}

void ABForceRestingTrue:: output(ArmBehaviorData *d)
{
	d->forceResting(true);
}

void ABForceRestingFalse:: output(ArmBehaviorData *d)
{
	d->forceResting(false);
}

void ABInhibitResting:: output(ArmBehaviorData *d)
{
	d->inhibitResting();
}

void ABInhibitRestingTrue:: output(ArmBehaviorData *d)
{
	d->inhibitResting(true);
}

void ABInhibitRestingFalse:: output(ArmBehaviorData *d)
{
	d->inhibitResting(false);
}

void ABOutputHibernate::output(ArmBehaviorData *d)
{
	d->terminate(true);	// no timeout
	ACE_OS::printf("ARM_BEHAVIOR: it is now safe to turn off the amplifier\n");
}

void ABOutputResume::output(ArmBehaviorData *d)
{
	d->start();	// blocking
}

void ABSimpleOutput::output(ArmBehaviorData *d)
{
	d->_data.writeVocab(vocab);
	d->send();
}

bool ABInputCommand::input(YARPBabyBottle *in, ArmBehaviorData *d)
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

bool ABInputShakeCommand::input(YARPBabyBottle *in, ArmBehaviorData *d)
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

bool ABSimpleInput::input(YARPBabyBottle *in, ArmBehaviorData *d)
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

bool ABInputSetStiffness::input(YARPBabyBottle *in, ArmBehaviorData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (key != tmpK)
		return false;

	in->moveOn();

	if (!in->readInt(&d->_newGainJoint))
		return false;
	
	if (!in->readFloat(&d->_newGain))
		return false;

	return true;
}

void ABSetStiffnessState::handle(ArmBehaviorData *d)
{
	d->setStiffness(d->_newGainJoint, d->_newGain);
}

void ABOutputPrintPids::output(ArmBehaviorData *d)
{
	d->printPids();
}