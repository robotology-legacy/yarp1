//
// YARPGaussianMixture.cpp
//	-
//

#include "YARPGaussianMixture.h"

//
//
void YARPGaussianMixture::Serialize (FILE *fp) const
{
	if (!m_initialized)
	{
		printf ("Gaussian mixture - nothing to serialize\n");
		return;
	}

	fprintf (fp, "%d %d ", m_dim, m_ncentres);
	fprintf (fp, "%d\n", m_vartype);

	//SerializeVector (fp, k_means_err);
	fprintf (fp, "%lf\n", m_error);

	// all of size m_ncentres.
	SerializeVector (fp, Priors);
	for (int i = 1; i <= m_ncentres; i++)
	{
		SerializeVector (fp, Centres[i]);
		SerializeMatrix (fp, Covariance[i]);
	}
}

void YARPGaussianMixture::Unserialize (FILE *fp) 
{
	if (m_initialized)
		Destroy ();

	fscanf (fp, "%d %d ", &m_dim, &m_ncentres);
	fscanf (fp, "%d\n", &m_vartype);

	//SerializeVector (fp, k_means_err);
	fscanf (fp, "%lf\n", &m_error);

	Create (m_dim, m_ncentres, m_vartype);

	// all of size m_ncentres.
	UnserializeVector (fp, Priors, m_ncentres);
	for (int i = 1; i <= m_ncentres; i++)
	{
		UnserializeVector (fp, Centres[i], m_dim);
		UnserializeMatrix (fp, Covariance[i], m_dim, m_dim);
	}
}

// KMeans. 
// data (nexample, time)
// centres (Q*M) each of size dim.
//
// DELTA1 is the delta on the center of clusters.
// DELTA2 is the delta on the difference of absolute error.
void YARPGaussianMixture::KMeans (CVisDVector *centres, CVisDVector& counters, const CVisDVector **data, int nex, int T, int niters, double DELTA1, double DELTA2)
{
	// if dim ~= data_dim
	assert (m_ncentres <= nex * T);

	m_kmeans_err.Resize (niters);
	m_kmeans_err = 0;

	// Check if centres and posteriors need to be initialised from data
	bool initialize_from_data = false;
	if (initialize_from_data)
	{
		const int range = nex * T;

		// initialize the centers from data.
		for (int i = 1; i <= m_ncentres; i++)
		{
			int index = int(YARPRandom::Rand (0, range-1));
			Centres[i] = data[index/T+1][index%T+1];
		}
	}

	CVisDVector *old_centres = new CVisDVector[m_ncentres];
	assert (old_centres != NULL);
	old_centres --;		// start from 1.

	// result of dist2.
	CVisDMatrix d2 (nex * T, m_ncentres);
	d2 = 0;
	CVisDVector minvalues (nex * T);	// the min.
	minvalues = 0;
	CVisDVector indexes (nex * T);		// the arg min.
	indexes = 0;

	double error = 0;
	double old_error = 0;

	// Main loop of algorithm
	for (int n = 1; n <= niters; n++)
	{
		// Save old centres to check for termination
		for (int j = 1; j <= m_ncentres; j++)
			old_centres[j] = centres[j];
  
		// Calculate posteriors based on existing centres
		// Compute all distances for each possible sample to each possible center.
		Dist2(data, nex, T, centres, m_ncentres, d2);

		// Assign each point to the nearest centre.
		// Search the minimum along rows of d2.
		FindMinimum (d2, minvalues, indexes);

		// Reset centres and use them as cumulative (for the average).
		for (j = 1; j <= m_ncentres; j++)
			centres[j] = 0;

		// holds the number of elements in each cluster.
		counters = 0;

		// run through all the samples.
		for (int smp = 1; smp <= nex * T; smp++)
		{
			centres[int(indexes(smp))] += data[(smp-1)/T+1][(smp-1)%T+1];
			counters(int(indexes(smp))) += 1.0;			
		}

		// take the average.
		for (j = 1; j <= m_ncentres; j++)
		{
			if (counters(j) > 0.0)
				centres[j] /= counters(j);
			else
				centres[j] = old_centres[j];	// use the old one.
		}

		// now check the termination condition(s).
		// Error value is total squared distance from cluster centres
		error = 0;
		for (smp = 1; smp <= nex * T; smp++)
			error += minvalues(smp);

		// store the error.
		m_kmeans_err(n) = error;
		if (n > 1)
		{
			// termination test center distance and error difference.
			if (TestCentresDelta (Centres, old_centres, m_ncentres) < DELTA1 &&
				fabs(old_error - error) < DELTA2)
			{
				goto YARPMixtureSmoothReturn;
			}
		}

		old_error = error;
	}

	// If we get here, then we haven't terminated in the given number of 
	// iterations.
	printf ("Warning in kmeans: Maximum number of iterations has been exceeded\n");

YARPMixtureSmoothReturn:

	m_error = error;

	old_centres ++;
	delete[] old_centres;

	return;
}


