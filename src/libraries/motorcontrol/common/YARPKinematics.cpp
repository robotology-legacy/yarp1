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
/// $Id: YARPKinematics.cpp,v 1.1 2003-11-07 14:19:34 babybot Exp $
///
///

// RobotKinematics.cpp: implementation of the DirectKinematics class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPKinematics.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPRobotKinematics::YARPRobotKinematics ()
{
	_nFrame = 0;
	_Ti = NULL;
	_Ti0 = NULL;
}

YARPRobotKinematics::YARPRobotKinematics (const YMatrix &dh, const YHmgTrsf &bline)
{
	resize (dh,bline);
}

YARPRobotKinematics::YARPRobotKinematics (const YMatrix &dh)
{
	YHmgTrsf tmp;
	tmp.Identity();
	resize (dh, tmp);
}

YARPRobotKinematics::~YARPRobotKinematics ()
{
	if (_Ti != NULL) delete[] _Ti;
	if (_Ti0 != NULL) delete[] _Ti0;
}

void YARPRobotKinematics::resize (const YMatrix &dh, const YHmgTrsf &bline)
{
	_dh_param = dh;

	_nFrame = dh.NRows();

	_Ti = new YHmgTrsf[_nFrame];
	_Ti0 = new YHmgTrsf[_nFrame];
	ACE_ASSERT (_Ti != NULL && _Ti0 != NULL);

	for(int i = 0; i < _nFrame ; i++)
	{
		_Ti[i].Identity();
		_Ti0[i].Identity();
	}

	_TB0 = bline;
	_joints.Resize(_nFrame);
}

void YARPRobotKinematics::computeDirect (const YVector &jnts)
{
	int i;	//ref frame counter
	int j;	//joint counter
	
	j = 1;
	/// the double counter i,j is used to add intermediate transforms while keeping
	/// the joint vector to its real size (e.g. the number of physical joint of the robot).
	for (i = 1; i <= _nFrame; i++)
	{
		if (_dh_param(i,5) == 0)
		{
			_joints(i) = _dh_param(i,4);
			_computeT(_Ti[i-1], _joints(i), _dh_param(i,1), _dh_param(i,2), _dh_param(i,3));
		}
		else
		{
			_joints(i) = jnts(j) * _dh_param(i,5) + _dh_param(i,4);
			_computeT(_Ti[i-1], _joints(i), _dh_param(i,1), _dh_param(i,2), _dh_param(i,3));
			j++;
		}
		
		if (i == 1)
			_Ti0[i-1] = _TB0 * _Ti[i-1];
		else
			_Ti0[i-1] = _Ti0[i-2] * _Ti[i-1];
	}
}