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

const unsigned int _gazeSamples = 10;

class GazeControlThread : public ControlThread {

public:

	GazeControlThread(const double,YARPOutputPortOf<YARPBottle>&,YARPOutputPortOf<YARPGenericImage>&,YARPSemaphore&,bool);

	void Body (void);
	void calibrate(void);
	void initialise(void);
	void shutdown(void);

private:

	void evaluateGazeStats(int,int);
	
	// ports to the hSaccades and to camview
	YARPOutputPortOf<YARPBottle>& _saccadesOutPort;
	YARPOutputPortOf<YARPGenericImage>& _imgOutPort;

	// logpolar to Cartesian stuff
	YARPGenericImage _flippedImg;
	YARPImageOf<YarpPixelBGR> _remappedImg;
	YARPImageOf<YarpPixelBGR> _remappedFoveaImg;
	YARPImageOf<YarpPixelMono> _remappedFoveaGrayscaleImg;
	YARPImageOf<YarpPixelBGR> _coloredImg;
	YARPLogpolar _LPMapper;

	// coordinates of the cross indicating the user's gaze
	// (normalised wrt the image)
//	LimitLinCalibration _gazeX, _gazeY;
	LinCalibration _gazeX, _gazeY;

	// gaze tracking and statistics
	bool _enableSaccade;
	double _gazeXPool[_gazeSamples];
	double _gazeYPool[_gazeSamples];
	unsigned int _gazeSampleCount;

	// saving images
	char imgPath[256];
	char imgFileName[256];
	int imgFileCount;

};

#endif
