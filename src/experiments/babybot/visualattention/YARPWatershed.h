#ifndef __YARPWATERSHED__
#define __YARPWATERSHED__

#include <YARPImage.h>
#include <YARPlogpolar.h>


class YARPBox {
public:
	YARPBox() {	valid = false; }

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

	// cartesian
	int xmax, ymax;
	int xmin, ymin;

	// all in cartesian.
	int areaLP;
	double areaCart;
	int xsum, ysum;
	int centroid_x;
	int centroid_y;

	long int id;

	//bool edge;

	// I use long to allow blob of dimension>255
	unsigned long int rgSum;
	unsigned long int grSum;
	unsigned long int bySum;
	
	unsigned long int rSum;
	unsigned long int gSum;
	unsigned long int bSum;
	
	unsigned char meanRG;
	unsigned char meanGR;
	unsigned char meanBY;

	YarpPixelBGR meanColors;
	
	unsigned char cRG;
	unsigned char cGR;
	unsigned char cBY;

	int salienceBU;
	int salienceTD;
};


class YARPWatershed {
	YARPBox *m_boxes;
	YARPBox *m_attn;
	
	int neighSize;
	int *neigh;
	int *neighL;
	int *neighR;
	int padding;
	//YARPImageOf<YarpPixelMono> borderLUT;
	//unsigned char *p_borderLUT;
	int imageSize;
	//int imageSizePad;

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

    void createNeighborhood(const int widthStep, const bool neigh8);
	//void initBorderLUT(const int width, const int height);
	//inline bool invalidNeighbor(const int currentPoint, const int currentNeighbor) const;
	//inline bool validNeighbor(const int currentPoint, const int currentNeighbor) const;
	int markMinimas(YARPImageOf<YarpPixelInt>& result);
	void letsRain(YARPImageOf<YarpPixelInt>& result);
	void findLowerNeigh(const YARPImageOf<YarpPixelMono>& src);
	void createTmpImage(const YARPImageOf<YarpPixelMono>& src);
	inline int TotalArea(YARPBox& box) { return (box.xmax - box.xmin + 1) * (box.ymax - box.ymin + 1); }

public:
	YARPWatershed::YARPWatershed();
	YARPWatershed::YARPWatershed(const int width1, const int height1, const int wstep, const YarpPixelMono th);
	void resize(const int width1, const int height1, const int wstep, const YarpPixelMono th);

	void setThreshold(YarpPixelMono th) {threshold=th;}

	//bool apply(YARPImageOf<YarpPixelMono>& srcdest);
	//bool apply(const YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& dest);
	int apply(const YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &result);
	int applyOnOld(const YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &result);

	//void tags2Watershed(const YARPImageOf<YarpPixelInt>& src, YARPImageOf<YarpPixelMono>& dest);

	void blobCatalog(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r1, YARPImageOf<YarpPixelMono> &g1, YARPImageOf<YarpPixelMono> &b1, int last_tag);
	void removeFoveaBlob(YARPImageOf<YarpPixelInt>& tagged) {m_boxes[tagged(0, 0)].valid=false;}
	void SortAndComputeSalience(int num_tag, int last_tag);
	void ComputeSalience(int num_blob, int last_tag);
	void ComputeSalienceAll(int num_blob, int last_tag);
	void ComputeMeanColors(int last_tag);
	void RemoveNonValid(int last_tag, const int max_size, const int min_size);
	int DrawMeanColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged);
	int DrawMeanOpponentColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged);
	int DrawContrastLP(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby);
	int DrawGrayLP(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int numBlob);
	void DrawFoveaBlob(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, const YarpPixelMono gray=255);
	void drawBlobList(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag, const YarpPixelMono gray=255);
	void SeedColor(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int x, int y, int col);
	void findNeighborhood(YARPImageOf<YarpPixelInt>& tagged, int x, int y, bool *blobList, int max_tag);
	void fuseFoveaBlob(YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag);
	void fuseFoveaBlob2(YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag);
};

#endif