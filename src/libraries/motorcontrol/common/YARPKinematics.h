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
/// $Id: YARPKinematics.h,v 1.1 2003-11-07 14:19:34 babybot Exp $
///
///

// DirectKinematics.h: interface for the DirectKinematics class.
//
// compute direct kinematics according to Denavit Hartenberg
//////////////////////////////////////////////////////////////////////

#ifndef __YARPKinematicsh__
#define __YARPKinematicsh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>

#include <YARPMath.h>

///
/// DH params representation
/// 1- a
/// 2- alfa
/// 3- d
///
/// --- non standard, to adapt the angle value from the encoders
///
/// 4- theta - the joint angle offset
/// 5- sign - the joint angle sign (scale actually)
///
///
///

class YARPRobotKinematics  
{
public:
	YARPRobotKinematics ();

	///
	/// construct the class, args are the DH parameters (1 row per joint) and a constant Htrsf with a base tranform
	/// bline is used to roto-translate the origin of the coordinate system.
	YARPRobotKinematics (const YMatrix &dh, const YHmgTrsf &bline);
	YARPRobotKinematics (const YMatrix &dh);
	virtual ~YARPRobotKinematics ();

	/// actual construction, allocation
    void resize (const YMatrix &dh, const YHmgTrsf &bline);
	
	void computeDirect (const YVector &jnts);

	inline const YHmgTrsf & endFrame (void) { return _Ti0[_nFrame-1]; }

	YHmgTrsf *_Ti;
	YHmgTrsf *_Ti0;

	// baseline
	YHmgTrsf _TB0;

	// Denavit Hartenberg parameters
	// a, alfa, d, theta, sign
	YMatrix _dh_param;

	int _nFrame;

protected:
	YVector _joints;

	inline void _computeT (YHmgTrsf &T, double q, double a, double alfa, double d);
};

inline void YARPRobotKinematics::_computeT (YHmgTrsf &T, double q, double a, double alfa, double d)
{
	T(1,1) = cos(q);
	T(1,2) = -sin(q);
	T(1,3) = 0;
	T(1,4) = a;

	T(2,1) = cos(alfa)*sin(q);
	T(2,2) = cos(alfa)*cos(q);
	T(2,3) = -sin(alfa);
	T(2,4) = -sin(alfa)*d;

	T(3,1) = sin(alfa)*sin(q);
	T(3,2) = sin(alfa)*cos(q);
	T(3,3) = cos(alfa);
	T(3,4) = cos(alfa)*d;

	T(4,1) = 0;
	T(4,2) = 0;
	T(4,3) = 0;
	T(4,4) = 1;
}

#endif // !defined(AFX_DIRECTKINEMATICS_H__7C61135C_1BEE_46C5_9C9F_CCE82A4FD40B__INCLUDED_)
