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
/// $Id: ImgTrack.cpp,v 1.6 2003-10-28 18:01:27 babybot Exp $
///
///

#include <math.h>
#include <stdlib.h>

#include "ImgTrack.h"

struct Pixel3 
{ 
	unsigned char b, g, r; 
};

///
///
///
///
static float ComparePixel(ImgInt s1, int r1, int c1, ImgInt s2, int r2, int c2, int BLOCK_SIZEX, int BLOCK_SIZEY, int NORMALIZE)
{
	int delx = BLOCK_SIZEX;
	int dely = BLOCK_SIZEY;
	int i, j;
	float d, diff = 0;

	if (!NORMALIZE)
	{
		for (i = -dely; i <= dely; i++)
		{
			for (j = -delx; j <= delx; j++)
			{
				d = s1[r1+i][c1+j] - s2[r2+i][c2+j];
				diff += (d > 0) ? d : -d;
			}
		}
	}
	else
	{
		float t1=0, t2=0;
		float mu1, mu2;
		float s12 = 1;
		for (i = -dely; i <= dely; i++)
		{
			for (j = -delx; j <= delx; j++)
			{
				t1 += s1[r1+i][c1+j];
				t2 += s2[r2+i][c2+j];
			}
		}

		float area = (2*delx+1)*(2*dely+1);
		mu1 = t1/area;
		mu2 = t2/area;
		float theta = 0.1f;

		if (t1>theta && t2>theta)
		{
			s12 = t2/t1;
		}

		for (i=-dely; i<=dely; i++)
		{
			for (j=-delx; j<=delx; j++)
			{
				d = (s1[r1+i][c1+j]*s12)-(s2[r2+i][c2+j]);
				diff += (d > 0) ? d : -d;
			}
		}
	}

	return diff;
}


template <class T>
T max(T x, T y)
{ 
	return (x > y) ? x : y; 
}


static float ComparePixel(ImgInt3 s1, int r1, int c1, ImgInt3 s2, int r2, int c2,int BLOCK_SIZEX, int BLOCK_SIZEY, int NORMALIZE)
{
	int delx = BLOCK_SIZEX;
	int dely = BLOCK_SIZEY;
	int i, j;
	float d, diff=0;

	if (NORMALIZE)
    {
		for (i=-dely; i<=dely; i++)
		{
			for (j=-delx; j<=delx; j++)
			{
				d = s1[r1+i][c1+j][0] - s2[r2+i][c2+j][0];
				diff += (d>0)?d:-d;
				d = s1[r1+i][c1+j][1] - s2[r2+i][c2+j][1];
				diff += (d>0)?d:-d;
				d = s1[r1+i][c1+j][2] - s2[r2+i][c2+j][2];
				diff += (d>0)?d:-d;
			}
		}
    }
	else
    {
		Pixel3 *p1, *p2;
		for (i = -dely; i <= dely; i++)
		{
			for (j = -delx; j <= delx; j++)
			{
				p1 = (Pixel3*) s1[r1+i][c1+j];
				p2 = (Pixel3*) s2[r2+i][c2+j];
				d = max(abs(p1->r - p1->g), abs(p2->r - p2->g)) * abs((p1->r - p1->g) - (p2->r - p2->g));
				d += max(abs(p1->r - p1->b),abs(p2->r - p2->b)) * abs((p1->r - p1->b) - (p2->r - p2->b));
				d *= abs(p1->r + p1->g + p1->b - (p2->r + p2->g + p2->b));
				diff += d;
			}
		}
    }

	return diff;
}


