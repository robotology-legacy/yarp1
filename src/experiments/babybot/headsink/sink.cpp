#include "sink.h"
#include "sinkconstants.h"

char *binString(int value)
{
    static char bin[17];
    int index;

    for(index=0;index<16;index++)
    {
        if(value & 0x8000)
            bin[index] = '1';
        else
            bin[index] = '0';

        value = value << 1;
    }
    bin[16] = 0x00;

    return(bin);
}

Sink::Sink(int rate, const char *name, const char *ini_file):
YARPRateThread(name, rate),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_inPortPosition (YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
{
	_inPorts[SinkChVor] = new YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inPorts[SinkChTracker] = new YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inPorts[SinkChVergence] = new YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inPorts[SinkChSaccades] = new YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inPorts[SinkChSmoothPursuit] = new YARPInputPortOf<YARPBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

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

		_inhibitions[i] = SINK_INHIBIT_NONE;
		// register port
		_inPorts[i]->Register(YARPString(base).append(__portNameSuffixes[i]).c_str());
	}

	_globalInhibition = SINK_INHIBIT_NONE;
	// _manualInhibition = SINK_INHIBIT_ALL;
	_manualInhibition = SINK_INHIBIT_NONE | SINK_INHIBIT_VOR | SINK_INHIBIT_SMOOTH;
	_saveInhibition();

	suppressHead(); //start suppressed

	// position port
	_inPortPosition.Register(YARPString(base).append(__portPositionNameSuffix).c_str());
	_position.Resize(_nj);
	_position = 0;
	
	// output port and vector
	_outPort.Register(YARPString(base).append("o").c_str());
	_outCmd.Resize(_nj);
	_outCmd = 0.0;
	
	// control of the neck
	_neckControl = new NeckControl(_iniFile, _nj, _nj);

	// 
	_counter = 0;
//	_log.open("c:/temp/head.log");
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
	_globalInhibition = SINK_INHIBIT_NONE;
	for(i = 0; i < SinkChN; i++)
	{
		bool ret = _polPort(*(_inPorts[i]), _inVectors[i], _inhibitions[i]);
		_globalInhibition = _globalInhibition | _inhibitions[i];
	}

	_globalInhibition = _globalInhibition | _manualInhibition;
	_polPort(_inPortPosition, _position);

	_outCmd = 0.0;

	// check inhibit vergence

	double dtmp;
	dtmp = sqrt((_inVectors[SinkChTracker](4)*_inVectors[SinkChTracker](4))/(0.007*0.007));

	// form command
	if (!(_globalInhibition & SINK_INHIBIT_VOR))
		_outCmd = _outCmd + _inVectors[SinkChVor];

	if (!(_globalInhibition & SINK_INHIBIT_SMOOTH))
		_outCmd = _outCmd + _inVectors[SinkChTracker];
	
	if (!(_globalInhibition & SINK_INHIBIT_VERGENCE))
		_outCmd = _outCmd + _inVectors[SinkChVergence];
	
	// finally compute neck
	if (!(_globalInhibition & SINK_INHIBIT_SMOOTH))	// it used to be NECK
	{
		const YVector &neck = _neckControl->apply(_position);
		_outCmd = _outCmd + neck;
	}

	// add smooth pursuit
	if (!(_globalInhibition & SINK_INHIBIT_SMOOTHPURSUIT))
		_outCmd = _outCmd + _inVectors[SinkChSmoothPursuit];

	// add saccades
	if (!(_globalInhibition & SINK_INHIBIT_SACCADES))
	{
		_outCmd = _outCmd + _inVectors[SinkChSaccades];
	}

	_counter++;

	_outPort.Content() = _outCmd;
	_outPort.Write();
}

void Sink::doRelease()
{
	// release
}

void Sink::inhibitAll()
{
	_manualInhibition = SINK_INHIBIT_ALL;
	_saveInhibition();
	ACE_OS::printf("Manual inhibition set to %d\n", _manualInhibition);
}

void Sink::enableAll()
{
	_manualInhibition = SINK_INHIBIT_NONE;
	_saveInhibition();
	ACE_OS::printf("Manual inhibition set to %d\n", _manualInhibition);
}

void Sink::suppressHead()
{
	_saveInhibition();
	_manualInhibition = SINK_INHIBIT_ALL;
	ACE_OS::printf("Head suppressed\n");
}

void Sink::releaseHead()
{
	_restoreInhibition();
	ACE_OS::printf("Head released\n");
}

void Sink::inhibitChannel(int n)
{
	_manualInhibition = _manualInhibition | n;
	_saveInhibition();
}

void Sink::enableChannel(int n)
{
	_manualInhibition = _manualInhibition & (~n);
	_saveInhibition();
}

void Sink::printChannelsStatus()
{
	/*int i;
	for (i = 0; i < SinkChN; i++)
		ACE_OS::printf("%d\t", _enableVector[i]);
		
	ACE_OS::printf("\n");*/

	ACE_OS::printf("Manual inhibition set to %s\n", binString(_manualInhibition));
}
