/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                            #bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id:
///
///

#ifndef __YARPIMGATT__
#define __YARPIMGATT__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <YARPImage.h>
#include <YARPLogpolar.h>
//#include <YARPBlobDetector.h>
//#include <YARPIntegralImage.h>
#include <YARPConicFitter.h>
#include "YARPConvKernel.h"
#include "YARPConvKernelFile.h"
//#include "YARPColorVQ.h"
//#include "YARPBlobFinder.h"
#include "YARPWatershed.h"



#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif


const int range = 256;

typedef struct { int x; int y; } Vett;

// la eredito da YARPFilter???
class YARPImgAtt
{
protected:
	int height, width, fovHeight;
	
	YARPImageOf<YarpPixelMonoSigned> scr_s;
	
	YARPImageOf<YarpPixelMono> r1;
	YARPImageOf<YarpPixelMono> g1;
	YARPImageOf<YarpPixelMono> b1;
	YARPImageOf<YarpPixelMono> i1;

	//YARPImageOf<YarpPixelMono> i1_7;
	YARPImageOf<YarpPixelMonoSigned> is1;

	YARPImageOf<YarpPixelMono> r1_g_c;
	YARPImageOf<YarpPixelMono> g1_g_c;
	YARPImageOf<YarpPixelMono> b1_g_c;
	YARPImageOf<YarpPixelMonoSigned> is1_g_c;
	YARPImageOf<YarpPixelMono> r1_g_s;
	YARPImageOf<YarpPixelMono> g1_g_s;
	YARPImageOf<YarpPixelMono> y1_g_s;
	YARPImageOf<YarpPixelMonoSigned> is1_g_s;

	//YARPImageOf<YarpPixelMonoSigned> ors[12];
	YARPImageOf<YarpPixelMono> or[4];
	//YARPImageOf<YarpPixelMono> or_r[4];

	YARPImageOf<YarpPixelMono> edge;
	//YARPImageOf<YarpPixelMono> edge2;
	
	YARPImageOf<YarpPixelMono> rg;
	YARPImageOf<YarpPixelMono> gr;
	YARPImageOf<YarpPixelMono> by;
	//YARPImageOf<YarpPixelMono> ii;

	YARPImageOf<YarpPixelMonoSigned> rgs;
	YARPImageOf<YarpPixelMonoSigned> grs;
	YARPImageOf<YarpPixelMonoSigned> bys;
	//YARPImageOf<YarpPixelMonoSigned> iis;

	YARPImageOf<YarpPixelMono> rgf;
	YARPImageOf<YarpPixelMono> grf;
	YARPImageOf<YarpPixelMono> byf;
	//YARPImageOf<YarpPixelMono> iif;

	YARPImageOf<YarpPixelMono> tmp1;
	YARPImageOf<YarpPixelMono> tmp2;
	YARPImageOf<YarpPixelMono> tmp3;
	YARPImageOf<YarpPixelMono> tmp4;
	YARPImageOf<YarpPixelMonoSigned> tmp1s;
	YARPImageOf<YarpPixelMonoSigned> tmp2s;
	
	/*YARPImageOf<YarpPixelMonoSigned> tmp7;
	YARPImageOf<YarpPixelMonoSigned> tmp8;
	YARPImageOf<YarpPixelMonoSigned> tmp9;
	YARPImageOf<YarpPixelMonoSigned> tmp10;
	YARPImageOf<YarpPixelMonoSigned> tmp11;
	YARPImageOf<YarpPixelMonoSigned> tmp12;
	YARPImageOf<YarpPixelMonoSigned> tmp13;
	YARPImageOf<YarpPixelMonoSigned> tmp14;
	YARPImageOf<YarpPixelMonoSigned> tmp15;
	YARPImageOf<YarpPixelMonoSigned> tmp16;*/

	//YARPImageOf<YarpPixelMonoSigned> tmpScale;
	
	YARPImageOf<YarpPixelBGR> meanOppCol;
	YARPImageOf<YarpPixelBGR> meanCol;

	YARPImageOf<YarpPixelBGR> tmpBGR1;
	//YARPImageOf<YarpPixelBGR> tmpBGR2;
	
	YARPImageOf<YarpPixelMono> map[4];
	
	//YARPImageOf<YarpPixelMono> *array1[8];
	YARPImageOf<YarpPixelMono> *array2[3];
	//YARPImageOf<YarpPixelMonoSigned> *array3[12];

	//YARPImageOf<YarpPixelMono> comb;

