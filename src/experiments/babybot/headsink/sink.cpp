#include "sink.h"

Sink::Sink(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	_inPorts[SinkChVor] = new YARPInputPortOf<YVector>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inPorts[SinkChPosition] = new YARPInputPortOf<YVector>(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	_inPorts[SinkChTracker] = new YARPInputPortOf<YVector>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inPorts[SinkChVergence] = new YARPInputPortOf<YVector>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	_iniFile = YARPString(ini_file);
	
	_nj = __nJoints;
	YARPString base = __baseName;

	// initialize input vectors and register ports
	int i;
	for(i = 0; i < SinkChN; i++)	// for each channel
	{
		// resize vector and initialze it
		_inVectors[i].Resize(_nj);
		_inVectors[i] = 0.0;

		_enableVector[i] = 0;		// initially inhibit all channels

		// register port
		_inPorts[i]->Register(YARPString(base).append(__portNameSuffixes[i]).c_str());
	}

	// output port and vector
	_outPort.Register(YARPString(base).append("o").c_str());
	_outCmd.Resize(_nj);
	_outCmd = 0.0;
	
	// control of the neck
	_neckControl = new NeckControl(_iniFile, _nj, _nj);
}

Sink::~Sink()
{
	int i = 0;
	
	// delete 
	for(i = 0; i < SinkChN; i++)
		delete _inPorts[i];

	delete _neckControl;
}

void Sink::doInit()
{
	// init;
}

void Sink::doLoop()
{
	/// polls all ports.
	int i;
	for(i = 0; i < SinkChN; i++)
		_polPort(*(_inPorts[i]), _inVectors[i]);

	_outCmd = 0.0;

	// form command
	_outCmd = _outCmd + _enableVector[SinkChVor]*_inVectors[SinkChVor];
	_outCmd = _outCmd + _enableVector[SinkChTracker]*_inVectors[SinkChTracker];
	_outCmd = _outCmd + _enableVector[SinkChVergence]*_inVectors[SinkChVergence];
	
	// finally compute neck
	const YVector &neck = _neckControl->apply(_inVectors[SinkChPosition], _outCmd);
	_outCmd = _outCmd + _enableVector[SinkChPosition]*neck;

	_outPort.Content() = _outCmd;
	_outPort.Write();
}

void Sink::doRelease()
{
	// release
}

void Sink::inhibitAll()
{
	int i;
	for(i = 0; i < SinkChN; i++)
		_enableVector[i] = 0;
}

void Sink::enableAll()
{
	int i;
	for(i = 0; i < SinkChN; i++)
		_enableVector[i] = 1;
}

void Sink::inhibitChannel(int n)
{
	ACE_ASSERT( (n >= 0) && (n<SinkChN) );

	if (_enableVector[n])
		_enableVector[n] = 0;
	else
		_enableVector[n] = 1;
}

void Sink::printChannelsStatus()
{
	int i;
	for (i = 0; i < SinkChN; i++)
		ACE_OS::printf("%d\t", _enableVector[i]);

	ACE_OS::printf("\n");

}
