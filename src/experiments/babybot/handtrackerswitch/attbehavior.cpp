#include "attbehavior.h"

AttSharedData::AttSharedData():
_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_predictionPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_positionPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
{
	_out.Register("/hts/o");

	// _predictionPort.Register("/hts/prediction/i");
	// _positionPort.Register("/hts/position/i");
}

AttSharedData::~AttSharedData()
{
	// dtor
}

void AttBWait::handle(AttSharedData *d)
{
	ACE_OS::printf("Waiting on %s\n", _msg.c_str());
}

void AttBPollCurrent::handle(AttSharedData *d)
{
	// poll
//	d->_positionPort.Read();
}

void AttBPollPrediction::handle(AttSharedData *d)
{
	// poll
//	d->_predictionPort.Read();
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

void AttBSimpleOutput::output(AttSharedData *d)
{
	d->_out.Content() = _bottle;
	d->_out.Write(1);
}