//
// YARPConverter.h
//	- used to import from the old image types.
//	- avoid using this for new code. 
//

#ifndef __YARPConverterh__
#define __YARPConverterh__

#ifdef WIN32
#error "*** Don't use this file under win32"
#endif

// it doesn't make sense under NT.
#ifdef __QNX__

#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPlogpolar.h"

//
//
// LATER: remove completely

#if 0
// there might be type clashes (need to be tested).
#	include "ImageIO.h"

//
class YARPConverter
{
public:
	static void ConvertFromCogToYARP (const GenericImage& src, YARPGenericImage& dst);
	static void ConvertFromYARPToCog (const YARPGenericImage& src, GenericImage& dst);
	static void ConvertFromYARPToCogRectangular (const YARPGenericImage& src, GenericImage& dst);

	static void PasteInto (const YARPImageOf<YarpPixelMono>& src, int x, int y, int zoom, Image<PixelMono>& dst);
	//static void ConvertFromYARPToCog (const YARPImageOf<YarpPixelRGB>& src, Image<PixelBGR>& dst);
};


//
// a few rather specialized converters.
class YARPLpConverter : public YARPLogPolar
{
private:
	YARPLpConverter (const YARPLpConverter&);
	void operator= (const YARPLpConverter&);

public:
	YARPLpConverter() : YARPLogPolar() {}
	YARPLpConverter(int necc,int nang,double rfmin,int size) : YARPLogPolar (necc, nang, rfmin, size) {}
	virtual ~YARPLpConverter () {}

	void Cart2LpAverage (const GenericImage& is, YARPGenericImage& id) const;
	void Cart2LpAverage(const YARPGenericImage& is, YARPGenericImage& id) const { YARPLogPolar::Cart2LpAverage (is, id); }
	void Lp2Cart(const YARPGenericImage& is, GenericImage& id) const;
	void Lp2Cart(const YARPGenericImage& is, YARPGenericImage& id) const { YARPLogPolar::Lp2Cart (is, id); }

	// swap channels.
	void Cart2LpAverageSwap(const Image<PixelBGR>& is, YARPImageOf<YarpPixelRGB>& id) const;
	void Lp2CartSwap(const YARPImageOf<YarpPixelRGB>& is, Image<PixelBGR>& id) const;
};

#endif	// closes #if 0
//
//
//

// things are getting really weird here.
#include "YARPBlobFinder.h"

// the old stuff.
#include "conf/tx_data.h"

class YARPBoxCopier
{
public:
	static void Copy (const YARPBox& src, CBox2Send& dst);
};

#endif	// __QNX__

#endif	// __YARPConverterh__