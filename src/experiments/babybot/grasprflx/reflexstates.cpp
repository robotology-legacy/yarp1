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

bool GRBInit::input(YARPBottle *in, ReflexShared *d)
{
	return (d->checkPalmTouch());
}
