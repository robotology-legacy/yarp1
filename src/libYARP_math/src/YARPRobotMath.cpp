/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/// $Id: YARPRobotMath.cpp,v 1.1 2004-07-27 10:52:35 babybot Exp $
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

#include <yarp/YARPRobotMath.h>

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
	do {} while (&t == 0);
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
	do {} while (&t == 0);
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

