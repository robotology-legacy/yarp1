/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                      ///
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
/// $Id: YARPQRFactorization.h,v 1.1 2004-09-13 17:54:25 beltran Exp $
///

#ifndef __YARPQRFactorizationh__
#define __YARPQRFactorizationh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPConfigFile.h>

/** 
  * @class YARPQRFactorization
  * @brief This class implements the QR factorization of a matrix A.
  *
  * The code if based in code from the Turku PET Centre: 
  * (http://pet.utu.fi/staff/vesoik/programs/libpet/qr.c)
  * that at the same time have based their routines in the code of the 
  * GNU Scientific Library by Gerard Jungman and Brian Gough
  * (http://sources.redhat.com/gsl/)
  * 
  */
class YARPQRFactorization
{
public:

	/** 
	  * Constructor.
	  */
	YARPQRFactorization(){};
	/** 
	  * Destructor.
	  */
	~YARPQRFactorization(){};

private:

      double ** cchain; /** m*n array of working space. */
      double * chain;   /** 2m array or working space.  */
      YMatrix mcchain;  /** Matrix to support cchain.   */
      YVector vchain;   /** Vector to support chain.    */

	/** 
	  * Returns the euclidean norm of a vector.
	  * 
	  * @param v The pointer to the vector.
	  * @param size The size of the vector.
	  * 
	  * @return The euclidean norm.
	  */
	double 
	householder_norm(double *v, int size) 
	{
		int i;
		double help;

		for(i=0, help=0; i<size; i++) {
			help+=v[i]*v[i];
		}
		return sqrt(help);
	}

	/** 
	  * Applies a householder transformation defined by vector "vector"
	  * and scalar tau to the left-hand side of the matrix.
	  * (I - tau vector vector^I)*matrix
	  * The result of the transform is stored in matrix.
	  * 
	  * @param tau Coefficient defining householder transform.
	  * @param vector Vector defining householdel transform.
	  * @param matrix Pointer to the matrix that is to be transformed.
	  * @param rowNr Number of rows in the matrix.
	  * @param columnNr Number of columns in the matrix. 
	  * 
	  * @return 
	  * 	-# YARP_OK
	  */
	int
	householder_hm (double tau,     
					double *vector, 
					double **matrix,
					int rowNr,      
					int columnNr    
				   ) 
	{
		int i, j;
		double wj, A0j, Aij, vi;

		if (tau == 0.0) {
			return YARP_OK;
		}

		for (j = 0; j < columnNr; j++) {
			/* Compute wj = vk Akj */

			wj = matrix[0][j];

			for (i = 1; i < rowNr; i++)  /* note, computed for v(0) = 1 above */
			{
				wj += vector[i] * matrix[i][j];
			}

			/* Aij = Aij - tau vi wj */

			/* i = 0 */

			A0j = matrix[0][j];
			matrix[0][j]= A0j - tau *  wj;


			/* i = 1 .. M-1 */

			for (i = 1; i < rowNr; i++) {
				Aij = matrix[i][j];
				vi  = vector[i];
				matrix[i][j]= Aij - tau * vi * wj;
			}
		}

		return YARP_OK;
	}

