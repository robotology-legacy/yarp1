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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPBlobDetector.cpp,v 1.7 2003-08-22 13:28:58 babybot Exp $
///
///

#include "YARPBlobDetector.h"
#include <YARPImageFile.h>
#include "math.h"

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
			_segmented(c,r) = (YarpPixelMono)(tmp+0.5);
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
				/*int maxX = c+_filterSizeTheta[f];
				int minX = c-_filterSizeTheta[f];
				int maxY = r+_filterSizeRho[f];
				int minY = r-_filterSizeRho[f];*/

				int deltaT = int (_filterSizeTheta[f]/pSize(c,r,_nfovea) + 0.5);
				int deltaR = int (_filterSizeRho[f]/pSize(c,r,_nfovea) + 0.5);

				int maxX = c+deltaR;
				int minX = c-deltaR;
				int maxY = r+deltaT;
				int minY = r-deltaT;
				
				_filtered[f](c,r) = _integralImg.getSaliencyLp(maxX, minX, maxY, minY);
				tmp += _filtered[f](c,r)*255/_nfilters;

			}
			if (_threshold > 0)
			{
				// threshold
				if (tmp>_threshold)
					tmp = 255;
				else 
					tmp = 0;
			}
			_segmented(c,r) = (YarpPixelMono)(tmp+0.5);
	}
}

void YARPBlobDetector::debug()
{
	char tmpName[255];
	YARPImageOf<YarpPixelMono> tmpImage;
	tmpImage.Resize(_nCols, _nRows);

	int r;
	int c;
	int f;
	for(f = 0; f < _nfilters; f++)
	{
		sprintf(tmpName, "%s%d.ppm", "blobDebug", f);
		tmpImage.Zero();

		for(r = 0; r < _nRows; r++)
			for(c = 0; c < _nCols; c++)
			{
				int deltaT = int (_filterSizeTheta[f]/pSize(c,r,_nfovea) + 0.5);
				int deltaR = int (_filterSizeRho[f]/pSize(c,r,_nfovea) + 0.5);

				int maxX = c+deltaR;
				int minX = c-deltaR;
				int maxY = r+deltaT;
				int minY = r-deltaT;

				_drawBox(maxX, minX, maxY, minY, tmpImage);
				
			}
		YARPImageFile::Write(tmpName, tmpImage);
	}
}

void YARPBlobDetector::_drawBox(int maxX, int minX, int maxY, int minY, YARPImageOf<YarpPixelMono> &out)
{
	int r,c;
	for(r = minY; r<=maxY; r++)
	{
		out(r, minX) = 255;
		out(r, maxX) = 255;
	}

	for(c = minX; c<=maxX; c++)
	{
		out(minY, c) = 255;
		out(maxY, c) = 255;
	}

}