#include "reachingBehavior.h"

const char *__nnetFile1 = "reaching.ini";

ABSharedData::ABSharedData():
_map(__nnetFile1),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_armPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_headPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
{
	_headPort.Register("/reaching/head/i");
	_armPort.Register("/reaching/arm/i");
	_outPort.Register("/reaching/behavior/o");
}

bool RBInputCommand::input(YARPBottle *in, ABSharedData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (key != tmpK)
		return false;

	in->moveOn();
	return true;
}

void RBLearnOutputCommand::output(ABSharedData *d)
{
	if (d->_headPort.Read(0))
	{
		if (d->_armPort.Read(0))
		{
			const YVector& head = d->_headPort.Content();
			const YVector& arm = d->_armPort.Content()._current_position;
			d->_map.learn(head, arm);
		}
		else
		{
			cout << "Check arm connection \n";
		}
	}
	else
	{
		cout << "Check head connection\n";
	}
}

void RBOutputCommand::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	if (d->_headPort.Read(0))
	{
		if (d->_armPort.Read(0))
		{
			const YVector& head = d->_headPort.Content();
			const YVector& arm = d->_armPort.Content()._current_position;
			d->_map.query(arm, head);

			const YVector& cmd = d->_map.prepareCmd();

			cout << "Preparing arm\n";

			_bottle.writeYVector(cmd);
			d->_outPort.Content() = _bottle;
			d->_outPort.Write(1);
		}
		else
		{
			cout << "Check arm connection\n";
		}
	}
	else
	{
		cout << "Check head connection\n";
	}
}

void RB2Output::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle1;
	d->_outPort.Write(1);

//	Sleep(500);

	d->_outPort.Content() = _bottle2;
	d->_outPort.Write(1);
}

void RBOutputReaching1::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	const YVector& cmd = d->_map.getCommand(0);

	_bottle.writeYVector(cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
	
}

void RBOutputReaching2::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	const YVector& cmd = d->_map.getCommand(1);

	_bottle.writeYVector(cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
	
}

void RBOutputReaching3::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	const YVector& cmd = d->_map.getCommand(2);

	_bottle.writeYVector(cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
	
}
void RBSimpleOutput::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(_key);

	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
}

void RBOutputBack::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	const YVector& cmd = d->_map.prepareCmd();

	_bottle.writeYVector(cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
	
}

void RBHandClose::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
}

void RBHandOpen::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
}