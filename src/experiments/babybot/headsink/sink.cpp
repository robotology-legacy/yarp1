#include "sink.h"

Sink::Sink(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_inPortVor(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_inPortTrack(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP),
_inPortPosition(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
{
	_iniFile = YARPString(ini_file);
	
	_nj = __nJoints;
	_inVor.Resize(_nj);
	_inTrack.Resize(_nj);
	_outCmd.Resize(_nj);
	_inPosition.Resize(_nj);

	_inVor = 0.0;
	_inTrack = 0.0;
	_outCmd = 0.0;
	_inPosition = 0.0;

	YARPString base = __baseName;
	_outPort.Register(YARPString(base).append("o").c_str());
	_inPortVor.Register(YARPString(base).append("vor/i").c_str());
	_inPortTrack.Register(YARPString(base).append("track/i").c_str());
	_inPortPosition.Register(YARPString(base).append("position/i").c_str());

	_neckControl = new NeckControl(_iniFile, _nj, _nj);

	_inhibitAll = 0;
}

Sink::~Sink()
{
	delete _neckControl;

}

void Sink::doInit()
{
	// init;
}

void Sink::doLoop()
{
	_polPort(_inPortVor, _inVor);
	_polPort(_inPortTrack, _inTrack);
	_polPort(_inPortPosition, _inPosition);

	// form command
		
	_outCmd = _inVor + _inTrack;

	// compute neck
	const YVector &neck = _neckControl->apply(_inPosition, _outCmd);

	_outCmd = _outCmd + neck;

	if (_inhibitAll)
		_outCmd = 0.0;

	_outPort.Content() = _outCmd;
	_outPort.Write();
}

void Sink::doRelease()
{
	// release
}

void Sink::inhibitAll()
{
	if (_inhibitAll)
		_inhibitAll = 0;
	else
		_inhibitAll = 1;
}
