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
///                    #nat, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPBabybotHeadKin.cpp,v 1.1 2004-07-28 17:17:35 babybot Exp $
///
/// read from file (July 04 by nat)

#include "YARPBabybotHeadKin.h"
#include <yarp/YARPConfigFile.h>


#ifdef __WIN32__
#pragma init_seg(lib)
#endif

///
/// global vars.
///
const double DH_left[_dh_nrf][5] = {
	{0, 0, 0, 0, -1},
	{0, -pi/2, 0, -pi/2, 1},
	{125, 0, 0, pi/2, 1},
	{0, pi/2, 0, pi/2, 0},	// zero in the fifth position means it's a convenience trsf.
	{-71.5, 0, 0, pi/2, 1},
};

const double DH_right[_dh_nrf][5] = {
	{0, 0, 0, 0, -1},
	{0, -pi/2, 0, -pi/2, 1},
	{125, 0, 0, pi/2, 1},
	{0, pi/2, 0, pi/2, 0},
	{71.5, 0, 0, pi/2, -1},
};

/// maybe not needed now. Height of the neck
const double TBaseline[4][4] = {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 133},
	{0, 0 ,0, 1},
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPBabybotHeadKin::YARPBabybotHeadKin ()
{

}

///
///
/// each of <dh_left>, <dh_right> describes independently the two kinematic chains from 
/// base to left and base to right eye respectively.
/// each matrix has njoint occasional fictious links lines.
///
YARPBabybotHeadKin::YARPBabybotHeadKin (const YMatrix &dh_left, const YMatrix &dh_right, const YHmgTrsf &bline)
	: _leftCamera (dh_left, bline),
	  _rightCamera (dh_right, bline)
{
	///
	ACE_ASSERT (dh_left.NRows() == dh_right.NRows());

	/// nFrame must be rather the NRows - the # of rows with fifth colum == 0.
	_nFrame = dh_left.NRows();

	_leftJoints.Resize (_nFrame);
	_rightJoints.Resize (_nFrame);

	_leftJoints = 0;
	_rightJoints = 0;

	_q.Resize(3);
	_it.Resize(3);
	_o.Resize(3);
	_epx.Resize(3);
	_tmp.Resize(3);
	_tmpEl.Resize(3);
}

YARPBabybotHeadKin::~YARPBabybotHeadKin ()
{

}

int YARPBabybotHeadKin::load(const YARPString &path, const YARPString &filename)
{
	YARPConfigFile cfgFile;

	// set path and filename
	cfgFile.set(path.c_str(), filename.c_str());
		
	int nj = 0;
	int nrf = 0;
	// get number of joints
	cfgFile.get("[GENERAL]", "Joints", &nj, 1);
	cfgFile.get("[DIRECTKINEMATICS]", "NRefFrame", &nrf, 1);
		
	YMatrix dhLeft;
	YMatrix dhRight;
	YHmgTrsf bline;

	ACE_ASSERT(nrf>0);
	ACE_ASSERT(nj>0);

	dhLeft.Resize(nrf, nj);
	dhRight.Resize(nrf, nj);
	
	cfgFile.get("[DIRECTKINEMATICS]", "DHLeft", dhLeft.data(), nrf, nj);
	cfgFile.get("[DIRECTKINEMATICS]", "DHRight", dhRight.data(), nrf, nj);
	cfgFile.get("[DIRECTKINEMATICS]", "BaseLine", bline.data(), 4, 4);
		
	_leftCamera.resize(dhLeft, bline);
	_rightCamera.resize(dhRight, bline);

	/// nFrame must be rather the NRows - the # of rows with fifth colum == 0.
	_nFrame = nrf;

	_leftJoints.Resize (_nFrame);
	_rightJoints.Resize (_nFrame);

	_leftJoints = 0;
	_rightJoints = 0;

	_q.Resize(3);
	_it.Resize(3);
	_o.Resize(3);
	_epx.Resize(3);
	_tmp.Resize(3);
	_tmpEl.Resize(3);
	
	return YARP_OK;
}

