#include "triggerbehavior.h"

const int N = 200;

TBSharedData::TBSharedData():
_headKinematics(YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) ),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_vergencePort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_targetTrackingPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_armPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_headPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
{
	_outPort.Register("/collect/behavior/o");
	_vergencePort.Register("/collect/vergence/i");
	_targetTrackingPort.Register("/collect/target/i");
	_armPort.Register("/collect/arm/i");
	_headPort.Register("/collect/head/i");
	
	_vergence.Resize(1);
	_vergence = 0.0;

	_target.Resize(2);
	_target = 0.0;

	_armFrame = 0;
	_vergenceFrame = 0;
	_targetFrame = 0;
	_headFrame = 0;

}

TBSharedData::~TBSharedData()
{

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
	// block here
	_armPort.Read();
	_arm = _armPort.Content()._current_position;
	_armFrame++;

	if (_headPort.Read(0))
	{
		_head = _headPort.Content();
		_headFrame++;
	}

	if (_targetTrackingPort.Read(0))
	{
		_target = _targetTrackingPort.Content();
		_targetFrame++;
	}
	if (_vergencePort.Read(0))
	{
		_vergence = _vergencePort.Content();	
		_vergenceFrame++;
	}

	_printStatus();
}

void TBWaitRead::handle(TBSharedData *d)
{
	d->read();	// blocking call
	_count++;

	if (_count%N == 0)
	{
		ACE_OS::printf("Wating for %s...\n", _message.c_str());
		_count = 0;
	}
}

bool TBIsTargetCentered::input(YARPBabyBottle *in, TBSharedData *d)
{
	return d->checkTarget();
}

bool TBSimpleInput::input(YARPBabyBottle *in, TBSharedData *d)
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

void TBOutput::output(TBSharedData *d)
{
	ACE_OS::printf("Dump current point\n");

	_file.dump(d->getPolar());
	_file.dump(d->_arm);
	_file.newLine();
	
	_file.flush();
}