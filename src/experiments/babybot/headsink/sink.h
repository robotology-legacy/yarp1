#ifndef __sinkclasshh__
#define __sinkclasshh__

#include <conf/YARPConfig.h>
#include <YARPRateThread.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>

#define SINK_VERBOSE

#ifdef SINK_THREAD_VERBOSE
#define SINK_THREAD_DEBUG(string) YARP_DEBUG("SINK_THREAD_DEBUG:", string)
#else  SINK_THREAD_DEBUG(string) YARP_NULL_DEBUG
#endif

const int __nJoints = 5;
const YARPString __baseName = "/headsink/";

class Sink: public YARPRateThread
{
public:
	Sink(int rate, const char *name, const char *ini_file = 0);
	~Sink();

	void doInit();
	void doLoop();
	void doRelease();

private:
	void _polPort(YARPInputPortOf<YVector> &port, YVector &v)
	{
		if (port.Read(0))
			v = port.Content();
	}

	YARPOutputPortOf<YVector>  _outPort;
	YARPInputPortOf<YVector>   _inPortVor;
	YARPInputPortOf<YVector>   _inPortTrack;
	
	YVector _inVor;
	YVector _inTrack;
	YVector _outCmd;
	int _nj;
};

#endif