template <class ImgType>
static float FindPixel(ImgType s1, int r1, int c1, 
					   ImgType s2, int *pr2, int *pc2,
					   int BLOCK_SIZEX, int BLOCK_SIZEY, 
					   int SEARCH_SIZEX, int SEARCH_SIZEY, 
					   int OFFSET_X, int OFFSET_Y, int NORMALIZE)
{
	int r2=(*pr2) + OFFSET_Y, c2=(*pc2) + OFFSET_X;
	float best = 999999999999.0f, current, central=0;
	float worst = 0;
	int i, j;
	int delx = SEARCH_SIZEX;
	int dely = SEARCH_SIZEY;
	
	for (i = -dely+OFFSET_Y; i <= dely+OFFSET_Y; i++)
    {
		for (j = -delx+OFFSET_X; j <= delx+OFFSET_X; j++)
		{
			int r1d = (*pr2)+i;
			int c1d = (*pc2)+j;
			if (c1 + BLOCK_SIZEX < IMG_W && c1 - BLOCK_SIZEX >= 0 &&
				r1 + BLOCK_SIZEY < IMG_H && r1 - BLOCK_SIZEY >= 0 &&
				c1d + BLOCK_SIZEX < IMG_W && c1d - BLOCK_SIZEX >= 0 &&
				r1d + BLOCK_SIZEY < IMG_H && r1d - BLOCK_SIZEY >= 0)
			{
				current = ComparePixel(s1,r1,c1,s2,r1d,c1d,BLOCK_SIZEX,BLOCK_SIZEY,NORMALIZE);
				if (i==0 && j==0)
				{
					central = current;
				}

				if (current<best || (current==best && i==0 && j==0))
				{
					r2 = (*pr2)+i;
					c2 = (*pc2)+j;
					best = current;
				}

				if (current > worst)
				{
					worst = current;
				}
			}
		}
    }

	*pr2 = r2;
	*pc2 = c2;

	float safe = 10;
	return (fabs(worst-best)+safe)/(best+safe);
}

void YARPImageTrackTool::Apply(ImgInt frame1, ImgInt frame2, int x, int y)
{
	quality = FindPixel(frame1, y, x, frame2, &ty, &tx, blockXSize, blockYSize, windowXSize, windowYSize, windowXOffset, windowYOffset, normalize);
}

void YARPImageTrackTool::Apply(ImgInt3 frame1, ImgInt3 frame2, int x, int y)
{
	quality = FindPixel(frame1, y, x, frame2, &ty, &tx, blockXSize, blockYSize, windowXSize, windowYSize, windowXOffset, windowYOffset, normalize);
}

void YARPImageTrackTool::Apply(YARPImageOf<YarpPixelMono>& src)
{
	YARPImageOf<YarpPixelMono> nextImg, blah;
	int ox, oy;

	if (src.GetWidth() != IMG_W || src.GetHeight()!=IMG_H)
	{
		printf("Image tracking code is old, and specific to %dx%d images\n", IMG_W, IMG_H);
		exit(1);
	}

	nextImg.Refer(src);
	if (first)
	{
		first = 0;
		ResetXY();
		prevImg.PeerCopy(nextImg);
		delta = 0;
	}
	else
	{
		ox = tx;
		oy = ty;
		ImgInt& ii1 = *((ImgInt *)prevImg.GetRawBuffer());
		ImgInt& ii2 = *((ImgInt *)src.GetRawBuffer());
		Apply(ii1, ii2, tx, ty);
		
		if (ox != tx || oy != ty || delta)
		{
			prevImg.PeerCopy(nextImg);
			delta = 0;
		}
	}
}

void YARPImageTrackTool::Apply(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& dest, int x, int y)
{
	YARPImageOf<YarpPixelMono> prevImg, nextImg;

	if ((src.GetWidth()!=IMG_W||src.GetHeight()!=IMG_H) || 
		(dest.GetWidth()!=IMG_W||dest.GetHeight()!=IMG_H))
    {
		printf("Image tracking code is old, and specific to %dx%d images\n", IMG_W, IMG_H);
		exit(1);
    }

	prevImg.Refer(src);
	nextImg.Refer(dest);
	tx = x; ty = y;
	ImgInt& ii1 = *((ImgInt *)prevImg.GetRawBuffer());
	ImgInt& ii2 = *((ImgInt *)nextImg.GetRawBuffer());
	Apply(ii1, ii2, tx, ty);
}

void YARPImageTrackTool::Apply(YARPImageOf<YarpPixelBGR>& src)
{
	YARPImageOf<YarpPixelBGR> nextImg, blah;
	int ox, oy;

	if (src.GetWidth() != IMG_W || src.GetHeight() != IMG_H)
	{
		printf("Image tracking code is old, and specific to %dx%d images\n", IMG_W, IMG_H);
		exit(1);
	}

	nextImg.Refer(src);
	if (first)
	{
		first = 0;
		ResetXY();
		prevImg3.PeerCopy(nextImg);
		delta = 0;
	}
	else
	{
		ox = tx;
		oy = ty;
		ImgInt3& ii1 = *((ImgInt3 *)prevImg3.GetRawBuffer());
		ImgInt3& ii2 = *((ImgInt3 *)src.GetRawBuffer());
		Apply(ii1, ii2, tx, ty);
		if (ox != tx || oy != ty || delta)
		{
			prevImg3.PeerCopy(nextImg);
			delta = 0;
		}
	}
}

