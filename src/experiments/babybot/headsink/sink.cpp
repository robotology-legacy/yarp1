#include "sink.h"

Sink::Sink(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_inPortVor(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_inPortTrack(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
{
	_nj = __nJoints;
	_inVor.Resize(_nj);
	_inTrack.Resize(_nj);
	_outCmd.Resize(_nj);

	_inVor = 0.0;
	_inTrack = 0.0;
	_outCmd = 0.0;

	YARPString base = __baseName;
	_outPort.Register(base.append("o").c_str());
	_inPortVor.Register(base.append("vor/i").c_str());
	_inPortTrack.Register(base.append("track/i").c_str());
}

Sink::~Sink()
{

}

void Sink::doInit()
{
	// init;
}

void Sink::doLoop()
{
	_polPort(_inPortVor, _inVor);
	_polPort(_inPortTrack, _inTrack);

	// form command

	_outCmd = _inVor + _inTrack;

	_outPort.Content() = _outCmd;
	_outPort.Write();
}

void Sink::doRelease()
{
	// release
}


