//
// YARPGaussianMixture.h
//	- gaussian mixture model.

//

#ifndef __YARPGaussianMixtureh__
#define __YARPGaussianMixtureh__

#include <assert.h>
#include <math.h>

#include <VisMatrix.h>

#include "YARPRandomNumber.h"
#include "YARPHmmMath.h"

enum YARPVarianceModel { HmmSpherical = 0, HmmDiag = 1, HmmFull = 2, HmmConstant = 3 };

//
// encapsulates a gaussian mixture model
class YARPGaussianMixture
{
protected:
	int m_dim;
	int m_ncentres;
	YARPVarianceModel m_vartype;
	int m_initialized;

	CVisDVector m_kmeans_err;
	double m_error;

public:
	YARPGaussianMixture ()
	{
		m_initialized = false;	
	}

	~YARPGaussianMixture ()
	{
		Destroy ();
	}

	void Destroy (void)
	{
		if (m_initialized)
		{
			Centres++;
			delete[] Centres;
			Covariance++;
			delete[] Covariance;
		}
	}

	CVisDVector Priors;
	CVisDVector *Centres;
	CVisDMatrix *Covariance;

	void Create (int dim, int ncentres, YARPVarianceModel cov_type);
	void Initialize (const CVisDVector **data, int nex, int T);
	void NonRndInitialize (const CVisDVector **data, int nex, int T, const CVisDVector **centers, int Q, int M);
	void KMeans (CVisDVector *centres, CVisDVector& counters, const CVisDVector **data, int nex, int T, int niters, double DELTA1, double DELTA2);

	inline CVisDVector& GetKMeansError (void) { return m_kmeans_err; }
	inline double GetError (void) const { return m_error; }
	inline void SetError (double e) { m_error = e; }

	void Serialize (FILE *fp) const;
	void Unserialize (FILE *fp);
};

#endif