///
///
///
YARPComplexTrackerTool::YARPComplexTrackerTool () : _lock(1)
{
	_tx = ISIZE/2;
	_ty = ISIZE/2;
	_px = _tx;
	_py = _ty;

	/// not used now, it represents the movement in the img plane due to head motion.
	_dgx = _dgy = 0.0;

	/// timing issues.
	_last_update = -100000.0;
	_last_movement = -100000.0;

	_last_round = YARPTime::GetTimeAsSeconds();
	_last_reset = 0;
	_diff_total = 0;
	_diff_count = 0;

	/// details of the internal logic.
	_new_target = true;
	_ex = _tx;
	_ey = _ty;
	_xx = 0;
	_yy = 0;

	/// other params.
	_low_q_ct = 0;
	_movement = true;

	/// tmp's.
	_prev.Resize (ISIZE, ISIZE);
	_prev.Zero ();
	_mono.Resize (ISIZE, ISIZE);
	_mono.Zero ();
}

YARPComplexTrackerTool::~YARPComplexTrackerTool () {}

///
///
/// actual processing.
void YARPComplexTrackerTool::apply (YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelBGR>& dest)
{
	/// check whether it's a new target first.
	_lock.Wait ();
	bool _isnew = _new_target;
	_lock.Post ();

	/// timing stuff.
	const double now = YARPTime::GetTimeAsSeconds();

	///
	bool act_vector = false;
	
	/// print timing issues.
	_diff_total += now - _last_round;
	_diff_count ++;

	if (now - _last_reset > PRINT_TIME)
    {
		printf("Time between frames in ms is %g\n", 1000 * _diff_total / _diff_count);
		_diff_count = 0;
		_diff_total = 0;
		_last_reset = now;
    }
	_last_round = now;

	/// LATER: this might be the place to auto-reset the tracker in case that any timeout expired.
	///
	///
	if (now - _last_movement > 30)
	{
		setNewTarget (ISIZE/2, ISIZE/2);
	}

	/// a bit of copying.
	_mono.CastCopy(src);
	dest.PeerCopy(src);
	
	///
	/// deals with the new target.
	if (_isnew)
	{
		_prev.PeerCopy (_mono);
		
		_lock.Wait ();
		_px = _tx = _ex;
		_py = _ty = _ey;
		_new_target = false;
		_lock.Post ();

		printf("*** Got new target %d %d\n", _px, _py);
		_last_update = now;
	}

	_tracker.SetBlockSize (BLOCK_SIZE, BLOCK_SIZE);
	_tracker.SetSearchWindowSize (SEARCH_SIZE, SEARCH_SIZE);
	_sub_tracker.SetBlockSize (BLOCK_SIZE, BLOCK_SIZE);
	_sub_tracker.SetSearchWindowSize (SEARCH_SIZE, SEARCH_SIZE);
	
	if (!_isnew)
	{
		_tracker.SetSearchWindowOffset ((int)(_dgx+0.5), (int)(_dgy+0.5));
		_sub_tracker.SetSearchWindowOffset ((int)(_dgx+0.5), (int)(_dgy+0.5));
	}
	else
	{
		_tracker.SetSearchWindowOffset (0, 0);
		_sub_tracker.SetSearchWindowOffset (0, 0);
	}

	_tx = _px; 
	_ty = _py;

	/// checks borders.
	if (_tx < BXDX) _tx = BXDX;
	if (_tx > ISIZE-1-BXDX) _tx = ISIZE-1-BXDX;
	if (_ty < BXDX) _ty = BXDX;
	if (_ty > ISIZE-1-BXDX) _ty = ISIZE-1-BXDX;

	/// actual tracking.
	bool fell = false;
	double new_tx = ISIZE/2, new_ty = ISIZE/2, new_tx2 = ISIZE/2, new_ty2 = ISIZE/2;
	int sub_x = ISIZE/2, sub_y = ISIZE/2, sub_x2 = ISIZE/2, sub_y2 = ISIZE/2;
	
	_tracker.Apply (_prev, _mono, _tx, _ty);

	int x = _tx, y = _ty;
	x = _tracker.GetX();
	y = _tracker.GetY();

	/// predicted.
	double new_dx = x - (_tx + _dgx);
	double new_dy = y - (_ty + _dgy);

	/// direction of motion.
	double new_mag = sqrt (new_dx * new_dx + new_dy * new_dy);
	if (new_mag < 0.001) new_mag = 0.001;
	new_dx /= new_mag;
	new_dy /= new_mag;

	const double nscale = NSCALE;

	/// search along two directions.
	/// heuristic for exploring certain neighborhood of the current position.
	///
	///
	new_tx = _tx - new_dx * nscale;
	new_ty = _ty - new_dy * nscale;
	new_tx2 = _tx + new_dx * nscale;
	new_ty2 = _ty + new_dy * nscale;

	_sub_tracker.Apply (_prev, _mono, new_tx2, new_ty2);

	sub_x2 = _sub_tracker.GetX();
	sub_y2 = _sub_tracker.GetY();

	_sub_tracker.Apply (_prev, _mono, new_tx, new_ty);

	sub_x = _sub_tracker.GetX();
	sub_y = _sub_tracker.GetY();
	
	double sub_dx = sub_x - (new_tx + _dgx);
	double sub_dy = sub_y - (new_ty + _dgy);
	double sub_mag = sqrt (sub_dx * sub_dx + sub_dy * sub_dy);

	double sub_dx2 = sub_x2 - (new_tx2 + _dgx);
	double sub_dy2 = sub_y2 - (new_ty2 + _dgy);
	double sub_mag2 = sqrt (sub_dx2 * sub_dx2 + sub_dy2 * sub_dy2);

	if (new_mag > MAGDEFAULT)
	{
		act_vector = true;

		if (sub_mag > MAGDEFAULT && sub_mag2 < MAGDEFAULT)
		{
			printf("Should fall inwards\n");
			x = (int)sub_x;
			y = (int)sub_y;
			fell = true;
		}

		if (sub_mag2 > MAGDEFAULT && sub_mag < MAGDEFAULT)
		{
			printf("Should fall outwards\n");
			x = (int)sub_x2;
			y = (int)sub_y2;
			fell = true;
		}
	}

	_tx = x;
	_ty = y;

	float quality = _tracker.GetQuality();
	bool low_quality = false;

	if (quality < QTHRESHOLD)
	{
		///printf("low match quality (%g)\n", quality);

		if (_low_q_ct < QTHR2)
		{
			_low_q_ct++;
		}

		/// things are not going well.
		if (_low_q_ct > QTHR3)
		{
			low_quality = true;
			x = _tx = _px + _dgx;
			y = _ty = _py + _dgy;
		}
	}
	else
	{
		/// ok recovering?
		_low_q_ct -= 3;
		if (_low_q_ct < 0) _low_q_ct = 0;
	}

	_movement = false;

	/// to check for movement (of the target).
	double dist = sqrt((_px-_tx)*(_px-_tx)+(_py-_ty)*(_py-_ty));

	if (fell || (dist > 2) || (sqrt(_dgx * _dgx + _dgy * _dgy) > 2.0))
	{
		_prev.PeerCopy(_mono);
		_px = _tx; _py = _ty;
	}

	/// target moved, all ok?
	if (dist > 5)
	{
		_movement = true;
		_last_movement = now;
	}

	///
	///
	/// just a bit of display of results.
	YarpPixelBGR pix(255,0,0);

	YarpPixelBGR pixg(0,255,0);
	YarpPixelBGR pixb(0,0,255);
	YarpPixelBGR pixr(128,64,0);
	YarpPixelBGR pixk(0,0,0);
	YarpPixelBGR pixw(255,255,255);

	AddCircleOutline (dest, pixw, (int)x, (int)y, 5);
	AddCircleOutline (dest, pixk, (int)x, (int)y, 6);
	AddCircle (dest, pixk, (int)x, (int)y, 4);
	AddCrossHair (dest, pixw, (int)x+1, (int)y, 6);
	AddCrossHair (dest, pixw, (int)x-1, (int)y, 6);
	AddCrossHair (dest, pixw, (int)x, (int)y+1, 6);
	AddCrossHair (dest, pixw, (int)x, (int)y-1, 6);
	AddCrossHair (dest, pixr, (int)x, (int)y, 6);
	AddCircle (dest, pixw, (int)x, (int)y, 2);

	if (act_vector)
	{
		AddCircle (dest, pix, (int)(new_tx + _dgx), (int)(new_ty + _dgy), 3);
		AddCircle (dest, pix, (int)(new_tx2 + _dgx), (int)(new_ty2 + _dgy), 3);
		AddCircle (dest, pixb, (int)sub_x, (int)sub_y, 2);
		AddCircle (dest, pixb, (int)sub_x2, (int)sub_y2, 2);
	}

	_lock.Wait();
	_xx = (int)(x-ISIZE/2+.5);
	_yy = (int)(y-ISIZE/2+.5);
	_lock.Post();
}