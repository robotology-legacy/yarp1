//
// YARPBlobFinder.h
//	- finds colored blobs by region growing in logpolar domain.
//	- input image must be logpolar.
//	- small defects from time to time.
//

#ifndef __YARPBlobFinderh__
#define __YARPBlobFinderh__

#include <YARPImage.h>
#include <YARPFilters.h>
#include <YARPlogpolar.h>
//#include "YARPLinkedList.h"

//
// bounding box type.
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

// find blobs (box shaped) with almost constant color.
// some params are not correctly used at the moment.
// LATER: improve tracking by measuring region (e.g. ave color) quantities.
//
class YARPBlobFinder : public YARPFilter
{
private:
	YARPBlobFinder (const YARPBlobFinder& x);
	void operator= (const YARPBlobFinder& x);

protected:
	//YARPColorSaliencyFilter m_saliency;
	YARPBox *m_boxes;
	YARPBox *m_attn;

	long int *arrayTags1;
	long int *arrayTags2;

	double *pixelSize;
	
	//YARPLinkedList* listsTags; 

	YARPLogpolar m_lp;
	
	YARPImageOf<YarpPixelHSV> m_hsv_enhanced;

	int last_tag, num_tag;

	int max_tag;

	int m_sat_thr;
	int m_hue_thr;

	int height, width;
	
	// helper functions.
	void MergeBoxes();
	double CenterDistance(int i, int j);
	double CombinedSize(int i, int j);
	void FuseBoxes(int i, int j);
	
	void DrawBox(IplImage *img, int x1, int y1, int x2, int y2);
	void DrawBox(IplImage *img, 
				  int x1, int y1, 
				  int x2, int y2,
				  int r, int g, int b);

	inline bool SimilarSaturation(unsigned char v1, unsigned char v2)
		{ return (abs (v1 - v2) < m_sat_thr) ? true : false; }

	inline bool SimilarHue(unsigned char v1, unsigned char v2)
		{ return (abs (v1 - v2) < m_hue_thr) ? true : false; }

	/*inline int HueDistance(int i, int j)
	{ 
		const double vi = double(m_boxes[i].total_sal) / double(m_boxes[i].areaLP);
		const double vj = double(m_boxes[j].total_sal) / double(m_boxes[j].areaLP);
		const int tmp = int(vi - vj + .5);
		return abs(tmp);
	}*/

	// cartesian area
	inline int TotalArea(YARPBox& box)
		{ return (box.xmax - box.xmin + 1) * (box.ymax - box.ymin + 1); }

	void MergeRegions(YARPImageOf<YarpPixelInt>& src, int rTag, int cTag);
	inline bool Overlap (int i, int j);

public:
	YARPBlobFinder(int necc, int nang);
	YARPBlobFinder();
	virtual ~YARPBlobFinder () { Cleanup(); }

	void Resize(int x, int y);
	
	virtual void Cleanup (void);
	virtual bool InPlace () const { return false; }

	//virtual void Apply(const YARPImageOf<YarpPixelRGB>& is, YARPImageOf<YarpPixelRGB>& out);
	//virtual void Apply(const YARPImageOf<YarpPixelBGR>& is, YARPImageOf<YarpPixelBGR>& out);
	virtual void Apply(YARPImageOf<YarpPixelMono>& is, YARPImageOf<YarpPixelInt>& id, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r, YARPImageOf<YarpPixelMono> &g, YARPImageOf<YarpPixelMono> &b, bool remove);

	void blobCatalog(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono>& img, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r, YARPImageOf<YarpPixelMono> &g, YARPImageOf<YarpPixelMono> &b, int lasttag);
	
	void RemoveNonValid();

	void ComputeSalience();

	//inline int GetWidth (void) const { return m_saliency.GetWidth(); }
	//inline int GetHeight (void) const { return m_saliency.GetHeight(); }

	YARPBox * GetBoxes(void) { return m_attn; }

	void DrawBoxes(YARPImageOf<YarpPixelRGB>& id);
	void DrawBoxes(YARPImageOf<YarpPixelBGR>& id);
	void DrawBoxes(YARPImageOf<YarpPixelMono>& id);
	
	void DrawBoxesLP(YARPImageOf<YarpPixelMono>& id);

	
	int DrawContrastLP(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float prg, float pgr, float pby);
	
	void ComputeMeanColors();
	int DrawMeanColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged);
	
	int DrawGrayLP(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int numBlob);
	void DrawFoveaBlob(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged);

	// this is the threshold used when tagging the original HSV image.
	inline void SetSaturationThreshold (int value) { m_sat_thr = value; }

	// this is the threshold used when tagging the HSV image and later on to
	// assess whether two boxes are similar enough to be merged.
	inline void SetHueThreshold (int value) { m_hue_thr = value; }

	// this is the threshold used to remove low-saturated areas.
	//inline void SetSaliencyThreshold (int value) { m_saliency.SetSaturationThreshold (value); }

	// this is for the dark areas.
	//inline void SetLumaSaliencyThreshold (int value) { m_saliency.SetLumaThreshold (value); }

	//void PrintBoxes (YARPBox *m_boxes, int size);

	int SpecialTagging(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono>& img);
	void RemoveUnsedTags(YARPImageOf<YarpPixelInt>& tags);

	void SeedColor(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int x, int y, int col);
};

//
// overlap is in pixels.
inline bool YARPBlobFinder::Overlap (int i, int j)
{
	YARPBox* in1 = m_boxes + i;
	YARPBox* in2 = m_boxes + j;

	const int overlap = 3;

	if (in2->xmin < in1->xmax + overlap &&
		in2->xmin > in1->xmin - overlap &&
		in2->ymin < in1->ymax + overlap &&
		in2->ymin > in1->ymin - overlap)
		return true;
	else
	if (in2->xmin < in1->xmax + overlap &&
		in2->xmin > in1->xmin - overlap &&
		in2->ymax < in1->ymax + overlap &&
		in2->ymax > in1->ymin - overlap)
		return true;
	else
	if (in2->xmax < in1->xmax + overlap &&
		in2->xmax > in1->xmin - overlap &&
		in2->ymax < in1->ymax + overlap &&
		in2->ymax > in1->ymin - overlap)
		return true;
	else
	if (in2->xmax < in1->xmax + overlap &&
		in2->xmax > in1->xmin - overlap &&
		in2->ymin < in1->ymax + overlap &&
		in2->ymin > in1->ymin - overlap)
		return true;

	return false;
}

#define min(a,b) (a<b?a:b)
#define max(a,b) (a<b?b:a)

#endif