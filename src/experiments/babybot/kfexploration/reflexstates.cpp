#include "reflexstates.h"

void EBOutputCommand::output (ExplorationShared *d)
{
	// send command
	ACE_OS::printf("Exploration: sending command %s\n", _signal.c_str());
	
	d->_data.writeVocab(_signal);;
	d->_data.writeYVector(_cmd);
	d->send();
}

void EBSimpleOutput::output (ExplorationShared *d)
{
	// send command
	d->_data.writeVocab(_signal);
	d->send();
}