// data has size [nex][T]
// each vector has the same size as the output O.

void YARPGaussianMixture::Initialize (const CVisDVector **data, int nex, int T)
{
	// [ndata, xdim] = size(data);

	// Arbitrary width used if variance collapses to zero: make it 'large' so
	// that centre is responsible for a reasonable number of points.
	double GMM_WIDTH = 1.0;

	// get ncentres random vectors out of data.
	const int range = nex * T;

	// initialize the centers from data.
	for (int i = 1; i <= m_ncentres; i++)
	{
		int index = YARPRandom::Rand (0, range-1);
		Centres[i] = data[index/T+1][index%T+1];
	}

	//
	CVisDVector cluster_sizes (m_ncentres);
	
	// Use kmeans algorithm to set centres, result in Centres.
	KMeans (Centres, cluster_sizes, data, nex, T, 100, 1e-4, 1e-4);

	// Set priors depending on number of points in each cluster
	double cluster_sum = 0;
	for (i = 1; i <= m_ncentres; i++)
 		cluster_sum += cluster_sizes(i);

	for (i = 1; i <= m_ncentres; i++)
		Priors (i) = 
			((cluster_sizes(i) > 0.0) ? cluster_sizes(i) : 1.0) / cluster_sum;

	// Initialize covariance matrix.
	switch (m_vartype)
	{
	case HmmSpherical:
		if (m_ncentres > 1)
		{
			// tmp is the d2 matrix.
			CVisDMatrix tmp (m_ncentres, m_ncentres);
			Dist2 (Centres, Centres, m_ncentres, tmp);

			// scan tmp for min distances.
			for (int i = 1; i <= m_ncentres; i++)
			{
				double min = tmp(i,1);
				for (int j = 2; j <= m_ncentres; j++)
				{
					if (tmp(i,j) < min)
						min = tmp(i,j);
				}
				// min found.
				Diag(Covariance[i], (min < 1e-8) ? GMM_WIDTH : min);
			}
		}
		else
		{
			// use the covariance of all points averaged over all dimensions.
		    // mix.covars = mean(diag(cov(x)));
			printf ("KMeans - not implemented yet. Use at least 2 centers\n");
		}
		break;

	case HmmDiag:
		{
			::Covariance (data, nex, T, Centres, m_ncentres, Covariance, GMM_WIDTH);
		}
		break;

	case HmmFull:
		{
			::Covariance (data, nex, T, Centres, m_ncentres, Covariance, GMM_WIDTH);
		}
		break;

	case HmmConstant:
		{
			for (int i = 1; i <= m_ncentres; i++)
				Diag (Covariance[i], GMM_WIDTH);
		}
		break;

	default:
		printf ("KMeans - not a recognized variance model\n");
		break;
	}

	// at this point we KMeans initialized the gaussian mixtures (for the output).
}

