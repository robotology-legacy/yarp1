// SaverThread.cpp: implementation of the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SaverThread.h"
#include "GraspCaptureDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CSaverThread::Body(void)
{

	char fName[255];
	_nFrames = 0;

// -------------------- tmp BEGIN
int pressureThreshold = 24000;
int wait = 0;
bool countGrasps = false;
_graspCount = 0;
// -------------------- tmp BEGIN

	while ( ! IsTerminated() ) {

		_nFrames++;

		if ( useDataGlove || useGazeTracker || useTracker0 || useTracker1 || usePresSens ) {
			p_data_inport->Read();
			*pData = p_data_inport->Content();

// -------------------- tmp BEGIN
// beep when the thumb contacts something
if ( pData->pressureData.channelA < pressureThreshold && countGrasps == false && wait == 0 ) {
	countGrasps = true;
	Beep(500,50);
	_graspCount++;
}
// beep when the thumb leaves something
if ( pData->pressureData.channelA > pressureThreshold && countGrasps == true ) {
	countGrasps = false;
	wait = 200;
	if ( _graspCount % 30 ) {
		// beep LOW at each contact
		Beep(250,100);
	} else {
		if ( _graspCount == 120 ) {
			// beep LOUD 3 TIMES at 120 contacts
			Beep(750,300);
			Sleep(200);
			Beep(750,300);
			Sleep(200);
			Beep(750,300);
		} else {
			// beep LOUD 1 TIME every 30 contacts
			Beep(750,300);
		}
	}
}
// after a leave event, allow 2 second before next contact
// WARNING: we are assuming streaming frequency is 100Hz!!!
if ( wait > 0 ) {
	wait--;
}
// -------------------- tmp END

			writeDataToFile(_nFrames);
		}
		if ( useCamera0 ) {
			p_img0_inport->Read();
			pImg0->Refer(p_img0_inport->Content());
			ACE_OS::sprintf(fName,"%s.0.%03d.pgm", prefix, _nFrames);
			YARPImageFile::Write(fName, *pImg0,YARPImageFile::FORMAT_PPM);
		}
		if ( useCamera1 ) {
			p_img1_inport->Read();
			pImg1->Refer(p_img1_inport->Content());
			ACE_OS::sprintf(fName,"%s.1.%03d.pgm", prefix, _nFrames);
			YARPImageFile::Write(fName, *pImg1,YARPImageFile::FORMAT_PPM);
		}

	} // while()
		
	return;
}

void CSaverThread::writeDataToFile(int i)
{

	fprintf(pFile,"%d ",i);

	fprintf(pFile,"%.2f %.2f %.2f %.2f %.2f %.2f ", pData->tracker0Data.x, pData->tracker0Data.y, pData->tracker0Data.z, pData->tracker0Data.azimuth, pData->tracker0Data.elevation, pData->tracker0Data.roll);
	fprintf(pFile,"%.2f %.2f %.2f %.2f %.2f %.2f ", pData->tracker1Data.x, pData->tracker1Data.y, pData->tracker1Data.z, pData->tracker1Data.azimuth, pData->tracker1Data.elevation, pData->tracker1Data.roll);
	fprintf(pFile,"%d %.2f %.2f ", pData->GTData.valid, pData->GTData.pupilX, pData->GTData.pupilY);
	fprintf(pFile,"%d %d %d ", pData->gloveData.thumb[0], pData->gloveData.thumb[1], pData->gloveData.thumb[2]);
	fprintf(pFile,"%d %d %d ", pData->gloveData.index[0], pData->gloveData.index[1], pData->gloveData.index[2]);
	fprintf(pFile,"%d %d %d ", pData->gloveData.middle[0], pData->gloveData.middle[1], pData->gloveData.middle[2]);
	fprintf(pFile,"%d %d %d ", pData->gloveData.ring[0], pData->gloveData.ring[1], pData->gloveData.ring[2]);
	fprintf(pFile,"%d %d %d ", pData->gloveData.pinkie[0], pData->gloveData.pinkie[1], pData->gloveData.pinkie[2]);
	fprintf(pFile,"%d %d %d %d ", pData->gloveData.abduction[0], pData->gloveData.abduction[1], pData->gloveData.abduction[2], pData->gloveData.abduction[3]);
	fprintf(pFile,"%d %d %d ", pData->gloveData.palmArch, pData->gloveData.wristPitch, pData->gloveData.wristYaw);
	fprintf(pFile,"%d %d %d %d ", pData->pressureData.channelA, pData->pressureData.channelB, pData->pressureData.channelC, pData->pressureData.channelD);
	fprintf(pFile, "\n");

}


void CSaverThread::writeHeaderToFile()
{

	fprintf(pFile,"t ");
	fprintf(pFile,"x0 y0 z0 az0 el0 ro0 ");
	fprintf(pFile,"x1 y1 z1 az1 el1 ro1 ");
	fprintf(pFile,"pupValid pupX pupY ");
	fprintf(pFile,"thumb1 thumb2 thumb3 ");
	fprintf(pFile,"index1 index2 index3 ");
	fprintf(pFile,"middle1 middle2 middle3 ");
	fprintf(pFile,"ring1 ring2 ring3 ");
	fprintf(pFile,"pinkie1 pinkie2 pinkie3 ");
	fprintf(pFile,"thInd indMid midRing ringPinkie ");
	fprintf(pFile,"palmArch wristPitch wristYaw ");
	fprintf(pFile,"pres1 pres2 pres3 pres4\n");

}
