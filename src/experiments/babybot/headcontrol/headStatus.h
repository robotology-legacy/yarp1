// HeadStatus class
// this class stores the status of the head and should be used for transmission.
// 
//  
// May 2002 -- by nat
//

#define __headstatush__

#ifndef __headstatush__
#define __headstatush__

// #define HEAD_STATE_VERBOSE		//define this to enable debug messages
#ifdef HEAD_STATUS_VERBOSE
	#include <iostream>
	#define HEAD_STATUS_DEBUG(string) \
		std::cout << "HEAD_STATUS: " << string << flush
#else
	#define HEAD_STATUS_DEBUG if (1)
#endif


#include "NetworkData.h"
#include "RobotMath.h"
#include "HeadKinematics.h"

#define right_eye 4
#define left_eye  5
#define eye_tilt 3
#define neck_tilt 2
#define neck_pan 1

namespace _inertial {
	const int _ns = 3;
	const double _normGyroOut = 90;
	const double _in_thr = 0.088;
	const double _gain = 1.0;
}; // namespace


namespace _head {
	const int _nj = 5;
	const char _ini_file[] = "Q:\\Ini Files\\head.ini";

	const int _dh_np = 5;
	const int _dh_nrf = 5;

	const int _ns_saccade = 5;
	const int _ns_smooth_no_saccade = 20;

	const double _wait_time = 50;

	// LATER move this to file and make it dynamic
	// a, alfa, d, tetha, sign
	const double DH_left[_dh_nrf][_dh_np] = {
		{0, 0, 0, 0, -1},
		{0, -pi/2, 0, -pi/2, 1},
		{125, 0, 0, pi/2, 1},
		{0, pi/2, 0, pi/2, 0},	//zero in the fifth col means it is not linked to a joint
		{-87, 0, 0, pi/2, 1},
	};

	const double DH_right[_dh_nrf][_dh_np] = {
		{0, 0, 0, 0, -1},
		{0, -pi/2, 0, -pi/2, 1},
		{125, 0, 0, pi/2, 1},
		{0, pi/2, 0, pi/2, 0},	//zero in the fifth col means it is not linked to a joint
		{87, 0, 0, pi/2, -1},
	};

	const double TBaseline[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 133},
		{0, 0 ,0, 1},
	};


	const double max_vergence = 50;
	const double min_vergence = 0.5;
	const double max_version = 45;
	const double min_version = -45;
	const double max_pan = 60;
	const double min_pan = -60;
	const double max_tilt = 50;
	const double min_tilt = -50;

	const double max_eye_tilt = 50;
	const double min_eye_tilt = -50;

	enum  __state {
		smooth = 1,
		saccade_init = 2,
		saccade = 3,
		smooth_no_saccade = 4,
	};

	typedef struct __flags
	{
		bool limit_reached;
		int state;
	} _flags;

	typedef struct __dim
	{
		double version;
		double vergence;
		double tilt;
		double pan;
		double eyetilt;
	} _dim;

	class _limits
	{
	public:
		_limits()
		{
			max.version = max_version;
			max.vergence = max_vergence;
			max.pan = max_pan;
			max.tilt = max_tilt;

			max.eyetilt = max_eye_tilt;
			min.eyetilt = min_eye_tilt;

			min.version = min_version;
			min.vergence = min_vergence;
			min.pan = min_pan;
			min.tilt = min_tilt;
		}

		~_limits(){};

		_dim max;
		_dim min;
	};

}; //namespace

class HeadStatus: public GenericNetworkData
{
public:
	HeadStatus(int rate):
	  head_kinematics(CVisDMatrix(_head::_dh_nrf,_head::_dh_np,_head::DH_left[0]),
					 CVisDMatrix(_head::_dh_nrf,_head::_dh_np,_head::DH_right[0]), CHmgTrsf(_head::TBaseline[0]))
	{
		frame = 0;	//reset frame
		time = 0;	//reset time
		deltaT = (double) rate/1000;
		current_position.Resize(_head::_nj);
		old_position.Resize(_head::_nj);
		current_command.Resize(_head::_nj);
		velocity.Resize(_head::_nj);
	
		inertial.Resize(_inertial::_ns);

		adcready = 0;
		flags.limit_reached = false;
		flags.state = 0;

		length = _compute_size();

		buffer = new char [length];
		
		memset(buffer, 0, length);

	}
	
