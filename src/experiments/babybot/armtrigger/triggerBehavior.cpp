#include "triggerbehavior.h"

const int N = 200;

TBSharedData::TBSharedData():
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_egoMapPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_vergencePort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_handTrackingPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_targetTrackingPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
{
	_egoMapPort.Register("/trigger/egomap/o");
	_outPort.Register("/trigger/behavior/o");
	_vergencePort.Register("/trigger/vergence/i");
	_handTrackingPort.Register("/trigger/hand/i");
	_targetTrackingPort.Register("/trigger/target/i");

	_vergence.Resize(1);
	_vergence = 0.0;

	_target.Resize(2);
	_target = 0.0;

	_hand.Resize(2);
	_hand = 0.0;
}

TBSharedData::~TBSharedData()
{

}

bool TBSharedData::checkHand()
{
	bool flag = true;

	//ACE_OS::printf("%lf\t%lf\t%lf\n", _vergence(1), _hand(1), _hand(2));

	flag = flag && _checkVergence(_vergence);
	flag = flag && _checkTrack(_hand);

	//ACE_OS::printf("check hand returned: %d\n", flag);
	return flag;
}

bool TBSharedData::checkTarget()
{
	bool flag = true;

	// ACE_OS::printf("%lf\t%lf\t%lf\n", _vergence(1), _target(1), _target(2));

	flag = flag && _checkVergence(_vergence);
	flag = flag && _checkTrack(_target);

	// ACE_OS::printf("check target returned: %d\n", flag);
	return flag;
}

void TBSharedData::read()
{
//	_handTrackingPort.Read();
	_targetTrackingPort.Read();
	_vergencePort.Read();

	_target = _targetTrackingPort.Content();
	_vergence = _vergencePort.Content();
//	_hand = _handTrackingPort.Content();
}

void TBWaitRead::handle(TBSharedData *d)
{
	ACE_OS::printf("Entering wait for %s...\n", _message.c_str());
	d->read();	// blocking call
	_count++;

	if (_count%N == 0)
	{
		ACE_OS::printf("Wating for %s...\n", _message.c_str());
		_count = 0;
	}
}

bool TBIsTargetCentered::input(YARPBottle *in, TBSharedData *d)
{
	return d->checkTarget();
}

bool TBIsHandCentered::input(YARPBottle *in, TBSharedData *d)
{
	return d->checkHand();
}

bool TBSimpleInput::input(YARPBottle *in, TBSharedData *d)
{
	if (!in->tryReadVocab(tmpKey))
		return false;
	
	if (key != tmpKey)
		return false;

	in->moveOn();
	return true;
}

void TBOutputCommand::output(TBSharedData *d)
{
	ACE_OS::printf("Sending:%s\n", _cmd.c_str());
	_bottle.setID(YBVMotorLabel);
	_bottle.reset();
	_bottle.writeVocab(_cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
}

void TBOutputStoreEgoMap::output(TBSharedData *d)
{
	// ACE_OS::printf("Remove previous point\n");
	// _bottle.reset();
	// _bottle.writeVocab(YBVEgoMapRemove);
	// _bottle.writeText(_name.c_str());

	// d->_egoMapPort.Content() = _bottle;
	// d->_egoMapPort.Write(1);
	
	ACE_OS::printf("Add current point\n");
	_bottle.reset();
	_bottle.writeVocab(YBVEgoMapAdd);
	_bottle.writeText(_name.c_str());
	_bottle.writeInt(0);
	_bottle.writeInt(0);

	d->_egoMapPort.Content() = _bottle;
	d->_egoMapPort.Write(1);

	ACE_OS::printf("Set as current\n");
	_bottle.reset();
	_bottle.writeVocab(YBVEgoMapSetCurrent);
	_bottle.writeText(_name.c_str());
	d->_egoMapPort.Content() = _bottle;
	d->_egoMapPort.Write(1);
}