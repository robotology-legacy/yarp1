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
/// $Id: YARPExMatrix.h,v 1.1 2004-09-08 17:28:47 beltran Exp $
///
#ifndef __YARPExMatrixh__
#define __YARPExMatrixh__

/**
 * @class YARPCovMatrix Extents the YMatrix to implement a covariance matrix.
 * @todo Put this class in a coherent position; discuss whether this is the
 * better way to implemtent this.
 */
class YARPCovMatrix: public YMatrix
{

private:
    YMatrix _mA; /** The original variances matrix used to calculate the covariance
				   matrix if this is the case. In other words, the A matrix in the
				   C = A'A calculation. */
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
	YARPCovMatrix(const YCovMatrix &refmatrix):YMatrix(refMatrix)
	{
		_mA = refmatrix.getLocalVariancesMatrix();
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
				*this = (_mA.Transposed() * _mA) / (_mA.NRows()); 
			else
				*this = (_mA.Transposed() * _mA) / (_mA.NRows() - 1);

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
		if (getIsCovariance())
			_mA = mA; 
		else
			return YARP_FAIL;
		
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
		if(getIsCovariance())
			mA = _mA; 
		else
			return YARP_FAIL;
		
		return YARP_OK;
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
	YARPExMatrix(const YMatrix &refmatrix):YMatrix(refMatrix)
	{
		_vMeans.Resize(YMatrix::NRows());
		_mLocalVariaces.Resize(YMatrix::NRows,YMatrix::NCols);
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
				_vMeans(c) += (*this(r,c));

			_vMeans(c) /= YMatrix::NRows();
		}

		//----------------------------------------------------------------------
		//  Calculate local variances matrix
		//----------------------------------------------------------------------
		for ( r = 1; r <= YMatrix::NRows(); r++)
			for( c = 1; c <= YMatrix::NCols(); c++)
				_mLocalVariances(r,c) = (*this(r,c)) - _vMeans(c);

		//----------------------------------------------------------------------
		//  Calculate the final covariance matrix
		//----------------------------------------------------------------------
		if (flag)
			mCov = (_mLocalVariances.Transposed() * _mLocalVariances) / (YMatrix::NRows); 
		else
			mCov = (_mLocalVariances.Transposed() * _mLocalVariances) / (YMatrix::NRows-1);

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
