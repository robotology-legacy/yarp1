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
/// $Id: YARPBabybotHeadKin.cpp,v 1.1 2003-11-07 14:19:33 babybot Exp $
///
///

#include "YARPBabybotHeadKin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///
///
/// each of <dh_left>, <dh_right> describes independently the two kinematic chains from 
/// base to left and base to right eye respectively.
/// each matrix has njoint + occasional fictious links lines.
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
}

YARPBabybotHeadKin::~YARPBabybotHeadKin ()
{

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

///
/// given an up to date kinematic matrix, returns the ray passing from an image plane point x,y.
void YARPBabybotHeadKin::computeRay (__kinType k, YVector& v, int x, int y)
{
	ACE_ASSERT (k == KIN_LEFT);

	if (k == KIN_LEFT)
	{
		YHmgTrsf ep = _leftCamera.endFrame();

		/// pixels -> mm
		double dx = x / PixScaleX;
		double dy = y / PixScaleY;

		v(1) = F * ep (1, 1) - dx * ep (1, 2) - dy * ep (1, 3);
		v(2) = F * ep (2, 1) - dx * ep (2, 2) - dy * ep (2, 3);
		v(3) = F * ep (3, 1) - dx * ep (3, 2) - dy * ep (3, 3);

		v /= v.norm2();
	}
}

///
/// given an up to date kin matrix, it computes the x,y point where a given ray v intersects the img plane.
void YARPBabybotHeadKin::intersectRay (__kinType k, const YVector& v, int& x, int& y)
{
	ACE_ASSERT (k == KIN_LEFT);

	if (k == KIN_LEFT)
	{
		YVector q(3), it(3);
		YHmgTrsf ep = _leftCamera.endFrame();

		YVector o(3), epx(3);
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

		YVector tmp(3);

		///
		///tmp(1) = ep(1,1) * it(1) + ep(2,1) * it(2) + ep(3,1) * it(3);
		tmp(2) = ep(1,2) * it(1) + ep(2,2) * it(2) + ep(3,2) * it(3);
		tmp(3) = ep(1,3) * it(1) + ep(2,3) * it(2) + ep(3,3) * it(3);

		/// mm -> pixels
		x = int (-tmp(2) * PixScaleX + .5);
		y = int (-tmp(3) * PixScaleY + .5);
	}
}