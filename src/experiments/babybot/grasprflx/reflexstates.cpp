#include "reflexstates.h"

void GRBOpenOutputCommand::output (ReflexShared *d)
{
	// send command
	cmd = d->getOpen();

	d->_data.writeVocab(YBVHandNewCmd);//writeAndSend(cmd);
	d->_data.writeYVector(cmd);
	d->send();
}

void GRBCloseOutputCommand::output (ReflexShared *d)
{
	// send command
	cmd = d->getClose();

	d->_data.writeVocab(YBVHandNewCmd);//writeAndSend(cmd);
	d->_data.writeYVector(cmd);
	d->send();
}

bool GRBInit::input(YARPBottle *in, ReflexShared *d)
{
	return (d->checkPalmTouch());
}
