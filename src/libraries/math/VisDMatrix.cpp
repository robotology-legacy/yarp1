///////////////////////////////////////////////////////////////////////////
//
// NAME
//  VisMatrix.cpp -- implementation of matrix/vector ops
//
// DESCRIPTION
//  The CVisDMatrix class provides some basic matrix operations, using calls
//  to external software (IMSL for the moment) to perform the more
//  complicated operations.
//
// SEE ALSO
//  CVisDMatrix.h        more complete description
//
// BUGS
//  I sure hope that the IMSL routines don't modify their input
//  parameters!
//
// DESIGN
//  Right now, we call out to IMSL to perform the linear systems
//  solving.  We could change this later.
//
//  To use IMSL, include the following in your 
//  Build Settings Link Object/Library modules:
//      mathd.lib libf.lib
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// Modified by PASA. 
//	see VisDMatrix.h
//
// 
///////////////////////////////////////////////////////////////////////////

#include "VisDMatrix.h"


//  IMSL routines used (double precision)

#ifdef VIS_USE_IMSL
#include "..\VisXIMSL\VisXIMSL.h"
#else // VIS_USE_IMSL
static void VisGaussJordanSolveDMatrix(CVisDMatrix& A);
#endif // VIS_USE_IMSL

// LATER:  The vector and matrix arithmetic operators assume that both
// arguments are of the same size.  We could easily change many of these
// operators to do the right thing when the arguments are not the same
// size.  (For example, adding a 3-vector to a 4-vector.)


//
// Constructors (default destructor and copy constructor)
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        CVisDVector::CVisDVector
//  
//  DECLARATION:
//          CVisDVector::CVisDVector(int length, double *storage);
//  
//  INPUT:
//      length (int) - length of vector
//  
//      *storage (double) - optional storage location (will not dealloca
//                  te on destruction)
//  
//  DISCRIPTION:
//      Create a dynamically sized double precision vector
//  
////////////////////////////////////////////////////////////////////////////
void CVisDVector::Resize(int length, double *storage)
{
    m_length = length;
	if (m_data != NULL)
		delete[] m_data;

	m_data = new double[m_length];
	if (storage != 0)
	{
		memcpy (m_data, storage, sizeof(double) * m_length);
	}
	else
	{
		memset (m_data, 0, sizeof(double) * m_length);
	}
}

CVisDVector::CVisDVector(int length, double *storage) :  m_length(0), m_data(0)
{
    Resize(length, storage);
}

CVisDVector::CVisDVector(void)
  : m_length(0), m_data(0)
{
}

CVisDVector::CVisDVector(const CVisDVector &refvector) :  m_length(0), m_data(0)
{
    //Resize(refvector.Length());
	*this = refvector;
}

