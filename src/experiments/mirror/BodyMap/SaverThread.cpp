// SaverThread.cpp: implementation of the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SaverThread.h"
#include "BodyMap.h"

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

	nFrames = 0;

	while ( ! IsTerminated() ) {

		nFrames++;

		// must use synchronous version, otherwise there is no guarantee
		// that data will be synchronised.

		if ( useTracker0 ) {
			p_data_inport->Read();
			*pData = p_data_inport->Content();
		}
		if ( useCamera0 ) {
			p_img0_inport->Read();
			pImg0->Refer(p_img0_inport->Content());
			FindTrackerXY(*pImg0, &x0, &y0);
		}
		if ( useCamera1 ) {
			p_img1_inport->Read();
			pImg1->Refer(p_img1_inport->Content());
			FindTrackerXY(*pImg1, &x1, &y1);
		}

		writeDataToFile(nFrames);

	}
		
}

void CSaverThread::writeHeaderToFile()
{

	fprintf(pFile,"N;");
	fprintf(pFile,"X;Y;Z;");
	fprintf(pFile,"X0;Y0;");
	fprintf(pFile,"X1;Y1\n");

}

void CSaverThread::writeDataToFile(int i)
{

	fprintf(pFile,"%d;",i);
	fprintf(pFile,"%.1f;%.1f;%.1f;", pData->tracker0Data.x, pData->tracker0Data.y, pData->tracker0Data.z);
	fprintf(pFile,"%d;%d;", x0, y0);
	fprintf(pFile,"%d;%d\n", x1, y1);

}
