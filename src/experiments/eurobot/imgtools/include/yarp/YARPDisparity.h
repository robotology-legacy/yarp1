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
///                    #fabio + nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDisparity.h,v 1.1 2004-12-27 15:20:49 beltran Exp $
///
///
// disparity.h: interface for the YARPDisparityTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPARITY_H__E3019D2A_4BC3_4AD0_8355_AAA601391249__INCLUDED_)
#define AFX_DISPARITY_H__E3019D2A_4BC3_4AD0_8355_AAA601391249__INCLUDED_

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPMatrix.h>

#include <yarp/LogPolarSDK.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

const int __nShifts = 4;

///
///
///

class YARPDisparityTool
{
public:
	YARPDisparityTool();
	virtual ~YARPDisparityTool();

	Image_Data lpInfo (int SXR, int SYR, int rho, int theta, int fovea, int res, double ratio, int pad, int zoom = 1);

	inline int getShiftLevels()
		{ return _shiftLevels; }
	inline const double *getCorrFunction()
		{ return _corrFunct; }
	inline const double *getPhase()
		{ return _phase; }
	inline const double *getCoeff()
		{ return _coeff;}
	inline const double *getStd1()
		{ return _std1;}
	inline const double *getStd2()
		{ return _std2;}
	inline int getShift(int i = 0)
		{ return _shifts[i]; }
	
	inline const double *getSSDFunction()
		{ return _ssdFunct; }

	inline const double *getVarFunction()
		{ return _varFunct; }

	inline void setInhibition(int max, int min)
	{ _shiftMax = max; _shiftMin = min; }

	int init(int rings, float ratio);

	int loadShiftTable()
		{ return loadShiftTable(&_imgS); }

	int loadDSTable()
		{ return loadDSTable(&_imgL, &_imgS); }

	int loadRemapTable()
		{ return loadRemapTable(&_imgFovea); }

	int zeroShift()
	{ return (_shiftLevels/2); }

	int shiftLevels()
	{ return _shiftLevels; }

	// to keep compatinbility with old code (if any...)
	int loadShiftTable(Image_Data* Par);
	int loadDSTable(Image_Data* Par, Image_Data* ParDS);
	int loadRemapTable(Image_Data* Par);

	void downSample(YARPImageOf<YarpPixelBGR> &inImg, YARPImageOf<YarpPixelBGR> & outImg);
	void remap(YARPImageOf<YarpPixelBGR> &inImg, YARPImageOf<YarpPixelBGR> &out);
	int computeMono (YARPImageOf<YarpPixelBGR> & inRImg,
						 YARPImageOf<YarpPixelBGR> & inLImg, 
						 double *value);

	int computeCorrProd (YARPImageOf<YarpPixelBGR> & inRImg,
						YARPImageOf<YarpPixelBGR> & inLImg,
						double *value);

	int computeMonoNorm (YARPImageOf<YarpPixelBGR> & inRImg,
							  YARPImageOf<YarpPixelBGR> & inLImg, 
							   double *value);

	int computeSSDMono (YARPImageOf<YarpPixelBGR> & inRImg,
						 YARPImageOf<YarpPixelBGR> & inLImg, 
						 double *value);

	int computeSSDRGB (YARPImageOf<YarpPixelBGR> & inRImg,
						 YARPImageOf<YarpPixelBGR> & inLImg, 
						 double *value);

	int computeSSDRGBUN (YARPImageOf<YarpPixelBGR> & inRImg,
   						YARPImageOf<YarpPixelBGR> & inLImg, double *value);

	int computeSSDRGBxVar (YARPImageOf<YarpPixelBGR> & inRImg,
						  YARPImageOf<YarpPixelBGR> & inLImg, 
						  double *value);
	
	int computeSSDRGBxVar2 (YARPImageOf<YarpPixelBGR> & inRImg,
   							YARPImageOf<YarpPixelBGR> & inLImg, double *value);

	int computeSSDRGBVar (YARPImageOf<YarpPixelBGR> & inRImg,
						  YARPImageOf<YarpPixelBGR> & inLImg, 
							double *value);

	int computeSSDRGBTH (YARPImageOf<YarpPixelBGR> & inRImg,
						  YARPImageOf<YarpPixelBGR> & inLImg, 
							double *value);

	int computeABSRGB (YARPImageOf<YarpPixelBGR> & inRImg,
						  YARPImageOf<YarpPixelBGR> & inLImg, 
							double *value);

	int computeSSDWorstCase (YARPImageOf<YarpPixelBGR> & inRImg,
									  YARPImageOf<YarpPixelBGR> & inLImg, 
										double *value);

	int computeFullAverage (YARPImageOf<YarpPixelBGR> & inRImg,
							 YARPImageOf<YarpPixelBGR> & inLImg, 
							 double *value);

	int computeRGB (YARPImageOf<YarpPixelBGR> & inRImg,
				     YARPImageOf<YarpPixelBGR> & inLImg, 
					 double *value);

	int computeRGBAv (YARPImageOf<YarpPixelBGR> & inRImg,
				     YARPImageOf<YarpPixelBGR> & inLImg, 
					 double *value);

	int computeRGBVar2 (YARPImageOf<YarpPixelBGR> & inRImg,
   						YARPImageOf<YarpPixelBGR> & inLImg, double *value);
	
