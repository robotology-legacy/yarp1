// ArmStatus class
// this class stores the status of the head and should be used for transmission.
// 
//  
// June 2002 -- by nat
//

#ifndef __armstatush__
#define __armstatush__

// #include "NetworkData.h"
#include <YARPRobotMath.h>
// #include "RobotKinematics.h"
#include "YARPConfigFile.h"
#include <assert.h>

#define wrist3		6
#define wrist2		5
#define wrist1		4
#define elbow		3
#define shoulder	2
#define waist		1

namespace _armThread
{
	enum  __state {
			directCommand = 0,
			resting = 1,
			restingInit = 2,
			restingLowerGains = 3,
			restingRaiseGains = 4,
			restingWaitIdle = 5,
			waitForMotion = 6,
			move = 7,
			directCommandMove = 8,
			zeroGInit = 9,
			zeroGEnd = 10
	};

	enum __pidState
	{
		low = 0,
		high = 1
	};

	typedef enum __pidState PIDState;

	typedef enum __state TState;

	typedef struct FLAGS
	{
		bool _limit_reached;
		TState  _thread;
	} ArmState;
};

class ArmStatus //: public GenericNetworkData
{
public:
	ArmStatus(){};
	ArmStatus(const char *confFile)
	{ resize(confFile); };
	ArmStatus(int rate, int njoints)
	{ resize(rate, njoints);	}
	
	~ArmStatus(){
		delete [] buffer;
	}

	void resize(const char *confFile)
	{
		YARPConfigFile file;
		int nj;
		int rate;
		file.set("", confFile);
		file.get("[GENERAL]", "Joints", &nj, 1);
		file.get("[THREAD]", "Rate", &rate, 1);
		resize(rate, nj);
	}
	void resize(int rate, int njoints)
	{
		_pidStatus = _armThread::low;
		_nj = njoints;
		frame = 0;
		_time = 0;	//reset time
		_deltaT = (double) rate/1000;
		
		_current_position.Resize(_nj);
		_old_position.Resize(_nj);
		_velocity.Resize(_nj);
		_final_position.Resize(_nj);
		_torques.Resize(_nj);

		length = _compute_size();
		buffer = new char [length];

		memset(buffer, 0, length);
	}

	inline void serialize(char *buff) const;
	
	const char * serialize(void) const {
		serialize(buffer);
		return buffer;
	}

	inline void tick() { _time++; }
	
	inline void de_serialize(const char *buff);
	
	int size(void) const { return length; }
	
	////////////////////////////
	// these need to be sent !
	YVector _current_position;	// current position as read by the mei board
	YVector _torques;			// current torques
	YVector _old_position;		// old stored position
	YVector _final_position;	// final position
	YVector _velocity;			// current speed, as read by the mei board

	unsigned int _time;				// very simple timer, maybe it will be useful
	_armThread::ArmState _state;					// arm state
	////////////////////////////
	double _deltaT;
	int _nj;

	_armThread::PIDState _pidStatus;

	bool isPIDHigh()
	{
		if (_pidStatus == _armThread::high)
			return true;
		else
			return false;
	}

private:
	inline int _compute_size() const;
	int frame;
	int length;
	char *buffer;
	int time;
};

inline int ArmStatus::_compute_size() const {
	int tmp;
	tmp = 5*_nj*sizeof(double);		// current position, error, old_position, velocity
	
	// _state
	tmp = tmp + sizeof(_state);	
	// time
	tmp = tmp + sizeof(int);
	// frame
	tmp = tmp + sizeof(int);

	return tmp;
}

inline void ArmStatus::de_serialize(const char *buff)
{
	int offset = 0;
	memcpy((char *)&frame, buff + offset, sizeof(int));
	offset += sizeof(int);
	//current position
	memcpy(_current_position.data(), buff+offset, sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	//old position
	memcpy(_old_position.data(), buff+offset, sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	//velocity
	memcpy(_final_position.data(), buff+offset,sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// error
	memcpy(_velocity.data(), buff+offset,sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// torques
	memcpy(_torques.data(), buff+offset,sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// _state
	memcpy(&_state, buff+offset, sizeof(_state));
	offset += sizeof(_state);
	// time
	memcpy(&time, buff+offset, sizeof(int));
}

inline void ArmStatus::serialize(char *buff) const {
	int offset = 0;
	memcpy(buff+offset, (char *) &frame, sizeof(int));
	offset +=sizeof(int);
	// current position
	memcpy(buff+offset, _current_position.data(), sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// old position
	memcpy(buff+offset, _old_position.data(), sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// final position
	memcpy(buff+offset, _final_position.data(), sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// velocity
	memcpy(buff+offset, _velocity.data(), sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// torques
	memcpy(buff+offset, _torques.data(), sizeof(double)*_nj);
	offset += sizeof(double)*_nj;
	// _state
	memcpy(buff+offset, &_state, sizeof(_state));
	offset += sizeof(_state);
	// time
	memcpy(buff+offset, &_time, sizeof(int));
}

#endif //.h