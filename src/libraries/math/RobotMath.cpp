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

#include "RobotMath.h"

//
// classe CHmgTrsf 
//
CHmgTrsf::CHmgTrsf(void) : CVisDMatrix(4,4) 
{ 
	(*(CVisDMatrix *)this)=0.0;
	(*this)(4,4)=1.0;
}

CHmgTrsf::CHmgTrsf(const CHmgTrsf& m) : CVisDMatrix(m) {}
CHmgTrsf::CHmgTrsf(const CVisDMatrix& m) : CVisDMatrix(m) {}
CHmgTrsf::CHmgTrsf(const double d) : CVisDMatrix(4,4) { (*this)=d; }

C3dVector CHmgTrsf::Position(void) const 
{
	assert (NRows() != 4 && NCols() != 4);
	C3dVector retMatrix;

	for (int i=1;i<=3;i++)
		retMatrix(i)=(*this)(i,4);

	return retMatrix;
}

CRotMatrix CHmgTrsf::Rotation(void) const 
{ 
	assert (NRows() != 4 && NCols() != 4);
	CRotMatrix retMatrix;

	for (int i=1;i<=3;i++)
		for (int j=1;j<=3;j++)
			retMatrix(i,j)=(*this)(i,j);

	return retMatrix; 
}

C3dVector CHmgTrsf::Z(void) const 
{
	assert (NRows() != 4 && NCols() != 4);
	C3dVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(3,i);

	return retV;
}

C3dVector CHmgTrsf::X(void) const 
{ 
	assert (NRows() != 4 && NCols() != 4);
	C3dVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(1,i);

	return retV;
}
C3dVector CHmgTrsf::Y(void) const 
{ 
	assert (NRows() != 4 && NCols() != 4);
	C3dVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(2,i);

	return retV;
}


// si potrebbe definire una moltiplicazione di una matrice CHmgTrsf * C3dVector!
// come operatore invece che come metodo.

//
// classe CRotMatrix
//
CRotMatrix::CRotMatrix(void) : CVisDMatrix(3,3) 
{ 
	(*(CVisDMatrix *)this)=0.0; 
}

C3dVector CRotMatrix::Z(void) const 
{ 
	assert (NRows() != 3 && NCols() != 3);
	
	C3dVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(3,i);

	return retV;
}

C3dVector CRotMatrix::X(void) const 
{ 
	assert (NRows() != 3 && NCols() != 3);
	
	C3dVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(1,i);

	return retV;
}

C3dVector CRotMatrix::Y(void) const 
{ 
	assert (NRows() != 3 && NCols() != 3);
	
	C3dVector retV;

	for (int i=1;i<=3;i++)
		retV(i)=(*this)(2,i);

	return retV;
}


//
// classe C3dVector
//
C3dVector::C3dVector(void) : CVisDVector(3) 
{ 
	(*this)=0.0; 
}

inline CRotMatrix C3dVector::CrossMatrix(void) const
{
	assert (Length() != 0);

	CRotMatrix rot;
	rot=0.0;
	rot(1,2)=-(*this)(3);
	rot(1,3)=(*this)(2);
	rot(2,1)=(*this)(3);
	rot(2,3)=-(*this)(1);
	rot(3,1)=-(*this)(2);
	rot(3,2)=(*this)(1);

	return rot;
}

C3dVector C3dVector::Cross(const C3dVector& v) const 
{
	assert (v.Length() != 0);

	C3dVector ret;
	ret(1)=(*this)(2)*v(3)-(*this)(3)*v(2);
	ret(2)=-(*this)(1)*v(3)+v(1)*(*this)(3);
	ret(3)=(*this)(1)*v(2)-v(1)*(*this)(2);
  	return ret;
}

//
// classe CDiff
//
CDiff::CDiff(void) : CVisDVector(6) 
{ 
	(*this)=0.0; 
}

//
// classe CForce.
//
CForce::CForce(void) : CVisDVector(6) 
{ 
	(*this)=0.0; 
}

//
// funzioni (DA IMPLEMENTARE).
//
CForce TransForce(const CForce& f,const CHmgTrsf& t)
{
	return f;

#if 0
	Matrix jt(6,6);
	CForce fret;

	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	fret=jt.t()*f;
	return fret;
#endif
}

CDiff TransDiff(const CDiff& d,const CHmgTrsf& t)
{
	return d; // just to avoid complains by the compiler.

#if 0
 	Matrix jt(6,6);
	CDiff fret;

	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	fret=jt*d;
	return fret;
#endif
}

