///////////////////////////////////////////////////////////////////////////
//
// NAME
//  VisDMatrix.h -- double precision matrix/vector operations
//
// DESCRIPTION
//  The CVisDMatrix class provides some basic matrix operations, using calls
//  to external software (IMSL for the moment) to perform the more
//  complicated operations.
//
//  To take advantage of the IMSL numerical analysis routines,
//  define VIS_USE_IMSL in the Build Settings C/C++ property page.
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
// Modifiche di PASA. 
//
// OK --> 1) Aggiunta operatori del tipo double op Matrix/Vector.
// OK --> 1.1) Aggiunta operatori del tipo op= double e somma con double.
// 2) Supporto least square (copiate da NR). ----->>Da completare<<-----
// 2.bis) Supporto QR decomposition.
// OK --> 3) Operatore () con indici che partono da 1.
// OK --> 4) Operatori cos e sin (solo per vettori).
// 5) moltiplicazione vettore * matrice.
// 6) allocazione automatica su op= vettore/matrice non allocata (da testare). 
//
// 
///////////////////////////////////////////////////////////////////////////

// Readapted by pasa on Dec 2000
// Avoid MFC.
// Dll-> Static library.
// Modified class names,
// 
 
#ifndef __VIS_MATRIX_DMATRIX_H__
#define __VIS_MATRIX_DMATRIX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <iostream>

#include <math.h>
#include <assert.h>

#define VISAPI 
#define VisMatrixExport

//#ifndef _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC
//#endif

// UNDONE: Move inline functions to an INL file.


// LATER:  Could we use std::vector internally or derive from it?  (Would
// the data be adjacent in memory and the operators be as efficient?)


// LATER:  Should there be options to initialize elements when creating or
// resizing?



// QNX Specific.
#ifdef __QNX__
#include "YARPSafeNew.h"
#include "YARPBool.h"
#endif

////////////////////////////////////////////////////////////////////////////
//  
//  CLASS:
//      CVisDVector
//  
////////////////////////////////////////////////////////////////////////////
class CVisDVector
{
public:
    // Constructors (default destructor and copy constructor)
    CVisDVector(void);
    CVisDVector(int length, double *storage = 0);
    CVisDVector(const CVisDVector &vec);
	virtual ~CVisDVector ();

    // Vector length
    int Length(void) const { return m_length; }
    void Resize(int length, double *storage = 0);

    // Element access
    double& operator[](int i) { return m_data[i]; }
    const double& operator[](int i) const { return m_data[i]; }

	double& operator()(int i) { return m_data[--i]; }
	const double& operator()(int i) const { return m_data[--i]; }

    // Assignment
    CVisDVector& operator=(const CVisDVector &vec);
    CVisDVector& operator=(double value);

	// We define operator < so that this class can be used in STL containers.
	bool operator==(const CVisDVector& refvector) const;
	bool operator!=(const CVisDVector& refvector) const;
	bool operator==(double dbl) const;
	bool operator!=(double dbl) const;
	bool operator<(const CVisDVector& refvector) const;
	
	CVisDVector operator+(const CVisDVector &b) const;

	CVisDVector operator-(void) const;
	CVisDVector operator-(const CVisDVector &b) const;

	CVisDVector operator*(double dbl) const;
	double operator*(const CVisDVector &b) const;   // dot product

	// norm 2.
	double norm2(void) const;
	double norm2square(void) const;

	// assume vector is row and multiply.
	//CVisDVector operator*(const CVisDMatrix &A) const;

	CVisDVector operator/(double dbl) const;

	CVisDVector& operator+=(const CVisDVector &b);
	CVisDVector& operator-=(const CVisDVector &b);

	CVisDVector& operator+=(double dbl);
	CVisDVector& operator-=(double dbl);
	CVisDVector operator+(double dbl) const;
	CVisDVector operator-(double dbl) const;

	CVisDVector& operator*=(double dbl);
	CVisDVector& operator/=(double dbl);

	CVisDVector& cos(void);
	CVisDVector& sin(void);

    // Self-describing input/output format
    //enum FieldType { eftName, eftLength, eftData, eftEnd};
    //const char *ReadWriteField(CVisSDStream& s, int field_id);

	// Flag used with the std::ostream file I/O methods.
	// This may not be supported in future releases.
    static bool s_fVerboseOutput; // print out dimensions on output

	inline double * data(void) { return m_data; }

	inline const double * data(void) const {return m_data; }

private:
    int m_length;       // number of elements
    double *m_data;     // pointer to data
    //CVisMemBlockOf<double> m_memblockStorage;  // reference counted storage
};


