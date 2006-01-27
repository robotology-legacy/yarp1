// SaverThread.cpp: implementation of the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SaverThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int CSaverThread::openFile()
{

	char tmpFileNamePrefix[255];

	// build save file name (both for data and images files)
	ACE_OS::sprintf(tmpFileNamePrefix, "%s\\%s%04d", _settings->savePath, _settings->saveFilenamePrefix, _settings->_sequenceNumber);

	// if necessary, open the data file
	if ( _options->useDataGlove || _options->useGazeTracker || _options->useTracker0 || _options->useTracker1 || _options->usePresSens ) {
		char tmpFileName[255];
		ACE_OS::sprintf(tmpFileName, "%s.csv", tmpFileNamePrefix);
		_saveFile = fopen(tmpFileName, "w");
		if ( _saveFile == NULL ) {
//			MessageBox("Saving thread could not open data output file.", "Error.", MB_ICONERROR);
//			GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
//			GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
//			GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
//			GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
			return YARP_FAIL;
		}
		writeHeaderToFile();
	}

	return YARP_OK;

}

void CSaverThread::closeFile()
{

	fclose(_saveFile);

}

void CSaverThread::Body(void)
{

	_frameN = 0;

	while ( ! IsTerminated() ) {

		++_frameN;

		// must use synchronous version, otherwise there is no guarantee
		// that data will be synchronised.

		if ( _options->useTracker0 ) {
			_settings->_data_inport.Read();
			_settings->_data = _settings->_data_inport.Content();
		}
		if ( _options->useCamera0 ) {
			_settings->_img0_inport.Read();
			_settings->_img0.Refer(_settings->_img0_inport.Content());
			FindTrackerXY(_settings->_img0, &_x0, &_y0, &_w0);
		}
		if ( _options->useCamera1 ) {
			_settings->_img1_inport.Read();
			_settings->_img1.Refer(_settings->_img1_inport.Content());
			FindTrackerXY(_settings->_img1, &_x1, &_y1, &_w1);
		}

		writeDataToFile();

	}
		
}

void CSaverThread::writeHeaderToFile()
{

	fprintf( _saveFile, "N;" );
	fprintf( _saveFile, "X;Y;Z;" );
	fprintf( _saveFile, "X0;Y0;" );
	fprintf( _saveFile, "X1;Y1\n" );

}

void CSaverThread::writeDataToFile()
{

	fprintf( _saveFile, "%d;", _frameN );
	fprintf( _saveFile, "%.1f;%.1f;%.1f;", _settings->_data.tracker0Data.x, _settings->_data.tracker0Data.y, _settings->_data.tracker0Data.z);
	fprintf( _saveFile, "%d;%d;", _x0, _y0 );
	fprintf( _saveFile, "%d;%d\n", _x1, _y1 );

}
