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
	int getSavedFramesN();

	int useDataGlove;
	int useTracker; 
	int usePresSens;
	int useCamera;
	YARPImageOf<YarpPixelBGR> *pImg;
	FILE *pFile;
	MNumData *pData;
	YARPInputPortOf<MNumData> *p_data_inport;
	YARPInputPortOf<YARPGenericImage> *p_img_inport;	
	char prefix[255];

private:
	int nFrames;
};

#endif // !defined(AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_)
