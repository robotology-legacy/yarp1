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
///                    #original paulfitz, changed pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: ImgTrack.h,v 1.4 2004-01-13 16:39:20 beltran Exp $
///
///


#ifndef _YARPImageTrackTool_INC
#define _YARPImageTrackTool_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPImage.h>
#include <YARPImageDraw.h>
#include <YARPSemaphore.h>
#include <YARPTime.h>

///
/// LATER: from motor control library.
#if defined(__QNXEurobot__)
#include "YARPEurobotHeadKin.h"
#else      // ----- #ifdef __QNXEurobot__  ----- 
#include "YARPBabybotHeadKin.h"
#endif     // ----- #ifdef __QNXEurobot__  ----- 

///
/// never tested but 128 square.
/// 
const int ISIZE = 128;
const int IMG_H = ISIZE;
const int IMG_W = ISIZE;

typedef unsigned char ImgInt[IMG_H][IMG_W];
typedef unsigned char ImgInt3[IMG_H][IMG_W][3];

///
///
///
class YARPImageTrackTool
{
private:
	  int tx;
	  int ty;

	  YARPImageOf<YarpPixelMono> prevImg;
	  YARPImageOf<YarpPixelBGR> prevImg3;

	  int first;
	  int delta;
	  int blockSize;
	  int blockXSize;
	  int blockYSize;
	  int windowSize;
	  int windowXSize;
	  int windowYSize;
	  int windowXOffset;
	  int windowYOffset;
	  int normalize;
	  float quality;

public:
	YARPImageTrackTool()
    {
		ResetXY();
		quality = 0;
		first = 1;
		blockSize = 9;
		blockXSize = blockSize;
		blockYSize = blockSize;
		windowSize = 5;
		windowXSize = windowSize;
		windowYSize = windowSize;
		windowXOffset = 0;
		windowYOffset = 0;
		normalize = 0;
    }
  
	void SetBlockSize(int size)
    {
		blockSize = size;
		blockXSize = blockSize;
		blockYSize = blockSize;
    }
  
	void SetBlockSize(int dx, int dy)
    {
		blockXSize = dx;
		blockYSize = dy;
    }

	// search will be from -size to +size pixels from the search center
	void SetSearchWindowSize(int size) { windowSize = windowXSize = windowYSize = size; }
	void SetSearchWindowOffset(int dx, int dy)
    {
		windowXOffset = dx;
		windowYOffset = dy;
    }

	void SetSearchWindowSize(int xsize, int ysize)
    {
		windowXSize = xsize;
		windowYSize = ysize;
		windowSize = xsize;
    }
  
	void SetNormalize(int flag=1) { normalize = flag; }
	int GetX() { return tx; }
	int GetY() { return ty; }
    float GetQuality() { return quality; }
  
	void SetXY(int x, int y)
    {
		tx = x; ty = y;
		delta = 1;
    }
  
	void Restart() { first = 1; }
	void ResetXY() { SetXY(IMG_W/2,IMG_H/2); }
  
	void Apply(ImgInt frame1, ImgInt frame2, int x, int y);
	void Apply(YARPImageOf<YarpPixelMono>& src);
	void Apply(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& dest, int x, int y);
	void Apply(ImgInt3 frame1, ImgInt3 frame2, int x, int y);
	void Apply(YARPImageOf<YarpPixelBGR>& src);
};


///
///
///
class YARPComplexTrackerTool
{
protected:
	enum { PRINT_TIME = 5, BLOCK_SIZE = 6, SEARCH_SIZE = 10, NSCALE = 7, MAGDEFAULT = 2, };
	enum { QTHRESHOLD = 2, QTHR2 = 10, QTHR3 = 3, BXDX = 15, };

	YARPImageTrackTool _tracker;
	YARPImageTrackTool _sub_tracker;
	YARPSemaphore _lock;

	YARPImageOf<YarpPixelMono> _prev;
	YARPImageOf<YarpPixelMono> _mono;

	int _tx, _ty;
	int _px, _py;
	double _dgx, _dgy;

	double _last_update;	/// last new target (round time).
	double _last_movement;	/// last movement (>threshold) of tracker.

	/// just for printing.
	double _last_round;
	double _last_reset;
	double _diff_total;
	int _diff_count;

	/// signal a new target.
	bool _new_target;
	int _ex, _ey;
	int _xx, _yy;

	bool _movement;
	int _low_q_ct;

#if defined(__QNXEurobot__)
	YARPEurobotHeadKin _gaze;
#else      // ----- #ifdef __QNXEurobot__  ----- 
	YARPBabybotHeadKin _gaze;
#endif     // ----- #ifdef __QNXEurobot__  ----- 
	YVector _prevRay;
	int _prev_gaze_x, _prev_gaze_y;

public:

	YARPComplexTrackerTool ();
	~YARPComplexTrackerTool ();

	void apply (YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelBGR>& dest);
	void apply (YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelBGR>& dest, const YVector& jnts);

	void setNewTarget (int x, int y)
	{
		_lock.Wait ();

		_new_target = true;
		_ex = x;
		_ey = y;
		
		_lock.Post ();
	}

	void getTarget (int& rx, int& ry)
	{
		_lock.Wait ();

		rx = _xx;
		ry = _yy;
		
		_lock.Post ();
	}
};


#endif
