#ifndef __YARPSALIENCE__
#define __YARPSALIENCE__

#include <yarp/YARPImage.h>
#include <yarp/YARPlogpolar.h>
#include <yarp/YARPBabybotHeadKin.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPIntegralImage.h>
#include "YARPColorVQ.h"


class YARPBox {
public:
	YARPBox() { valid = false; }

	~YARPBox() {}

	bool operator==(const YARPBox& x)
	{
		bool ret = true;
		ret &= (xmax == x.xmax);
		ret &= (xmin == x.xmin);
		ret &= (ymax == x.ymax);
		ret &= (ymin == x.ymin);
		ret &= (cmax == x.cmax);
		ret &= (cmin == x.cmin);
		ret &= (rmax == x.rmax);
		ret &= (rmin == x.rmin);
		return ret;
	}

	bool valid;

	// logpolar
	int cmax, rmax;
	int cmin, rmin;
	int areaLP;
	
	// cartesian
	int xmax, ymax;
	int xmin, ymin;
	double areaCart;
	int xsum, ysum;
	double centroid_x;
	double centroid_y;

	bool cutted;
	
	long int id;

	//bool edge;

	// I use long to allow blob of dimension>255, but in MSVC int is = long!
	unsigned long int rgSum;
	unsigned long int grSum;
	unsigned long int bySum;
	
	unsigned long int rSum;
	unsigned long int gSum;
	unsigned long int bSum;
	
	YarpPixelMono meanRG;
	YarpPixelMono meanGR;
	YarpPixelMono meanBY;

	YarpPixelBGR meanColors;
	
	unsigned char cRG;
	unsigned char cGR;
	unsigned char cBY;

	int salienceBU;
	int salienceTD;
	YarpPixelMono salienceTotal;

	double elev;
	double az;

	double cmp;
	double ect;

	int indexCutted;
};


class YARPSalience {
	YARPBox *m_boxes;
	
	int imageSize;

	bool *_checkCutted;
	YarpPixelInt *_indexCutted;

	int height, width;

	YARPLogpolar m_lp;

	YARPImageOf<YarpPixelMono> tmp;
	IplImage *tmpMsk;

	YARPIntegralImage integralRG;
	YARPIntegralImage integralGR;
	YARPIntegralImage integralBY;

	YARPColorVQ colorVQ;

	YARPBabybotHeadKin _gaze;

	inline int TotalArea(YARPBox& box) { return (box.xmax - box.xmin + 1) * (box.ymax - box.ymin + 1); }

public:
	//YARPWatershed::YARPWatershed();
	YARPSalience::~YARPSalience();
	YARPSalience::YARPSalience(const int width1, const int height1);
	void resize(const int width1, const int height1);

	void blobCatalog(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r1, YARPImageOf<YarpPixelMono> &g1, YARPImageOf<YarpPixelMono> &b1, int last_tag);
	inline void removeFoveaBlob(YARPImageOf<YarpPixelInt>& tagged) {m_boxes[tagged(0, 0)].valid=false;}
	void removeBlobList(char *blobList, int max_tag);
	void updateFoveaBlob(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag);
	void SortAndComputeSalience(int num_tag, int last_tag);
	void ComputeSalience(int num_blob, int last_tag);
	void ComputeSalienceAll(int num_blob, int last_tag);
	void checkIOR(YARPImageOf<YarpPixelInt>& tagged, YARPBox* boxes, int num);
	void ComputeMeanColors(int last_tag);
	void RemoveNonValid(int last_tag, const int max_size, const int min_size);
	void DrawMeanColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged);
	void DrawMeanOpponentColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged);
	void DrawVQColor(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged);
	int DrawContrastLP(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby);
	int DrawContrastLP2(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby);
	//int DrawGrayLP(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int numBlob);
	void drawFoveaBlob(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, const YarpPixelMono gray=255);
	void drawBlobList(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag, const YarpPixelMono gray=255);
	void drawBlobListRandom(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag);
	void drawBlobListMask(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag);
	void SeedColor(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int x, int y, int col);
	
	void findNeighborhood(YARPImageOf<YarpPixelInt>& tagged, int x, int y, char *blobList, int max_tag);
	void fuseFoveaBlob(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag);
	void fuseFoveaBlob2(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag);
	void fuseFoveaBlob3(YARPImageOf<YarpPixelInt>& tagged, char *blobList, YarpPixelBGR var, int max_tag);
	int countSmallBlobs(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag, int min_size);
	void mergeBlobs(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag, int numBlob);
	YarpPixelBGR varBlob(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, int tag);
	void statBlobList(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag, YARPBox &blob);
	
	void centerOfMassAndMass(YARPImageOf<YarpPixelInt> &in, YarpPixelInt tag, int *x, int *y, double *mass);
	inline void getBlobXY(YARPImageOf<YarpPixelInt>& tagged, int x, int y, YARPBox &blob)
		{blob=m_boxes[tagged(x, y)];}
	inline void getBlobNum(int num, YARPBox &blob)
		{blob=m_boxes[num];}
	inline YARPBox & getBlobNum(int num)
		{return m_boxes[num];}


	void maxSalienceBlobs(YARPImageOf<YarpPixelInt>& tagged, int max_tag, YARPBox* boxes, int num);
	void maxSalienceBlob(YARPImageOf<YarpPixelInt>& tagged, int max_tag, YARPBox &box);

	void doIOR(YARPImageOf<YarpPixelInt>& tagged, YARPBox* boxes, int num);
	void drawIOR(YARPImageOf<YarpPixelMono>& out, YARPBox* boxes, int num);
	void foveaBlob(YARPImageOf<YarpPixelInt>& tagged, YARPBox &box);

	void setPosition(const YVector &p) { _gaze.update(p); }
	inline bool isWithinRange(int x, int y, double &elev, double &az)
	{
		_gaze.computeRay(YARPBabybotHeadKin::KIN_LEFT_PERI, elev, az , x, y);
		//if (elev<2.*PI*(-65.)/360. || elev>2.*PI*(-20.)/360. || az<2.*PI*(-40.)/360. || az>2.*PI*50./360.)
		if (elev<2.*PI*(-65.)/360. || elev>2.*PI*(-30.)/360. || az<2.*PI*(-40.)/360. || az>2.*PI*50./360.)
			return false;
		else
			return true;
	}

	void blobs2Peaks(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono>& edge, const int min, const int last_tag);
};

#endif