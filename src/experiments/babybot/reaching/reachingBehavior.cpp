#include "reachingBehavior.h"

const char *__reachingNNet = "reaching.ini";
const char *__handlocNNet = "handfk1.ini";
const char *__ellipseNNet = "handfk2.ini";

using namespace std;

ABSharedData::ABSharedData():
_map(__reachingNNet, __handlocNNet,__ellipseNNet),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_armPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_headPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
{
	_headPort.Register("/reaching/head/i");
	_armPort.Register("/reaching/arm/i");
	_outPort.Register("/reaching/behavior/o");
}

bool RBInputCommand::input(YARPBabyBottle *in, ABSharedData *d)
{
	if (!in->tryReadVocab(tmpK))
		return false;
	
	if (key != tmpK)
		return false;

	in->moveOn();
	return true;
}

bool RBInputString::input(YARPBabyBottle *in, ABSharedData *d)
{
	char tmp[255];
	printf("Checking input string\n");

	if (!in->tryReadText(tmp))
	{
		printf("not a string\n");
		return false;
	}

	printf("Received %s\n", tmp);

	tmpK = YARPString(tmp);
	
	if (key != tmpK)
	{
		printf("Returning false\n");
		return false;
	}

	in->moveOn();
	printf("Returning true\n");
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
	// read (poll ports)
	if (d->_headPort.Read(0))
	{
		if (d->_armPort.Read(0))
		{
			const YVector& head = d->_headPort.Content();
			const YVector& arm = d->_armPort.Content()._current_position;
			
			if (d->_map.query(arm, head))
			{
				cout << "Target can be reached !\n";
				const YVector& cmd = d->_map.prepareCmd();
				_sendReachingCommand(d, cmd);
				return;
			}
		}
	}

	cout << "Target is out of reach (or head/arm not connected)... aborting!\n";
	_sendAbort(d);
}

void RBOutputCommand::_sendAbort(ABSharedData *d)
{
	_bottle1.reset();
	_bottle1.setID(YBVMotorLabel);
	_bottle1.writeVocab(YBVReachingAbort);
	d->_outPort.Content() = _bottle1;
	d->_outPort.Write(1);
}

void RBOutputCommand::_sendReachingCommand(ABSharedData *d, const YVector &cmd)
{
	cout << "Reaching has started\n";
	/////// 
	d->_outPort.Content() = _bottle3;
	d->_outPort.Write(1);

	cout << "Preparing arm\n";
	_bottle1.reset();
	_bottle1.setID(YBVMotorLabel);
	_bottle1.writeVocab(YBVArmForceNewCmd);
	_bottle1.writeYVector(cmd);
	d->_outPort.Content() = _bottle1;
	d->_outPort.Write(1);

	Sleep(2000);

	cout << "Opening hand\n";
	/////// 
	d->_outPort.Content() = _bottle2;
	d->_outPort.Write(1);
}

void RB2Output::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle1;
	d->_outPort.Write(1);

//	Sleep(500);

	d->_outPort.Content() = _bottle2;
	d->_outPort.Write(1);
}

void RBOutputMessage::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
}

void RB4Output::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle4;
	d->_outPort.Write(1);

	d->_outPort.Content() = _bottle3;
	d->_outPort.Write(1);

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

	const YVector& cmd = d->_map.getCommand(2);

	_bottle.writeYVector(cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
	
}

void RBOutputReaching3::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	const YVector& cmd = d->_map.getCommand(3);

	_bottle.writeYVector(cmd);
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
	
}

void RBOutputReachingCL::output(ABSharedData *d)
{
	_bottle.reset();
	_bottle.setID(YBVMotorLabel);
	_bottle.writeVocab(YBVArmNewCmd);

	const YVector& cmd = d->_map.getCommand(1);

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

void RBMotorCommand::output(ABSharedData *d)
{
	d->_outPort.Content() = _bottle;
	d->_outPort.Write(1);
}