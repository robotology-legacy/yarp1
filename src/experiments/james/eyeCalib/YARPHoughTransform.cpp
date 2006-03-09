// YARPHoughTransform.cpp: implementation of the YARPHoughTransform class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPHoughTransform.h"



#define PI 3.1415926535
#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPHoughTransform::YARPHoughTransform()
{
	_houghSpace = NULL;
	_filtered = NULL;
	_sizeRho = 0;
	_sizeTheta = 0;
	_thetaMax = PI;
}

YARPHoughTransform::YARPHoughTransform(int sizeTheta, int sizeRho)
{
	_sizeRho = sizeRho;
	_sizeTheta = sizeTheta;
	_thetaMax = PI;
	_thetaInc = _thetaMax / sizeTheta;
	_allocate();
}

YARPHoughTransform::~YARPHoughTransform()
{
	_deallocate();
}

void YARPHoughTransform::resize(int sizeTheta, int sizeRho)
{
	_sizeRho = sizeRho;
	_sizeTheta = sizeTheta;
	_thetaInc = _thetaMax / sizeTheta;
	_deallocate();
	_allocate();
}

void YARPHoughTransform::_allocate()
{
	if (_houghSpace == NULL)
	{
		_houghSpace = new double * [_sizeTheta];
		for (int i=0; i<_sizeTheta; i++)
		{	
			_houghSpace[i] = new double[_sizeRho];
		}
	}
	if (_filtered == NULL)
	{
		_filtered = new double * [_sizeTheta];
		for (int i=0; i<_sizeTheta; i++)
		{
			_filtered[i] = new double[_sizeRho];
		}
	}
}

void YARPHoughTransform::_deallocate()
{
	if (_houghSpace != NULL)
	{
		for (int i=0; i<_sizeTheta; i++)
		{
			delete [] _houghSpace[i];
		}
		delete [] _houghSpace;
		_houghSpace = NULL;
	}
	
	if (_filtered != NULL)
	{
		for (int i=0; i<_sizeTheta; i++)
		{
			delete [] _filtered[i];
		}
		delete [] _filtered;
		_filtered = NULL;
	}
}

void YARPHoughTransform::_initialize()
{
	// Matrix initialization
	for (int i=0; i<_sizeTheta; i++)
		for(int j=0; j<_sizeRho; j++)
		{
			_houghSpace[i][j] = 0.0;
			_filtered[i][j] = 0.0;
		}
}

bool YARPHoughTransform::apply(YARPImageOf<YarpPixelMono> *img)
{
	if (_houghSpace == NULL)
		return false;

	int sizeX, sizeY;
	double rho, theta;
	unsigned char value;
	double vote;
	int x,y,i,j;
	int x_tras,y_tras;

	sizeX = img->GetWidth();
	sizeY = img->GetHeight();
	_rhoMax = sqrt(sizeX/2*sizeX/2 + sizeY/2*sizeY/2);
	_rhoInc = (2*_rhoMax) / _sizeRho;

	_initialize();

	// Main Hough Tranformation Cycle
	for (x=0; x<sizeX; x++)
		for(y=0; y<sizeY; y++)
		{
			rho = 0.0;
			theta = 0.0;
			value = img->SafePixel(x,y);
			
			x_tras = x - sizeX/2;
			y_tras = y - sizeY/2;

			if (value != 0)
				for (i=0; i<_sizeTheta; i++)
				{
					theta = _thetaInc * i;
					rho = (x_tras * sin(theta) - y_tras * cos(theta));
					j = (int) ((rho + _rhoMax) / _rhoInc);

					vote = value/255.0;
					_houghSpace[i][j] += vote;
				}
			
		}

	return true;
}


bool YARPHoughTransform::getHoughMatrix(double **dst)
{
	if (_houghSpace == NULL)
		return false;
	
	memcpy(dst, _houghSpace, (_sizeTheta * _sizeRho * sizeof(double)) );
		
	return true;
}

bool YARPHoughTransform::getHoughMatrixNorm(double **dst)
{
	if (_houghSpace == NULL)
		return false;

	double maxVal;
	maxVal = getMaxValue();

	for (int i=0; i<_sizeTheta; i++)
		for (int j=0; j<_sizeRho; j++)
			dst[i][j] = _houghSpace[i][j] / maxVal;

	return true;
}

