//
// YARPHmmMath.h
//	- misc functions for the HMM implementation.

#ifndef __YARPHmmMathh__
#define __YARPHmmMathh__

#include <stdio.h>
#include <assert.h>
#include <math.h>

#include <VisMatrix.h>

#include "YARPRandomNumber.h"

// fills a vector of normal distributed numbers (mean 0, stddev 1).
void RandN (CVisDVector& v);

// find the distance of every data point from its center (d2), find the minimum and the index of
// the center.
void FindMinimum (const CVisDMatrix& d2, CVisDVector& minvalues, CVisDVector& indexes);

// square of the scalare product.
inline double SquareScalar (const CVisDVector& x, const CVisDVector& y)
{
	double sum = 0;
	assert (x.Length() == y.Length());
	for (int i = 1; i <= x.Length(); i++)
	{
		double tmp = x(i) - y(i);
		sum += (tmp * tmp);
	}

	return sum;
}

// compute the distance of two vectors. Find the max (inf-norm?).
double TestCentresDelta (const CVisDVector *x, const CVisDVector *y, int ncentres);

// fill out a diagonal matrix.
void Diag (CVisDMatrix& x, double value);

// computes the matrix of all possible distances starting from 2 data sets.
void Dist2 (const CVisDVector **data, int nex, int T, const CVisDVector *centres, int ncentres, CVisDMatrix& out);
void Dist2 (const CVisDVector *data, const CVisDVector *centres, int ncentres, CVisDMatrix& out);

// additional vector/matrix manip stuff.
// normalize to 1.
inline void Normalise (CVisDVector& v)
{
	const int len = v.Length();
	double sum = 0;
	for (int i = 1; i <= len; i++)
		sum += v(i);

	if (sum != 0.0)
		v /= sum;
}

inline void Normalise (CVisDMatrix& m)
{
	const int rows = m.NRows();
	const int cols = m.NCols();
	double sum = 0;

	for (int i = 1; i <= rows; i++)
	{
		for (int j = 1; j <= cols; j++)
		{
			sum += m(i,j);
		}
	}

	if (sum != 0.0)
		m /= sum;
}

inline void MkStochastic (CVisDMatrix& m)
{
	const int rows = m.NRows();
	const int cols = m.NCols();
	for (int i = 1; i <= rows; i++)
	{
		double sum = 0;
		
		for (int j = 1; j <= cols; j++)
		{
			sum += m(i,j);
		}

		if (sum != 0.0)
			for (j = 1; j <= cols; j++)
				m(i,j) /= sum;
	}
}

// c must have as many elements as m.NRows().
inline void MkStochastic (CVisDMatrix& m, CVisDVector& c)
{
	const int rows = m.NRows();
	const int cols = m.NCols();
	for (int i = 1; i <= rows; i++)
	{
		c(i) = 0;
		
		for (int j = 1; j <= cols; j++)
		{
			c(i) += m(i,j);
		}

		if (c(i) != 0.0)
			for (j = 0; j <= cols; j++)
				m(i,j) /= c(i);
	}
}

// normalize column t of alpha and returns the norm factor.
inline double NormaliseColumn (CVisDMatrix& alpha, int t)
{
	const int Q = alpha.NRows();

	double n = 0;
	for (int q = 1; q <= Q; q++)
	{
		n += alpha (q, t);
	}

	const double precision = 1e-10;
	if (fabs(n) > precision)
		for (q = 1; q <= Q; q++)
		{
			alpha (q, t) /= n;
		}
	else
		n = 1.0;

	return n;
}

inline void MultiplySpecial (const CVisDMatrix& a, const CVisDMatrix& b, int t, CVisDVector& out)
{
	out = 0;
	for (int j = 1; j <= a.NCols(); j++)
		for (int i = 1; i <= a.NRows(); i++)
		{
			out(j) += a(i,j) * b(i,t);
		}
}

inline void SerializeVector (FILE *fp, const CVisDVector& v)
{
	for (int i = 1; i <= v.Length(); i++)
		fprintf (fp, "%lf ", v(i));
	fprintf (fp, "\n");
}

inline void SerializeMatrix (FILE *fp, const CVisDMatrix& m)
{
	for (int i = 1; i <= m.NRows(); i++)
	{
		for (int j = 1; j <= m.NCols(); j++)
			fprintf (fp, "%lf ", m(i,j));
		fprintf (fp, "\n");
	}
}

inline void UnserializeVector (FILE *fp, CVisDVector& v, int size)
{
	double tmp;
	for (int i = 1; i <= size; i++)
	{
		fscanf (fp, "%lf ", &tmp);
		v(i) = tmp;
	}
	fscanf (fp, "\n");
}

inline void UnserializeMatrix (FILE *fp, CVisDMatrix& m, int r, int c)
{
	double tmp;
	for (int i = 1; i <= r; i++)
	{
		for (int j = 1; j <= c; j++)
		{
			fscanf (fp, "%lf ", &tmp);
			m (i, j) = tmp;
		}
		fscanf (fp, "\n");
	}
}

// computes the covariance matrices out of the data and cluster.
void Covariance (const CVisDVector **data, int nex, int T, const CVisDVector *centres, int ncentres, CVisDMatrix *cov, double defval);

// random numbers from a discrete pdf.
int SampleDiscrete (const CVisDVector& prob);

// sample gaussian pdf.
void SampleGaussianMixture (const CVisDVector& mu, const CVisDMatrix& sigma, CVisDVector& out);
void SampleGaussianMixtureDiagonal (const CVisDVector& mu, const CVisDMatrix& sigma, CVisDVector& out);

// helper allocation.
CVisDVector **AllocVector (int rows, int cols, int size);
CVisDMatrix **AllocMatrix (int rows, int cols, int size);
int **AllocStates (int rows, int cols);
void FreeVector(CVisDVector **ar, int rows);
void FreeMatrix(CVisDMatrix **ar, int rows);
void FreeStates(int **ar, int rows);

// full covariance matrix.
inline double Gaussian (const CVisDVector& x, const CVisDVector& m, const CVisDMatrix& cov)
{
	printf ("Gaussian - not implemented yet\n");
	return 0.0;
}

// diagonal covariance matrix.
inline double GaussianDiagonal (const CVisDVector& x, const CVisDVector& m, const CVisDMatrix& cov)
{
	const double tolerance = 1e-5;

	const int dim = x.Length();
	double detC = 1;
	for (int i = 1; i <= dim; i++)
	{
		if (fabs(cov(i,i)) > tolerance)
			detC *= cov(i,i);
	}

	double denom = pow(2.0 * pi, double(dim) / 2.0) * sqrt(fabs(detC));
	double out = 0;
	
	for (i = 1; i <= dim; i++)
	{
		if (fabs(cov(i,i)) > tolerance)
			out += ((x(i) - m(i)) * (x(i) - m(i)) / cov(i,i));
	}

	return exp (-0.5 * out) / denom;
	//return exp (-0.5 * out - log(denom));
}

#endif