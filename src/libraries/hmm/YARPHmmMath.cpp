//
// YARPHmmMath.cpp 
//	- misc math functions for the HMM.

#include "YARPHmmMath.h"

void RandN (CVisDVector& v)
{ 
	for (int i = 1; i <= v.Length(); i++)
		v(i) = YARPRandom::RandN();
}

// used for kmeans
void FindMinimum (const CVisDMatrix& d2, CVisDVector& minvalues, CVisDVector& indexes)
{
	for (int k = 1; k <= d2.NRows(); k++)
		minvalues(k) = d2(k, 1);
	indexes = 1;
	
	for (int i = 1; i <= d2.NRows(); i++)
		for (int j = 1; j <= d2.NCols(); j++)
		{
			if (d2(i,j) < minvalues(i))
			{
				minvalues(i) = d2(i,j);
				indexes(i) = j;
			}
		}
}

void Dist2 (const CVisDVector **data, int nex, int T, const CVisDVector *centres, int ncentres, CVisDMatrix& out)
{
	for (int i = 1; i <= nex; i++)
	{
		for (int j = 1; j <= T; j++)
		{
			for (int k = 1; k <= ncentres; k++)
			{
				out ((i-1) * T + j, k) = SquareScalar (data[i][j], centres[k]);
			}
		}
	}
}

// out is ncentres X ncentres
void Dist2 (const CVisDVector *data, const CVisDVector *centres, int ncentres, CVisDMatrix& out)
{
	for (int i = 1; i <= ncentres; i++)
	{
		for (int k = 1; k <= ncentres; k++)
		{
			out (i, k) = SquareScalar (data[i], centres[k]);
		}
	}
}

double TestCentresDelta (const CVisDVector *x, const CVisDVector *y, int ncentres)
{
	// ret could be removed.
	CVisDVector temp;
	temp.Resize (x[1].Length());
	temp = 0;

	double ret = 0;
	double max = 0;

	for (int i = 1; i <= ncentres; i++)
	{
		temp = x[i] - y[i];
		max = temp(1);
		for (int j = 2; j <= x[i].Length(); j++)
			if (max < temp(j))
				max = temp(j);

		if (max > ret)
			ret = max;
	}

	return ret;
}

void Diag (CVisDMatrix& x, double value)
{
	assert (x.NRows() == x.NCols());
	x = 0;

	for (int i = 1; i <= x.NRows(); i++)
		x(i,i) = value;
}

void Covariance (const CVisDVector **data, int nex, int T, const CVisDVector *centres, int ncentres, CVisDMatrix *cov, double defval)
{
	CVisDMatrix d2 (nex * T, ncentres);

	Dist2 (data, nex, T, centres, ncentres, d2);

	CVisDVector minvalues (nex * T);
	CVisDVector indexes (nex * T);

	FindMinimum (d2, minvalues, indexes);

	const int size = data[1][1].Length();

	CVisDMatrix tempvar (size, size);
	tempvar = 0;

	CVisDVector counters (nex * T);
	counters = 0;

	for (int i = 1; i <= ncentres; i++)
		cov[i] = 0;

	//
	for (i = 1; i <= nex * T; i++)
	{
		int index = int(indexes(i));
		counters (index) += 1;
		
		for (int j = 1; j <= size; j++)
			for (int k = 1; k <= size; k++)
			{
				const CVisDVector& tmpv = data[(i-1)/T+1][(i-1)%T+1];
				tempvar (j, k) = (tmpv(j) - centres[index](j)) * (tmpv(k) - centres[index](k));
			}

		cov[index] += tempvar;
	}

	for (i = 1; i <= ncentres; i++)
	{
		if (counters (i) >= 2.0)
			cov[i] /= (counters (i) - 1);
		else
			Diag (cov[i], defval);
	}
}

// a random number drawn from the discrete probability function.
int SampleDiscrete (const CVisDVector& prob)
{
	// R must be != 0
	double R = 0.0;
	while (R == 0.0)
	{
		R = YARPRandom::RandOne ();	// between 0 and 1.
	};

	// count.
	int M = 0;
	double cumsum = 0;
	for (int i = 1; i <= prob.Length(); i++)
	{
		if (R <= cumsum)
			return M;
		else
		{
			cumsum += prob(i);
			M ++;
		}
	}

	return prob.Length();
}

// random vector out of a gaussian distribution (mu, sigma).
void SampleGaussianMixture (const CVisDVector& mu, const CVisDMatrix& sigma, CVisDVector& out)
{
	// LATER:
	// here I would need to diagonalize sigma and get eigenvectors and
	// eigenvalues.
	out = 0;
}

void SampleGaussianMixtureDiagonal (const CVisDVector& mu, const CVisDMatrix& sigma, CVisDVector& out)
{
	int d = mu.Length();

	// we are in diagonal mode thus the evec = I columns and eval = sigma
	// [evec, eval] = eig(covar);

	for (int i = 1; i <= d; i++)
		out(i) = YARPRandom::RandN() * sqrt (sigma(i,i));	// since it's diagonal.
	//coeffs = randn(nsamp, d)*sqrt(eval);

	out += mu;
	//x = ones(nsamp, 1)*mu + coeffs*evec';
}

// alloc double arrays, indexes start from 1.
CVisDVector **AllocVector (int rows, int cols, int size)
{
	CVisDVector **ret = new CVisDVector *[rows];
	assert (ret != NULL);
	ret --; // start from 1.
	for (int i = 1; i <= rows; i++)
	{
		ret[i] = new CVisDVector[cols];
		assert (ret[i] != NULL);
		ret[i] --;

		for (int j = 1; j <= cols; j++)
		{
			ret[i][j].Resize (size);
			ret[i][j] = 0;
		}
	}

	return ret;
}

// matrix is square.
CVisDMatrix **AllocMatrix (int rows, int cols, int size)
{
	CVisDMatrix **ret = new CVisDMatrix *[rows];
	assert (ret != NULL);
	ret --; // start from 1.
	for (int i = 1; i <= rows; i++)
	{
		ret[i] = new CVisDMatrix[cols];
		assert (ret[i] != NULL);
		ret[i] --;

		for (int j = 1; j <= cols; j++)
		{
			ret[i][j].Resize (size, size);
			ret[i][j] = 0;
		}
	}

	return ret;
}

int **AllocStates (int rows, int cols)
{
	int **ret = new int *[rows];
	assert (ret != NULL);
	ret --; // start from 1.
	for (int i = 1; i <= rows; i++)
	{
		ret[i] = new int[cols];
		assert (ret[i] != NULL);
		memset (ret[i], 0, sizeof(int) * cols);
		ret[i] --;
	}

	return ret;
}

void FreeVector(CVisDVector **ar, int rows)
{
	for (int i = 1; i <= rows; i++)
	{
		ar[i] ++;
		delete[] ar[i];
	}

	ar ++;
	delete[] ar;
}

void FreeMatrix(CVisDMatrix **ar, int rows)
{
	for (int i = 1; i <= rows; i++)
	{
		ar[i] ++;
		delete[] ar[i];
	}

	ar ++;
	delete[] ar;
}

void FreeStates(int **ar, int rows)
{
	for (int i = 1; i <= rows; i++)
	{
		ar[i] ++;
		delete[] ar[i];
	}

	ar ++;
	delete[] ar;
}
