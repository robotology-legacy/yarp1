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
/// $Id: YARPBabybotHeadKin.h,v 1.2 2003-11-07 17:31:10 babybot Exp $
///
///

// HeadKinematics.h: interface for the HeadKinematics class.
//
// solve fwd kinematics for the babybot head,
// as of 2003.
//////////////////////////////////////////////////////////////////////

#ifndef __YARPBabybotHeadKinh__
#define __YARPBabybotHeadKinh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

#include <YARPMath.h>
#include <YARPKinematics.h>

///
///
///
/// things to move into configuration file
const int _dh_nrf = 5;

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

/// maybe not needed now. Height of the neck?
const double TBaseline[4][4] = {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 133},
	{0, 0 ,0, 1},
};

///
/// these params are very approximate.
const double F = 4;						/// camera F length.
const double PixScaleX = 120;			/// camera mm to pixel conversion factor.
const double PixScaleY = 120;			/// same along the y coord.

const double Periphery2Fovea = 2.0;		/// conversione from foval to peripheral.

///
///
///	example of using the YARPBabybotHeadKin class.
///
///	  head_kinematics (
///		YMatrix (_dh_nrf, 5, DH_left[0]),
///		YMatrix (_dh_nrf, 5, DH_right[0]), 
///		YHmgTrsf (TBaseline[0]) )
///

///
///
/// Babybot head kinematics.
///
class YARPBabybotHeadKin  
{
public:
	///
	/// eventually it should be possible to call this function also for mapping to the peripheral image.
	/// ONLY foveal remapped now.
	typedef enum { KIN_LEFT = 1, KIN_RIGHT = 2, KIN_LEFT_PERI = 3, KIN_RIGHT_PERI = 4 } __kinType;
	
	///
	/// constructor, takes two set of DH params.
	/// left describes the structure from base to the left eye (camera).
	/// right describes the structure from base to the right eye (camera).
	/// the third parameter is the base to T0 transform. It's simply premultiplied to the 
	/// final T.
	YARPBabybotHeadKin (const YMatrix &dh_left, const YMatrix &dh_right, const YHmgTrsf &bline);
	virtual ~YARPBabybotHeadKin ();

	///
	/// recompute the internal matrices for a new joint position.
	void computeDirect (const YVector& joints);
	inline void update (const YVector& joints) { computeDirect (joints); }

	///
	/// given an up to date kinematic matrix, it returns the ray passing from an image plane point x,y.
	void computeRay (__kinType k, YVector& v, int x, int y);

	///
	/// given an up to date kin matrix, it computes the x,y point where a given ray v intersects the img plane.
	void intersectRay (__kinType k, const YVector& v, int& x, int& y);

	///
	/// given a point in the peripheral img returns the corresp foval one.
	inline void peripheryToFovea (int x, int y, int& rx, int& ry) { rx = x * Periphery2Fovea; ry = y * Periphery2Fovea; }
	inline void foveaToPeriphery (int x, int y, int& rx, int& ry) { rx = x / Periphery2Fovea; ry = y / Periphery2Fovea; }

protected:
	inline void _computeFixation (const YHmgTrsf &T1, const YHmgTrsf &T2);

	YARPRobotKinematics _leftCamera;
	YARPRobotKinematics _rightCamera;

	YVector _leftJoints;
	YVector _rightJoints;
	Y3DVector _fixationPoint;
	int _nFrame;
};

///
///
inline void YARPBabybotHeadKin::_computeFixation (const YHmgTrsf &T1, const YHmgTrsf &T2)
{
	double tmp1;
	double tmp2;
	double tmp3;

	double u;
	
	if (T2(1,1) != 0)
		tmp1 = T2(2,4)-T1(2,4)-(T2(2,1)/T2(1,1))*(T2(1,4)-T1(1,4));
	else
		tmp1 = 0;
	
	tmp3 = T2(2,1)*T1(1,1)-T1(2,1)*T2(1,1);

	if (tmp3 != 0)
		tmp2 = -T2(1,1)/tmp3;
	else
		tmp2 = 0;
	
	u = tmp2*tmp1;

	_fixationPoint(1) = T1(1,4) + T1(1,1) * u;
	_fixationPoint(2) = T1(2,4) + T1(2,1) * u;
	_fixationPoint(3) = T1(3,4) + T1(3,1) * u;

}

#endif
