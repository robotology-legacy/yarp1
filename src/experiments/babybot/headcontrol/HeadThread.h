// HeadThread.h: interface for the HeadThread class.
//
//
//////////////////////////////////////////////////////////////////////

#ifndef __headthreadh__
#define __headthreadh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPRobotMath.h>	// useful stuff, like degToRad, pi and radToDeg include also VisDMatrix.h
#include <yarp/YARPRateThread.h>

#if defined(__QNXEurobot__)

	#include <YARPEurobotHead.h>

#else

	#include <YARPBabybotHead.h>

#endif
#include <yarp/YARPPort.h>
#include <yarp/YARPVectorPortContent.h>
#include <yarp/YARPString.h>

#define HEAD_THREAD_VERBOSE

#ifdef HEAD_THREAD_VERBOSE
#define HEAD_THREAD_DEBUG(string) YARP_DEBUG("HEAD_THREAD_DEBUG:", string)
#else  HEAD_THREAD_DEBUG(string) YARP_NULL_DEBUG
#endif

// state
#include "HeadFSMStates.h"

class HeadThread: public YARPRateThread
{
public:
	HeadThread(int rate, 
			   const char *name = "head control thread",
			   const char *ini_file = NULL);
	virtual ~HeadThread();

	void doInit();
	void doLoop();
	void doRelease();

	bool directCommand(const YVector &cmd)
	{
		// later, cosider to check _directCmdFlag
		_hsDirectCmd.set(cmd);
		_hiDirectCmdEnd.set(cmd);
		_directCmdFlag = true;
		return true;
	}

	void setAcceleration(const YVector &acc)
	{ _head.setAccs(acc.data()); }

	void stop()
	{
		if (_stopFlag)
		{
			HEAD_THREAD_DEBUG(("Head is now free to move\n"));
			_stopFlag = false;
		}
		else 
		{
			HEAD_THREAD_DEBUG(("Head is now stopped\n"));
			_stopFlag = true;
		}
	}

	// require hibernation
	void askHibernation()
	{	
		_askHib = true;
	}

private:
	//move the head to 0
	void park(int flag);

	void calibrateInertial();

	YVector _deltaQ;				// command
	double  _deltaT;				// thread rate (s)
	HeadFSM *_fsm;
	HeadSharedData _head;

	YVector _inertial;
	bool _directCmdFlag;

	YARPString _iniFile;
	YARPString _path;

	// output ports
	YARPOutputPortOf<YVector> _inertialPort;
	// YARPOutputPortOf<YARPControlBoardNetworkData> _statusPort;
	YARPOutputPortOf<YVector> _positionPort;

	//
	YARPInputPortOf<YVector> _inPort;
	//	YARPInputPortOf<YVector> _directCmdPort;

	// stop flag
	bool _stopFlag;
	bool _askHib;
	
	// FSM
	HIDirectCmdStart	_hiDirectCmdStart;
	HODirectCmdEnd		_hoDirectCmdEnd;
	HIDirectCmdEnd		_hiDirectCmdEnd;
	HSDirectCmd			_hsDirectCmd;
	HSDirectCmdStop		_hsDirectCmdStop;
	HSTrack				_hsTrack;

	// counters
	unsigned int _threadCounter;	// #control cycles
	unsigned int _inPortCounter;	// #control cycles elapsed since
									// the last frame received from _inPort
};

#endif // __headthreadh__