	int computeRGBChainVar (YARPImageOf<YarpPixelBGR> & inRImg,
   						YARPImageOf<YarpPixelBGR> & inLImg, double *value);

	int computeRGBChain (YARPImageOf<YarpPixelBGR> & inRImg,
   						  YARPImageOf<YarpPixelBGR> & inLImg, double *value);

	double computeSNRatio(int disparity);

	void makeHistogram(YARPImageOf<YarpPixelMono> & hImg);

	void plotRegion(const YARPImageOf<YarpPixelBGR> &imageRight, YARPImageOf<YarpPixelBGR> &out, int shift);
	void plotCorrelationMap(const YARPImageOf<YarpPixelBGR> &inRight, 
							const YARPImageOf<YarpPixelBGR> &inLeft, 
							YARPImageOf<YarpPixelBGR> &out, int shift);

	void plotSSDMap(const YARPImageOf<YarpPixelBGR> &inRight, 
							const YARPImageOf<YarpPixelBGR> &inLeft, 
							YARPImageOf<YarpPixelBGR> &out, int shift);

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

	void setLimits (float dispMax, float dispMin)
	{
		_shiftMax = _disparityToShift(dispMax);
		_shiftMin = _disparityToShift(dispMin);
	}

	int getLimitsMax()
	{ return _shiftMax;}

	int getLimitsMin()
	{ return _shiftMin;}

	int filterMaxes();
	
// private:
	public:
	inline void _findShift(const double *corr, int lv);
	void _findSecondMax(const double *corr, int lv, int posMax);
	void _findAllMax(const double *corr, int lv);
	inline float _shiftToDisparity(int shift);
	inline int _disparityToShift(float disp);
	inline void _computeCountVector(int *count);
	inline void _cleanCorr();
	void _setRings(int r)
	{ _actRings = r; }
			
	Image_Data _imgL;
	Image_Data _imgS;
	Image_Data _imgFovea;

	int   _shiftLevels;
	int * _shiftMap;
	int * _shiftFunction;
	int * _shiftFunctionInv;
	int * _remapMap;
	IntNeighborhood * _dsTable;
	double * _corrFunct;
	double * _phase;
	double * _coeff;
	double *_std1;
	double *_std2;
	double *_ssdFunct;
	double *_varFunct;
	double * _gaussFunction;
	double _corrTreshold;
	int _shifts[__nShifts];
	
	int *_count;
	int _maxCount;
		
	int _actRings;
	double _gMean;
	double _gSigma;
	double _gMagn;
	double _squareError;
	double _snRatio;

	int _shiftMax;
	int _shiftMin;

	float _varMax;
	float _varMin;

protected:
	char _path[256];
};


inline void YARPDisparityTool::_findShift(const double *corr, int lv)
{
	// search max
	double corrMax = 0;
	int ret = 0;
	int l;
	int maxIndex;

	if (_shiftMax<lv)
		maxIndex = _shiftMax;
	else
		maxIndex = lv-1;

	for(l = _shiftMin; l <= maxIndex; l++)
	{
		if (corr[l]>corrMax)
		{
			corrMax = corr[l];
			ret = l;
		}
	}
	
	_shifts[0] = ret;
}

inline float YARPDisparityTool::_shiftToDisparity(int sh)
{
	float ret;
	ret = _shiftFunction[sh];
	ret = ret * _imgS.Size_X_Remap / (float) _imgS.Resolution;
	return ret;
}

inline int YARPDisparityTool::_disparityToShift(float disp)
{
	float tmpf = disp*_imgS.Resolution/ (float) _imgS.Size_X_Remap;
	int tmp;
	if (tmpf>=0)
		tmp = (int) (tmpf + 0.5);
	else
		tmp = (int) (tmpf - 0.5);

	tmp += _imgS.Resolution;

	// be sure we are within the vector limits
	if (tmp<0)
		tmp = 0;
	else if (tmp>=2*_imgS.Resolution)
		tmp = (2*_imgS.Resolution-1);

	return _shiftFunctionInv[tmp];
}

inline void YARPDisparityTool::_cleanCorr()
{
	// search max
	int l;
	for(l = 0; l<_shiftLevels; l++)
	{
		if (_count[l] < _maxCount)
			_corrFunct[l] = 0;
	}
}

inline void YARPDisparityTool::_computeCountVector(int *count)
{
	int tIndex;
	int k,i,j,k1, i2;
	for (k = 0; k < _shiftLevels; k++)
	{
		k1 = k * _imgS.Size_LP; //Positioning on the table
		count[k] = 0;

		for (j=0; j<_actRings; j++)
		{
			tIndex = j*_imgS.Size_Theta;
			for (i=0; i<_imgS.Size_Theta; i++)
			{
				i2 = _shiftMap[k1 + tIndex+i];
				if (i2 > 0)
					count[k]++;
			}
		}
	}

	// find max
	_maxCount = 0;
	for (k = 0; k < _shiftLevels; k++)
	{
		if (count[k] > _maxCount)
			_maxCount = count[k];
	}

}


#endif // !defined(AFX_DISPARITY_H__E3019D2A_4BC3_4AD0_8355_AAA601391249__INCLUDED_)