	~HeadStatus(){
		delete [] buffer;
	}

	inline void serialize(char *buff) const;
	
	const char * serialize(void) const {
		serialize(buffer);
		return buffer;
	}
	
	inline void tick() { time++; }
	
	inline void de_serialize(const char *buff);
	
	int size(void) const { return length; }
	// compute vergence angle
	inline double vergence(){
		return -(current_position(right_eye) + current_position(left_eye));
	}
	// compute vergence angle first derivative
	inline double dot_vergence(){
		return current_command(right_eye) + current_command(left_eye);
	}

	// compute version angle
	inline double version(){
		return .5*(current_position(right_eye) - current_position(left_eye));
	}
	// compute version angle first derivative 
	inline double dot_version(){
		return .5 * (current_command(right_eye) - current_command(left_eye));
	}

	//compute 3 d.o.f head position
	inline void five_to_three (CVisDVector& threed)
	{
		threed(1) = .5 * (current_position(4) - current_position(5)) - current_position(1);	// version
		threed(2) = -current_position(4) - current_position(5);	// vergence
		threed(3) = current_position(2) + current_position(3);	// tilt
	}

	// compute direct kinematics using the current position --> use x(), y(), z()
	inline void direct_kinematics()
	{
		head_kinematics.compute_direct(current_position);
	}

	// compute direct kineamtics using a specified position
	inline void direct_kinematics(const CVisDVector &pos, C3dVector &out)
	{
		head_kinematics.compute_direct(pos);
		
		out = head_kinematics.fixation_point;
	}

	// check whether all the joints are within limits
	inline bool check_limits_joint();
	// check whether the fixation point is within some limits
	inline bool check_limits_cartesian() {return true;};
	// check whether the fixation point is within max and min
	inline bool check_limits_cartesian(const double *max, const double *min)
	{
		HEAD_STATUS_DEBUG("Target too far\n");

		if ( (x() > max[0]) || (x() < min[0]) ||
				(y() > max[1]) || (y() < min[1]) ||
				(z() > max[2]) || (z() < min[2]) )
			return false;
		else
			return true;
		
	};

	// return treu if the head is currently still
	inline bool is_still();

	//return x coordinate of the position of the fixation point
	inline double x() const {
		return head_kinematics.fixation_point(1);
	}
	//return y coordinate of the position of the fixation point
	inline double y() const {
		return head_kinematics.fixation_point(2);
	}

	//return z coordinate of the position of the fixation point
	inline double z() const {
		return head_kinematics.fixation_point(3);
	}

	HeadKinematics head_kinematics;
	
	////////////////////////////
	// these need to be sent !
	CVisDVector current_position;	// current position as read by the mei board
	CVisDVector old_position;		// old stored position
	CVisDVector velocity;			// current speed, as read by the mei board
	CVisDVector current_command;	// current command
	CVisDVector inertial;			// inertial reading
	
	_head::_flags flags;	// just some flags about the head status
	int adcready;			// inertial flag
	unsigned int time;		// very simple timer, maybe it will be useful
	////////////////////////////
	double deltaT;

	// limits;
	_head::_limits limits;

private:
	inline int _compute_size();
	inline bool check_single_joint(double pos, double cmd, double upper, double lower, int sign);
	char *buffer;
};

inline int HeadStatus::_compute_size() {
	int tmp;
	tmp = 4*_head::_nj*sizeof(double);		// current position, current command, velocity, old position
	tmp += _inertial::_ns*sizeof(double);	// inertial sensor

	// adcready
	tmp += sizeof(int);

	//flags
	tmp += sizeof(_head::_flags);

	// time
	tmp += sizeof(int);

	// frame
	tmp += sizeof(int);		

	return tmp;
}

