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

	HandControlThread(double frequency, YARPOutputPortOf<YARPBabyBottle>& outPort, YARPSemaphore& sema) :
	  ControlThread(frequency, sema),
	  _outPort(outPort),
	  _thumb(0,100), _index(0,-70), _fingers(0,-70),
	  _handInit0(0), _handInit1(-60), _handInit2(0),
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
	LimitLinCalibration _thumb, _index, _fingers;

	// hand joints initial positions (degrees)
	const double _handInit0;
	const double _handInit1;
	const double _handInit2;
	const double _handInit3;
	const double _handInit4;
	const double _handInit5;

};

#endif
