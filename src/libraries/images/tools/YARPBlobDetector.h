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
/// $Id: YARPBlobDetector.h,v 1.5 2003-08-21 09:43:06 natta Exp $
///
/// August 2003 -- by nat


#ifndef __YARPBLOBDETECTOR__
#define __YARPBLOBDETECTOR__

#include <YARPImage.h>
#include "YARPIntegralImage.h"

const int _nfilters = 11;
// const int _filterSizeCart[] = {3, 5, 9, 15, 19, 21, 25, 27, 31};
const int _filterSizeCart[] = {7, 11, 15, 21, 25, 31, 35, 41, 45, 51};

const int _filterSizeTheta[] = {7, 11, 15, 21, 25, 31, 35, 41, 45, 51, 61};
const int _filterSizeRho[] = {11, 15, 21, 25, 31, 35, 41, 45, 51, 61, 71};

// const double _alfa = 0.023;
// const double _beta = 0.28;

// const double _alfa = 0.025;
// const double _beta = 0.19;

class YARPBlobDetector
{
public:
	YARPBlobDetector(unsigned char thr = 20);
	~YARPBlobDetector();

	void Apply(YARPImageOf<YarpPixelMono> &in)
	{
		filter(in);
	}

	void ApplyLp(YARPImageOf<YarpPixelMono> &in)
	{
		filterLp(in);
	}

	void filter(YARPImageOf<YarpPixelMono> &in);
	void filterLp(YARPImageOf<YarpPixelMono> &in);

	void resize(int nC, int nR, int sf = 0)
	{ _resize (nC, nR, sf); }

	YARPImageOf<YarpPixelMono> & getSegmented()
	{
		return _segmented;
	}

private:
	void _resize(int nC, int nR, int sf);
	void _dealloc();

	int _nRows;
	int _nCols;
	int _nfovea;

	YARPIntegralImage _integralImg;
	YARPImageOf<YarpPixelFloat> *_filtered;
	YARPImageOf<YarpPixelMono> _segmented;

	unsigned char _threshold;

};

#endif
