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
	nFrames = 0;

	while (!IsTerminated()) {

		nFrames++;

		if ( useDataGlove || useGazeTracker || useTracker0 || useTracker1 || usePresSens ) {
			p_data_inport->Read();
			*pData = p_data_inport->Content();
			writeDataToFile(nFrames);
		}

		if ( useCamera0 || useCamera1 ) {

			p_img0_inport->Read();
			pImg0->Refer((p_img0_inport->Content()));
			ACE_OS::sprintf(fName,"%s_0_%03d.pgm", prefix, nFrames);
			YARPImageFile::Write(fName, *pImg0,YARPImageFile::FORMAT_PPM);

			p_img1_inport->Read();
			pImg1->Refer((p_img1_inport->Content()));
			ACE_OS::sprintf(fName,"%s_1_%03d.pgm", prefix, nFrames);
			YARPImageFile::Write(fName, *pImg1,YARPImageFile::FORMAT_PPM);

		}
		
	}

	return;
}

void CSaverThread::writeDataToFile(int i)
{

	fprintf(pFile,"%d",i);
	fprintf(pFile,";%.3f;%.3f;%.3f;%.3f;%.3f;%.3f", pData->tracker0Data.x, pData->tracker0Data.y, pData->tracker0Data.z, pData->tracker0Data.azimuth, pData->tracker0Data.elevation, pData->tracker0Data.roll);
	fprintf(pFile,";%.3f;%.3f;%.3f;%.3f;%.3f;%.3f", pData->tracker1Data.x, pData->tracker1Data.y, pData->tracker1Data.z, pData->tracker1Data.azimuth, pData->tracker1Data.elevation, pData->tracker1Data.roll);
	fprintf(pFile,";%d;%d;%d", pData->GTData.pupilDiam, pData->GTData.pupilX, pData->GTData.pupilY);
	fprintf(pFile,";%d;%d;%d", pData->gloveData.thumb[0], pData->gloveData.thumb[1], pData->gloveData.thumb[2]);
	fprintf(pFile,";%d;%d;%d", pData->gloveData.index[0], pData->gloveData.index[1], pData->gloveData.index[2]);
	fprintf(pFile,";%d;%d;%d", pData->gloveData.middle[0], pData->gloveData.middle[1], pData->gloveData.middle[2]);
	fprintf(pFile,";%d;%d;%d", pData->gloveData.ring[0], pData->gloveData.ring[1], pData->gloveData.ring[2]);
	fprintf(pFile,";%d;%d;%d", pData->gloveData.pinkie[0], pData->gloveData.pinkie[1], pData->gloveData.pinkie[2]);
	fprintf(pFile,";%d;%d;%d;%d;%d", pData->gloveData.abduction[0], pData->gloveData.abduction[1], pData->gloveData.abduction[2], pData->gloveData.abduction[3], pData->gloveData.abduction[4]);
	fprintf(pFile,";%d", pData->gloveData.palmArch);
	fprintf(pFile,";%d;%d", pData->gloveData.wrist[0], pData->gloveData.wrist[1]);
	fprintf(pFile,";%d;%d;%d;%d", pData->pressureData.channelA, pData->pressureData.channelB, pData->pressureData.channelC, pData->pressureData.channelD);
	fprintf(pFile, "\n");

}


void CSaverThread::writeHeaderToFile()
{

	fprintf(pFile,"frameN;");
	fprintf(pFile,"X0;Y0;Z0;Azimuth0;Elevation0;Roll0;");
	fprintf(pFile,"X1;Y1;Z1;Azimuth1;Elevation1;Roll1;");
	fprintf(pFile,"ThumbInner;ThumbMiddle;ThumbOuter;");
	fprintf(pFile,"IndexInner;IndexMiddle;IndexOuter;");
	fprintf(pFile,"MiddleInner;MiddleMiddle;MiddleOuter;");
	fprintf(pFile,"RingInner;RingMiddle;RingOuter;");
	fprintf(pFile,"PinkieInner;PinkieMiddle;PinkieOuter;");
	fprintf(pFile,"Abduct1;Abduct2;Abduct3;Abduct4;Abduct5;");
	fprintf(pFile,"PalmArch;");
	fprintf(pFile,"Wrist1;Wrist2;");
	fprintf(pFile,"PressureA;PressureB;PressureC;PressureC\n");

}

int CSaverThread::getSavedFramesN()
{

	return nFrames;

}

