#include "YARPBlobDetector.h"

YARPBlobDetector::YARPBlobDetector(unsigned char thrs)
{
	_nRows = 0;
	_nCols = 0;

	_threshold = thrs;

	_filtered = NULL;
}

YARPBlobDetector::~YARPBlobDetector()
{
	_dealloc();
}

void YARPBlobDetector::_resize(int nC, int nR, int sf)
{
	_dealloc();

	_nRows = nR;
	_nCols = nC;
	_nfovea = sf;
	
	_filtered = new YARPImageOf<YarpPixelFloat>[_nfilters];
	int i = 0;
	for(i = 0; i < _nfilters; i++)
	{
		_filtered[i].Resize(nC, nR);
		_filtered[i].Zero();
	}
	
	_integralImg.resize(nC, nR, sf);
	_segmented.Resize(nC, nR);
	_segmented.Zero();
}

void YARPBlobDetector::_dealloc()
{
	if (_filtered != NULL)
		delete [] _filtered;

	_filtered = NULL;
}

void YARPBlobDetector::filter(YARPImageOf<YarpPixelMono> &in)
{
	// compute integral img
	_integralImg.computeCartesian(in);

	int f;
	float tmp;

	int r;
	int c;
	for(r = 0; r < _nRows; r++)
		for(c = 0; c < _nCols; c++)
		{
			// for each filer
			tmp = 0.0;
			for(f = 0; f < _nfilters; f++)
			{
				int maxX = c+_filterSizeCart[f];
				int minX = c-_filterSizeCart[f];
				int maxY = r+_filterSizeCart[f];
				int minY = r-_filterSizeCart[f];
		
				_filtered[f](c,r) = _integralImg.getSaliency(maxX, minX, maxY, minY);
				tmp += _filtered[f](c,r)*255/_nfilters;

				if (_threshold > 0)
				{
					// threshold
					if (tmp>_threshold)
						tmp = 255;
					else 
						tmp = 0;
				}
			}
			_segmented(c,r) = tmp+0.5;
	}
}

void YARPBlobDetector::filterLp(YARPImageOf<YarpPixelMono> &in)
{
	// compute integral img
	_integralImg.computeLp(in);

	int f;
	float tmp;

	int r;
	int c;
	for(r = 0; r < _nRows; r++)
		for(c = 0; c < _nCols; c++)
		{
			// for each filer
			tmp = 0.0;
			for(f = 0; f < _nfilters; f++)
			{
				int maxX = c+_filterSizeTheta[f];
				int minX = c-_filterSizeTheta[f];
				int maxY = r+_filterSizeRho[f];
				int minY = r-_filterSizeRho[f];
				
				_filtered[f](c,r) = _integralImg.getSaliencyLp(maxX, minX, maxY, minY);
				tmp += _filtered[f](c,r)*255/_nfilters;

				if (_threshold > 0)
				{
					// threshold
					if (tmp>_threshold)
						tmp = 255;
					else 
						tmp = 0;
				}
			}
			_segmented(c,r) = tmp+0.5;
	}
}
