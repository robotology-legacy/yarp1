//
// YARPHmm.h
//

#ifndef __YARPHmmh__
#define __YARPHmmh__

#include <assert.h>
#include <stdio.h>

#ifdef __QNX__
#include "YARPSafeNew.h"
#include "YARPBool.h"
#endif

#include <VisMatrix.h>

#include "YARPHmmMath.h"
#include "YARPRandomNumber.h"
#include "YARPGaussianMixture.h"

class YARPMixtureHmm 
{
private:
	// handling arrays.
	void _init (int Q, int M, int O);
	void _uninit (int Q, int M, int O);
	bool m_initialized;

	// internally used stuff.
	CVisDMatrix B;
	CVisDMatrix *B2;
	CVisDMatrix gamma;
	CVisDMatrix *gamma2;
	CVisDMatrix *xit;

protected:
	CVisDVector m_init_state_prob;	// P(q(t) = i)
	CVisDMatrix m_trans_mat;		// P(q(t+1)=j | q(t)=i)

	CVisDMatrix m_mix_mat;			// P(M(t)=k | q(t)=j)
									// indexed j,k.
	CVisDVector **m_mu;				// array of mean of mixture components.
									// index [j][k] q(t)=j, M(t)=k
	CVisDMatrix **m_sigma;			// array of variance of mixture components.
									// index [j][k] as before.

	int m_Q;						// state vector size
	int m_M;						// # of gaussian comp
	int m_O;						// output size
	YARPVarianceModel m_mix_comp;	// type of mixture
	bool m_left_right;				// left-right model

	// unefficient...
	YARPGaussianMixture m_mixture_model;

	// variables for incremental processing.
	int m_io_maxt;
	CVisDMatrix m_io_obslik;
	CVisDMatrix m_io_alpha;
	int m_io_step;
	double m_io_loglik;
	CVisDVector m_io_scale;
	
	// helper functions for incremental processing.
	void ComputeOneSampleObservationLikelihood (const CVisDVector& data, int t, CVisDMatrix& B);
	double OneSampleForwards (const CVisDMatrix& obslik, int t, CVisDMatrix& alpha);

	// get state sequences out of the model.
	void SampleState (int **hidden, int nex, int T);

	// compute the obs. likelihood.
	void ComputeObservationLikelihood (const CVisDVector *data, int T, CVisDMatrix& out);
	void ComputeObservationLikelihood (const CVisDVector *data, int T, CVisDMatrix& B, CVisDMatrix *B2);

	// implements the forward algorithm: used to compute the prob of a given sequence.
	double Forwards (const CVisDMatrix& obslik, int T, CVisDMatrix& alpha, CVisDMatrix *xi);
	double ForwardsDebug (const CVisDMatrix& obslik, int T, CVisDMatrix& alpha, CVisDMatrix *xi);
	double ForwardBackward (const CVisDMatrix &obslik, const CVisDMatrix *obslik2, int T, CVisDMatrix *xi, CVisDMatrix& gamma, CVisDMatrix *gamma2);

	// test for EM convergence.
	bool LearningConverged (double loglik, double previous_loglik, double threshold);

	// compute the E step of the EM algorithm.
	void ComputeEss (const CVisDVector **data, const CVisDVector& length, int nex,
					 double &loglik, CVisDMatrix& exp_num_trans, CVisDVector& exp_num_visits1, 
					 CVisDMatrix& postmix, CVisDVector **m,	CVisDMatrix& ip, CVisDMatrix **op);

	// E step for the segmental Kmeans training.
	void ComputeEssViterbi (
					 const CVisDVector **data, const CVisDVector& length, int nex,
					 double &loglik, CVisDMatrix& exp_num_trans, CVisDVector& exp_num_visits1, 
					 CVisDMatrix& postmix, CVisDVector **m,	CVisDMatrix& ip, CVisDMatrix **op);

	// helper function for ComputeEssViterbi.
	void ComputeAuxFunctions (const CVisDMatrix *B2, const CVisDVector& best_state, int T, CVisDMatrix *xi, CVisDMatrix& gamma, CVisDMatrix *gamma2);

	// determines the optimum path given the observ. likelihood.
	double ViterbiPath (const CVisDMatrix& B, CVisDVector& path);

	// compute distances of each sample from all the possible gaussians.
	void ComputeDistances (const CVisDVector *data, int T, CVisDMatrix *B2);

public:
	// creation: Q: # of states, M: # of mix comp, 
	//				cov_type, left_right (bool).
	YARPMixtureHmm ();
	YARPMixtureHmm (int Q, int M, int O, YARPVarianceModel model) 
	{
#ifdef _DEBUG
		debug = NULL;
#endif
		Init (Q, M, O, model); 
	}
	virtual ~YARPMixtureHmm();

	void Init (int Q, int M, int O, YARPVarianceModel model);
	void Uninit ();

	// initialize from data using kmeans.
	void KMeansInit (const CVisDVector **data, int nex, int T, int ncycles = 10);

	// kmeans with hopefully good centers.
	void NonRndKMeansInit (const CVisDVector **data, int nex, int T, const CVisDVector **centers);

	// random initialization.
	void RandomInit (const CVisDVector **data, int nex, int T, double sigma);

	// just copy the arrays there.
	void DataInit (const CVisDVector& prior, 
				   const CVisDMatrix& a, 
				   const CVisDMatrix& mix,
				   const CVisDVector **mu,
				   const CVisDMatrix **sigma);

	// serialize/unserialize methods.
	void Serialize (const char *filename) const;
	void Unserialize (const char *filename);

	// sample hmm to get one or more sequences.
	void Sample (CVisDVector **data, int **hidden, int nex, int T);

	// computes the log likelihood of a given bunch of data vectors.
	double ComputeLogLikelihood (const CVisDVector** data, int nex, int T);

	// Baum-welch EM algorithm.
	void Learn (const CVisDVector **data, const CVisDVector& length, int nex, CVisDVector& LL, int maxiter = 10);

	// computes the most probable state sequence (dynamic programming).
	double Viterbi (const CVisDVector *data, int T, CVisDVector& path);

	// segmental kmeans.
	void LearnViterbi (const CVisDVector **data, const CVisDVector& length, int nex, CVisDVector& LL, int max_iter = 10);

	// incremental version (online) of likelihood estimation.
	void PrepareIncrementalObservation (int maxT);
	void StartIncrementalObservation ();
	double AddNewIncrementalObservation (const CVisDVector& data);
	void ClearIncrementalObservation ();

#ifdef _DEBUG
// weird stuff for debugging.
	FILE * debug;
#endif
};

#endif