bool YARPHoughTransform::getHoughImage(YARPImageOf<YarpPixelMono> &dst)
{
	if (_houghSpace == NULL)
		return false;
	
	if ( (dst.GetWidth() != _sizeTheta) || (dst.GetHeight() != _sizeRho) )
		dst.Resize(_sizeTheta, _sizeRho);
	double maxVal;
	maxVal = getMaxValue();
	unsigned char pixVal;
	
	for (int i=0; i<_sizeTheta; i++)
		for (int j=0; j<_sizeRho; j++)
		{
			pixVal = FTOI(_houghSpace[i][j] * 255.0 / maxVal);
			dst(i,j) = pixVal;
		}
	return true;

}

bool YARPHoughTransform::getMaxTheta(double *dst)
{
	if (_houghSpace == NULL)
		return false;

	int maxRhoIndex, maxThetaIndex;
	getMaxValue(&maxThetaIndex, &maxRhoIndex);

	*dst = _thetaInc * double(maxThetaIndex);

	return true;
}

bool YARPHoughTransform::getMaxRho(double *dst)
{
	if (_houghSpace == NULL)
		return false;
	
	int maxRhoIndex, maxThetaIndex;
	getMaxValue(&maxThetaIndex, &maxRhoIndex);
	*dst = (_rhoInc * maxRhoIndex) - _rhoMax;

	return true;
}

double YARPHoughTransform::getMaxValue(int *thetaIndex, int *rhoIndex)
{
	if (_houghSpace == NULL)
		return 0;

	double maxVal = 0.0;
	int maxI = 0;
	int maxJ = 0;
	double value;
	for (int i=0; i<_sizeTheta; i++)
		for (int j=0; j<_sizeRho; j++)
		{
			value = _houghSpace[i][j];
			if (value > maxVal)
			{
				maxVal = value;
				maxI = i;
				maxJ = j;
			}
		}

	if(thetaIndex != NULL)
		*thetaIndex = maxI;
	if (rhoIndex != NULL)
		*rhoIndex = maxJ;
	return maxVal;
}

void YARPHoughTransform::_gaussianSmoothing(double **dst, double **src, int sizeX, int sizeY)
{

	int kernel[5][5];

	kernel[0][0] = kernel[0][1] = kernel[0][3] = kernel[0][4] = 2;
	kernel[1][0] = kernel[1][4] = 2;
	kernel[3][0] = kernel[3][4] = 2;
	kernel[4][0] = kernel[4][1] = kernel[4][3] = kernel[4][4] = 2;
	kernel[0][2] = 4;
	kernel[1][1] = kernel[1][3] = 4;
	kernel[2][0] = kernel[2][4] = 4;
	kernel[3][1] = kernel[3][3] = 4;
	kernel[4][2] = 4;
	kernel[1][2] = 8;
	kernel[2][1] = kernel[2][3] = 8;
	kernel[3][2] = 8;
	kernel[2][2] = 16;
	int kappa = 104;

	int pX,pY;
	double acc;
	int x,y,i,j;

	for(x=0; x<sizeX; x++)
	{
		for(y=0; y<sizeY; y++)
		{
			acc = 0.0;
			for(i=0; i<5; i++)
			{
				for(j=0; j<5; j++)
				{
					pX = x + i - 2;
					pY = y + j - 2;
					if ( (pX >= 0) && (pY >= 0) && (pX < sizeX) && (pY <sizeY) )
						acc += src[pX][pY] * kernel[i][j];
				}
			}
			dst[x][y] = acc/kappa;			
		}
	}
	
}

bool YARPHoughTransform::getThetaVector(double *vector, int vSize, double threshold)
{
	if (vSize != _sizeTheta)
		return false;

	double value;
	double max = 0.0;

	for (int i=0; i< _sizeTheta; i++)
		vector[i] = 0.0;
	for (i=0; i< _sizeTheta; i++)
		for (int j=0; j<_sizeRho; j++)
	{
		value = _houghSpace[i][j];
			if ( value >= threshold)
				vector[i] += value;
	}

	for (i=0; i< _sizeTheta; i++)
	{
		if (vector[i] > max)
		{
			max = vector[i];
		}
	}

	for (i=0; i< _sizeTheta; i++)
	{	
		vector[i] = vector[i] / max;
	}

	return true;
}