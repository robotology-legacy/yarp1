///////////////////////////////////////////////////////////////////////////
//
// NAME
//  RobotMath.h -- double precision matrix/vector operations for robotics
//
// DESCRIPTION
//
// Per ora sono stati implementati alcuni tipi e funzioni di utilita' 
// generale.. sarebbe bello poter estendere e migliorare tutto quanto.
// 
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// tipi definiti in questo file:
//	CHmgTrsf  4x4
//	CRotMatrix 3x3
//	CDiff   6x1
//	CForce  6x1
//	C3dVector 3x1 
//
//
//////////////////////////////////////////////////////////////////////////

//
// Pasa's note: da completare:
//	1. assertions.
//	2. migliorare alcune routines.. troppe copie!
//	3. transCDiff & CForce non sono stati riscritti (da fare!).
// 

#ifndef __RobotMathh__
#define __RobotMathh__

#ifdef WIN32

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#endif

#ifdef __QNX__
// QNX specific
#endif

#include "VisDMatrix.h"

//
// general purpose constants.
const double pi			=3.14159265359;       // PI definition.
const double radToDeg	=57.29577951308;	  // rad to deg conversion.
const double degToRad	=0.01745329251994;	  // vv.

//
// forward definition.
//
class C3dVector;
class CRotMatrix;

//
// homogeneous transform
//
class VisMatrixExport CHmgTrsf : public CVisDMatrix {
public:
	CHmgTrsf(void);
	virtual ~CHmgTrsf() {}

	CHmgTrsf(const CHmgTrsf&);
	CHmgTrsf(const CVisDMatrix&);
	CHmgTrsf(const double);

    void operator=(const CVisDMatrix& m) { CVisDMatrix::operator=(m); }
    void operator=(double f) { CVisDMatrix::operator=(f); }
    //void operator=(const CHmgTrsf& m) { operator=((const CVisDMatrix&)m); }

	C3dVector Position(void) const;
	CRotMatrix Rotation(void) const;
	C3dVector Z(void) const;
	C3dVector X(void) const;
	C3dVector Y(void) const;

	inline void Zero(void);
	inline void Identity(void);
	inline C3dVector Mpy(const C3dVector& v) const;

	inline C3dVector operator*(const C3dVector&) const;
	inline CHmgTrsf operator*(const CHmgTrsf&);

	// rccl compatibility!
	inline double& nx(void) { return (*this)(1,1); }
	inline double& ny(void) { return (*this)(2,1); }
	inline double& nz(void) { return (*this)(3,1); }
	inline double& ox(void) { return (*this)(1,2); }
	inline double& oy(void) { return (*this)(2,2); }
	inline double& oz(void) { return (*this)(3,2); }
	inline double& ax(void) { return (*this)(1,3); }
	inline double& ay(void) { return (*this)(2,3); }
	inline double& az(void) { return (*this)(3,3); }
	inline double& px(void) { return (*this)(1,4); }
	inline double& py(void) { return (*this)(2,4); }
	inline double& pz(void) { return (*this)(3,4); }
};
								  
//
// rotation matrix
//
class VisMatrixExport CRotMatrix : public CVisDMatrix {
public:
	CRotMatrix(void);
	CRotMatrix(const CRotMatrix& m) : CVisDMatrix(m) {}
	CRotMatrix(const CVisDMatrix& m) : CVisDMatrix(m) {}

	virtual ~CRotMatrix() {}

	void operator=(const CVisDMatrix& m) { CVisDMatrix::operator=(m); }
	void operator=(double f) { CVisDMatrix::operator=(f); }
    void operator=(const CRotMatrix& m) { operator=((const CVisDMatrix&)m); }
	
	C3dVector Z(void) const;
	C3dVector X(void) const;
	C3dVector Y(void) const;

	inline void Zero(void);
	inline void Identity(void);
};

//
// 3d vector (position, etc)
//
class VisMatrixExport C3dVector : public CVisDVector {
public:
	C3dVector(void);
	virtual ~C3dVector() {}

	C3dVector(const C3dVector& v) : CVisDVector(v) {}
	C3dVector(const CVisDVector& v) : CVisDVector(v) {}

	void operator=(const CVisDVector& m) { CVisDVector::operator=(m); }
    void operator=(double f) { CVisDVector::operator=(f); }
    void operator=(const C3dVector& m) { operator=((const CVisDVector&)m); }

	inline CRotMatrix CrossMatrix(void) const;
  	C3dVector Cross(const C3dVector& v) const;
	inline double Scalar(const C3dVector& v) const; // si puo' fare con op *
};

//
// 6d velocity vector
//
class VisMatrixExport CDiff : public CVisDVector {
public:
	CDiff(void); 
	CDiff(const CDiff& d) : CVisDVector(d) {}
	CDiff(const CVisDVector& d) : CVisDVector(d) {}
	virtual ~CDiff() {}

	void operator=(const CVisDVector& m) { CVisDVector::operator=(m); }
    void operator=(double f) { CVisDVector::operator=(f); }
    void operator=(const CDiff& m) { operator=((const CVisDVector&)m); }

// se t va da B->C allora l'uscita e' CDiff in C
	inline CDiff& TransCDiff(const CHmgTrsf& t);
};

//
// 6d force/torque vector
//
class VisMatrixExport CForce : public CVisDVector {
public: 
	CForce(void);
	CForce(const CForce& f) : CVisDVector(f) {}
	CForce(const CVisDVector& f) : CVisDVector(f) {}
	virtual ~CForce() {}

	void operator=(const CVisDVector& m) { CVisDVector::operator=(m); }
    void operator=(double f) { CVisDVector::operator=(f); }
    void operator=(const CForce& m) { operator=((const CVisDVector&)m); }

	inline CForce& TransForce(const CHmgTrsf& t);
};

//
// coordinate system transform for 6d vectors.
// 
CForce TransForce(const CForce&,const CHmgTrsf&);
CDiff  TransCDiff(const CDiff&,const CHmgTrsf&);

#include "RobotMath.inl"

#endif