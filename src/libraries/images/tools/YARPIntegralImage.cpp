#include "YARPIntegralImage.h"

YARPIntegralImage::YARPIntegralImage()
{
	_nRows = 0;
	_nCols = 0;
	_nfovea = 0;
}

YARPIntegralImage::YARPIntegralImage(int nC, int nR, int sf)
{
	_resize(nC, nR, sf);
}

YARPIntegralImage::~YARPIntegralImage()
{

}

void YARPIntegralImage::resize(int nC, int nR, int sf)
{
	_resize(nC, nR, sf);
}

void YARPIntegralImage::_resize(int nC, int nR, int sf)
{
	ACE_ASSERT ((nR>0) && (nC>0));

	_nCols = nC;
	_nRows = nR;
	_nfovea = sf;
	
	_integralImg.Resize(nC, nR);
	_rowSum.Resize(nC,nR);
}

int YARPIntegralImage::computeCartesian(YARPImageOf<YarpPixelMono> &input)
{
	
	int r;
	int c;

	unsigned char tmp;
	// first pixel
	_rowSum(0,0) = 0; //input(0,0)/255.0;
	_integralImg(0,0) = _rowSum(0,0);
	// first row, c >= 1
	for(c = 1; c < _nCols; c++)
	{
		tmp = input(0,c);
		_rowSum(c,0) = _rowSum(0, c-1) + tmp;
		_integralImg(c,0) = _rowSum(c,0);
	}
	
	for(r = 1; r < _nRows; r++)
	{
		// c = 0
		tmp = input(c,r);
		
		_rowSum(0,r) = tmp;
		_integralImg(0, r) = _integralImg(0, r-1) + _rowSum(0,r);
		for(c = 1; c < _nCols; c++)
		{
			tmp = input(c,r);
						
			_rowSum(c,r) = _rowSum(c-1,r) + tmp;
			_integralImg(c,r) = _integralImg(c, r-1) + _rowSum(c,r);
		}
	}

	return YARP_OK;
}

int YARPIntegralImage::computeLp(YARPImageOf<YarpPixelMono> &input)
{
	int r;
	int c;

	// first pixel, init
	_rowSum(0,0) = 0; //input(0,0)/255.0;
	_integralImg(0,0) = _rowSum(0,0);
	
	// fovea, zero it for now
	for(r = 0; r<_nfovea; r++)
		for(c = 0; c < _nCols; c++)
		{
			_rowSum(c,r) = 0;
			_integralImg(c,r) = 0;
		}
	
	
	for(r = _nfovea; r < _nRows; r++)
	{
		// first col
		_rowSum(0,r) = input(0,r);
		_integralImg(0, r) = _integralImg(0, r-1) + _rowSum(0,r);
		
		for(c = 1; c < _nCols; c++)
		{
			_rowSum(c,r) = _rowSum(c-1,r) + input(c,r);
			_integralImg(c,r) = _integralImg(c, r-1) + _rowSum(c,r);
		}
	}

	return YARP_OK;
}