/// <joints is a 5 dim vector>
/// since this file is for the Babybot's head, I can asser if Size is different.
void YARPBabybotHeadKin::computeDirect (const YVector &joints)
{
	ACE_ASSERT (joints.Length() == 5); 

	// the joint vector is devided into right and left
	// I *KNOW* this is awful because doesn't use n_ref_frame
	_leftJoints(1) = joints(1);
	_leftJoints(2) = joints(2);
	_leftJoints(3) = joints(3);
	_leftJoints(4) = joints(5);

	_rightJoints(1) = joints(1);
	_rightJoints(2) = joints(2);
	_rightJoints(3) = joints(3);
	_rightJoints(4) = joints(4);

	_leftCamera.computeDirect (_leftJoints);
	_rightCamera.computeDirect (_rightJoints);
	_computeFixation (_rightCamera.endFrame(), _leftCamera.endFrame());
}

void YARPBabybotHeadKin::computeRay(__kinType k, double& el, double& az, int x, int y)
{
	computeRay(k, _tmpEl, x, y);
	// convert tmpV -> (el,az)
	_cartesianToPolar(_tmpEl, el, az);
}

void YARPBabybotHeadKin::intersectRay (__kinType k, double el, double az, int& x, int& y)
{
	_polarToCartesian(el, az, _tmpEl);
	intersectRay(k, _tmpEl, x, y);
}
///
/// given an up to date kinematic matrix, returns the ray passing from an image plane point x,y.
void YARPBabybotHeadKin::computeRay (__kinType k, YVector& v, int x, int y)
{
	if (k == KIN_LEFT)
	{
		x -= CenterFoveaX;
		y -= CenterFoveaY;

		const YHmgTrsf &ep = _leftCamera.endFrame();

		/// pixels -> mm
		double dx = double(x) / PixScaleX;
		double dy = double(y) / PixScaleY;

		v(1) = F * ep (1, 1) - dx * ep (1, 2) - dy * ep (1, 3);
		v(2) = F * ep (2, 1) - dx * ep (2, 2) - dy * ep (2, 3);
		v(3) = F * ep (3, 1) - dx * ep (3, 2) - dy * ep (3, 3);

		v /= v.norm2();
	}
	else
	if (k == KIN_LEFT_PERI)
	{
		x -= CenterPeripheryX;
		y -= CenterPeripheryY;

		int rx, ry;
		peripheryToFovea (x, y, rx, ry);

		const YHmgTrsf &ep = _leftCamera.endFrame();

		/// pixels -> mm
		double dx = double(rx) / PixScaleX;
		double dy = double(ry) / PixScaleY;

		v(1) = F * ep (1, 1) - dx * ep (1, 2) - dy * ep (1, 3);
		v(2) = F * ep (2, 1) - dx * ep (2, 2) - dy * ep (2, 3);
		v(3) = F * ep (3, 1) - dx * ep (3, 2) - dy * ep (3, 3);

		v /= v.norm2();
	}
	else
	if (k == KIN_RIGHT)
	{
		x -= CenterFoveaX;
		y -= CenterFoveaY;

		const YHmgTrsf &ep = _rightCamera.endFrame();

		/// pixels -> mm
		double dx = double(x) / PixScaleX;
		double dy = double(y) / PixScaleY;

		v(1) = F * ep (1, 1) - dx * ep (1, 2) - dy * ep (1, 3);
		v(2) = F * ep (2, 1) - dx * ep (2, 2) - dy * ep (2, 3);
		v(3) = F * ep (3, 1) - dx * ep (3, 2) - dy * ep (3, 3);

		v /= v.norm2();
	}
	else
	if (k == KIN_RIGHT_PERI)
	{
		x -= CenterPeripheryX;
		y -= CenterPeripheryY;

		int rx, ry;
		peripheryToFovea (x, y, rx, ry);

		const YHmgTrsf &ep = _rightCamera.endFrame();

		/// pixels -> mm
		double dx = double(rx) / PixScaleX;
		double dy = double(ry) / PixScaleY;

		v(1) = F * ep (1, 1) - dx * ep (1, 2) - dy * ep (1, 3);
		v(2) = F * ep (2, 1) - dx * ep (2, 2) - dy * ep (2, 3);
		v(3) = F * ep (3, 1) - dx * ep (3, 2) - dy * ep (3, 3);

		v /= v.norm2();
	}
	else
		v = 0;
}

