/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                     ///
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
/// $Id: YARPExMatrix.h,v 1.2 2004-09-13 17:54:25 beltran Exp $
///
#ifndef __YARPExMatrixh__
#define __YARPExMatrixh__

#include "YARPQRFactorization.h"

/**
 * @class YARPCovMatrix 
 * @brief Extents the YMatrix to implement a covariance matrix.
 * 
 * @todo Put this class in a coherent position; discuss whether this is the
 * better way to implemtent this.
 */
class YARPCovMatrix: public YMatrix
{

private:
    YMatrix _mA; /** The original variances matrix used to calculate the covariance
				   matrix if this is the case. In other words, the A matrix in the
				   C = A'A calculation. */
	YMatrix _mR; /** This is the R matrix in the QR factorization. This is used to 
				   factorize A = QR and caculate the determinant of the covariance
				   matrix as det(C) = det(R)^2 */

	YVector _vTau;
	YARPQRFactorization _Qr;
public:
	/** 
	  * Constructor.
	  */
	YARPCovMatrix(){}
	
	/** 
	  * Overloaded constructor with a initialization matrix. 
	  * 
	  * @param refmatrix The reference to the matrix to copy.
	  */
	YARPCovMatrix(YARPCovMatrix &refmatrix):YMatrix(refmatrix)
	{
		refmatrix.getOriginalVariancesMatrix(_mA);
	}

	/** 
	  * Destructor.
	  */
	~YARPCovMatrix(){}

	/** 
	  * Calculates the covariance matrix based in the variances matrix (_mA); _mA needs
	  * to be filled previously.
	  * 
	  * @param flag Whether use n-1 or n to normalize the covariance
	  * 	-# 1 normalize with n
	  * 	-# 0 normalize with n-1
	  * 
	  * @return YARP_FAIL The variances matrix has not been filled.
	  * 		YARP_OK
	  */
	int calculateCovariance(int flag = 0)
	{
		if (_mA.NRows() == 0 || _mA.NCols() == 0)
			return YARP_FAIL;
		else
			if (flag)
				(*this) = (_mA.Transposed() * _mA) / (_mA.NRows()); 
			else
				(*this) = (_mA.Transposed() * _mA) / (_mA.NRows() - 1);

		return YARP_OK;
	}

	/** 
	  * Method to set the A matrix in the case this is a covariance
	  * matrix. This applies to the frequent way of calculating the
	  * covariance matrix as C = A'A (where A' means A transpose).
	  * 
	  * @param mA The external A matrix.
	  * @return YARP_FAIL This is not a covariance matrix.
	  * 		YARP_OK 
	  */
	int setOriginalVariancesMatrix(const YMatrix &mA) 
	{ 
		_mA = mA; 	
		_vTau.Resize(_mA.NCols());
		_mR = _mA; // The _mR matrix will contain the right data well the determinant calculation is called
		calculateR();
		
		return YARP_OK;
	}

	/** 
	  * Gets the A matrix in the case this is a covariance matrix.
	  * 
	  * @param mA The external matrix to fill.
	  * @return YARP_FAIL This is not a covariance matrix.
	  * 		YARP_OK
	  */
	int getOriginalVariancesMatrix(YMatrix &mA) 
	{ 
		mA = _mA; 
		return YARP_OK;
	}

	/** 
	  * Calculate internal R matrix (from QR factorization).
	  * 
	  * @return YARP_OK 
	  */
	int calculateR()
	{
		_Qr.QRFactorization(_mR.data(),
							 _mR.NRows(),
							 _mR.NCols(),
							 _vTau.data());

		return YARP_OK;
	}

	/** 
	  * Function to get the internal R.
	  * 
	  * @param R The reference to the external R to fill.
	  * 
	  * @return YARP_OK
	  */
	int getR(YMatrix &R)
	{
		R = _mR;
		return(YARP_OK);
	}

	/** 
	  * Returns the value of the local vector Tau.
	  * 
	  * @param v A reference to the external vector to fill.
	  * 
	  * @return YARP_OK
	  */
	int getTau(YVector &v)
	{
		v = _vTau;
		return (YARP_OK);
	}