////////////////////////////////////////////////////////////////////////////
//  
//  CLASS:
//      CVisDMatrix
//  
////////////////////////////////////////////////////////////////////////////
class CVisDMatrix
{
public:
    // Constructors (default destructor and copy constructor)
    CVisDMatrix(void);
    CVisDMatrix(int rows, int cols, double *storage = 0);
    CVisDMatrix(const CVisDMatrix &mat);
	virtual ~CVisDMatrix ();

    // Matrix shape
    int NRows(void) const { return m_nRows; }
    int NCols(void) const { return m_nCols; }
    void Resize(int rows, int cols, double *storage = 0);

    // Element access
    double *operator[](int i) { return m_data[i]; }
    const double *operator[](int i) const { return m_data[i]; }

	double& operator()(int i,int j) { return m_data[--i][--j]; }
	const double& operator()(int i,int j) const { return m_data[--i][--j]; }

    // Assignment
    CVisDMatrix& operator=(const CVisDMatrix &mat);
    CVisDMatrix& operator=(double value);

	// We define operator < so that this class can be used in STL containers.
	bool operator==(const CVisDMatrix& refmatrix) const;
	bool operator!=(const CVisDMatrix& refmatrix) const;
	bool operator==(double dbl) const;
	bool operator!=(double dbl) const;
	bool operator<(const CVisDMatrix& refmatrix) const;

	CVisDMatrix operator+(double dbl) const;
	CVisDMatrix operator-(double dbl) const;
	CVisDMatrix operator+(const CVisDMatrix& A) const;

	CVisDMatrix operator-(void) const;
	CVisDMatrix operator-(const CVisDMatrix& A) const;

	CVisDMatrix operator*(double dbl) const;
	CVisDMatrix operator*(const CVisDMatrix& refmatrix) const;
	CVisDVector operator*(const CVisDVector& refvector) const;

	CVisDMatrix operator/(double dbl) const;

	CVisDMatrix& operator+=(const CVisDMatrix& A);
	CVisDMatrix& operator-=(const CVisDMatrix& A);
	CVisDMatrix& operator+=(double dbl);
	CVisDMatrix& operator-=(double dbl);
	CVisDMatrix& operator*=(double dbl);
	CVisDMatrix& operator*=(const CVisDMatrix& A);
	CVisDMatrix& operator/=(double dbl);
	
	bool IsSymmetric(void) const;

	CVisDMatrix Inverted(void) const;
	CVisDMatrix& Invert(void);

	CVisDMatrix Transposed(void) const;
	CVisDMatrix& Transpose(void);

    // Self-describing input/output format
    //enum FieldType { eftName, eftDims, eftData, eftEnd};
    //const char *ReadWriteField(CVisSDStream& s, int field_id);
    
	// Flag used with the std::ostream file I/O methods.
	// This may not be supported in future releases.
    static bool s_fVerboseOutput; // print out dimensions on output

	inline double ** data(void) { return m_data; }

private:
    int m_nRows;        // number of rows in matrix
    int m_nCols;        // number of columns in matrix
    double **m_data;    // Iliffe vector (array of pointers to data)
    //CVisMemBlockOf<double> m_memblockStorage;  // reference counted storage
    //CVisMemBlockOf<double *> m_memblockIliffe;   // Iliffe vector r. c. storage
};


//
//  Supported functions
//

//
// New pasa operator(s).
//
VisMatrixExport CVisDVector VISAPI operator*(double d,const CVisDVector& A);
VisMatrixExport CVisDMatrix VISAPI operator*(double d,const CVisDMatrix& A);

VisMatrixExport CVisDVector VISAPI operator+(double d,const CVisDVector& A);
VisMatrixExport CVisDVector VISAPI operator-(double d,const CVisDVector& A);
VisMatrixExport CVisDMatrix VISAPI operator+(double d,const CVisDMatrix& A);
VisMatrixExport CVisDMatrix VISAPI operator-(double d,const CVisDMatrix& A);

VisMatrixExport CVisDVector VISAPI cos(const CVisDVector& A);
VisMatrixExport CVisDVector VISAPI sin(const CVisDVector& A);

// LATER:  Add these and fns to work with other exponents.
// VisMatrixExport CVisDMatrix VISAPI VisMatrixSqrt(const CVisDMatrix& A);
// VisMatrixExport void VISAPI VisMatrixSqrt(const CVisDMatrix& A, CVisDMatrix &Asqrt);

VisMatrixExport CVisDMatrix VISAPI VisDMatrixSqrtInverse(const CVisDMatrix& A);
VisMatrixExport void VISAPI VisDMatrixSqrtInverse(const CVisDMatrix& A,
		CVisDMatrix &AsqrtInv);

