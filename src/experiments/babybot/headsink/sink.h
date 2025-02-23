#ifndef __sinkclasshh__
#define __sinkclasshh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPRateThread.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPLogFile.h>

class NeckControl;

#define SINK_VERBOSE

#ifdef SINK_THREAD_VERBOSE
#define SINK_THREAD_DEBUG(string) YARP_DEBUG("SINK_THREAD_DEBUG:", string)
#else  SINK_THREAD_DEBUG(string) YARP_NULL_DEBUG
#endif

const int __nJoints = 5;
const YARPString __baseName = "/headsink/";

enum __SinkCh
{
	SinkChVor = 0,
	SinkChTracker = 1,
	SinkChVergence = 2,
	SinkChSaccades= 3,
	SinkChSmoothPursuit= 4,
	SinkChN = 5,
};

const char __portPositionNameSuffix[] = "position/i";
const char __portNameSuffixes[SinkChN][255] = {
										"vor/i",
										"track/i",
										"vergence/i",
										"saccades/i",
										"smoothpursuit/i"
										};

class Sink: public YARPRateThread
{
public:
	Sink(int rate, const char *name, const char *ini_file = 0);
	~Sink();

	void doInit();
	void doLoop();
	void doRelease();

	// inhibit channel
	void inhibitChannel(int n);
	// inhibit all inputs
	void inhibitAll();
	// enable all channels
	void enableAll();
	// enable channel
	void enableChannel(int n);

	// stop head, but remember current inhibitions
	void suppressHead();
	// restore inhibitions as they were before calling suppressHead
	void releaseHead();

	void printChannelsStatus();

private:
	inline void _saveInhibition()
	{ _storedInhibition = _manualInhibition; }

	inline void _restoreInhibition()
	{ _manualInhibition = _storedInhibition; }

	inline bool _polPort(YARPInputPortOf<YARPBabyBottle> &port, YVector &v, int &inhibition)
	{
		if (port.Read(0))
		{
			// port.Content().display();
			YARPBabyBottle &tmp = port.Content();
			tmp.readYVector(v);
			tmp.readInt(&inhibition);
			return true;
		}
		return false;
	}

	inline bool _polPort(YARPInputPortOf<YVector> &port, YVector &v)
	{
		if (port.Read(0))
		{
			v = port.Content();
			return true;
		}

		return false;
	}

	NeckControl *_neckControl;
	
	// output port
	YARPOutputPortOf<YVector>  _outPort;
	YVector _outCmd;
	
	// input channels
	YARPInputPortOf<YARPBabyBottle>  *_inPorts[SinkChN];
	YARPInputPortOf<YVector> _inPortPosition;
	YVector _inVectors[SinkChN];
	int _inhibitions[SinkChN];	// these are received from channels
	int _manualInhibition;		// this is set manually from external commands
	int _globalInhibition;		// global vector which is computed at each frame
	int	_storedInhibition;		// temp value to store current inhibition
	YVector _position;

	YARPString _iniFile;
	YARPString _path;
	
	int _nj;

	// YARPLogFile _log;

	unsigned int _counter;
};

#include <yarp/YARPPidFilter.h>
#include <yarp/YARPConfigFile.h>

class NeckControl
{
public:
	NeckControl(const YARPString &iniFile, int insize, int outsize)
	{
		_iniFile = iniFile;
		_inSize = insize;
		_outSize = outsize;

		YARPConfigFile file;
		char *root = GetYarpRoot();
		_path.append(root);
		_path.append("/conf/babybot/"); 
	
		file.set(_path, _iniFile);
		file.get("[NECKCONTROL]", "nPids", &_nPids);

		_pids = new YARPPidFilter[_nPids];
		double temp[3];
		int i;
		for(i = 0; i < _nPids; i++)
		{
			char tmpField[80];
			sprintf(tmpField, "Pid%d", i);
			file.get("[NECKCONTROL]", tmpField, temp, 3);
			_pids[i].setKs(temp[0], temp[1], temp[2]);
		}

		_cmd.Resize(outsize);
		_cmd = 0.0;
	}
	
	~NeckControl()
	{
		delete [] _pids;
	}

	const YVector & apply(const YVector &in)
	{
		double pred_r = in(4);
		double pred_l = in(5);

		double pred_tilt = in(3);

		_cmd(1) = _pids[0].pid(pred_r-pred_l);
		_cmd(2) = _pids[1].pid(pred_tilt-in(2));
		_cmd(3) = 0.0;
		_cmd(4) = 0.0;
		_cmd(5) = 0.0;

		return _cmd;
	}

private:
	YARPPidFilter *_pids;
	int _nPids;
	YVector _cmd;
	YARPString _iniFile;
	YARPString _path;
	int _inSize;
	int _outSize;
};

#endif

