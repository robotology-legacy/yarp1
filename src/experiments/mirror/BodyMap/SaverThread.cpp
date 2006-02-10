// SaverThread.cpp: implementation of the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SaverThread.h"
#include "BodyMapDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CSaverThread::Body(void)
{

int i=0;

	while ( ! IsTerminated() ) {

		// must use synchronous version, otherwise there is no guarantee
		// that data will be synchronised.

		if ( _options->useTracker0 ) {
			_settings->_data_inport.Read();
			_settings->_data = _settings->_data_inport.Content();
		}
		if ( _options->useCamera0 ) {
			_settings->_img0_inport.Read();
			_settings->_img0.Refer(_settings->_img0_inport.Content());
		}
		if ( _options->useCamera1 ) {
			_settings->_img1_inport.Read();
			_settings->_img1.Refer(_settings->_img1_inport.Content());
		}

		FindTrackerXY(_settings->_img0, &_x0, &_y0, &_w0);
		FindTrackerXY(_settings->_img1, &_x1, &_y1, &_w1);
		int addedOk = _learningBlock->addSample(
			_settings->_data.tracker0Data.x,
			_settings->_data.tracker0Data.y,
			_settings->_data.tracker0Data.z,
			_x0);
		if ( addedOk == YARP_FAIL ) {
			break;
		}

	}

	_learningBlock->train();
	AfxGetMainWnd()->PostMessage(USER_STOP_ACQUIRE);
	return;

}
