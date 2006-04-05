#include "TeleCtrl.h"

#ifndef GazeControlThreadH
#define GazeControlThreadH

// babybot
#include <conf/babybot/YARPVocab_babybot.h>
// logpolar
#include <yarp/YARPLogpolar.h>

// -----------------------------------
// class GazeControlThread
// -----------------------------------

using namespace _logpolarParams;

class GazeControlThread : public ControlThread {

public:

	GazeControlThread(const double frequency,
		YARPOutputPortOf<YARPBottle>& saccadesOutPort, YARPOutputPortOf<YARPGenericImage>& imgOutPort,
		YARPSemaphore& sema) 
		: ControlThread(frequency, sema), _saccadesOutPort(saccadesOutPort), _imgOutPort(imgOutPort), _x(0), _y(0)
	{
		_remappedImg.Resize (256, 256);
		_coloredImg.Resize (_stheta, _srho);
		_flippedImg.Resize (_remappedImg.GetWidth(), _remappedImg.GetHeight(), _remappedImg.GetID());
	}

	void Body (void);
	void calibrate(void);
	void initialise(void);
	void shutdown(void);

private:

	// ports to the hSaccades and to camview
	YARPOutputPortOf<YARPBottle>& _saccadesOutPort;
	YARPOutputPortOf<YARPGenericImage>& _imgOutPort;

	// logpolar to Cartesian stuff
	YARPGenericImage _flippedImg;
	YARPImageOf<YarpPixelBGR> _remappedImg;
	YARPImageOf<YarpPixelBGR> _coloredImg;
	YARPLogpolar _LPMapper;

	// coordinates of the cross indicating the user's gaze
	int _x, _y;

};

#endif
