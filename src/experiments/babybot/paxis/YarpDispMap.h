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
///                            #Bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///

// YARPDisparityMap.h: interface for the YARPDisparityMap class.
//
//////////////////////////////////////////////////////////////////////

#ifndef YARPDISPARITYMAPH
#define YARPDISPARITYMAPH

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ipl/ipl.h>

#include <yarp/YARPImages.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPMatrix.h>

/*#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif*/

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define DEFORMATION 

class YARPDispMap  
{
public:

	struct node {
		int no_of_nodes;
		int no_of_prev_nodes;
		int * foll_nodes;
		float * arcweight;
		bool valid;
		double pathweight;
		int prevNode;
		int left;
		int right;
	};

	int _xSize;
	int _ySize;
	int _imgSize;
	int _totSize;
	int _dilMask;

	int _epsVal;

	float _alpha;
	float _beta;
	float _epsScale;

	int _minDisp;
	int _maxDisp;
	int _deltaDisp;

//	YARPImageOf<YarpPixelMono>  _dispImg;
	YARPImageOf<YarpPixelMono>  _idispImg;
//	YARPImageOf<YarpPixelMono>  _maskImg;
	YARPImageOf<YarpPixelMono>  _imaskImg;
	YARPImageOf<YarpPixelMono>  _tempImg;
	YARPImageOf<YarpPixelMono>  _dilatImg;

	YARPImageOf<YarpPixelMono>  _DataSet1;
	YARPImageOf<YarpPixelMono>  _DataSet2;

	YARPImageOf<YarpPixelFloat> _fTempImg;
	YARPImageOf<YarpPixelFloat> _coeffArray;
	YARPImageOf<YarpPixelFloat> _epsImg;

	int  * _sobel;
	int  * _dispOffset;
	int  * _tmpDisp;
	unsigned char * _oneLine;

	void dispWrapper(	YARPImageOf<YarpPixelMono>  & left, 
						YARPImageOf<YarpPixelMono>  & right,
						YARPImageOf<YarpPixelMono>  & mask,
						YARPImageOf<YarpPixelMono>  & disparity);

	void dynProgr(int totNodes, YARPDispMap::node * graph);

	void edgePreservingSmooth(unsigned char * inImg);

	void convertUCharToFloat(unsigned char * ucImg, float * fImg);
	void iterativeEps();

	void getDisparity(	unsigned char * img1, 
						unsigned char * img2,
						unsigned char * disparity);

	void getImageSet(	unsigned char * inImg,
						unsigned char * outDataSet);

	void getRightSegm(unsigned char * inImg, unsigned char * outImg);

	void invertDisparity(unsigned char * idisp);
	void downSampleImages(unsigned char * inImg, unsigned char * outImg, int imgSize);

	void buildGraph2D(	node * graph, 
						int length1, int length2,
						int Offs1, int Offs2, 
						int * prevDisp, int * totNodes, int * totArcs);

	void assignWeights(	node * graph, int length, 
						unsigned char * data1, unsigned char * data2,
						int * Sobel, int * tmpDisparity, float alpha, int totArcs, int totNodes);

	float computeCost(unsigned char left, unsigned char right, unsigned char nRight, unsigned char pRight);
	float computeInterCost(float alpha, int Sobel,int prevdisp, int currdisp);

	YARPDispMap();
	virtual ~YARPDispMap();


};

#endif
