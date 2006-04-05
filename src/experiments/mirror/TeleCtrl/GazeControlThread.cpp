#include "GazeControlThread.h"

// -----------------------------------
// thread body
// -----------------------------------

void GazeControlThread::Body (void)
{

	// loop until the thread is terminated
	while ( !IsTerminated() ) {

		// set operating frequency
		YARPTime::DelayInSeconds(_streamingFrequency);

		// output to screen
		cout << "Gaze:" << "\t"
			 << _data.GTData.pupilDiam  << "\t"
			 << _data.GTData.pupilX  << "\t"
			 << _data.GTData.pupilY  << "\t"
			 << "       \r";
		cout.flush();

		// convert logpolar image to Cartesian
		_sema.Wait();
		_LPMapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)_image, _coloredImg);
		_sema.Post();
		_LPMapper.Logpolar2Cartesian (_coloredImg, _remappedImg);

		// draw a moving cross on it
		YarpPixelBGR tmpWhitePixel(255,255,255);
		YARPSimpleOperations::DrawCross<YarpPixelBGR>(_remappedImg, _x, _y, tmpWhitePixel, 5, 1);
		_x = ++_x % 120;
		_y = ++_y % 120;

		// send image to camview
		_imgOutPort.Content().Refer(_remappedImg);
		_imgOutPort.Write();

	}
    
	return;

}

// -----------------------------------
// thread mandatory methods
// -----------------------------------

void GazeControlThread::calibrate(void)
{

	return;

}

void GazeControlThread::initialise(void)
{

	return;

}

void GazeControlThread::shutdown(void)
{

	return;

}
