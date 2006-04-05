#include "TeleCtrl.h"

#ifndef GazeControlThreadH
#define GazeControlThreadH

// babybot
#include <conf/babybot/YARPVocab_babybot.h>

// -----------------------------------
// class GazeControlThread
// -----------------------------------

class GazeControlThread : public ControlThread {

public:

	GazeControlThread(double frequency, 
		YARPOutputPortOf<YARPBottle>& saccadesOutPort, YARPOutputPortOf<YARPGenericImage>& imgOutPort,
		YARPSemaphore& sema) 
		: ControlThread(frequency, sema), _saccadesOutPort(saccadesOutPort), _imgOutPort(imgOutPort), _x(0), _y(0) {}

	void Body (void);
	void calibrate(void);
	void initialise(void);
	void shutdown(void);

private:

	YARPOutputPortOf<YARPBottle>& _saccadesOutPort;
	YARPOutputPortOf<YARPGenericImage>& _imgOutPort;

	int _x, _y;

};

#endif
