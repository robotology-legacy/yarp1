#include "reflexstates.h"

void GRBOpenOutputCommand::output (ReflexShared *d)
{
	ACE_OS::printf("GRASPREFLEX: send open cmd\n");
	// send command
	cmd = d->getOpen();

	d->_data.writeVocab(YBVHandNewCmd);
	d->_data.writeYVector(cmd);
	d->send();
}

void GRBCloseOutputCommand::output (ReflexShared *d)
{
	// send command
	ACE_OS::printf("GRASPREFLEX: send close cmd\n");
	cmd = d->getClose();

	d->_data.writeVocab(YBVHandNewCmd);
	d->_data.writeYVector(cmd);
	d->send();
}

void GRBOutputSignal::output (ReflexShared *d)
{
	// send command
	ACE_OS::printf("GRASPREFLEX: sending signal %s\n", _signal.c_str());
	
	d->_data.writeVocab(_signal);;
	d->send();
}

bool GRBInit::input(YARPBabyBottle *in, ReflexShared *d)
{
	return (d->checkPalmTouch());
}
