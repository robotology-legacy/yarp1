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
	    
		YarpPixelMono tmpMonoWhitePixel = 255;
		YARPSimpleOperations::DrawCross<YarpPixelMono>((YARPImageOf<YarpPixelMono>&)_image, _x, _y, tmpMonoWhitePixel, 7);
		_x = ++_x % 120;
		_y = ++_y % 120;

		_imgOutPort.Content().Refer(_image);
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