	YARPImageOf<YarpPixelMono> out;
	
	YARPImageOf<YarpPixelMono> blobFov;
	
	YARPImageOf<YarpPixelMono> oldWshed;

	YARPArrayConvKernel prewitt_diag1;
	YARPArrayConvKernel prewitt_diag2;
	//YARPArrayConvKernel prewitt_o_s;
	YARPArrayConvKernel sobel;
	
	YARPArrayConvKernel gauss_c_s;
	YARPArrayConvKernel gauss_s_s;

	IplLUT lut;

	IplConvKernel* scala;

	YARPImageOf<YarpPixelBGR> imgVQ;
	//YARPColorVQ colorVQ;
		
	//YARPBlobDetector blob;
	//YARPIntegralImage integral;

	YARPImageOf<YarpPixelInt> tagged;
	//YARPBlobFinder blobFinder;

	YARPWatershed rain;

	int max_tag;

	bool* blobList;

	YarpPixelMono searchRG;
	YarpPixelMono searchGR;
	YarpPixelMono searchBY;

	float salienceBU;
	float salienceTD;

	YARPLpConicFitter fit;

	int num_IORBoxes;

	char savename[512];
	
	inline void colorOpponency(YARPImageOf<YarpPixelBGR> &src);
	inline void findEdges();
	inline void normalize();
	inline void findBlobs();
	inline void quantizeColors();
	
	float DenteDiSega(short x);
	void Combine(YARPImageOf<YarpPixelMono> **src, int num, YARPImageOf<YarpPixelMono> &dst);
	void CombineMax(YARPImageOf<YarpPixelMono> **src, int num, YARPImageOf<YarpPixelMono> &dst);
	void CombineMaxAbs(YARPImageOf<YarpPixelMonoSigned> **src, int num, YARPImageOf<YarpPixelMono> &dst);
	void MinMax(IplImage* img, int &mn, int &mx);
	void FullRange(IplImage* img, IplImage* out, const int mn, const int mx);
	void FullRange2(IplImage* img, IplImage* out);
	void RealOrientations(YARPImageOf<YarpPixelMono> srcMap[4], YARPImageOf<YarpPixelMono> dstMap[4]);
	void RealOrientations2(YARPImageOf<YarpPixelMono> srcMap[4], YARPImageOf<YarpPixelMono> dstMap[4]);
	void WTA(YARPImageOf<YarpPixelMono> src[4]);
	void WTA2(short *p);
	void CreateOrMap(YARPImageOf<YarpPixelMono> map[4]);
	void ZeroLow(YARPImageOf<YarpPixelMono> &src, unsigned char th);
	void ZeroFovea(YARPGenericImage &dst, int val);
	void ZeroMask(YARPImageOf<YarpPixelMono> &dst, YARPImageOf<YarpPixelMono> &mask, int val, int xmin, int xmax, int ymin, int ymax);
	void MultiplyFloat(YARPImageOf<YarpPixelInt> &src, YARPImageOf<YarpPixelInt> &dst, const float c);
	void LineMax(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelMono> &dst);
	void lineMax2(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelMono> &dst);
	void LineStat(YARPImageOf<YarpPixelMono> &src, int *vett);
	
public:
	YARPImgAtt(int x, int y, int fovea, int num);
	~YARPImgAtt();
	
	void Apply(YARPImageOf<YarpPixelBGR> &src);
	void FindNMax(int num, Vett* pos);
	void FindMax(YARPImageOf<YarpPixelMono> &src, Vett &pos);
	void FindMax(YARPImageOf<YarpPixelInt> &src, Vett &pos);
	YARPImageOf<YarpPixelMono>& Saliency() { return out; }
	YARPImageOf<YarpPixelMono>& BlobFov() { return blobFov; }
	void GetTarget(int &x, int &y);
	void saveImages(YARPImageOf<YarpPixelBGR> &src);
	void setWatershedTh(YarpPixelMono th) { rain.setThreshold(th); }

	inline void setParameters(const YarpPixelMono sRG, const YarpPixelMono sGR, const YarpPixelMono sBY, const float sBU, const float sTD)
	{
		searchRG=sRG;
		searchGR=sGR;
		searchBY=sBY;

		salienceBU=sBU;
		salienceTD=sTD;
	}
	inline void setPosition(const YVector &p) { rain.setPosition(p); }
	void updateIORTable();
	void resetIORTable();
	void drawIORTable();

	YARPBox* IORBoxes;
	YARPBox* max_boxes;

	YARPBox fovBox;
};


#endif
