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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDisparity.h,v 1.10 2004-04-26 10:26:29 babybot Exp $
///
///
// disparity.h: interface for the YARPDisparityTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPARITY_H__E3019D2A_4BC3_4AD0_8355_AAA601391249__INCLUDED_)
#define AFX_DISPARITY_H__E3019D2A_4BC3_4AD0_8355_AAA601391249__INCLUDED_

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPAll.h>
#include <YARPImage.h>
#include <YARPImageDraw.h>
#include <YARPSemaphore.h>
#include <YARPTime.h>
#include <YARPImages.h>
#include <YARPMatrix.h>

#include <LogPolarSDK.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
///
///

class YARPDisparityTool
{
private:

public:
	Image_Data _imgL,_imgS;

	Image_Data lpInfo (int SXR, int SYR, int rho, int theta, int fovea, int res, double ratio, int pad);

	int   _shiftLevels;
	int * _shiftMap;
	int * _shiftFunction;
	IntNeighborhood * _dsTable;
	double * _corrFunct;
	double * _gaussFunction;
	double _corrTreshold;

	int *_count;
		
	int _actRings;
	double _gMean;
	double _gSigma;
	double _gMagn;
	double _squareError;
	double _snRatio;


	inline int getShiftLevels()
		{ return _shiftLevels; }
	inline const double *getCorrFunction()
		{ return _corrFunct; }

	void setRings(int r)
	{ _actRings = r; }

	int loadShiftTable(Image_Data* Par);
	int loadDSTable(Image_Data* Par);

	void downSample(YARPImageOf<YarpPixelBGR> & inImg, YARPImageOf<YarpPixelBGR> & outImg);
	int computeDisparity (YARPImageOf<YarpPixelBGR> & inRImg,
						 YARPImageOf<YarpPixelBGR> & inLImg);

	int computeDisparityRGB (YARPImageOf<YarpPixelBGR> & inRImg,
						     YARPImageOf<YarpPixelBGR> & inLImg);

	int corrAdjust(int * count);
	double computeSNRatio(int disparity);

	void makeHistogram(YARPImageOf<YarpPixelMono> & hImg);

	void (YARPDisparityTool::*fittingFunct)(double, YVector&, double *, YVector&, int);	

	void gaussian(double x, YVector& a, double *y, YVector& dyda, int na);

	void findFittingFunction();
	int functFitting(YVector& x,
					 double y[],
					 YVector& sig,
					 int ndata,
					 YVector& a,
					 bool* ia,
					 int ma,
					 YMatrix& covar,
					 YMatrix& alpha,
					 double *chisq,
					 void (YARPDisparityTool::*fittingFunct) (double,YVector&,double*,YVector&,int),
					 double *alambda);


	void covSwap(YMatrix& covar, int ma, bool * ia, int mfit);

	int gaussJordan(YMatrix& a, int n, YMatrix& b, int m);

		
	void mrqCof(YVector& x, 
				double y[], 
				YVector& sig, 
				int ndata, 
				YVector& a, 
				bool * ia,
				int ma, 
				YMatrix& alpha, 
				YVector& beta, 
				double *chisq,
				void (YARPDisparityTool::*fittingFunct)(double, YVector&, double *, YVector&, int));


	YARPDisparityTool();
	virtual ~YARPDisparityTool();

protected:
	char _path[256];
};

#endif // !defined(AFX_DISPARITY_H__E3019D2A_4BC3_4AD0_8355_AAA601391249__INCLUDED_)