	/** 
	  * Calculates the determinant of the covariance matrix.
	  * 
	  * @param det A reference to a variable where to store the value of the determinant.
	  * 
	  * @return YARP_OK
	  */
	int determinant(double &det)
	{
		int i;
		double determinant = 1.0;

		for( i = 1; i <= _mR.NCols(); i++)
			determinant *= _mR(i,i);

		det = pow(determinant,2);
		
		return YARP_OK;
	}

	YARPCovMatrix& operator=(YARPCovMatrix& refmatrix)
	{
		refmatrix.getR(_mR);
		refmatrix.getOriginalVariancesMatrix(_mA);
		refmatrix.getTau(_vTau);
	}
	
	YARPCovMatrix& operator=(YMatrix& refmatrix)
	{
		double ** refdata = refmatrix.data();
		double ** ldata = data();

		if (NRows() != refmatrix.NRows() || NCols() != refmatrix.NCols())
			Resize(refmatrix.NRows(),refmatrix.NCols());
		memcpy(ldata[0], refdata[0], NRows()*NCols()*sizeof(double));
		return *this;
	}


};


/** 
  * @class YARPExMatrix Extents the YMatrix to implement the covariance 
  * and the determinant.
  * @todo Merge this with the YMatrix code ().
  */
class YARPExMatrix: public YMatrix
{

private:
    YVector _vMeans;          /** The local vector of means (used for the covariance calculation). */
    YMatrix _mLocalVariances; /** The matrix to store the local variances.                         */

public:
	/** 
	  * Constructor. 
	  */
	YARPExMatrix()
	{}

	/** 
	  * Overloaded constructor with size initialization.
	  * 
	  * @param rows The number of rows
	  * @param cols The number of columns
	  * @param *storage A value to fill the matrix with
	  */
	YARPExMatrix(int rows, int cols, const double *storage = 0):YMatrix(rows, cols, storage)
	{
		_vMeans.Resize(rows);
		_mLocalVariances.Resize(rows,cols);
	}

	/** 
	  * Overloaded constructor with a initialization matrix. 
	  * 
	  * @param refmatrix The reference to the matrix to copy.
	  */
	YARPExMatrix(const YMatrix &refmatrix):YMatrix(refmatrix)
	{
		_vMeans.Resize(YMatrix::NRows());
		_mLocalVariances.Resize(NRows(),NCols());
	}

	/** 
	  * Destructor.
	  */
	~YARPExMatrix()
	{}

	/** 
	  * Resize the matrix.
	  * 
	  * @param rows Number of rows.
	  * @param cols Number of columns.
	  * @param *storage A value to fill the matrix with.
	  */
	void Resize(int rows, int cols, const double *storage = 0)
	{
		_vMeans.Resize(rows);
		_mLocalVariances.Resize(rows,cols);
		YMatrix::Resize(rows,cols,storage);
	}

	/** 
	  * Calculates the covariance matrix.
	  * 
	  * @return The covariance matrix.
	  */
	inline int covarianceMatrix(YARPCovMatrix &mCov, int flag = 0)
	{
		int c;
		int r;

		//----------------------------------------------------------------------
		//  Calculate means
		//----------------------------------------------------------------------
		for ( c = 1; c <= YMatrix::NCols(); c++)
		{
			for( r = 1; r <= YMatrix::NRows(); r++)
				_vMeans(c) += ((*this)(r,c));

			_vMeans(c) /= YMatrix::NRows();
		}

		//----------------------------------------------------------------------
		//  Calculate local variances matrix
		//----------------------------------------------------------------------
		for ( r = 1; r <= YMatrix::NRows(); r++)
			for( c = 1; c <= YMatrix::NCols(); c++)
				_mLocalVariances(r,c) = ((*this)(r,c)) - _vMeans(c);

		//----------------------------------------------------------------------
		//  Calculate the final covariance matrix
		//----------------------------------------------------------------------
		if (flag)
			mCov = (_mLocalVariances.Transposed() * _mLocalVariances) / (NRows()); 
		else
			mCov = (_mLocalVariances.Transposed() * _mLocalVariances) / (NRows()-1);

		mCov.setOriginalVariancesMatrix(_mLocalVariances);

		return(YARP_OK);
	}

	/** 
	  * Calculates the determinant of the matrix.
	  * 
	  * @return The determinant value.
	  */
	inline int determinant()
	{
	}
};
#endif
