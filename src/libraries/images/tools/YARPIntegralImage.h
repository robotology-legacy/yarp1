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
///                    #nat
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPIntegralImage.h,v 1.6 2003-08-22 15:39:55 natta Exp $
///
/// August 2003 -- by nat

#ifndef __YARPINTEGRALIMAGE__
#define __YARPINTEGRALIMAGE__

#include <YARPImage.h>

const double _alfa = 0.0130;
const double _beta = 0.91;

#include <math.h>

inline double pSize(int c, int r, int nf)
{
	double ret;
	ret = _beta*exp(_alfa*(r-nf));
	// ret = _alfa*(r-nf) + _beta;
	if (ret < 1)
		ret = 1;
	
	return ret;
}

class YARPIntegralImage
{
public:
	YARPIntegralImage();
	YARPIntegralImage(int nR, int nC, int sfovea = 0);
	~YARPIntegralImage();

	void resize(int nR, int nC, int sfovea = 0);

	void get(YARPImageOf<YarpPixelMono> &out)
	{
		int r,c;
		for(r = 0; r < _nRows; r++)
			for(c = 0; c < _nCols; c++)
				out(c,r) = (YarpPixelMono)((_integralImg(c,r)*255)/_max + 0.5);
	}

	inline float get(int c, int r)
	{
		// float max = _integralImg(_nCols-1, _nRows-1);
		return _integralImg(c,r)/_max;
	}

	inline double getSaliency(int maxX, int minX, int maxY, int minY)
	{
		double tmp1; 
		double tmp2; 
		double tmp3; 
		double tmp4; 

		if (minX < 0)
			minX = 0;
		if (minY < 0)
			minY = 0;
		if (maxX > _nCols-1)
			maxX = _nCols-1;
		if (maxY > _nRows-1)
			maxY = _nRows-1;

		tmp1 = get(minX, minY);
		tmp2 = get(maxX, minY);
		tmp3 = get(minX, maxY);
		tmp4 = get(maxX, maxY);

		return (tmp4 + tmp1 - (tmp2+tmp3));
	}

	inline double getSaliencyLp(int maxT, int minT, int maxR, int minR)
	{
		double tmp1, tmp2, tmp3, tmp4; 

		if (minR < 0)
			minR = 0;
				
		if (maxR > _nRows-1)
			maxR = _nRows-1;
		
		if ( (minT>=0) && (maxT<_nCols) ) {
			// nothing special tmp4+tmp3 - (tmp1 + tmp2)

			tmp1 = get(minT, minR);
			tmp2 = -get(maxT, minR);
			tmp3 = -get(minT, maxR);
			tmp4 = get(maxT, maxR);
		}
		else if ( (minT<0) && (maxT<_nCols) )
		{
			tmp1 = get(0, minR);
			tmp2 = -get(maxT, minR);
			tmp3 = -get(0, maxR);
			tmp4 = get(maxT, maxR);

			tmp1 += get(_nCols-1+minT, minR);
			tmp2 += -get(_nCols-1, minR);
			tmp3 += -get(_nCols-1+minT, maxR);
			tmp4 += get(_nCols-1, maxR);
		}
		else if ( (minT>0) && (maxT>=_nCols) )
		{
			tmp1 = get(minT, minR);
			tmp2 = -get(_nCols-1, minR);
			tmp3 = -get(minT, maxR);
			tmp4 = get(_nCols-1, maxR);

			tmp1 += get(0, minR);
			tmp2 += -get(maxT-_nCols, minR);
			tmp3 += -get(0, maxR);
			tmp4 += get(maxT-_nCols, maxR);
		
		}
		else
			return 0;		// case not supported

		return (tmp4 + tmp3 + tmp1 + tmp2);
	}

	int computeCartesian(YARPImageOf<YarpPixelMono> &input);
	int computeLp(YARPImageOf<YarpPixelMono> &input);

private:
	void _resize(int nC, int nR, int sfovea);
	YARPImageOf<YarpPixelFloat> _integralImg;
	YARPImageOf<YarpPixelFloat> _rowSum;
	int _nRows;
	int _nCols;
	int _nfovea;
	float _max;
};

#endif