CVisDVector::~CVisDVector ()
{
	if (m_data != NULL)
		delete[] m_data;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        CVisDMatrix::CVisDMatrix
//  
//  DECLARATION:
//          CVisDMatrix::CVisDMatrix(int rows, int cols, double *storage);
//  
//  INPUT:
//      rows (int) - number of rows in matrix
//  
//      cols (int) - number of columns in matrix
//  
//      *storage (double) - optional storage location (will not dealloca
//                  te on destruction)
//  
//  DISCRIPTION:
//      Create a dynamically sized double precision matrix
//  
////////////////////////////////////////////////////////////////////////////
void CVisDMatrix::Resize(int rows, int cols, double *storage)
{
	if (m_nRows != rows || m_nCols != cols)
	{
		// deallocate the array of ptrs to rows.
		// if dims are the same do not deallocate.
		if (m_data != NULL)
		{
			delete[] m_data[0];
			delete[] m_data;

			m_data = new double *[rows];
			m_data[0] = new double[rows * cols];
			for (int i = 1; i < rows; i++)
			{
				m_data[i] = m_data[i-1] + cols;
			}

			m_nRows = rows;
			m_nCols = cols;
		}
		else
		{
			// allocate.
			m_data = new double *[rows];
			m_data[0] = new double[rows * cols];
			for (int i = 1; i < rows; i++)
			{
				m_data[i] = m_data[i-1] + cols;
			}

			m_nRows = rows;
			m_nCols = cols;
		}
	}

	if (storage != 0)
		memcpy (m_data[0], storage, sizeof(double) * rows * cols);
	else
		memset (m_data[0], 0, sizeof(double) * rows * cols);
}

CVisDMatrix::CVisDMatrix(int rows, int cols, double *storage) : m_nRows(0), m_nCols(0), m_data(0)
{
    Resize(rows, cols, storage);
}

CVisDMatrix::CVisDMatrix(void) : m_nRows(0), m_nCols(0), m_data(0)
{
}

CVisDMatrix::CVisDMatrix(const CVisDMatrix &refmatrix) : m_nRows(0), m_nCols(0), m_data(0)
{
    //Resize(refmatrix.NRows(), refmatrix.NCols());
	*this = refmatrix;
}

CVisDMatrix::~CVisDMatrix ()
{
	if (m_data != NULL)
	{
		delete[] m_data[0];
		delete[] m_data;
	}
}

//
// Assignment
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          CVisDVector& CVisDVector::operator=(const CVisDVector &vec);
//  
//  RETURN VALUE:
//      vector being copied
//  INPUT:
//      &vec (const CVisDVector) - vector being copied
//  
//  DISCRIPTION:
//      assignment operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector& CVisDVector::operator=(const CVisDVector &vec)
{
	if (Length() != vec.Length())
		Resize(vec.Length());
    memcpy(m_data, vec.m_data, m_length*sizeof(double));
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          CVisDVector& CVisDVector::operator=(double value);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      value (double) - fill value
//  
//  DISCRIPTION:
//      Fill vector with constant value
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector& CVisDVector::operator=(double value)
{
	assert (Length() != 0);
    if (value == 0.0)   // IEEE float
        memset(m_data, 0, m_length*sizeof(double));
    else
        for (int i = 0; i < m_length; i++)
            m_data[i] = value;
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          CVisDMatrix& CVisDMatrix::operator=(const CVisDMatrix &mat);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      &mat (const CVisDMatrix) - matrix being copied
//  
//  DISCRIPTION:
//      Assignment operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDMatrix& CVisDMatrix::operator=(const CVisDMatrix &mat)
{
	if (NRows() != mat.NRows() || NCols() != mat.NCols())
		Resize(mat.m_nRows,mat.m_nCols);
    memcpy(m_data[0], mat.m_data[0], m_nRows*m_nCols*sizeof(double));
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          CVisDMatrix& CVisDMatrix::operator=(double value);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      value (double) - fill value
//  
//  DISCRIPTION:
//      Assignment operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDMatrix& CVisDMatrix::operator=(double value)
{
	assert (m_nRows != 0 && m_nCols != 0);
    if (value == 0.0)   // IEEE float
        memset(m_data[0], 0, m_nRows*m_nCols*sizeof(double));
    else {
        int n = m_nRows * m_nCols;
        double *p = m_data[0];
        for (int i = 0; i < n; i++)
            p[i] = value;
    }
    return *this;
}

//
//  Comparison operators
//

bool CVisDVector::operator==(const CVisDVector& refvector) const
{
    if (Length() == refvector.Length())
	{
		if (Length() == 0)
			return true;

		if (memcmp(m_data, refvector.m_data, Length() * sizeof(double)) == 0)
			return true;
	}

    return false; 
}

bool CVisDMatrix::operator==(const CVisDMatrix& refmatrix) const
{
    if ((NRows() == refmatrix.NRows())
			&& (NCols() == refmatrix.NCols()))
	{
		if ((NRows() == 0) || (NCols() == 0))
			return true;

		int cbRow = NCols()  * sizeof(double);
		for (int iRow = 0; iRow < NRows(); ++iRow)
		{
			if (memcmp(m_data[iRow], refmatrix.m_data[iRow], cbRow) != 0)
				return false;
		}
		
		return true;
	}


    return false; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator==
//  
//  DECLARATION:
//          bool CVisDVector::operator==(double dbl) const;
//			bool CVisDMatrix::operator==(double dbl) const;
//
//  RETURN VALUE:
//      bool
//
//  INPUT:
//      dbl (double) - check value
//  
//  DISCRIPTION:
//      Compare operator(s)
//  
////////////////////////////////////////////////////////////////////////////
bool CVisDVector::operator==(double dbl) const
{
	int n=Length();
	if (n == 0)
		return true;

	bool retflag=true;
	for (int i=0;i<n;i++)
	{
		retflag &= ((*this)[i] == dbl);
	}

    return retflag; 
}

bool CVisDMatrix::operator==(double dbl) const
{
	int n=NRows();
	int m=NCols();
    if ((n*m) == 0)
		return true;

	bool retflag=true;
	for (int i=0;i<n;i++)
		for (int j=0;j<m;j++)
			retflag &= (m_data[i][j] == dbl);

	return retflag;
}

bool CVisDVector::operator<(const CVisDVector& refvector) const
{
    if (Length() == refvector.Length())
	{
		if (Length() == 0)
			return false;

		return (memcmp(m_data, refvector.m_data,
				Length() * sizeof(double)) < 0);
	}

    return (Length() < refvector.Length()); 
}

bool CVisDMatrix::operator<(const CVisDMatrix& refmatrix) const
{
    if (NRows() == refmatrix.NRows())
	{
		if (NCols() == refmatrix.NCols())
		{
			if ((NRows() == 0) || (NCols() == 0))
				return false;

			int cbRow = NCols()  * sizeof(double);
			for (int iRow = 0; iRow < NRows(); ++iRow)
			{
				int wCmp = memcmp(m_data[iRow], refmatrix.m_data[iRow], cbRow);
				if (wCmp != 0)
					return (wCmp < 0);
			}
			
			return false;
		}
    
		return (NCols() < refmatrix.NCols()); 
	}

    return (NRows() < refmatrix.NRows()); 
}


//
//  Matrix / vector products, dot product
//

CVisDVector& CVisDVector::operator+=(const CVisDVector& refvector)
{
    int n = refvector.Length(); 
    assert(n == Length()); 

    for (int i=0; i<n; i++)
        (*this)[i] += refvector[i];

    return *this; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+=
//  
//  DECLARATION:
//          CVisDVector& CVisDVector::operator+=(double dbl);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//
//  INPUT:
//      dbl (double) - add value
//  
//  DISCRIPTION:
//      Sum and assign operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector& CVisDVector::operator+=(double dbl)
{
	int n = Length();
    assert(n != 0); 

    for (int i=0; i<n; i++)
        (*this)[i] += dbl;

    return *this; 
}

CVisDVector& CVisDVector::operator-=(const CVisDVector& refvector)
{
    int n = refvector.Length(); 
    assert(n == Length()); 

    for (int i=0; i<n; i++)
        (*this)[i] -= refvector[i];

    return *this; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator-=
//  
//  DECLARATION:
//          CVisDVector& CVisDVector::operator-=(double dbl);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//
//  INPUT:
//      dbl (double) - subtract value
//  
//  DISCRIPTION:
//      Subtract and assign operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector& CVisDVector::operator-=(double dbl)
{
	int n = Length();
    assert(n != 0); 

    for (int i=0; i<n; i++)
        (*this)[i] -= dbl;

    return *this; 
}

CVisDVector& CVisDVector::operator*=(double dbl)
{
    for (int i=0; i<Length(); i++)
        (*this)[i] *= dbl;

    return *this; 
}

CVisDVector& CVisDVector::operator/=(double dbl)
{
	assert(dbl != 0);

    for (int i=0; i<Length(); i++)
        (*this)[i] /= dbl;

    return *this; 
}

CVisDVector CVisDVector::operator+(const CVisDVector& refvector) const
{
    assert(Length() == refvector.Length()); 
    CVisDVector vectorRet(*this);

    return (vectorRet += refvector); 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+
//  
//  DECLARATION:
//          CVisDVector CVisDVector::operator+(double dbl) const;
//  
//  RETURN VALUE:
//		ret = v + dbl;     
//
//  INPUT:
//      dbl (double) - sum value
//  
//  DISCRIPTION:
//      Sum operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector CVisDVector::operator+(double dbl) const
{
    assert(Length() != 0); 
    CVisDVector vectorRet(*this);

    return (vectorRet += dbl); 
}

CVisDVector CVisDVector::operator-(void) const
{
    CVisDVector vectorRet(Length());

    for (int i=0; i < Length(); i++)
        vectorRet[i] = - (*this)[i];

    return vectorRet; 
}

CVisDVector CVisDVector::operator-(const CVisDVector& refvector) const
{
    assert(Length() == refvector.Length()); 
    CVisDVector vectorRet(*this);

    return (vectorRet -= refvector); 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator-
//  
//  DECLARATION:
//          CVisDVector CVisDVector::operator-(double dbl) const;
//  
//  RETURN VALUE:
//		ret = v - dbl;     
//
//  INPUT:
//      dbl (double) - subtract value
//  
//  DISCRIPTION:
//      Subtract operator
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector CVisDVector::operator-(double dbl) const
{
    assert(Length() != 0); 
    CVisDVector vectorRet(*this);

    return (vectorRet -= dbl); 
}

CVisDVector CVisDVector::operator*(double dbl) const
{
    CVisDVector vectorRet(*this);

    return (vectorRet *= dbl); 
}

CVisDVector CVisDVector::operator/(double dbl) const
{
	assert(dbl != 0);

    CVisDVector vectorRet(*this);

    return (vectorRet /= dbl); 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator*
//  
//  DECLARATION:
//          double CVisDVector::operator*(const CVisDVector& refvector) const;
//  
//  RETURN VALUE:
//		scalar product
//
//  INPUT:
//      refvector 
//  
//  DISCRIPTION:
//      Scalar product
//  
////////////////////////////////////////////////////////////////////////////
double CVisDVector::operator*(const CVisDVector& refvector) const
{
    double sum = 0.0;
    assert(Length() == refvector.Length());

    for (int i = 0; i < Length(); i++)
        sum += (*this)[i] * refvector[i];

    return sum;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        norm2, norm2square
//  
//  DECLARATION:
//          double CVisDVector::norm2(void) const;
//          double CVisDVector::norm2square(void) const;
//  
//  RETURN VALUE:
//		norm 2.
//
//  INPUT:
//      none 
//  
//  DISCRIPTION:
//      Compute the 2 norm of the vector.
//  
////////////////////////////////////////////////////////////////////////////
double CVisDVector::norm2(void) const
{
	int n = Length();
	assert(n != 0);
   
	double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += (m_data[i] * m_data[i]);

    return sqrt(sum);
}

double CVisDVector::norm2square(void) const
{
	int n = Length();
	assert(n != 0);
   
	double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += (m_data[i] * m_data[i]);

    return sum;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        sin,cos
//  
//  DECLARATION:
//          CVisDVector& CVisDVector::cos(void);
//          CVisDVector& CVisDVector::sin(void);
//  
//  RETURN VALUE:
//		reference to l.h.s.
//
//  INPUT:
//      none 
//  
//  DISCRIPTION:
//      Compute the sin (cos) of the vector.
//  
////////////////////////////////////////////////////////////////////////////
CVisDVector& CVisDVector::cos(void)
{
	int n=Length();
	assert(n != 0);

	for (int i = 0; i < n; i++)
		m_data[i]=::cos(m_data[i]);

	return *this;
}

CVisDVector& CVisDVector::sin(void)
{
	int n=Length();
	assert(n != 0);

	for (int i = 0; i < n; i++)
		m_data[i]=::sin(m_data[i]);

	return *this;
}

//
// Specific Matrix code starts here (??).
//
CVisDMatrix& CVisDMatrix::operator+=(const CVisDMatrix& refmatrix)
{
	assert((NRows() == refmatrix.NRows())
			&& (NCols() == refmatrix.NCols()));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] += refmatrix[i][j];

    return *this;
}

CVisDMatrix& CVisDMatrix::operator-=(const CVisDMatrix& refmatrix)
{
	assert((NRows() == refmatrix.NRows())
			&& (NCols() == refmatrix.NCols()));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] -= refmatrix[i][j];

    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+=, operator-=
//  
//  DECLARATION:
//          CVisDMatrix& CVisDMatrix::operator+=(double dbl);
//          CVisDMatrix& CVisDMatrix::operator-=(double dbl);
//  
//  RETURN VALUE:
//		reference to l.h.s.
//
//  INPUT:
//      dbl (double)
//  
//  DISCRIPTION:
//      Sum or subtract dbl.
//  
////////////////////////////////////////////////////////////////////////////
CVisDMatrix& CVisDMatrix::operator+=(double dbl)
{
	assert((NRows() != 0) && (NCols() != 0));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] += dbl;

    return *this;
}

CVisDMatrix& CVisDMatrix::operator-=(double dbl)
{
	assert((NRows() != 0) && (NCols() != 0));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] -= dbl;

    return *this;
}

CVisDMatrix& CVisDMatrix::operator*=(const CVisDMatrix& refmatrix)
{
    CVisDMatrix matrixT(*this);

    // Note that operator= requires that the LHS and RHS have the same
	// dimensions.
	return (*this = (matrixT * refmatrix));
}

CVisDMatrix& CVisDMatrix::operator*=(double dbl)
{
    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] *= dbl;

    return *this;
}

CVisDMatrix& CVisDMatrix::operator/=(double dbl)
{
	assert(dbl != 0);

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] /= dbl;

    return *this;
}

CVisDMatrix CVisDMatrix::operator-(void) const
{
    CVisDMatrix matrixRet(NRows(), NCols());

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            matrixRet[i][j] = - (*this)[i][j];

    return matrixRet;
}

CVisDMatrix CVisDMatrix::operator+(const CVisDMatrix& refmatrix) const
{
    CVisDMatrix matrixRet(*this);

    return (matrixRet += refmatrix);
}

CVisDMatrix CVisDMatrix::operator-(const CVisDMatrix& refmatrix) const
{
    CVisDMatrix matrixRet(*this);

    return (matrixRet -= refmatrix);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+, operator-
//  
//  DECLARATION:
//          CVisDMatrix CVisDMatrix::operator+(double dbl) const;
//          CVisDMatrix CVisDMatrix::operator-(double dbl) const;
//  
//  RETURN VALUE:
//		A new matrix (Sum or difference).
//
//  INPUT:
//      dbl (double)
//  
//  DISCRIPTION:
//      Sum or subtract dbl.
//  
////////////////////////////////////////////////////////////////////////////
CVisDMatrix CVisDMatrix::operator+(double dbl) const
{
    CVisDMatrix matrixRet(*this);

    return (matrixRet += dbl);
}

CVisDMatrix CVisDMatrix::operator-(double dbl) const
{
    CVisDMatrix matrixRet(*this);

    return (matrixRet -= dbl);
}

CVisDMatrix CVisDMatrix::operator*(double dbl) const
{
    CVisDMatrix matrixRet(*this);

    return (matrixRet *= dbl);
}

CVisDMatrix CVisDMatrix::operator/(double dbl) const
{
    CVisDMatrix matrixRet(*this);

    return (matrixRet /= dbl);
}

CVisDVector CVisDMatrix::operator*(const CVisDVector& refvector) const
{
    CVisDVector vectorRet(NRows());

    assert(NRows() == vectorRet.Length()
			&& NCols() == refvector.Length());

    for (int i = 0; i < NRows(); i++) {
        double sum = 0.0;
        for (int j = 0; j < NCols(); j++)
            sum += (*this)[i][j] * refvector[j];
        vectorRet[i] = sum;
    }

	return vectorRet;
}

CVisDMatrix CVisDMatrix::operator*(const CVisDMatrix& refmatrix) const
{
    CVisDMatrix matrixRet(NRows(), refmatrix.NCols());

    assert(NCols() == refmatrix.NRows() &&
           NRows() == matrixRet.NRows() &&
           refmatrix.NCols() == matrixRet.NCols());

    for (int i = 0; i < NRows(); i++) {
        for (int j = 0; j < refmatrix.NCols(); j++) {
            double sum = 0.0;
            for (int k = 0; k < NCols(); k++)
                sum += (*this)[i][k] * refmatrix[k][j];
            matrixRet[i][j] = sum;
        }
    }

    return matrixRet;
}


//
//  Matrix inverse
//

CVisDMatrix CVisDMatrix::Inverted(void) const
{
    CVisDMatrix matrixInverse(NRows(), NCols());

#ifdef VIS_USE_IMSL

    VisIMSL_dlinrg(NRows(), (*this)[0], NRows(),
			matrixInverse[0], NRows());

#else // VIS_USE_IMSL

    // Use Gauss-Jordan elimination
    int i, j, n = NRows();
    CVisDMatrix matrixT(n, 2*n);
    for (i = 0; i < n; i++)       // copy into a single system
        for (j = 0; j < n; j++)
            matrixT[i][j] = (*this)[i][j], matrixT[i][j+n] = (i == j);
    VisGaussJordanSolveDMatrix(matrixT);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            matrixInverse[i][j] = matrixT[i][j+n];

#endif // VIS_USE_IMSL

	return matrixInverse;
}

CVisDMatrix CVisDMatrix::Transposed(void) const
{
    int i, j, n = NRows(), m = NCols();
    CVisDMatrix matrixTranspose(m, n);

    assert(m > 0 && n > 0);

    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            matrixTranspose[i][j] = (*this)[j][i];

	return matrixTranspose;
}

bool CVisDMatrix::IsSymmetric(void) const
{
    if (NRows() != NCols())
        return false;

    for (int i = 0; i < NRows(); i++)
        for (int j = i+1; j < NCols(); j++)
            if ((*this)[i][j] != (*this)[j][i])
                return false;

    return true;
}


//
// Global functions
//


////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSqrtInverse
//  
//  DECLARATION:
//          CVisDMatrix VisDMatrixSqrtInverse(const CVisDMatrix& A);
//  
//  RETURN VALUE:
//      result matrix
//  INPUT:
//      A (const CVisDMatrix&) - input matrix
//  
//  DISCRIPTION:
//      Matrix square root inverse
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport CVisDMatrix VISAPI VisDMatrixSqrtInverse(const CVisDMatrix& A)
{
    CVisDMatrix result(A.NRows(), A.NCols());
    VisDMatrixSqrtInverse(A, result);
    return result;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSqrtInverse
//  
//  DECLARATION:
//     void VisDMatrixSqrtInverse(const CVisDMatrix& A, CVisDMatrix &AsqrtInv);
//  
//  INPUT:
//      A (const CVisDMatrix&) - input matrix
//  
//      &AsqrtInv (CVisDMatrix) - result matrix
//  
//  DISCRIPTION:
//      Matrix t
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSqrtInverse(const CVisDMatrix& A,
		CVisDMatrix &AsqrtInv)
{
#ifdef VIS_USE_IMSL

    assert(A.NRows() == A.NCols() && A.NRows() > 0);
    assert(A.NRows() == AsqrtInv.NRows());
    assert(A.NCols() == AsqrtInv.NCols());

    int i, j, n = A.NRows();
    CVisDMatrix B(n, n);

	B = A.Transposed() * A;

    CVisDVector Eval(n);
    CVisDMatrix Evec(n,n);

    VisIMSL_devcsf(n, B[0], n, &Eval[0], Evec[0], n);

    for (i=0; i<n; i++){
        assert(Eval[i] > 0.0);
        Eval[i] = 1.0/sqrt(Eval[i]); // can be easily modified for sqrt
    }

    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            AsqrtInv[i][j] = 0.0; 
            for (int k=0; k<n; k++)
                AsqrtInv[i][j] += Eval[k] * Evec[k][i] * Evec[k][j];
        }
    }

#else // VIS_USE_IMSL

	// Not implemented!
	assert(0);

    //throw CVisError("Inverse sqrt only works with IMSL for now",
    //                eviserrorOpFailed, "VisDMatrixSqrtInverse",
    //                "VisDMatrix.cpp", __LINE__);

#endif // VIS_USE_IMSL
}


//
//  Linear systems solution
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolve
//  
//  DECLARATION:
//          CVisDVector VisDMatrixSolve(const CVisDMatrix& A,
//                        const CVisDVector& b);
//  
//  RETURN VALUE:
//      result vector
//  INPUT:
//      A (const CVisDMatrix&) - input matrix (l.h.s. of linear system)
//                  
//  
//      b (const CVisDVector&) - input vector (r.h.s. of linear system)
//                  
//  
//  DISCRIPTION:
//      Linear system solution
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport CVisDVector VISAPI VisDMatrixSolve(const CVisDMatrix& A,
		const CVisDVector& b)
{
    CVisDVector result(b.Length());
    VisDMatrixSolve(A, b, result);
    return result;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolve
//  
//  DECLARATION:
//          void VisDMatrixSolve(const CVisDMatrix& A, const CVisDVector& b,
//                        CVisDVector& x);
//  
//  INPUT:
//      A (const CVisDMatrix&) - input matrix (l.h.s. of linear system)
//                  
//  
//      b (const CVisDVector&) - input vector (r.h.s. of linear system)
//                  
//  
//      x (CVisDVector&) - output vector (solution to linear system)
//                  
//  
//  DISCRIPTION:
//      Linear system solution
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSolve(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x)
{
    assert(A.NRows() == A.NCols() && A.NRows() > 0);
    assert(A.NRows() == b.Length());

#ifdef VIS_USE_IMSL

    VisIMSL_dlslrg(A.NRows(), A[0], A.NRows(), &b[0], 2, &x[0]);

#else // VIS_USE_IMSL

    // Use Gauss-Jordan elimination
    int i, j, n = A.NRows();
    CVisDMatrix B(n, n+1);
    for (i = 0; i < n; i++) {       // copy into a single system
        for (j = 0; j < n; j++)
            B[i][j] = A[i][j];
        B[i][n] = b[i];
    }
    VisGaussJordanSolveDMatrix(B);
    for (i = 0; i < n; i++)
        x[i] = B[i][n];

#endif // VIS_USE_IMSL
}

// Symmetric Positive Definite matrix A:

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolveSPD
//  
//  DECLARATION:
//          CVisDVector VisDMatrixSolveSPD(const CVisDMatrix& A,
//                        const CVisDVector& b);
//  
//  RETURN VALUE:
//      result of linear system solution
//  INPUT:
//      A (const CVisDMatrix&) - input matrix
//  
//      b (const CVisDVector&) - input vector (r.h.s. of linear system)
//                  
//  
//  DISCRIPTION:
//      Linear system solution of a symmetric positive definite matrix
//      
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport CVisDVector VISAPI VisDMatrixSolveSPD(const CVisDMatrix& A,
		const CVisDVector& b)
{
    CVisDVector result(b.Length());
    VisDMatrixSolveSPD(A, b, result);
    return result;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolveSPD
//  
//  DECLARATION:
//          void VisDMatrixSolveSPD(const CVisDMatrix& A, const CVisDVector& b,
//                      CVisDVector& x, int n);
//  
//  INPUT:
//      A (const CVisDMatrix&) - input matrix (l.h.s. of linear system)
//                  
//  
//      b (const CVisDVector&) - input vector (r.h.s. of linear system)
//                  
//  
//      x (CVisDVector&) - output vector (solution to linear system)
//                  
//  
//      n (int) - size of linear system (may be smaller than size of A)
//                  
//  
//  DISCRIPTION:
//      Linear system solution of a symmetric positive definite matrix
//      
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSolveSPD(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x, int n)
{
    if (n < 1)      // optionally solve a sub-system (faster)
        n = A.NRows();
    assert(A.NRows() == A.NCols() && A.NRows() > 0);
    assert(A.NRows() == b.Length());

#if defined(_DEBUG) && defined(VIS_USE_IMSL)

    // Before solving, check for all positive eigenvalues
    assert(A.IsSymmetric());
    CVisDVector eigenvalues(b.Length());
    VisIMSL_devlsf(n, A[0], A.NCols(), &eigenvalues[0]);
    for (int i = 0; i < n; i++)
        assert(eigenvalues[i] > 0.0);

#endif // defined(_DEBUG) && defined(VIS_USE_IMSL)

    // Now solve it
#ifdef VIS_USE_IMSL

    x = 0.0;
    VisIMSL_dlslds(n, A[0], A.NCols(), &b[0], &x[0]);

#else // VIS_USE_IMSL

    // Use Gauss-Jordan elimination
    int i, j;
    CVisDMatrix B(n, n+1);
    for (i = 0; i < n; i++) {       // copy into a single system
        for (j = 0; j < n; j++)
            B[i][j] = A[i][j];
        B[i][n] = b[i];
    }
    VisGaussJordanSolveDMatrix(B);
    for (i = 0; i < n; i++)
        x[i] = B[i][n];

#endif // VIS_USE_IMSL
}



////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixLeastSquares
//  
//  DECLARATION:
//      CVisDMatrix VisDMatrixLeastSquares(const CVisDMatrix& A,
//                        const CVisDMatrix& B);
//      void VisDMatrixLeastSquares(const CVisDMatrix& A,
//                        const CVisDMatrix& B, CVisDMatrix& X);
//      CVisDVector VisDMatrixLeastSquares(const CVisDMatrix& A,
//                        const CVisDVectir& b);
//      void VisDMatrixLeastSquares(const CVisDMatrix& A,
//                        const CVisDVector& b, CVisDVector& x);
//  
//  INPUT:
//      A (const CVisDMatrix&) - input matrix (l.h.s. of linear system)
//                  
//      B (const CVisDMatrix&) - input matrix (r.h.s. of linear system)
//  
//      X (CVisDMatrix&) - output matrix (solution to linear system)
//
//      b (const CVisDVector&) - input vector (r.h.s. of linear system)
//                  
//      x (CVisDVector&) - output vector (solution to linear system)
//  
//  DISCRIPTION:
//      Least squares system solution using Householder transforms
//      (with IMSL), or using normal matrix solution (without)
//      
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport CVisDMatrix VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDMatrix& B)
{
    CVisDMatrix result(B.NRows(), B.NCols());
    VisDMatrixLeastSquares(A, B, result);
    return result;
}

VisMatrixExport void VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDMatrix& B, CVisDMatrix& X)
{
    assert(A.NRows() == B.NRows() && A.NRows() > 0);

#ifdef VIS_USE_IMSL

    // Transpose the matrices and form into one
    CVisDMatrix M(A.NCols() + B.NCols(), A.NRows());
    for (int i = 0; i < A.NRows(); i++) {
        for (int j = 0; j < A.NCols(); j++)
            M[j][i] = A[i][j];
        for (j = 0; j < B.NCols(); j++)
            M[j+A.NCols()][i] = B[i][j];
    }

    // Use the IMSL least squares solver (Householder QR decomposition)
    CVisDMatrix N(X.NCols(), X.NRows());
    VisIMSL_dlqrrv(A.NRows(), A.NCols(), B.NCols(), &M[0][0], A.NRows(),
                   &N[0][0], X.NRows());

    // Transpose the result
    for (i = 0; i < X.NRows(); i++) {
        for (int j = 0; j < X.NCols(); j++)
            X[i][j] = N[j][i];
    }

#else // VIS_USE_IMSL

    // Form the normal equations
    CVisDMatrix An(A.NCols(), A.NCols()), Bn(B.NCols(), A.NCols());
    An = 0.0, Bn = 0.0;
    for (int i = 0; i < A.NRows(); i++) {
        for (int j = 0; j < A.NCols(); j++) {
            for (int k = 0; k < A.NCols(); k++)
                An[j][k] += A[i][j] * A[i][k];
            for (k = 0; k < B.NCols(); k++)
                Bn[k][j] += A[i][j] * B[i][k];
        }
    }

    // Solve for each column of X independently
    CVisDVector b(X.NRows());
    for (int j = 0; j < X.NCols(); j++) {
        // Copy the input (rhs)
        for (int i = 0; i < X.NRows(); i++)
            b[i] = Bn[i][j];

        // Solve the system
        CVisDVector x = VisDMatrixSolveSPD(An, b);
        
        // Copy to the output (solution)
        for (i = 0; i < X.NRows(); i++)
            X[i][j] = x[i];
    }

#endif // VIS_USE_IMSL
}

VisMatrixExport CVisDVector VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDVector& b)
{
    CVisDVector result(A.NCols());
    VisDMatrixLeastSquares(A, b, result);
    return result;
}

VisMatrixExport void VISAPI VisDMatrixLeastSquares(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x)
{
    CVisDMatrix B(b.Length(), 1, (double *) &b[0]);
    CVisDMatrix X(x.Length(), 1, &x[0]);
    VisDMatrixLeastSquares(A, B, X);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:     VisGaussJordanSolveDMatrix   
//  
//  DECLARATION:
//          static void VisGaussJordanSolveDMatrix(CVisDMatrix& A)
//  
//  INPUT:
//    CVisDMatrix& A - a matrix of coefficients.
//  
//  DISCRIPTION:
//
// Gauss-Jordan elimination, no pivoting:  not very stable or accurate
//
//  
////////////////////////////////////////////////////////////////////////////


static void VisGaussJordanSolveDMatrix(CVisDMatrix& A)
{
    int n = A.NRows(), m = A.NCols(), i, j, k;
    
    // Reduce to triangular form
    for (i = 0; i < n; i++) {
        
        // Make sure diagonal entry is non-zero
        if (A[i][i] == 0.0) {
            for (j = i+1; j < n; j++)
                if (A[j][i] != 0.0)   // find non-zero entry
                    break;
            if (j >= n)       // if failed, matrix is singular
				assert (0);
                //throw CVisError("matrix is singular", eviserrorOpFailed,
                //         "VisGaussJordanSolveDMatrix()", __FILE__, __LINE__);
            for (k = i; k < m; k++)
                A[i][k] += A[j][k];
        }

        // VisDMatrixScale this row to unity
        double aii_inv = 1.0 / A[i][i];
        A[i][i] = 1.0;
        for (k = i+1; k < m; k++)
            A[i][k] *= aii_inv;

        // Subtract from other rows
        for (j = i+1; j < n; j++) {
            double aij = A[j][i];
            A[j][i] = 0.0;
            for (k = i+1; k < m; k++)
                A[j][k] -= aij * A[i][k];
        }
    }

    // Back substitute
    for (i = n-1; i > 0; i--) {
        for (j = 0; j < i; j++)  {
            double aji = A[j][i];
            A[j][i] = 0.0;
            for (k = n; k < m; k++)
                A[j][k] -= aji * A[i][k];
        }
    }
}



// QR factorization related functions
VisMatrixExport CVisDVector VISAPI VisDMatrixSolveQR(const CVisDMatrix& A,
		const CVisDVector& b)
{
    int nr = A.NRows(); 
    int nc = A.NCols(); 
    assert(nr >= nc && nc > 0);
    assert(nr == b.Length());

    CVisDVector result(nc);
    VisDMatrixSolveQR(A, b, result);
    return result;
}

// IMSL (Fortune) takes column input for a matrix (unlike C which uses row
// indexing
// This is why transpose of A is used 
// unlike SPD solver, QR solver doesn't require A to be square
VisMatrixExport void VISAPI VisDMatrixSolveQR(const CVisDMatrix& A,
		const CVisDVector& b, CVisDVector& x)
{
    int nr = A.NRows(); 
    int nc = A.NCols(); 
    assert(nr >= nc && nc > 0);
    assert(nr == b.Length());

    // Transpose the matrice
    CVisDMatrix M = A.Transposed(); 

    x = 0.0; 
    //VisIMSL_SolveQR(nr, nc, A[0], &b[0], &x[0]);
#ifdef VIS_USE_IMSL
    VisIMSL_SolveQR(nr, nc, M[0], &b[0], &x[0]);
#else // VIS_USE_IMSL
	assert (0);
    //throw CVisError("QR solver only works with IMSL for now",
    //                eviserrorOpFailed, "VisDMatrixSolveQR",
    //                "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSVD
//  
//  DECLARATION:
//      void VisDMatrixSVD(const CVisDMatrix& A, CVisDVector& s,
//                        CVisDMatrix& U, CVisDMatrix& V,
//                        int compute_left = 1, compute_right = 1);
//  
//  INPUT:
//      A (const CVisDMatrix&) - input matrix
//                  
//      s (CVisDVector&) - singular values (output vector)
//
//      U (CVisDMatrix&) - left singular vectors
//  
//      V (CVisDMatrix&) - right singular vectors
//
//      compute_left (int) - compute the left singular vectors
//  
//      compute_right (int) - compute the right singular vectors
//  
//  DISCRIPTION:
//      Singular value decomposition (SVD), using IMSL routine.
//      The matrices returned are of the form A = U diag(s) V^T
//      with the columns of U and V being orthonormal.
//
//      If A in M x N, then s must be of length N,
//      U must be M x N (if computed), and V must be N x N (if computed).
//  
////////////////////////////////////////////////////////////////////////////

VisMatrixExport void VISAPI VisDMatrixSVD(const CVisDMatrix& A, CVisDVector& s, 
                  CVisDMatrix& U, CVisDMatrix& V,
                  int compute_left, int compute_right)
{
#ifdef VIS_USE_IMSL
    long nr = A.NRows(); 
    long nc = A.NCols();
    CVisDVector s2(__min(nr+1, nc));    // required by LSVRR
    CVisDMatrix AT(nc, nr);             // A^T
    CVisDMatrix UT(compute_left ? nc : 1, compute_left ? nr : 1);
    CVisDMatrix VT(compute_right ? nc : 1, compute_right ? nc : 1);
    double eps = 0.0;       // ignored
    long rank_a;             // ignored
    long iPath = compute_left * 20 + compute_right;

    // Perform the transpose
    for (int r = 0; r < nr; r++)
        for (int c = 0; c < nc; c++)
            AT[c][r] = A[r][c];

    VisIMSL_dlsvrr(nr, nc, &AT[0][0], nr, iPath, eps, rank_a,
                   &s2[0], &UT[0][0], nr, &VT[0][0], nc);

    // Copy the singular values
    memcpy(&s[0], &s2[0], __min(s2.Length(), s.Length()) * sizeof(double));
    // cout << "U^T = " << UT << "s = " << s << "V^T = " << VT;

    // Perform the transpose
    if (compute_left) {
        U = 0.0;    // in case nr < nc
        int nc2 = __min(nr, nc);
        for (int r = 0; r < nr; r++)
            for (int c = 0; c < nc2; c++)
                U[r][c] = UT[c][r];
    }
    if (compute_right) {
        for (int r = 0; r < nc; r++)
            for (int c = 0; c < nc; c++)
                V[r][c] = VT[c][r];
    }
#else // VIS_USE_IMSL
	assert (0);
    //throw CVisError(
	//		"Singular value decomposition only works with IMSL for now",
      //              eviserrorOpFailed, "VisDMatrixSVD",
      //              "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL
}


////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSVD
//  
//  DECLARATION:
//      void VisDMatrixSVD(CVisDMatrix& a, CVisDVector& w, CVisDMatrix& v);
//      void VisDMatrixSVD(const CVisDMatrix& a,
//						   CVisDMatrix& u, 
//						   CVisDVector& w, 
//						   CVisDMatrix& v);
//      void VisDMatrixSVD(const CVisDMatrix& a, 
//                         const CVisDVector& b, 
//                         CVisDMatrix& x);
//  
//  INPUT:
//      a (const CVisDMatrix&) - input matrix and U on exit!
//                  
//      w (CVisDVector&) - singular values (output vector)
//
//      v (CVisDMatrix&) - right singular vectors (output matrix)
//  
//		x (CVisDVector&) - solution of the LS problem using SVD
//
//		b (CVisDVector&) - r.h.s. of the linear equation to be solved
//
//  DISCRIPTION:
//      Singular value decomposition (SVD), using NR routine.
//      The matrices returned are of the form A = U diag(w) V^T
//      with the columns of U and V being orthonormal.
//
//      If A in M x N, then w must be of length N,
//      U is of course M x N, and V must be N x N.
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSVD(CVisDMatrix& a, CVisDVector& w, CVisDMatrix& v)
{
	SVD(a,w,v);	
}

VisMatrixExport void VISAPI VisDMatrixSVD(const CVisDMatrix& a,
										  CVisDMatrix& u,
										  CVisDVector& w,
										  CVisDMatrix& v)
{
	u = a;
	SVD(u,w,v);
}

VisMatrixExport void VISAPI VisDMatrixSVD(const CVisDMatrix& a,
										  const CVisDVector& b,
										  CVisDVector& x)
{
	const double Tolerance=1e-6;
	CVisDMatrix u = a;
	CVisDMatrix v(a.NCols(),a.NCols());
	CVisDVector w(a.NCols());

	SVD(u,w,v);

	double wmax,wmin;
	wmax=wmin=0.0;
	for (int k=1;k<=a.NCols();k++) 
		if (w(k)>wmax) wmax=w(k);

	wmin=wmax*Tolerance;
	
	for (k=1;k<=a.NCols();k++) 
		if (w(k)<wmin) w(k)=0.0;

	SvdSolve(u,w,v,b,x);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixLU
//  
//  DECLARATION:
//      void VisDMatrixLU(CVisDMatrix& a, CVisDVector& indx, double& d);
//      void VisDMatrixLU(const CVisDMatrix& a,
//						  const CVisDVector& b, 
//						  CVisDVector& x);
//  
//  INPUT:
//      a (const CVisDMatrix&) - input matrix and U on exit!
//                  
//		x (CVisDVector&) - solution of the LS problem using LU
//
//		b (CVisDVector&) - r.h.s. of the linear equation to be solved
//
//  DISCRIPTION:
//      LU decomposition, using NR routine.
//
//      A must be square. 
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixLU(CVisDMatrix& a, CVisDVector& indx, double& d)
{
	LU (a,indx,d);
}

VisMatrixExport void VISAPI VisDMatrixLU(const CVisDMatrix& a,
										 const CVisDVector& b,
										 CVisDVector& x)
{
	CVisDMatrix acopy(a);
	CVisDVector bcopy(b);
	CVisDVector indx(b.Length());
	double d;

	LU (acopy,indx,d);
	LuSolve(acopy,indx,bcopy);
	x=bcopy;
}

VisMatrixExport void VISAPI VisMinEigenValue(CVisDMatrix& A, CVisDVector& x)
{
#ifdef OLDANDSLOW
	int N = A.NRows();

	CVisDVector Eval(N);
	CVisDMatrix Evec(N, N);

	VisIMSL_devcsf(N, A[0], N, &Eval[0], Evec[0], N);
	for (int i = 0; i < N; i++)
		x[i] = Evec[N-1][i]; 
#else // OLDANDSLOW
    double emin = VisMinMaxEigenValue(A, x, true); 
#endif // OLDANDSLOW
}

VisMatrixExport void VISAPI VisMaxEigenValue(CVisDMatrix& A, CVisDVector& x)
{
#ifdef OLDANDSLOW
	int N = A.NRows();

	CVisDVector Eval(N);
	CVisDMatrix Evec(N, N);

	VisIMSL_devcsf(N, A[0], N, &Eval[0], Evec[0], N);
	for (int i = 0; i < N; i++)
		x[i] = Evec[0][i]; 
#else // OLDANDSLOW
    double emax = VisMinMaxEigenValue(A, x, false); 
#endif // OLDANDSLOW
}

VisMatrixExport double VISAPI VisMinMaxEigenValue(CVisDMatrix& A, CVisDVector& x,
		bool fMin)
{
	int n = A.NRows();
    double e = 0.0; 

    assert(x.Length() == n); 

#ifdef VIS_USE_IMSL
	VisIMSL_devesf(n, 1, A[0], n, (fMin & 1 : 0), &e, &x[0], n);
#else // VIS_USE_IMSL
	assert (0);
    //throw CVisError("Eigenvalues only work with IMSL for now",
      ///              eviserrorOpFailed, "VisMinMaxEigenValue",
         //           "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL

    return e; 
}

// implementation of Equality constrained Least-Squares on p.585 Golub and
// Van Loan also see MSR-TR-97-23
// current implementation assumes rank(A) = n and rank(B) = p -> user's
// responsibility
VisMatrixExport void VISAPI VisDMatrixEQConstrainedLS(CVisDMatrix& A, CVisDVector& b,
		CVisDMatrix& C, CVisDVector& d, CVisDVector& x)
{
#ifdef VIS_USE_IMSL
    int m = A.NRows(); 
    int n = A.NCols(); 
    int p = C.NRows(); 
    int i, j; 

    assert(b.Length() == m);
    assert(x.Length() == n); 
    assert(d.Length() == p); 

    CVisDVector x1(p); 
    CVisDVector x2(n-p); 

    x1 = 0.0; x2 = 0.0; 

    // QR factorization of C^T, i.e., C^T = Q [R | 0]^T
    // CVisDMatrix QR(n, p); 
    CVisDMatrix Q(n, n); 
    CVisDMatrix RT(p, p); 
    long* ipvt = new long[p]; 
    long pivot = 1; 

    CVisDMatrix QRTrans(p, n); 
    CVisDMatrix QTrans(n, n); 

    VisIMSL_GetQR(n, p, C[0], QTrans[0], QRTrans[0], ipvt, pivot); 

    // copy RT from QRTrans
    // Not necessary to create RT, but for clarity
    for (i = 0; i < p; i++) 
    {
        for (j = 0; j < p; j++)
        {
            RT[i][j] = QRTrans[i][j];
        }
    }

	Q = QTrans.Transposed();

    // permute d by P^{-1} d = dPermu
    // this step is necessary: see RoomBuilder paper
    // double* dPermu = new double[p]; 
    CVisDVector dPermu(p); 
    VisIMSL_DPERMU(p, &d[0], ipvt, 1, &dPermu[0]);

    // AQ = [A1, A2]
    CVisDMatrix AQ = A * Q; 
    CVisDMatrix A1(m, p); 
    CVisDMatrix A2(m, n-p); 

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
        {
            A1[i][j] = AQ[i][j]; 
        }
        for (j = p; j < n; j++)
        {
            A2[i][j-p] = AQ[i][j]; 
        }
    }

    // solve x1; RT is a lower triangular matrix
    // or we can call IMSL to solve it
    for (i = 0; i < p; i++)
    {
        double sum = 0; 
        for (j = 0; j < i; j++)
        {
            sum += RT[i][j] * x1[j]; 
        }
        x1[i] = (dPermu[i] - sum)/RT[i][i]; 
    }

    // solve x2; unconstrained LS
    CVisDVector b1 = A1 * x1; 
    CVisDVector b2 = b - b1; 
    VisDMatrixLeastSquares(A2, b2, x2); 

    // get x
    CVisDVector z(n); 
    for (j = 0; j < p; j++)
        z[j] = x1[j];
    for (j = p; j < n; j++)
        z[j] = x2[j-p]; 

    x = Q * z; 
    
    delete ipvt; 
    //delete dPermu; 
#else // VIS_USE_IMSL
	assert (0);
    //throw CVisError("Constrained least squares only works with IMSL for now",
      //              eviserrorOpFailed, "VisDMatrixEQConstrainedLS",
        //            "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL
}


//
//  Standard library Input/output (for debugging, mostly)
//

bool CVisDVector::s_fVerboseOutput = true;
bool CVisDMatrix::s_fVerboseOutput = true;


////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator<<
//  
//  DECLARATION:
//          ostream& operator<<(ostream& os, const CVisDVector& v);
//  
//  RETURN VALUE:
//      reference to output stream
//  INPUT:
//      os (ostream&) - output stream
//  
//      v (CVisDVector&) - vector being printed
//  
//  DISCRIPTION:
//      Print vector on output stream
//  
////////////////////////////////////////////////////////////////////////////
#ifndef __QNX__
VisMatrixExport std::ostream& VISAPI operator<<(std::ostream& os, const CVisDVector& v)
#else
VisMatrixExport ostream& VISAPI operator<<(ostream& os, const CVisDVector& v)
#endif
{
    if (CVisDVector::s_fVerboseOutput)
        os << "# CVisDVector<" << v.Length() << "> = {";
    else
        os << "{";
    for (int i = 0; i < v.Length(); i++)
        os << v[i] << ((i == v.Length()-1) ? "}" : ", ");
#ifndef __QNX__
    if (CVisDVector::s_fVerboseOutput)
        os << std::endl;
#else
    if (CVisDVector::s_fVerboseOutput)
        os << endl;
#endif

    return os;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator<<
//  
//  DECLARATION:
//          ostream& operator<<(ostream& os, const CVisDMatrix& mat );
//  
//  RETURN VALUE:
//      reference to output stream
//  INPUT:
//      os (ostream&) - output stream
//  
//      v (CVisDVector&) - matrix being printed
//  
//  DISCRIPTION:
//      Print matrix on output stream
//  
////////////////////////////////////////////////////////////////////////////

#ifndef __QNX__
VisMatrixExport std::ostream& VISAPI operator<<(std::ostream& os, const CVisDMatrix& mat)
#else
VisMatrixExport ostream& VISAPI operator<<(ostream& os, const CVisDMatrix& mat)
#endif
{
#ifndef __QNX__
    if (CVisDMatrix::s_fVerboseOutput)
        os << "# CVisDMatrix<" << mat.NRows() << "," << 
                              mat.NCols() << "> =" << std::endl;
#else
    if (CVisDMatrix::s_fVerboseOutput)
        os << "# CVisDMatrix<" << mat.NRows() << "," << 
                              mat.NCols() << "> =" << endl;
#endif
    for (int i = 0; i < mat.NRows(); i++) {
        os << ((i == 0) ? "{{" :  " {");
        for (int j = 0; j < mat.NCols(); j++)
            os << mat[i][j] << ((j == mat.NCols()-1) ? "}" : ", ");
        os << ((i == mat.NRows()-1) ? "}" : ",\n");
    }
#ifndef __QNX__
    if (CVisDMatrix::s_fVerboseOutput)
        os << std::endl;
#else
    if (CVisDMatrix::s_fVerboseOutput)
        os << endl;
#endif

    return os;
}



//
//  Self-describing stream Input/output
//
// LATER:  We might want to add newlines and indents to format this better.
//


// Global variables used with self-describing streams.
// LATER:  Find a way to avoid using these global variables.
#ifdef VIS_INCLUDE_SDSTREAM
VisMatrixExport CVisSDObject<CVisDVector> g_visdvectorExemplar;
VisMatrixExport CVisSDObject<CVisDMatrix> g_visdmatrixExemplar;
#endif

#if 0 // REMOVED
const char *CVisDVector::ReadWriteField(CVisSDStream& s, int field_id)
{
#ifdef VIS_INCLUDE_SDSTREAM
    switch (FieldType(field_id))
	{
    case eftName:
        return "class CVisDVector {";

    case eftLength:
		if (s.Status() == CVisSDStream::Read)
		{
			int lengthNew = m_length;
			s << lengthNew;
			assert(lengthNew >= 0);
			if (lengthNew != m_length)
				Resize(lengthNew);
		}
		else
		{
			s << m_length;
		}
		return "int m_length;             // number of elements";

    case eftData:
		s.OpenParen();
		for (int i = 0; i < m_length; i++)
		{
			s << m_data[i];
			if (i < m_length - 1)
				s.Comma();
		}
		s.CloseParen();
        return "double m_data[m_length];  // elements";
    }

    return 0;
#else
	assert(false);  // SDStreams are not included
	return 0;
#endif
}


const char *CVisDMatrix::ReadWriteField(CVisSDStream& s, int field_id)
{
#ifdef VIS_INCLUDE_SDSTREAM
    switch (FieldType(field_id))
	{
    case eftName:
        return "class CVisDMatrix {";

    case eftDims:
		s.OpenParen();
		if (s.Status() == CVisSDStream::Read)
		{
			int nRowsNew = m_nRows;
			int nColsNew = m_nCols;
			s << nRowsNew;
			s.Comma();
			s << nColsNew;
			assert((nRowsNew >= 0) && (nColsNew >= 0));
			if ((nRowsNew != m_nRows) || (nColsNew != m_nCols))
				Resize(nRowsNew, nColsNew);
		}
		else
		{
			s << m_nRows;
			s.Comma();
			s << m_nCols;
		}
		s.CloseParen();
		return "int m_nRows, m_nCols;     // dimensions";

    case eftData:
		s.NewLine();
		s.OpenParen();
		for (int r = 0; r < m_nRows; r++) {
			s.OpenParen();
			for (int c = 0; c < m_nCols; c++) {
				s << (*this)[r][c];
				if (c < m_nCols - 1)
					s.Comma();
			}
			s.CloseParen();
			if (r < m_nRows - 1)
				s.Comma(), s.NewLine();
		}
		s.CloseParen();
		s.NewLine();
        return "double m_data[m_nRows][m_nCols];  // elements";
    }

    return 0;
#else
	assert(false);  // SDStreams are not included
	return 0;
#endif
}
#endif

//
//
//

//
// New operator(s) by pasa.
//

VisMatrixExport CVisDVector VISAPI operator*(double d,const CVisDVector& A)
{
    CVisDVector vectorRet(A);

    return (vectorRet *= d); 
}

VisMatrixExport CVisDMatrix VISAPI operator*(double d,const CVisDMatrix& A)
{
    CVisDMatrix matrixRet(A);

    return (matrixRet *= d);
}

VisMatrixExport CVisDVector VISAPI operator+(double d,const CVisDVector& A)
{
	CVisDVector vectorRet(A);

	return (vectorRet += d);
}

VisMatrixExport CVisDVector VISAPI operator-(double d,const CVisDVector& A)
{
	CVisDVector vectorRet(A);

	return (vectorRet -= d);
}

VisMatrixExport CVisDMatrix VISAPI operator+(double d,const CVisDMatrix& A)
{
	CVisDMatrix matrixRet(A);

	return (matrixRet += d);
}

VisMatrixExport CVisDMatrix VISAPI operator-(double d,const CVisDMatrix& A)
{
	CVisDMatrix matrixRet(A);

	return (matrixRet -= d);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        sin,cos
//  
//  DECLARATION:
//          CVisDVector CVisDVector::cos(const CVisDVector &A);
//          CVisDVector CVisDVector::sin(const CVisDvector &A);
//  
//  RETURN VALUE:
//		reference to l.h.s.
//
//  INPUT:
//      none 
//  
//  DISCRIPTION:
//      Compute the sin (cos) of the vector.
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport CVisDVector VISAPI cos(const CVisDVector &A)
{
	CVisDVector vectorRet(A);

	return vectorRet.cos();
}

VisMatrixExport CVisDVector VISAPI sin(const CVisDVector &A)
{
	CVisDVector vectorRet(A);

	return vectorRet.sin();
}

