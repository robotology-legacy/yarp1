#include "reflexstates.h"

GRBOutputCommand::GRBOutputCommand(const YVector &c)
{
	cmd = c;
}

void GRBOutputCommand::output (ReflexShared *d)
{
	// send command
	d->_data.writeVocab(YBVHandNewCmd);//writeAndSend(cmd);
	d->_data.writeYVector(cmd);
	d->send();
}

bool GRBInit::input(YARPBottle *in, ReflexShared *d)
{
	return (d->checkPalmTouch());
}
