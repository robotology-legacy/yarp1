// HeadThread.h: interface for the HeadThread class.
//
//
//////////////////////////////////////////////////////////////////////

#ifndef __headthreadh__
#define __headthreadh__
#include <conf/YARPConfig.h>
#include <YARPRobotMath.h>	// useful stuff, like degToRad, pi and radToDeg include also VisDMatrix.h
#include <YARPRateThread.h>
#include <YARPBabybotHead.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPString.h>

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

	bool directCommand(YVector &cmd)
	{
		// later, cosider to check _directCmdFlag
		_hsDirectCmd.set(cmd);
		_hiDirectCmdEnd.set(cmd);
		_directCmdFlag = true;
		return true;
	}

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

private:
	//move the head to 0
	void park(int flag);

	void calibrateInertial();

	inline void read_status();
	inline void write_status();
	inline void send_commands();
	inline void check_limits();

	YVector _deltaQ;				//command
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
	
	// FSM
	HIDirectCmdStart	_hiDirectCmdStart;
	HODirectCmdEnd		_hoDirectCmdEnd;
	HIDirectCmdEnd		_hiDirectCmdEnd;
	HSDirectCmd			_hsDirectCmd;
	HSDirectCmdStop		_hsDirectCmdStop;
	HSTrack				_hsTrack;

	unsigned int _count;

};

inline void HeadThread::read_status()
{
	/// get head
	_head.getPositions(_head._status._current_position.data());
	_head.getVelocities(_head._status._velocity.data());
	_head._status._velocity*=radToDeg/10;		//normalize

	_head.readAnalogs(_inertial.data());

	// read vor info
	if (_inPort.Read(0))
	{
		_head._inCmd = _inPort.Content();
	}
}

inline void HeadThread::write_status()
{
	// send inertial info
	_inertialPort.Content() = _inertial;
	_inertialPort.Write();

	_positionPort.Content() = _head._status._current_position;
	_positionPort.Write();
	
	if (_head.checkLimits(_head._status._current_position.data(), _deltaQ.data()))
	{
//		HEAD_THREAD_DEBUG(("limit reached #%u\n", _count));
	}

//	printf("#%u %lf\n", _count, _deltaQ(3));
	_head.velocityMove(_deltaQ.data());
}

#endif // __headthreadh__