//
//
void YARPGaussianMixture::NonRndInitialize (const CVisDVector **data, int nex, int T, const CVisDVector **centers, int Q, int M)
{
	// Arbitrary width used if variance collapses to zero: make it 'large' so
	// that centre is responsible for a reasonable number of points.
	double GMM_WIDTH = 1.0;

	// get ncentres random vectors out of data.
	const int range = nex * T;

	// initialize the centers from data.
	int index = 1;
	for (int i = 1; i <= Q; i++)
		for (int j = 1; j <= M; j++)
		{
			Centres[index] = centers[i][j];
			index++;
		}

	//
	CVisDVector cluster_sizes (m_ncentres);
	
	// Use kmeans algorithm to set centres, result in Centres.
	KMeans (Centres, cluster_sizes, data, nex, T, 100, 1e-4, 1e-4);

	// Set priors depending on number of points in each cluster
	double cluster_sum = 0;
	for (i = 1; i <= m_ncentres; i++)
 		cluster_sum += cluster_sizes(i);

	for (i = 1; i <= m_ncentres; i++)
	{
		Priors (i) = 
			((cluster_sizes(i) > 0.0) ? cluster_sizes(i) : 1.0) / cluster_sum;
#ifdef _DEBUG
		printf ("cluster_sizes(%d)=%lf ", i, cluster_sizes(i));
#endif
	}
#ifdef _DEBUG
	printf ("\n");
#endif

	// Initialize covariance matrix.
	switch (m_vartype)
	{
	case HmmSpherical:
		if (m_ncentres > 1)
		{
			// tmp is the d2 matrix.
			CVisDMatrix tmp (m_ncentres, m_ncentres);
			Dist2 (Centres, Centres, m_ncentres, tmp);

			// scan tmp for min distances.
			for (int i = 1; i <= m_ncentres; i++)
			{
				double min = tmp(i,1);
				for (int j = 2; j <= m_ncentres; j++)
				{
					if (tmp(i,j) < min)
						min = tmp(i,j);
				}
				// min found.
				Diag(Covariance[i], (min < 1e-8) ? GMM_WIDTH : min);
			}
		}
		else
		{
			// use the covariance of all points averaged over all dimensions.
		    // mix.covars = mean(diag(cov(x)));
			printf ("KMeans - not implemented yet. Use at least 2 centers\n");
		}
		break;

	case HmmDiag:
		{
			::Covariance (data, nex, T, Centres, m_ncentres, Covariance, GMM_WIDTH);
		}
		break;

	case HmmFull:
		{
			::Covariance (data, nex, T, Centres, m_ncentres, Covariance, GMM_WIDTH);
		}
		break;

	case HmmConstant:
		{
			for (int i = 1; i <= m_ncentres; i++)
				Diag (Covariance[i], GMM_WIDTH);
		}
		break;

	default:
		printf ("KMeans - not a recognized variance model\n");
		break;
	}

	// at this point we KMeans initialized the gaussian mixtures (for the output).
}

void YARPGaussianMixture::Create (int dim, int ncentres, YARPVarianceModel cov_type)
{
	m_dim = dim;
	m_ncentres = ncentres;
	m_vartype = cov_type;

	// Initialise priors to be equal and summing to one
	Priors.Resize (ncentres);
	Priors = 1.0 / ncentres;

	// Initialise centres
	Centres = new CVisDVector[ncentres];
	assert (Centres != NULL);
	Centres --;

	for (int i = 1; i <= ncentres; i++)
	{
		Centres[i].Resize (dim);
		RandN (Centres[i]);
	}

	// Initialise all the variances to unity
	Covariance = new CVisDMatrix[ncentres];
	assert (Covariance != NULL);
	Covariance --;

	for (i = 1; i <= ncentres; i++)
	{
		Covariance[i].Resize (dim, dim);
		Covariance[i] = 1;
	}

	m_kmeans_err.Resize (100);
	m_kmeans_err = 0;
	m_error = -1.0;

	m_initialized = true;
}

