#include "TeleCtrl.h"

#ifndef ArmPosControlThreadH
#define ArmPosControlThreadH

// babybot
#include <conf/babybot/YARPVocab_babybot.h>

// -----------------------------------
// class ArmPosControlThread
//
// control the arm in position
// -----------------------------------

class ArmPosControlThread : public ControlThread {

public:

	ArmPosControlThread(const double frequency, YARPOutputPortOf<YARPBabyBottle>& outPort,
		YARPSemaphore& sema, bool enabled=true)
		: ControlThread(frequency, sema, enabled),
	  _outPort(outPort),
	  _wristRoll(0,90), _wristPitch(50,-50),
	  _armInit0(25), _armInit1(25), _armInit2(-40),
	  _armInit3(0), _armInit4(-20), _armInit5(-180) {}

	void Body (void);
	void calibrate(void);
	void initialise(void);
	void shutdown(void);

private:
	// overloading: the babybot arm port sends off BabyBottles
	YARPOutputPortOf<YARPBabyBottle>& _outPort;

	void sendPosCmd(const double, const double, const double,
		 const double, const double, const double);

	// reference frame (set by calibration at the beginning)
	struct referenceFrame {
		referenceFrame() { x = y = z = az = el = ro = 0.0; }
		double x, y, z, az, el, ro;
	} _ref;
	
	// wrist pitch
	LimitLinCalibration _wristPitch, _wristRoll;

	// arm joints initial position (in degrees). the arm is initially stretched with the hand down.
	const double _armInit0;
	const double _armInit1;
	const double _armInit2;
	const double _armInit3;
	const double _armInit4;
	const double _armInit5;

};

// -----------------------------------
// inverse kinematics stuff
// -----------------------------------

// Numerical Recipes - minimisation algorithm for inverse kinematics
#include "nr.h"

// inverse kinematics
void inverse_kinematics(YVector&, YVector&, YVector&);
// error wrt a desired position in Cartesian space
DP evaluate_error (Vec_I_DP&);
// forward kinematics
void forward_kinematics(YVector&, YVector&);

// minimisation algorithm
inline void get_psum(Mat_I_DP&, Vec_O_DP&);
void amoeba(Mat_IO_DP&, Vec_IO_DP&, const DP, DP funk(Vec_I_DP&), int&);
DP amotry(Mat_IO_DP&, Vec_O_DP&, Vec_IO_DP&, DP funk(Vec_I_DP&), const int, const DP);

#endif
