#include "attbehavior.h"

AttSharedData::AttSharedData():
_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_targetPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_handPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
{
	_out.Register("/attention/o");

	_targetPort.Register("/attention/target/i");
	_handPort.Register("/attention/hand/i");
}

AttSharedData::~AttSharedData()
{
	// dtor
}

void AttBHand::handle(AttSharedData *d)
{
	d->_handPort.Read();
	d->_out.Content() = d->_handPort.Content();
	d->_out.Write();
}

void AttBTarget::handle(AttSharedData *d)
{
	d->_targetPort.Read();
	d->_out.Content() = d->_targetPort.Content();
	d->_out.Write();
}

bool AttBSimpleInput::input(YARPBottle *in, AttSharedData *d)
{
	if (!in->tryReadVocab(newK))
		return false;
	
	if (newK != key)
		return false;

	in->moveOn();

	return true;
}