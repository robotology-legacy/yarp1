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
/// $Id: YARPRobotMath.inl,v 1.3 2003-07-02 10:43:15 babybot Exp $
///
///

// VisDMatrix.inl
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved


#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif // _DEBUG

//
// classe YHmgTrsf.
//

//
// Questo e' altamente inefficiente.
//
inline Y3DVector YHmgTrsf::Mpy(const Y3DVector& v) const 
{
	Y3DVector ret;
	ret=Rotation()*v+Position();
	return ret;
}

inline void YHmgTrsf::Zero(void) { (*this)=0.0; }

inline void YHmgTrsf::Identity(void)
{ 
	(*this)=0.0;
	(*this)(1,1)=1.0;
	(*this)(2,2)=1.0;
	(*this)(3,3)=1.0;
	(*this)(4,4)=1.0;
}

//
// Anche questa operazione e' una porcheria.
//	
inline Y3DVector YHmgTrsf::operator*(const Y3DVector& v) const 
{ 
	return Mpy(v); 
}

inline YHmgTrsf YHmgTrsf::operator*(const YHmgTrsf& h) 
{ 
	return YMatrix::operator*(h); 
}
	
//
// classe YRotMatrix.
//
inline void YRotMatrix::Zero(void) 
{ 
	(*this)=0.0; 
}

inline void YRotMatrix::Identity(void) 
{
	(*this)=0.0;
	(*this)(1,1)=1.0;
	(*this)(2,2)=1.0;
	(*this)(3,3)=1.0;
}

//
// classe Y3DVector.
//
inline double Y3DVector::Scalar(const Y3DVector& v) const
{
	double ret=0;
	ret+=(*this)(1)*v(1);
	ret+=(*this)(2)*v(2);
	ret+=(*this)(3)*v(3);
	return ret;
}

//
// classe YDiff.
//
inline YDiff& YDiff::TransYDiff(const YHmgTrsf& t)  // trasformazione di coordinate
{
	do {} while (&t == 0);
	return *this;
	// to be implemented!
#if 0
	Matrix jt(6,6);
	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	(*this)=jt*(*this);
	return *this;
#endif
}

//
// classe YForce.
//
inline YForce& YForce::TransForce(const YHmgTrsf& t)
{
	do {} while (&t == 0);
	return *this;
	// da implementare!
#if 0
	Matrix jt(6,6);
	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	(*this)=jt.t()*(*this);
	return *this;
#endif
}

#ifdef _DEBUG
#undef new
#endif // _DEBUG