///
/// given an up to date kin matrix, it computes the x,y point where a given ray v intersects the img plane.
void YARPBabybotHeadKin::intersectRay (__kinType k, const YVector& v, int& x, int& y)
{
	if (k == KIN_LEFT || k == KIN_LEFT_PERI)
	{
		YVector &q = _q;
		YVector &it= _it;
		const YHmgTrsf &ep = _leftCamera.endFrame();

		YVector &o = _o;
		YVector &epx = _epx;

		o(1) = ep(1,4);
		o(2) = ep(2,4);
		o(3) = ep(3,4);
		epx(1) = ep(1,1);
		epx(2) = ep(2,1);
		epx(3) = ep(3,1);

		q = o + F * epx;

		/// intersect plane w/ old ray v.
		/// normal vector to plane is ep(1/2/3, 1)
		double t = F / (ep(1,1)*v(1) + ep(2,1)*v(2) + ep(3,1)*v(3));
		it = v * t + o - q;

		YVector &tmp = _tmp;;
		///tmp(1) = ep(1,1) * it(1) + ep(2,1) * it(2) + ep(3,1) * it(3);
		tmp(2) = ep(1,2) * it(1) + ep(2,2) * it(2) + ep(3,2) * it(3);
		tmp(3) = ep(1,3) * it(1) + ep(2,3) * it(2) + ep(3,3) * it(3);

		/// mm -> pixels
		x = int (-tmp(2) * PixScaleX + .5);
		y = int (-tmp(3) * PixScaleY + .5);

		if (k == KIN_LEFT_PERI)
		{
			int rx = x, ry = y;
			foveaToPeriphery (rx, ry, x, y);

			x += CenterPeripheryX;
			y += CenterPeripheryY;
		}
		else
		{
			x += CenterFoveaX;
			y += CenterFoveaY;
		}
	}
	else
	if (k == KIN_RIGHT || k == KIN_RIGHT_PERI)
	{
		YVector &q = _q;
		YVector &it = _it;
		const YHmgTrsf &ep = _rightCamera.endFrame();

		YVector &o = _o;
		YVector &epx = _epx;

		o(1) = ep(1,4);
		o(2) = ep(2,4);
		o(3) = ep(3,4);
		epx(1) = ep(1,1);
		epx(2) = ep(2,1);
		epx(3) = ep(3,1);

		q = o + F * epx;

		/// intersect plane w/ old ray v.
		/// normal vector to plane is ep(1/2/3, 1)
		double t = F / (ep(1,1)*v(1) + ep(2,1)*v(2) + ep(3,1)*v(3));
		it = v * t + o - q;

		YVector &tmp = _tmp;
		///
		///tmp(1) = ep(1,1) * it(1) + ep(2,1) * it(2) + ep(3,1) * it(3);
		tmp(2) = ep(1,2) * it(1) + ep(2,2) * it(2) + ep(3,2) * it(3);
		tmp(3) = ep(1,3) * it(1) + ep(2,3) * it(2) + ep(3,3) * it(3);

		/// mm -> pixels
		x = int (-tmp(2) * PixScaleX + .5);
		y = int (-tmp(3) * PixScaleY + .5);

		if (k == KIN_RIGHT_PERI)
		{
			int rx = x, ry = y;
			foveaToPeriphery (rx, ry, x, y);
			x += CenterPeripheryX;
			y += CenterPeripheryY;
		}
		else
		{
			x += CenterFoveaX;
			y += CenterFoveaY;
		}
	}
	else
	{
		x = y = 0;
	}
}
