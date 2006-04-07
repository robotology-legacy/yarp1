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
			 << _data.GTData.valid   << "\t"
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
		YARPSimpleOperations::DrawCross<YarpPixelBGR>(
			_remappedImg,
			_gazeX(_data.GTData.pupilX), _gazeY(_data.GTData.pupilY),
			tmpWhitePixel, 5, 1);

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

	// read scene target points off gaze tracker
	SendCommandToCollector(CCmdGetData);
	ReadCollectorData();
	cout << "(gaze calibration) got target points coords: "
		 << _data.GTData.targetPoints[0]  << " "
		 << _data.GTData.targetPoints[1]  << " "
		 << _data.GTData.targetPoints[2]  << " "
		 << _data.GTData.targetPoints[3]
		 << endl;
	cout.flush();

	// calibrate accordingly
	_gazeX.eval(_data.GTData.targetPoints[0], _data.GTData.targetPoints[2]);
	_gazeY.eval(_data.GTData.targetPoints[1], _data.GTData.targetPoints[3]);

}

void GazeControlThread::initialise(void)
{

	return;

}

void GazeControlThread::shutdown(void)
{

	return;

}
