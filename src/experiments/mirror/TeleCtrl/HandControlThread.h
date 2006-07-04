#include "TeleCtrl.h"

#ifndef HandControlThreadH
#define HandControlThreadH

// babybot
#include <conf/babybot/YARPVocab_babybot.h>

// -----------------------------------
// class HandControlThread
// -----------------------------------

class HandControlThread : public ControlThread {

public:

	HandControlThread(const double frequency, YARPOutputPortOf<YARPBabyBottle>& outPort,
		YARPSemaphore& sema, bool enabled=true)
		: ControlThread(frequency, sema, enabled),
	  _outPort(outPort),
	  _thumb0(0,90), _thumb1(-20,40), 
	  _index0(0,-90), _index1(0,-90),
	  _fingers0(0,-90), _fingers1(0,-90),
	  _handInit0(0), _handInit1(0), _handInit2(0),
	  _handInit3(0), _handInit4(0), _handInit5(0) {}

	void Body (void);
	void calibrate(void);
	void initialise(void);
	void shutdown(void);

private:

	// overloading: the babybot hand port sends off BabyBottles
	YARPOutputPortOf<YARPBabyBottle>& _outPort;

	void sendPosCmd(double,double,double,double,double,double);

	// values for the robot fingers
	LimitLinCalibration _thumb0, _thumb1, _index0, _index1, _fingers0, _fingers1;

	// hand joints initial positions (degrees)
	const double _handInit0;
	const double _handInit1;
	const double _handInit2;
	const double _handInit3;
	const double _handInit4;
	const double _handInit5;

};

#endif