	/** 
	  * This function prepares a Householder transformation
	  * P = I - tau h h^T which can be used to zero all the elements
	  * of the input vector except the first one that will get value beta.
	  * On output the elements 1 - size-1 of the vector h are stored 
	  * in locations vector[1] - vector[size-1] of the input vector
	  * and value of beta is stored in location vector[0]. 
	  * The scalar tau is returned. 
	  * 
	  * @param vector The N-vector to be transformed.
	  * @param N The size of the vector.
	  * @param help N-Array of working space. 
	  * 
	  * @return The value of tau.
	  */
	double
	householder_transform (double *vector, 
						   int N,          
						   double *help   
						  ) 
	{

		double vnorm, alpha, beta, tau;
		double *subvector;
		int n;

		if (N == 1) {
			return 0.0; /* tau = 0 */
		} else {

			/* Local variable */
			subvector=help;

			for(n=1; n<N; n++) {
				subvector[n-1]=vector[n];
			}

			/* Euclidean norm of the subvector */
			vnorm = householder_norm(subvector, N-1);

			if (vnorm == 0) {
				return 0.0; /* tau = 0 */
			}

			/* Computing the coefficient tau */
			alpha = vector[0];
			beta  = - (alpha >= 0.0 ? +1.0 : -1.0) * hypot(alpha, vnorm);
			tau   = (beta - alpha) / beta;

			/* Scale the Householder vector so that the first
			 * element will be 1. (Scaling is also affecting the 
			 * coefficient tau.) Without scaling, the first element
			 * would have value (alpha - beta). 
			 */

			for(n=1; n<N; n++) {
				vector[n]=(1.0 / (alpha - beta))*vector[n];
			}

			vector[0]=beta;

			return tau;
		}
	}

public:
	/** 
	  * Factorise a general M x N matrix A into
	  *   A = Q R
	  * where Q is orthogonal (M x M) and R is upper triangular (M x N).
	  *
	  * Q is stored as a packed set of Householder vectors in the
	  * strict lower triangular part of the input matrix A and a set of
	  * coefficients in vector tau.
	  *
	  * R is stored in the diagonal and upper triangle of the input matrix.
	  * The full matrix for Q can be obtained as the product
	  *
	  *       Q = Q_1 Q_2 .. Q_k  and it's transform as the product
	  *       Q^T = Q_k .. Q_2 Q_1
	  *
	  * where k = min(M,N) and
	  *
	  *       Q_i = (I - tau_i * h_i * h_i^T)
	  *
	  * and where h_i is a Householder vector
	  *
	  *       h_i = [1, A(i+1,i), A(i+2,i), ... , A(M,i)]
	  *
	  * This storage scheme is the same as in LAPACK.  
	  * 
	  * @param a Pointer to the coefficient matrix A(m*n) as input and factorization QR as output.
	  * @param M Rows of the matrix A 
	  * @param N Columns in matrix A
	  * @param tau Pointer to n-vector for householder coefficients.
	  * 
	  * @return YARP_OK if ok.
	  */
	int
    QRFactorization (double **a,      
                     int M,           
                     int N,           
                     double *tau     
					) 
	{
		int i, m, n;
		double *subvector, **submatrix, *helpvector;
		double tau_i;
		double ** cchain;
		double *chain;
		int qr_M,qr_N,qr_MNmin;

		mcchain.Resize(M,N);
		vchain.Resize(2*M);

		cchain = mcchain.data();
		chain  = vchain.data();

		qr_M=M;
		qr_N=N;
		qr_MNmin=qr_N;

		/* Local variables */
		subvector  = chain;
		helpvector = chain + qr_M;
		submatrix  = cchain;

		for (i = 0; i < qr_MNmin; i++) {
			/* Compute the Householder transformation to reduce the j-th
			 * column of the matrix a to a multiple of the j-th unit vector. 
			 * Householder vector h_i is saved in the lower triangular part
			 * of the column and Householder coefficient tau_i in the
			 * vector tau. 
			 */

			for(m=i; m<qr_M; m++) {
				subvector[m-i]= a[m][i];
			}
			tau_i = householder_transform(subvector, qr_M-i, helpvector);

			tau[i]=tau_i;
			for(m=i; m<qr_M; m++) {
				a[m][i]=subvector[m-i];
			}

			/* Apply the transformation to the remaining columns
			   to get upper triangular part of matrix R  */

			if (i + 1 < qr_N) {
				for(m=i; m<qr_M; m++) {
					for(n=i+1; n<qr_N; n++) {
						submatrix[m-i][n-i-1]=a[m][n];
					}
				}

				householder_hm (tau_i, subvector, submatrix, qr_M-i, qr_N-i);

				for(m=i; m<qr_M; m++) {
					for(n=i+1; n<qr_N; n++) {
						a[m][n]=submatrix[m-i][n-i-1];
					}
				}

			}
		}
		return 0;
	}
};
#endif