inline bool HeadStatus::check_limits_joint()
{
	flags.limit_reached = false;

	// check eyes
	bool tmp;
	tmp = check_single_joint(vergence()*radToDeg, dot_vergence(), limits.max.vergence, limits.min.vergence, -1);
	tmp |= check_single_joint(version()*radToDeg, dot_version(), limits.max.version, limits.min.version, 1);

	if (tmp) {
		current_command(right_eye) = 0.0;
		current_command(left_eye) = 0.0;
		flags.limit_reached = true;
	}

	// check neck pan
	tmp = check_single_joint(current_position(neck_pan)*radToDeg, current_command(neck_pan), limits.max.pan, limits.min.pan, 1);
	if (tmp) {
		current_command(neck_pan) = 0.0;
		flags.limit_reached = true;
	}

	tmp = check_single_joint(current_position(neck_tilt)*radToDeg, current_command(neck_tilt), limits.max.tilt, limits.min.tilt, 1);
	if (tmp) {
		current_command(neck_tilt) = 0.0;
		flags.limit_reached = true;
	}

	return (flags.limit_reached);

}

inline bool HeadStatus::check_single_joint(double pos, double cmd, double upper, double lower, int sign)
{
	if (sign>0)
	{
		if ((pos < lower && cmd < 0) || (pos > upper && cmd > 0))
			return true;
		else
			return false;
	}
	else
	{
		if ((pos < lower && cmd > 0 ) || (pos > upper && cmd < 0))
			return true;
		else
			return false;
	}
}

inline void HeadStatus::de_serialize(const char *buff)
{
	int offset = 0;
	memcpy((char *) &frame, buff+offset, sizeof(int));
	offset += sizeof(int);
	//current position
	memcpy(current_position.data(), buff+offset, sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	//old position
	memcpy(old_position.data(), buff+offset, sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	//velocity
	memcpy(velocity.data(), buff+offset,sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	// current command
	memcpy(current_command.data(), buff+offset, sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	// inertial data
	memcpy(inertial.data(), buff+offset, sizeof(double)*_inertial::_ns);
	offset += sizeof(double)*_inertial::_ns;
	// adcready
	memcpy(&adcready, buff+offset, sizeof(int));
	offset += sizeof(int);
	// flags
	memcpy(&flags, buff+offset, sizeof(_head::_flags));
	offset += sizeof(_head::_flags);
	// time
	memcpy(&time, buff+offset, sizeof(int));
}

inline void HeadStatus::serialize(char *buff) const {
	int offset = 0;
	memcpy(buff+offset, (char *)&frame, sizeof(double)*_head::_nj);
	offset += sizeof(int);
	// current position
	memcpy(buff+offset, current_position.const_data(), sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	// old position
	memcpy(buff+offset, old_position.const_data(), sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	// velocity
	memcpy(buff+offset, velocity.const_data(), sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	// current command
	memcpy(buff+offset, current_command.const_data(), sizeof(double)*_head::_nj);
	offset += sizeof(double)*_head::_nj;
	// inertial data
	memcpy(buff+offset, inertial.const_data(), sizeof(double)*_inertial::_ns);
	offset += sizeof(double)*_inertial::_ns;
	// adcready
	memcpy(buff+offset, &adcready, sizeof(int));
	offset += sizeof(int);
	// flags
	memcpy(buff+offset, &flags, sizeof(_head::_flags));
	offset += sizeof(_head::_flags);
	// time
	memcpy(buff+offset, &time, sizeof(int));
}

inline bool HeadStatus::is_still()
{
	double tmp = current_command.norm2();

	if (tmp < 5*degToRad)
	{
		HEAD_STATUS_DEBUG("Head is still\n");
		return true;
	}
	else
	{
		HEAD_STATUS_DEBUG("Head is moving\n");
		return false;
	}
}

#endif //.h