VisMatrixExport CVisDVector VISAPI VisDMatrixSolve(const CVisDMatrix& A,
		const CVisDVector& b);
VisMatrixExport void VISAPI VisDMatrixSolve(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x);



// A is symmetric positive definite
VisMatrixExport CVisDVector VISAPI VisDMatrixSolveSPD(const CVisDMatrix& A,
		const CVisDVector& b);
VisMatrixExport void VISAPI VisDMatrixSolveSPD(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x, int n = -1);

VisMatrixExport CVisDMatrix VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDMatrix& B);
VisMatrixExport void VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDMatrix& B,
                           CVisDMatrix& X);
VisMatrixExport CVisDVector VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDVector& b);
VisMatrixExport void VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDVector& b,
                           CVisDVector& x);



// QR factorization related functions
VisMatrixExport CVisDVector VISAPI VisDMatrixSolveQR(const CVisDMatrix& A,
		const CVisDVector& b);
VisMatrixExport void VISAPI VisDMatrixSolveQR(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x);
VisMatrixExport void VISAPI VisDMatrixEQConstrainedLS(CVisDMatrix& A,
		CVisDVector& b, CVisDMatrix& C, CVisDVector& d, CVisDVector& x); 


// Singular Value Decomposition (SVD)
VisMatrixExport void VISAPI VisDMatrixSVD(const CVisDMatrix& A, CVisDVector& s, 
		CVisDMatrix& U, CVisDMatrix& V, int compute_left = 1,
		int compute_right = 1);

//
// SVD decomposition.
//
void SVD(CVisDMatrix& a, CVisDVector& w, CVisDMatrix& v);
void SvdSolve(const CVisDMatrix& u,
			  const CVisDVector& w, 
              const CVisDMatrix& v,
			  const CVisDVector& b,
			  CVisDVector& x);
 
VisMatrixExport void VISAPI VisDMatrixSVD(CVisDMatrix& a, CVisDVector& w, CVisDMatrix& v);
VisMatrixExport void VISAPI VisDMatrixSVD(const CVisDMatrix& a, 
										  CVisDMatrix& u, 
										  CVisDVector& w, 
										  CVisDMatrix& v);
VisMatrixExport void VISAPI VisDMatrixSVD(const CVisDMatrix& a,
										  const CVisDVector& b,
										  CVisDVector& x);
//
// LU decomposition.
//
void LU(CVisDMatrix& a, CVisDVector& indx, double& d);
void LuSolve(CVisDMatrix& a, CVisDVector& indx, CVisDVector& b);

VisMatrixExport void VISAPI VisDMatrixLU(CVisDMatrix& a, CVisDVector& indx, double& d);
VisMatrixExport void VISAPI VisDMatrixLU(const CVisDMatrix& a,
										 const CVisDVector& b,
										 CVisDVector& x);

// LATER:  Variations that don't find the eigenvector.
VisMatrixExport void VISAPI VisMinEigenValue(CVisDMatrix& A, CVisDVector& x);
VisMatrixExport void VISAPI VisMaxEigenValue(CVisDMatrix& A, CVisDVector& x);

// min = 1 -> minimum eigenvalue problem; min = 0 -> maximum eigenvalue
VisMatrixExport double VISAPI VisMinMaxEigenValue(CVisDMatrix& A, CVisDVector& x,
		bool fMin); 



#ifndef __QNX__
//  Rudimentary output for debugging
// This may not be supported in future releases.
std::ostream& VISAPI operator<<(std::ostream& os, const CVisDVector& v);
// VisMatrixExport istream& operator>>(istream& os, CVisDVector& v);
// This may not be supported in future releases.
std::ostream& VISAPI operator<<(std::ostream& os, const CVisDMatrix& mat);
// VisMatrixExport istream& operator>>(istream& os, CVisDVector& v);

#else

//  Rudimentary output for debugging
// This may not be supported in future releases.
ostream& VISAPI operator<<(ostream& os, const CVisDVector& v);
// VisMatrixExport istream& operator>>(istream& os, CVisDVector& v);
// This may not be supported in future releases.
ostream& VISAPI operator<<(ostream& os, const CVisDMatrix& mat);
// VisMatrixExport istream& operator>>(istream& os, CVisDVector& v);
#endif


// Self-describing stream I/O
//inline CVisSDStream& operator<<(CVisSDStream& s, CVisDVector& o);
//inline CVisSDStream& operator<<(CVisSDStream& s, CVisDMatrix& o);


#include "VisDMatrix.inl"


#endif // __VIS_MATRIX_DMATRIX_H__
