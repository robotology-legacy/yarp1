// YARPSusanFilter.h: interface for the YARPSusanFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YARPSUSANFILTER_H__3D4DFFA6_3028_493D_9897_3904A3C5C6CE__INCLUDED_)
#define AFX_YARPSUSANFILTER_H__3D4DFFA6_3028_493D_9897_3904A3C5C6CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//=============================================================================
// YARP Includes
//=============================================================================
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPImage.h>

//=============================================================================
// System Includes
//=============================================================================
#include <math.h>

#define MAX_N_EDGES		2650
#define NO_F2I_ROUNDING

class YARPSusanFilter  
{
public:
	void resize(int sizeX, int sizeY);
	bool apply(YARPImageOf<YarpPixelBGR> *inImg, YARPImageOf<YarpPixelMono> *outImg, float brightnessThreshold=20.0, bool smallMask=false);
	bool apply(YARPImageOf<YarpPixelMono> *inImg, YARPImageOf<YarpPixelMono> *outImg, float brightnessThreshold=20.0, bool smallMask=false);
	YARPSusanFilter(int sizeX, int sizeY);
	YARPSusanFilter();
	virtual ~YARPSusanFilter();

private:
	void _bgr2Mono(unsigned char *dstMono, unsigned char *srcBGR);
	void _int2uchar(int *r, unsigned char *in, int size);
	void _susanSmall(unsigned char *in, int max_no);
	void _susan(unsigned char *in, int max_no);
	void _setupBrightnessLUT(float thresh, int form);

	int x_size;
	int y_size;
	int *r;
	unsigned char *bp;
	unsigned char *pLUTData;
	unsigned char *pMono;
};

#endif // !defined(AFX_YARPSUSANFILTER_H__3D4DFFA6_3028_493D_9897_3904A3C5C6CE__INCLUDED_)
