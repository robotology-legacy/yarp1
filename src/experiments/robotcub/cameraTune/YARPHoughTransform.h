// YARPHoughTransform.h: interface for the YARPHoughTransform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YARPHOUGHTRANSFORM_H__66FA4FF5_33A3_44AC_8B2B_3F9137CACD41__INCLUDED_)
#define AFX_YARPHOUGHTRANSFORM_H__66FA4FF5_33A3_44AC_8B2B_3F9137CACD41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//=============================================================================
// System Includes
//=============================================================================
#include "math.h"

//=============================================================================
// YARP Includes
//=============================================================================
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPImage.h>

class YARPHoughTransform  
{
public:
	double getMaxValue(int *thetaIndex=NULL, int *rhoIndex=NULL);
	bool getMaxRho(double *dst);
	bool getMaxTheta(double *dst);
	bool getThetaVector(double *vector, int vSize, double threshold);
	bool getHoughImage(YARPImageOf<YarpPixelMono> &dst);
	bool getHoughMatrixNorm(double **dts);
	bool getHoughMatrix(double **dst);
	bool apply(YARPImageOf<YarpPixelMono> *img);

	void resize(int sizeTheta, int sizeRho);
	YARPHoughTransform();
	YARPHoughTransform(int sizeTheta, int sizeRho);
	virtual ~YARPHoughTransform();

private:
	void _gaussianSmoothing(double **dst, double **src, int sizeX, int sizeY);
	void _initialize();
	void _deallocate();
	void _allocate();

	double **_houghSpace;
	double **_filtered;
	int _sizeRho;
	int _sizeTheta;
	double _rhoMax, _thetaMax, _rhoInc, _thetaInc;
};

#endif // !defined(AFX_YARPHOUGHTRANSFORM_H__66FA4FF5_33A3_44AC_8B2B_3F9137CACD41__INCLUDED_)
