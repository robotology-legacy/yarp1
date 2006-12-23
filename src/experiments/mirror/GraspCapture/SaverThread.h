// SaverThread.h: interface for the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_)
#define AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSaverThread : public YARPThread  
{
public:
	virtual void Body();
	
	void writeDataToFile(int i);
	void writeHeaderToFile();

	int useDataGlove;
	int useGazeTracker;
	int useTracker0;
	int useTracker1;
	int usePresSens;
	int useCamera0;
	int useCamera1;

	CollectorNumericalData *pData;
	CollectorImage         *pImg0;
	CollectorImage         *pImg1;

	char prefix[255];
	FILE *pFile;

	YARPInputPortOf<CollectorNumericalData>* p_data_inport;
	YARPInputPortOf<YARPGenericImage>*       p_img0_inport;
	YARPInputPortOf<YARPGenericImage>*       p_img1_inport;

private:
	int _nFrames;
// -------------------- tmp BEGIN
int _graspCount;
// -------------------- tmp BEGIN
};

#endif // !defined(AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_)
