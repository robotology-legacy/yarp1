#ifndef __YARPWATERSHED__
#define __YARPWATERSHED__

#include <yarp/YARPImage.h>
#include <yarp/YARPlogpolar.h>
#include <yarp/YARPBabybotHeadKin.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPIntegralImage.h>
#include "YARPColorVQ.h"


class YARPWatershed {
	int neighSize;
	int *neigh;
	int *neighL;
	int *neighR;
	int padding;
	int imageSize;

	YarpPixelMono threshold;

	bool neighborhood8;

	int height, width;
	int widthStep;

	int watershedColor;
	int basinColor;

	YARPLogpolar m_lp;

	YARPImageOf<YarpPixelInt> downPos2;
	YARPImageOf<YarpPixelInt> downPos;
	YARPImageOf<YarpPixelMono> tSrc;

	YARPImageOf<YarpPixelMono> tmp;
	IplImage *tmpMsk;

	YARPColorVQ colorVQ;

    void createNeighborhood(const int widthStep, const bool neigh8);
	//void initBorderLUT(const int width, const int height);
	//inline bool invalidNeighbor(const int currentPoint, const int currentNeighbor) const;
	//inline bool validNeighbor(const int currentPoint, const int currentNeighbor) const;
	int markMinimas(YARPImageOf<YarpPixelInt>& result);
	void letsRain(YARPImageOf<YarpPixelInt>& result);
	void findLowerNeigh(const YARPImageOf<YarpPixelMono>& src);
	void createTmpImage(const YARPImageOf<YarpPixelMono>& src);

public:
	//YARPWatershed::YARPWatershed();
	YARPWatershed::~YARPWatershed();
	YARPWatershed::YARPWatershed(const int width1, const int height1, const int wstep, const YarpPixelMono th);
	void resize(const int width1, const int height1, const int wstep, const YarpPixelMono th);

	void setThreshold(YarpPixelMono th) {threshold=th;}

	//bool apply(YARPImageOf<YarpPixelMono>& srcdest);
	//bool apply(const YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& dest);
	int apply(const YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &result);
	int applyOnOld(const YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &result);

	void tags2Watershed(const YARPImageOf<YarpPixelInt>& src, YARPImageOf<YarpPixelMono>& dest);
	void findNeighborhood(YARPImageOf<YarpPixelInt>& tagged, int x, int y, char *blobList, int max_tag);
};

#endif