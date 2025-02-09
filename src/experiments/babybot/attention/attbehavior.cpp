#include "attbehavior.h"

AttSharedData::AttSharedData():
_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_targetPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_handPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_handPredictionPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_egoMapPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
{
	_out.Register("/attention/o");

	_egoMapPort.Register("/attention/egomap/i");
	_targetPort.Register("/attention/target/i");
	_handPort.Register("/attention/hand/i");
	_handPredictionPort.Register("/attention/handprediction/i");
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

void AttBHandPrediction::handle(AttSharedData *d)
{
	d->_handPredictionPort.Read();
	d->_out.Content() = d->_handPredictionPort.Content();
	d->_out.Write();
}

void AttBEgoMap::handle(AttSharedData *d)
{
	d->_egoMapPort.Read();
	d->_out.Content() = d->_egoMapPort.Content();
	d->_out.Write();
}

bool AttBSimpleInput::input(YARPBabyBottle *in, AttSharedData *d)
{
	if (!in->tryReadVocab(newK))
		return false;
	
	if (newK != key)
		return false;

	in->moveOn();

	return true;
}