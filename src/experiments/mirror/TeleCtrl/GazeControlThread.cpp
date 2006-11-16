#include "GazeControlThread.h"

// -----------------------------------
// thread constructor/destructor
// -----------------------------------

GazeControlThread::GazeControlThread(
	const double frequency,
	YARPOutputPortOf<YARPBottle>& saccadesOutPort, YARPOutputPortOf<YARPGenericImage>& imgOutPort,
	YARPSemaphore& sema, bool enabled=true) :
  ControlThread(frequency, sema, enabled),
  _saccadesOutPort(saccadesOutPort), _imgOutPort(imgOutPort),
  _gazeX(0,255), _gazeY(0,255),
  _gazeSampleCount(0), _enableSaccade(true)
{

	_remappedImg.Resize (256, 256);
	_remappedFoveaImg.Resize (128, 128);
	_remappedFoveaGrayscaleImg.Resize (128, 128);
	_coloredImg.Resize (_stheta, _srho);
	_flippedImg.Resize (_remappedImg.GetWidth(), _remappedImg.GetHeight(), _remappedImg.GetID());

	strcpy(imgPath,"d:\\tmp");
	strcpy(imgFileName,"prova");
	imgFileCount = 1;

}

// -----------------------------------
// thread body
// -----------------------------------

void GazeControlThread::Body (void)
{

	// loop until the thread is terminated
	while ( !IsTerminated() ) {

		// set operating frequency
		YARPTime::DelayInSeconds(_streamingFrequency);

//		cout << "Gaze:" << "\t"
//			 << _data.GTData.valid   << "\t" << _data.GTData.pupilX  << "\t" << _data.GTData.pupilY  << "\t"
//			 << "       \r";
//		cout.flush();

		// convert logpolar image to Cartesian
		_sema.Wait();
		_LPMapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)_image, _coloredImg);
		_sema.Post();
		_LPMapper.Logpolar2CartesianFovea (_coloredImg, _remappedFoveaImg);
		_LPMapper.Logpolar2Cartesian (_coloredImg, _remappedImg);

		if ( IWantToSave ) {
			char filename[256];
			sprintf(filename, "%s\\%s%02d.pgm",imgPath,imgFileName,imgFileCount++);
			YarpPixelMono tmpPixelMono;
			IMGFOR(_remappedFoveaImg,i,j) {
				tmpPixelMono = (_remappedFoveaImg(i,j).r +
					            _remappedFoveaImg(i,j).g +
								_remappedFoveaImg(i,j).b ) / 3;
				_remappedFoveaGrayscaleImg(i,j) = tmpPixelMono;
			}
			YARPImageFile::Write(filename,_remappedFoveaGrayscaleImg,YARPImageFile::FORMAT_PGM);
			cout << "image saved as " << filename << endl;
			IWantToSave = false;
		}

		// evaluate gaze statistics
		evaluateGazeStats(_data.GTData.pupilX,_data.GTData.pupilY);

			// send image to camview
		_imgOutPort.Content().Refer(_remappedImg);
		_imgOutPort.Write();

	}
    
	return;

}

// -----------------------------------
// evaluate gaze fixation
// -----------------------------------

void GazeControlThread::evaluateGazeStats(int pupilX, int pupilY)
{

	// for offline purposes
	gazeX = pupilX; gazeY = pupilY;

	// store new gaze sample (circular array)
	_gazeSampleCount = (_gazeSampleCount==_gazeSamples-1 ? 0 : _gazeSampleCount+1 );
	if ( _gazeSampleCount == 0 ) {
		_enableSaccade = true;
	}
	_gazeXPool[_gazeSampleCount] = _gazeX(pupilX);
	_gazeYPool[_gazeSampleCount] = _gazeY(pupilY);
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
	gazeStdv = sqrt(stdvX*stdvX+stdvY*stdvY);

	// usually, a red cross is drawn where the user's gaze is;
	// but if the intention to grasp is signalled, the cross will be green
	YarpPixelBGR tmpPixel(255,0,0);
	// if gaze is fixated, the cross will turn green
	if ( IWantToGrasp ) {
		tmpPixel.r = 0;
		tmpPixel.g = 255;
	}
	YARPSimpleOperations::DrawCross<YarpPixelBGR>(
		_remappedImg, 
		_gazeX(_data.GTData.pupilX), _gazeY(_data.GTData.pupilY), 
		tmpPixel, 8, 0);

	// OLD CODE: send a saccade to the fixated point
	/*	if ( _enableSaccade ) {
		YARPBottle tmpBottle;
		tmpBottle.writeInt((int)meanX);
		tmpBottle.writeInt((int)meanY);
		if ( _enabled ) {
			_hs_out.Content() = tmpBottle;
			_hs_out.Write();
		}
		_enableSaccade = false;
	}*/

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
