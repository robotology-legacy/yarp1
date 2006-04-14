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

		// store new gaze sample (circular array)
		_gazeSampleCount = (_gazeSampleCount==_gazeSamples-1 ? 0 : _gazeSampleCount+1 );

		_gazeXPool[_gazeSampleCount] = _gazeX(_data.GTData.pupilX);
		_gazeYPool[_gazeSampleCount] = _gazeY(_data.GTData.pupilY);
		// evaluate mean and stdv
		double meanX = 0.0, meanY = 0.0, stdvX = 0.0, stdvY = 0.0;
		for ( int i=0; i<_gazeSamples; ++i ) {
			meanX += _gazeXPool[i];
			meanY += _gazeYPool[i];
		}
		meanX /= _gazeSamples;
		meanY /= _gazeSamples;
		for ( i=0; i<_gazeSamples; ++i ) {
			stdvX += (_gazeXPool[i]-meanX)*(_gazeXPool[i]-meanX);
			stdvY += (_gazeYPool[i]-meanY)*(_gazeYPool[i]-meanY);
		}
		stdvX = sqrt(stdvX / ((double)_gazeSamples-1.0));
		stdvY = sqrt(stdvY / ((double)_gazeSamples-1.0));
		double stdv = sqrt(stdvX*stdvX+stdvY*stdvY);
		// draw current mean and stdv (thin, variable red cross)
		YarpPixelBGR tmpPixel2(255,0,0);
		YARPSimpleOperations::DrawCross<YarpPixelBGR>(_remappedImg, meanX, meanY, tmpPixel2, stdv, 1);
		// draw current gaze position. if stdv is below T,
		// it becomes green; otherwise, it is yellow
		if ( stdv < 10.0 ) {
			YarpPixelBGR tmpPixel1(0,255,0);
			YARPSimpleOperations::DrawCross<YarpPixelBGR>(
				_remappedImg, 
				_gazeX(_data.GTData.pupilX), _gazeY(_data.GTData.pupilY),
				tmpPixel1, 8, 0);
		} else {
			YarpPixelBGR tmpPixel1(255,255,0);
			YARPSimpleOperations::DrawCross<YarpPixelBGR>(_remappedImg, 
				_gazeX(_data.GTData.pupilX), _gazeY(_data.GTData.pupilY), 
				tmpPixel1, 8, 0);
		}

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
	_gazeX.eval(_data.GTData.targetPoints[2], _data.GTData.targetPoints[0]);
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
