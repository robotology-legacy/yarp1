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
		
	while (!IsTerminated())
	{
		
		nFrames++;
		if ( useDataGlove || useTracker || usePresSens)
		{
			p_data_inport->Read();
			*pData = p_data_inport->Content();
			writeDataToFile(nFrames);
		}
		if ( useCamera)
		{	
			p_img_inport->Read();
			pImg->Refer((p_img_inport->Content()));
			ACE_OS::sprintf(fName,"%s_%03d.pgm", prefix, nFrames);
			YARPImageFile::Write(fName, *pImg,YARPImageFile::FORMAT_PPM);
		}
		
	}
	

	return;
}

void CSaverThread::writeDataToFile(int i)
{
	fprintf(pFile,"%d",i);
	fprintf(pFile,";%f;%f;%f;%f;%f;%f", pData->tracker.x, pData->tracker.y, pData->tracker.z, pData->tracker.azimuth, pData->tracker.elevation, pData->tracker.roll);
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
	fprintf(pFile,"frameN;" );
	fprintf(pFile,"X;Y;Z;Azimuth;Elevation;Roll;" );
	fprintf(pFile,"ThumbInner;ThumbMiddle;ThumbOuter;" );
	fprintf(pFile,"IndexInner;IndexMiddle;IndexOuter;" );
	fprintf(pFile,"MiddleInner;MiddleMiddle;MiddleOuter;" );
	fprintf(pFile,"RingInner;RingMiddle;RingOuter;" );
	fprintf(pFile,"PinkieInner;PinkieMiddle;PinkieOuter;" );
	fprintf(pFile,"Abduct1;Abduct2;Abduct3;Abduct4;Abduct5;" );
	fprintf(pFile,"PalmArch;" );
	fprintf(pFile,"Wrist1;Wrist2;" );
	fprintf(pFile,"PressureA;PressureB;PressureC;PressureC\n" );
}

int CSaverThread::getSavedFramesN()
{
	return nFrames;
}