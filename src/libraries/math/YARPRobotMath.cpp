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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPRobotMath.cpp,v 1.2 2003-05-31 06:02:58 gmetta Exp $
///
///

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  RobotMath.cpp -- double precision matrix/vector operations for robotics
//
// DESCRIPTION
//
// Per ora sono stati implementati alcuni tipi e funzioni di utilita' 
// generale.. sarebbe bello poter estendere e migliorare tutto quanto.
// 
///////////////////////////////////////////////////////////////////////////

//
// Pasa's note: questa implementazione puo' essere migliorata 
//	sfruttando il fatto che e' nota la dimensione delle matrici a 
//	priori (si potrebbero ottimizzare tutti gli operator). 
//
//

#include "YARPRobotMath.h"

//
// classe YHmgTrsf 
//
YHmgTrsf::YHmgTrsf(void) : YMatrix(4,4) 
{ 
	(*(YMatrix *)this)=0.0;
	(*this)(4,4)=1.0;
}

YHmgTrsf::YHmgTrsf(const YHmgTrsf& m) : YMatrix(m) {}
YHmgTrsf::YHmgTrsf(const YMatrix& m) : YMatrix(m) {}
YHmgTrsf::YHmgTrsf(const double d) : YMatrix(4,4) { (*this)=d; }

Y3DVector YHmgTrsf::Position(void) const 
{
	assert (NRows() != 4 && NCols() != 4);
	Y3DVector retMatrix;

	for (int i=1;i<=3;i++)
		retMatrix(i)=(*this)(i,4);

	return retMatrix;
}

YRotMatrix YHmgTrsf::Rotation(void) const 
{ 
	assert (NRows() != 4 && NCols() != 4);
	YRotMatrix retMatrix;

	for (int i=1;i<=3;i++)
		for (int j=1;j<=3;j++)
			retMatrix(i,j)=(*this)(i,j);

	return retMatrix; 
}

Y3DVector YHmgTrsf::Z(void) const 
{
	assert (NRows() != 4 && NCols() != 4);
	Y3DVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(3,i);

	return retV;
}

Y3DVector YHmgTrsf::X(void) const 
{ 
	assert (NRows() != 4 && NCols() != 4);
	Y3DVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(1,i);

	return retV;
}
Y3DVector YHmgTrsf::Y(void) const 
{ 
	assert (NRows() != 4 && NCols() != 4);
	Y3DVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(2,i);

	return retV;
}


// si potrebbe definire una moltiplicazione di una matrice YHmgTrsf * Y3DVector!
// come operatore invece che come metodo.

//
// classe YRotMatrix
//
YRotMatrix::YRotMatrix(void) : YMatrix(3,3) 
{ 
	(*(YMatrix *)this)=0.0; 
}

Y3DVector YRotMatrix::Z(void) const 
{ 
	assert (NRows() != 3 && NCols() != 3);
	
	Y3DVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(3,i);

	return retV;
}

Y3DVector YRotMatrix::X(void) const 
{ 
	assert (NRows() != 3 && NCols() != 3);
	
	Y3DVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(1,i);

	return retV;
}

Y3DVector YRotMatrix::Y(void) const 
{ 
	assert (NRows() != 3 && NCols() != 3);
	
	Y3DVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(2,i);

	return retV;
}


//
// classe Y3DVector
//
Y3DVector::Y3DVector(void) : YVector(3) 
{ 
	(*this)=0.0; 
}

inline YRotMatrix Y3DVector::CrossMatrix(void) const
{
	assert (Length() != 0);

	YRotMatrix rot;
	rot=0.0;
	rot(1,2)=-(*this)(3);
	rot(1,3)=(*this)(2);
	rot(2,1)=(*this)(3);
	rot(2,3)=-(*this)(1);
	rot(3,1)=-(*this)(2);
	rot(3,2)=(*this)(1);

	return rot;
}

Y3DVector Y3DVector::Cross(const Y3DVector& v) const 
{
	assert (v.Length() != 0);

	Y3DVector ret;
	ret(1)=(*this)(2)*v(3)-(*this)(3)*v(2);
	ret(2)=-(*this)(1)*v(3)+v(1)*(*this)(3);
	ret(3)=(*this)(1)*v(2)-v(1)*(*this)(2);
  	return ret;
}

//
// classe YDiff
//
YDiff::YDiff(void) : YVector(6) 
{ 
	(*this)=0.0; 
}

//
// classe YForce.
//
YForce::YForce(void) : YVector(6) 
{ 
	(*this)=0.0; 
}

//
// NOT IMPLEMENTED YET. DO NOT USE!
//
YForce TransForce(const YForce& f,const YHmgTrsf& t)
{
	return f;

#if 0
	Matrix jt(6,6);
	YForce fret;

	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	fret=jt.t()*f;
	return fret;
#endif
}

YDiff TransDiff(const YDiff& d,const YHmgTrsf& t)
{
	return d; // just to avoid complaints by the compiler.

#if 0
 	Matrix jt(6,6);
	YDiff fret;

	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	fret=jt*d;
	return fret;
#endif
}

