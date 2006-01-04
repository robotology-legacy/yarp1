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

		if ( useDataGlove || useTracker0 || useTracker1 || usePresSens ) {
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
	fprintf(pFile,";%.3f;%.3f;%.3f;%.3f;%.3f;%.3f", pData->tracker0.x, pData->tracker0.y, pData->tracker0.z, pData->tracker0.azimuth, pData->tracker0.elevation, pData->tracker0.roll);
	fprintf(pFile,";%.3f;%.3f;%.3f;%.3f;%.3f;%.3f", pData->tracker1.x, pData->tracker1.y, pData->tracker1.z, pData->tracker1.azimuth, pData->tracker1.elevation, pData->tracker1.roll);
	fprintf(pFile,";%d;%d;%d", pData->glove.thumb[0], pData->glove.thumb[1], pData->glove.thumb[2]);
	fprintf(pFile,";%d;%d;%d", pData->glove.index[0], pData->glove.index[1], pData->glove.index[2]);
	fprintf(pFile,";%d;%d;%d", pData->glove.middle[0], pData->glove.middle[1], pData->glove.middle[2]);
	fprintf(pFile,";%d;%d;%d", pData->glove.ring[0], pData->glove.ring[1], pData->glove.ring[2]);
	fprintf(pFile,";%d;%d;%d", pData->glove.pinkie[0], pData->glove.pinkie[1], pData->glove.pinkie[2]);
	fprintf(pFile,";%d;%d;%d;%d;%d", pData->glove.abduction[0], pData->glove.abduction[1], pData->glove.abduction[2], pData->glove.abduction[3], pData->glove.abduction[4]);
	fprintf(pFile,";%d", pData->glove.palmArch);
	fprintf(pFile,";%d;%d", pData->glove.wrist[0], pData->glove.wrist[1]);
	fprintf(pFile,";%d;%d;%d;%d", pData->pressure.channelA, pData->pressure.channelB, pData->pressure.channelC, pData->pressure.channelD);
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

