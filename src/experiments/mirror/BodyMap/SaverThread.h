// SaverThread.h: interface for the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_)
#define AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BodyMap.h"

class CSaverThread : public YARPThread  
{
public:
	CSaverThread(BodyMapOptions& programOptions,
		         BodyMapSettings& programSettings,
				 BodyMapLearningBlock& learningBlock) :
	  _options(&programOptions),
	  _settings(&programSettings),
	  _learningBlock(&learningBlock) {}

	~CSaverThread() {}
	
	virtual void Body();
	
private:

	int _x0, _y0, _w0, _x1, _y1, _w1;

	BodyMapOptions* _options;
	BodyMapSettings* _settings;
	BodyMapLearningBlock* _learningBlock;

};

#endif // !defined(AFX_SAVERTHREAD_H__F6E88ED1_F198_4E75_83FB_05438B0AD6A6__INCLUDED_)
