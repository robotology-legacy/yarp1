//
// YARPHmm.cpp
//

// LATER: there are too many allocation stuff on the fly. All that should
//	be moved to the initialization phase.

// the HMM model implementation.

#include "YARPHmm.h"

//
// sequences could be variable length. Use an additional vector to represent length.
//
// This is Viterbi training. The difference with respect to Baum-Welch is mostly
// in the ComputeEss.
void YARPMixtureHmm::LearnViterbi (const CVisDVector **data, const CVisDVector& length, int nex, CVisDVector& LL, int max_iter)
{
	const double thresh = 1e-4;
	const bool verbose = true;
	const bool mstatic = false;

	// needed for initialization, but to be changed with the first val.
	double previous_loglik = -1e100;
	double loglik = 0;
	bool converged = false;
	int iter = 1;

	LL.Resize (max_iter);
	LL = 0;

	int maxT = length(1);
	for (int j = 2; j <= length.Length(); j++)
		if (length(j) > maxT) maxT = length(j);

	B.Resize (m_Q, maxT);
	B2 = new CVisDMatrix[maxT];
	assert (B2 != NULL);
	B2 --;
	for (j = 1; j <= maxT; j++)
		B2[j].Resize (m_Q, m_M);
	gamma.Resize (m_Q, maxT);
	gamma2 = new CVisDMatrix[maxT];
	assert (gamma2 != NULL);
	gamma2 --;
	for (j = 1; j <= maxT; j++)
		gamma2[j].Resize (m_Q, m_M);
	xit = new CVisDMatrix[maxT];
	assert (xit != NULL);
	xit --;
	for (j = 1; j <= maxT; j++)
		xit[j].Resize (m_Q, m_Q);

	CVisDMatrix exp_num_trans (m_Q, m_Q);
	CVisDVector exp_num_visits1 (m_Q);
	CVisDMatrix postmix (m_Q, m_M);
	CVisDVector **m = AllocVector (m_Q, m_M, m_O);
	CVisDMatrix ip (m_Q, m_M);
	CVisDMatrix **op = AllocMatrix (m_Q, m_M, m_O);

	// main loop.
	while ((iter <= max_iter) && !converged)
	{
		// E step

		// ess_mhmm(init_state_prob, transmat, mixmat, mu, Sigma, data);
		ComputeEssViterbi (
			data,
			length,
			nex,
			loglik, 
			exp_num_trans, 
			exp_num_visits1, 
			postmix, 
			m,
			ip,
			op);

		if (verbose)
			printf("iteration %d, loglik = %f\n", iter, loglik);

		// M step
		if (!mstatic)
		{
			m_init_state_prob = exp_num_visits1;
			Normalise (m_init_state_prob);
			m_trans_mat = exp_num_trans;
			MkStochastic (m_trans_mat);
		}

		if (m_M == 1)
			m_mix_mat = 1.0;
		else
		{
			m_mix_mat = postmix;
			MkStochastic (m_mix_mat);			//m_mix_mat = mk_stochastic(postmix);
		}

		for (int j = 1; j <= m_Q; j++)
			for (int k = 1; k <= m_M; k++)
			{
				// it might crash here if dividing by zero.
				// ex: a null state collapsed.
				m_mu[j][k] = m[j][k] / postmix(j,k);

				switch (m_mix_comp)
				{
				case HmmSpherical:
					{
						double coeff = m_mu[j][k].norm2square();
						double s2 = (1.0 / m_O) * ((ip(j,k) / postmix(j,k)) - coeff);
						Diag(m_sigma[j][k], s2);
					}
					break;

				case HmmFull:
				case HmmConstant:
					{
						for (int o = 1;  o <= m_O; o++)
							for (int o2 = 1; o2 <= m_O; o2++)
								m_sigma[j][k](o,o2) = op[j][k](o,o2) / postmix(j,k) - m_mu[j][k](o) * m_mu[j][k](o2);
					}
					break;

				case HmmDiag:
					{
						for (int o = 1;  o <= m_O; o++)
							for (int o2 = 1; o2 <= m_O; o2++)
							{
								if (o == o2)
									m_sigma[j][k](o,o2) = op[j][k](o,o2) / postmix(j,k) - m_mu[j][k](o) * m_mu[j][k](o2);
								else
									m_sigma[j][k](o,o2) = 0;
							}
					}
					break;
				}
			}

			converged = LearningConverged(loglik, previous_loglik, thresh);
			previous_loglik = loglik;
			LL(iter) = loglik;

			iter ++;

	}	// closes while()

	// delete internal stuff.
	B2 ++;
	if (B2 != NULL)
		delete[] B2;
	gamma2 ++;
	if (gamma2 != NULL)
		delete[] gamma2;

	xit ++;
	if (xit != NULL)
		delete[] xit;

	FreeVector (m, m_Q);
	FreeMatrix (op, m_Q);
}

// compute the statistics. Since it's viterbi training we determine the
// optimal sequence first.
void YARPMixtureHmm::ComputeEssViterbi (
			const CVisDVector **data, 
			const CVisDVector& length,
			int nex,
			double &loglik, 
			CVisDMatrix& exp_num_trans, 
			CVisDVector& exp_num_visits1, 
			CVisDMatrix& postmix, 
			CVisDVector **m,
			CVisDMatrix& ip,
			CVisDMatrix **op)
{
	bool verbose = false;

	exp_num_trans = 0;		// size is Q x Q
	exp_num_visits1 = 0;	// size is Q x 1
	postmix = 0;			// size is Q x M
	ip = 0;					// size is Q x M

	for (int i = 1; i <= m_Q; i++)
		for (int j = 1; j <= m_M; j++)
		{
			m[i][j] = 0;		// size O
			op[i][j] = 0;		// size O x O
		}

	loglik = 0;

	if (verbose)
		printf("forwards-backwards example # ");

	for (int ex = 1; ex <= nex; ex++)
	{
		if (verbose) 
			printf("%d ", ex);

		int T = int(length (ex));
		CVisDVector best_state (T);	// very bad programming.

		// obs = data[ex];
		// matrix obslik sized Q x T, obslik2 is a vector of length T of Q x M matrices.
		//
		// gamma is Q x T
		// gamma2 vect length T of Q x M matrices
		// xi is a vector of length T of matrices Q x Q but T-1 would be enough.
		//
		//ComputeObservationLikelihood (data[ex], T, B, B2);
		ComputeObservationLikelihood (data[ex], T, B);
		double current_loglik = ViterbiPath (B, best_state);

		// xit = nit
		// gamma = eta
		// gamma2 = eta2
		ComputeDistances (data[ex], T, B2);
		ComputeAuxFunctions (B2, best_state, T, xit, gamma, gamma2);

		// the rest of the computation is like Baum-Welch
		loglik += current_loglik; 
		if (verbose)
			printf("ll at ex %d = %f\n", ex, loglik);

		// T-1 should be enough! In any case the vector is filled out til T-1.
		for (int t = 1; t <= T-1; t++)
			exp_num_trans += xit[t];

		for (int q = 1; q <= m_Q; q++)
			exp_num_visits1(q) += (gamma(q,1));

		for (t = 1; t <= T; t++)
			postmix += gamma2[t];

		CVisDVector wobs (m_O);	// must be an array of length T.
		CVisDMatrix tmpx (m_O, m_O);

		for (int j = 1; j <= m_Q; j++)
			for (int k = 1; k <= m_M; k++)
			{
				for (t = 1; t <= T; t++)
				{
					// this would be a vector of size O.
					wobs = data[ex][t] * gamma2[t] (j,k);

					for (int o = 1; o <= m_O; o++)
						m[j][k](o) += (wobs(o));

					for (o = 1; o <= m_O; o++)
						for (int o2 = 1; o2 <= m_O; o2++)
						{
							op[j][k](o,o2) += (wobs(o) * data[ex][t](o2));
						}

					for (o = 1; o <= m_O; o++)
					{
						ip(j,k) += (wobs(o) * data[ex][t](o));
					}
				}
			}
	} // closes for (nex).

	if (verbose)
		printf("\n");
}


//
// compute distances of data from gaussians.
void YARPMixtureHmm::ComputeDistances (const CVisDVector *data, int T, CVisDMatrix *B2)
{
	CVisDVector tmp (data[1].Length());

	switch (m_mix_comp)
	{
	case HmmFull:
		{
			printf ("ComputeDistances - not implemented yet for full cov matrix\n");
			return;
		}
		break;

	case HmmConstant:
	case HmmSpherical:
	case HmmDiag:
		{
			for (int q = 1; q <= m_Q; q++)
			{
				for (int t = 1; t <= T; t++)
				{
					for (int m = 1; m <= m_M; m++)
					{
						tmp = data[t] - m_mu[q][m];
						B2[t] (q, m) = tmp.norm2square();
					}
				}
			}
		}
		break;
	}
}

// compute the aux functions for the viterbi case.
void YARPMixtureHmm::ComputeAuxFunctions (const CVisDMatrix *B2, const CVisDVector& best_state, int T, CVisDMatrix *xi, CVisDMatrix& gamma, CVisDMatrix *gamma2)
{
	gamma = 0;

	for (int t = 1; t <= T; t++)
	{
		int q = int(best_state(t));

		// compute xi.
		if (t < T)		// esclude T.
		{
			int q1 = int(best_state(t+1));
			xi[t] = 0; 
			xi[t](q, q1) = 1.0;
		}

		// compute gamma.
		gamma (q, t) = 1.0;

		// compute gamma2.
		// mindist is the closest gaussian to the observation O.
		// this is exactly the argmin over m of B2 (distances).
		double min = B2[t](q,1);
		int mindist = 1;
		if (m_M > 1)
		{
			for (int m = 2; m <= m_M; m++)
				if (B2[t](q, m) < min)
				{
					min = B2[t](q, m);
					mindist = m;
				}
		}

		gamma2[t] = 0;
		gamma2[t] (q, mindist) = 1.0;
	}
}

//
// computes the most likely state sequence for a given observed sequence.
double YARPMixtureHmm::Viterbi (const CVisDVector *data, int T, CVisDVector& path)
{
	CVisDMatrix B (m_Q, T);

	ComputeObservationLikelihood (data, T, B);

	double loglik = ViterbiPath (B, path);

	// just in case we need to check anything before return.

	return loglik;
}

//
// this is the actual backward phase.
double YARPMixtureHmm::ViterbiPath (const CVisDMatrix& B, CVisDVector& path)
{
	//function [path, loglik] = viterbi_path(prior, transmat, obsmat)
	// VITERBI Find the most-probable (Viterbi) path through the HMM state trellis.
	// path = viterbi(prior, transmat, obsmat)
	//
	// Inputs:
	// prior(i) = Pr(Q(1) = i)
	// transmat(i,j) = Pr(Q(t+1)=j | Q(t)=i)
	// obsmat(i,t) = Pr(y(t) | Q(t)=i)
	//
	// Outputs:
	// path(t) = q(t), where q1 ... qT is the argmax of the above expression.

	// delta(j,t) = prob. of the best sequence of length t-1 and then going to state j, and O(1:t)
	// psi(j,t) = the best predecessor state, given that we ended up in state j at t

	bool scaled = true;

	const int T = B.NCols();

	CVisDMatrix delta (m_Q,T);
	delta = 0;

	CVisDMatrix psi (m_Q,T);
	psi = 0;

	CVisDVector scale (T);
	scale = 1;

	// zero is an invalid state.
	path = 0;

	int t = 1;
	for (int q = 1; q <= m_Q; q++)
		delta(q,t) = m_init_state_prob(q) * B(q,t);

	if (scaled)
	{
		double n = NormaliseColumn (delta, t);
		scale(t) = 1 / n;
	}

	CVisDVector tmp (m_Q);

	for (t = 2; t <= T; t++)
	{
		for (int j = 1; j <= m_Q; j++)
		{
			for (q = 1; q <= m_Q; q++)
				tmp(q) = delta(q,t-1) * m_trans_mat(q,j);

			psi(j,t) = 1;
			delta(j,t) = tmp(1);
			for (q = 2; q <= m_Q; q++)
			{
				if (tmp(q) > delta(j,t))
				{
					delta(j,t) = tmp(q);
					psi(j,t) = q;
				}
			}

			delta(j,t) = delta(j,t) * B(j,t);
		}

		if (scaled)
		{
			double n = NormaliseColumn (delta, t);
			scale(t) = 1 / n;
		}
	}

	path(T) = 1;
	double p = delta(1,T);
	for (q = 2; q <= m_Q; q++)
	{
		if (delta(q,T) > p)
		{
			p = delta(q,T);
			path(T) = q;
		}
	}

	for (t = T-1; t >= 1; t--)
	{
		path(t) = psi(path(t+1),t+1);
	}

	// loglik = log p.
	// If scaled==0, p = prob_path(best_path)
	// If scaled==1, p = Pr(replace sum with max and proceed as in the scaled forwards algo)
	// loglik computed by the two methods will be different, but the best path will be the same.

	double loglik = 0;
	if (scaled)
	{
		for (t = 1; t <= T; t++)
			loglik -= (log(scale(t)));
	}
	else
	{
		loglik = log(p);
	}

	return loglik;
}

// function [LL, init_state_prob, transmat, mu, Sigma, mixmat, mu_history] = ...
//    learn_mhmm(data, init_state_prob, transmat, mu, Sigma, mixmat, ...
//    max_iter, thresh, verbose, cov_type, static)
// LEARN_MHMM Compute the ML parameters of an HMM with mixtures of Gaussians output using EM.
// 
// [LL, PRIOR, TRANSMAT, MU, SIGMA, MIXMAT, MU_HISTORY] = LEARN_MHMM(DATA, PRIOR0, TRANSMAT0,
// MU0, SIGMA0, MIXMAT0) computes the ML estimates of the following parameters,
// where, for each time t, Q(t) is the hidden state, M(t) is the mixture component, and Y(t) is
// the observation.
//   prior(i) = Pr(Q(1) = i), 
//   transmat(i,j) = Pr(Q(t+1)=j | Q(t)=i)
//   mixmat(j,k) = Pr(M(t)=k | Q(t)=j) 
//   mu(:,j,k) = E[Y(t) | Q(t)=j, M(t)=k ]
//   Sigma(:,:,j,k) = Cov[Y(t) | Q(t)=j, M(t)=k]
// PRIOR0 is the initial estimate of PRIOR, etc.
// To learn an HMM with a single Gaussian output, just set mixmat = ones(Q,1).
//
// DATA(:,t,l) is the observation vector at time t for sequence l. If the sequences are of
// different lengths, you can pass in a cell array, so DATA{l} is an O*T matrix.
//
// LL is the "learning curve": a vector of the log lik. values at each iteration.
// LL might go positive, since prob. densities can exceed 1, although this probably
// indicates that something has gone wrong e.g., a variance has collapsed to 0.
// MU_HISTORY(:,:,:,r) is MU at iteration r.
//
// There are several optional arguments, which should be passed in the following order
//  LEARN_MHMM(DATA, INIT_STATE_PROB, TRANSMAT, MU, SIGMA, MIXMAT, ...
//    MAX_ITER, THRESH, VERBOSE, COV_TYPE, STATIC)
// These have the following meanings
//
// max_iter = max. num EM steps to take (default 10)
// thresh = threshold for stopping EM (default 1e-4)
// verbose = 0 to suppress the display of the log lik at each iteration (Default 1).
// cov_type = 'full', 'diag' or 'spherical' (default 'full')
// static = 1 if we don't want to re-estimate prior and transmat, i.e., no dynamics (default = 0)
//
// See "A tutorial on Hidden Markov Models and selected applications in speech recognition",
// L. Rabiner, 1989, Proc. IEEE 77(2):257--286.


//
// sequences could be variable length. Use an additional vector to represent length.
//
void YARPMixtureHmm::Learn (const CVisDVector **data, const CVisDVector& length, int nex, CVisDVector& LL, int maxiter)
{
//learn_mhmm(data, init_state_prob, transmat, mu, Sigma, mixmat, max_iter, thresh, verbose, cov_type, static)

//if nargin<7, max_iter = 10; end
//if nargin<8, thresh = 1e-4; end
//if nargin<9, verbose = 1; end
//if nargin<10, cov_type = 'full'; end
//if nargin<11, static = 0; end

	const double thresh = 1e-4;
	const bool verbose = true;
	const bool mstatic = false;

	// needed for initialization, but to be changed with the first val.
	double previous_loglik = -1e100;
	double loglik = 0;
	bool converged = false;
	int iter = 1;

	LL.Resize (maxiter);
	LL = 0;

	//mu_history = zeros(O,Q,M,max_iter); not much sense to keep all this data.

	// matrix obslik sized Q x T, obslik2 is a vector of length T of Q x M matrices.
	//
	// gamma is Q x T
	// gamma2 vect length T of Q x M matrices
	// xi is a vector of length T of matrices Q x Q but T-1 would be enough.
	//

	// find max T and alloc enough for that.
	// put all this name globals so we don't have to pass them.
	int maxT = length(1);
	for (int j = 2; j <= length.Length(); j++)
		if (length(j) > maxT) maxT = length(j);

	B.Resize (m_Q, maxT);
	B2 = new CVisDMatrix[maxT];
	assert (B2 != NULL);
	B2 --;
	for (j = 1; j <= maxT; j++)
		B2[j].Resize (m_Q, m_M);
	gamma.Resize (m_Q, maxT);
	gamma2 = new CVisDMatrix[maxT];
	assert (gamma2 != NULL);
	gamma2 --;
	for (j = 1; j <= maxT; j++)
		gamma2[j].Resize (m_Q, m_M);
	xit = new CVisDMatrix[maxT];
	assert (xit != NULL);
	xit --;
	for (j = 1; j <= maxT; j++)
		xit[j].Resize (m_Q, m_Q);

	CVisDMatrix exp_num_trans (m_Q, m_Q);
	CVisDVector exp_num_visits1 (m_Q);
	CVisDMatrix postmix (m_Q, m_M);
	CVisDVector **m = AllocVector (m_Q, m_M, m_O);
	CVisDMatrix ip (m_Q, m_M);
	CVisDMatrix **op = AllocMatrix (m_Q, m_M, m_O);

	// main loop.
	while ((iter <= maxiter) && !converged)
	{
		// E step
		//  [loglik, exp_num_trans, exp_num_visits1, postmix, m, ip, op] 

		// ess_mhmm(init_state_prob, transmat, mixmat, mu, Sigma, data);
		ComputeEss (
			data,
			length,
			nex,
			loglik, 
			exp_num_trans, 
			exp_num_visits1, 
			postmix, 
			m,
			ip,
			op);

		if (verbose)
			printf("iteration %d, loglik = %f\n", iter, loglik);

		// M step
		if (!mstatic)
		{
			m_init_state_prob = exp_num_visits1;
			Normalise (m_init_state_prob);
			m_trans_mat = exp_num_trans;
			MkStochastic (m_trans_mat);
		}

		if (m_M == 1)
			m_mix_mat = 1.0;
		else
		{
			m_mix_mat = postmix;
			MkStochastic (m_mix_mat);			//m_mix_mat = mk_stochastic(postmix);
		}

		for (int j = 1; j <= m_Q; j++)
			for (int k = 1; k <= m_M; k++)
			{
				m_mu[j][k] = m[j][k] / postmix(j,k);

				switch (m_mix_comp)
				{
				case HmmSpherical:
					{
						double coeff = m_mu[j][k].norm2square();
						double s2 = (1.0 / m_O) * ((ip(j,k) / postmix(j,k)) - coeff);
						Diag(m_sigma[j][k], s2);
					}
					break;

				case HmmFull:
				case HmmConstant:
					{
						for (int o = 1;  o <= m_O; o++)
							for (int o2 = 1; o2 <= m_O; o2++)
								m_sigma[j][k](o,o2) = op[j][k](o,o2) / postmix(j,k) - m_mu[j][k](o) * m_mu[j][k](o2);
					}
					break;

				case HmmDiag:
					{
						for (int o = 1;  o <= m_O; o++)
							for (int o2 = 1; o2 <= m_O; o2++)
							{
								if (o == o2)
									m_sigma[j][k](o,o2) = op[j][k](o,o2) / postmix(j,k) - m_mu[j][k](o) * m_mu[j][k](o2);
								else
									m_sigma[j][k](o,o2) = 0;
							}
					}
					break;
				}
			}

			//mu_history(:,:,:,iter) = mu; // not implemented yet.
			// ll_history(iter) = loglik;

			converged = LearningConverged(loglik, previous_loglik, thresh);
			previous_loglik = loglik;
			LL(iter) = loglik;

			iter ++;

	}	// closes while()

	// delete internal stuff.
	B2 ++;
	if (B2 != NULL)
		delete[] B2;
	gamma2 ++;
	if (gamma2 != NULL)
		delete[] gamma2;

	xit ++;
	if (xit != NULL)
		delete[] xit;

	FreeVector (m, m_Q);
	FreeMatrix (op, m_Q);
}

//function [converged, decrease] = em_converged(loglik, previous_loglik, threshold)
// EM_CONVERGED Has EM converged?
// [converged, decrease] = em_converged(loglik, previous_loglik, threshold)
//
// We have converged if the slope of the log-likelihood function falls below 'threshold', 
// i.e., |f(t) - f(t-1)| / avg < threshold,
// where avg = (|f(t)| + |f(t-1)|)/2 and f(t) is log lik at iteration t.
// 'threshold' defaults to 1e-4.
//
// This stopping criterion is from Numerical Recipes in C p423
//
// If we are doing MAP estimation (using priors), the likelihood can decrase,
// even though the mode of the posterior is increasing.

bool YARPMixtureHmm::LearningConverged (double loglik, double previous_loglik, double threshold)
{
	bool converged = false;
	bool decrease = false;

	if (loglik - previous_loglik < -1e-3) // allow for a little imprecision
	{
		printf("******likelihood decreased from %6.4f to %6.4f!\n", previous_loglik, loglik);
//		printf("****** exit forced (added by Pasa)\n");
		decrease = true;

//		return true;
	}

	const double eps = 0; // 1e-100;
	const double delta_loglik = fabs(loglik - previous_loglik);
	const double avg_loglik = (fabs(loglik) + fabs(previous_loglik) + eps) / 2.0;
	if ((delta_loglik / avg_loglik) < threshold)
		converged = true;

	return converged;
}

//function [loglik, exp_num_trans, exp_num_visits1, postmix, m, ip, op] = ...
//    ess_mhmm(prior, transmat, mixmat, mu, Sigma, data)
// ESS_MHMM Compute the Expected Sufficient Statistics for a MOG Hidden Markov Model.
//
// Outputs:
// exp_num_trans(i,j)   = sum_l sum_{t=2}^T Pr(Q(t-1) = i, Q(t) = j| Obs(l))
// exp_num_visits1(i)   = sum_l Pr(Q(1)=i | Obs(l))
// postmix(i,k) = sum_l sum_t w(i,k,t) where w(i,k,t) = Pr(Q(t)=i, M(t)=k | Obs(l))  (posterior mixing weights)
// m(:,i,k)   = sum_l sum_t w(i,k,l) * Obs(:,t,l)
// ip(i,k) = sum_l sum_t w(i,k,l) * Obs(:,t,l)' * Obs(:,t,l)
// op(:,:,i,k) = sum_l sum_t w(i,k,l) * Obs(:,t,l) * Obs(:,t,l)'
//
// where Obs(l) = Obs(:,:,l) = O_1 .. O_T for sequence l
void YARPMixtureHmm::ComputeEss (
			const CVisDVector **data, 
			const CVisDVector& length,
			int nex,
			double &loglik, 
			CVisDMatrix& exp_num_trans, 
			CVisDVector& exp_num_visits1, 
			CVisDMatrix& postmix, 
			CVisDVector **m,
			CVisDMatrix& ip,
			CVisDMatrix **op)
{
	bool verbose = false;

	exp_num_trans = 0;		// size is Q x Q
	exp_num_visits1 = 0;	// size is Q x 1
	postmix = 0;			// size is Q x M
	ip = 0;					// size is Q x M

	for (int i = 1; i <= m_Q; i++)
		for (int j = 1; j <= m_M; j++)
		{
			m[i][j] = 0;		// size O
			op[i][j] = 0;		// size O x O
		}

	loglik = 0;

	if (verbose)
		printf("forwards-backwards example # ");

#ifdef _DEBUG
	fprintf (debug, "ComputeEss:\n");
#endif

	for (int ex = 1; ex <= nex; ex++)
	{
		if (verbose) 
			printf("%d ", ex);

		int T = int(length (ex));

#ifdef _DEBUG
		fprintf (debug, "ex : %d\n", ex);
#endif
		// obs = data[ex];
		// matrix obslik sized Q x T, obslik2 is a vector of length T of Q x M matrices.
		//
		// gamma is Q x T
		// gamma2 vect length T of Q x M matrices
		// xi is a vector of length T of matrices Q x Q but T-1 would be enough.
		//
#ifdef _DEBUG
		fprintf (debug, "data[%d]:\n", ex);
		for (int xx = 1; xx <= T; xx++)
		{
			SerializeVector (debug, data[ex][xx]);
		}
#endif
		ComputeObservationLikelihood (data[ex], T, B, B2);
#ifdef _DEBUG
		fprintf (debug, "B:\n");
		SerializeMatrix (debug, B);
		for (xx = 1; xx <= T; xx++)
		{
			fprintf (debug, "B2:\n");
			SerializeMatrix (debug, B2[xx]);
		}
#endif

		double current_loglik = ForwardBackward (B, B2, T, xit, gamma, gamma2);

		//[gamma, xit, current_loglik, gamma2] = forwards_backwards_mix(prior, transmat, B, B2, mixmat);

		loglik += current_loglik; 
		if (verbose)
			printf("ll at ex %d = %f\n", ex, loglik);

		// T-1 should be enough! In any case the vector is filled out til T-1.
		for (int t = 1; t <= T-1; t++)
			exp_num_trans += xit[t];

		for (int q = 1; q <= m_Q; q++)
			exp_num_visits1(q) += (gamma(q,1));

		for (t = 1; t <= T; t++)
			postmix += gamma2[t];

		CVisDVector wobs (m_O);	// must be an array of length T.
		CVisDMatrix tmpx (m_O, m_O);

		for (int j = 1; j <= m_Q; j++)
			for (int k = 1; k <= m_M; k++)
			{
				for (t = 1; t <= T; t++)
				{
					// this would be a vector of size O.
					wobs = data[ex][t] * gamma2[t] (j,k);

					for (int o = 1; o <= m_O; o++)
						m[j][k](o) += (wobs(o));

					for (o = 1; o <= m_O; o++)
						for (int o2 = 1; o2 <= m_O; o2++)
						{
							op[j][k](o,o2) += (wobs(o) * data[ex][t](o2));
						}

					for (o = 1; o <= m_O; o++)
					{
						ip(j,k) += (wobs(o) * data[ex][t](o));
					}
				}

				// w is a slice along t of a particular Q and M.
				// w = reshape(gamma2(j,k,:), [1 T]); % w(t) = Pr(Q(t)=j, M(t)=k | obs)
				// wobs = obs .* repmat(w, [O 1]); % wobs(:,t) = w(t) * obs(:,t)
				// m(:,j,k) = m(:,j,k) + sum(wobs, 2); % m(:) = sum_t w(t) obs(:,t)
				// op(:,:,j,k) = op(:,:,j,k) + wobs * obs'; % op(:,:) = sum_t w(t) * obs(:,t) * obs(:,t)'
				// ip(j,k) = ip(j,k) + sum(sum(wobs .* obs, 2)); % ip = sum_t w(t) * obs(:,t)' * obs(:,t)
			}
	} // closes for (nex).

	if (verbose)
		printf("\n");
}


// Forward backward stuff.
//	matrix obslik sized [q][t] obslik2 is a vector of length M of q,t matrices.
//
// gamma is Q,T
// gamma2 Q,M,T a vector of matrices of size Q x M
// xi is a vector of length T of matrices Q,Q // T-1 would be enough.
// returns the log likelihood.
//
double YARPMixtureHmm::ForwardBackward (const CVisDMatrix &obslik, const CVisDMatrix *obslik2, int T, CVisDMatrix *xi, CVisDMatrix& gamma, CVisDMatrix *gamma2)
{
// function [gamma, xi, loglik, gamma2] = forwards_backwards_mix(prior, transmat, obslik, obslik2, mixmat, filter_only) 
// FORWARDS_BACKWARDS_MIX Compute the posterior probs. in an HMM using the forwards backwards algo.
//
// Use [GAMMA, XI, LOGLIK] = FORWARDS_BACKWARDS(PRIOR, TRANSMAT, OBSLIK)
// for HMMs where the Y(t) depends only on Q(t).
// Use OBSLIK = MK_DHMM_OBS_LIK(DATA, B) or OBSLIK = MK_GHMM_OBS_LIK(DATA, MU, SIGMA) first.
// If the sequence is of length 1, XI will have size S*S*0.
//
// Use [GAMMA, XI, LOGLIK, GAMMA2] = FORWARDS_BACKWARDS(PRIOR, TRANSMAT, OBSLIK, OBSLIK2, MIXMAT)
// for HMMs where Y(t) depends on Q(t) and M(t), the mixture component.
// Use [OBSLIK, OBSLIK2] = MK_MHMM_OBS_LIK(DATA, MU, SIGMA, MIXMAT) first.
// 
// Use [GAMMA, XI, LOGLIK, GAMMA2] = FORWARDS_BACKWARDS(PRIOR, TRANSMAT, OBSLIK)
//
// FORWARDS_BACKWARDS(PRIOR, TRANSMAT, OBSLIK, [], [], FILTER_ONLY) with FILTER_ONLY = 1
// will just run the forwards algorithm.
// 
// Inputs:
// PRIOR(I) = Pr(Q(1) = I)
// TRANSMAT(I,J) = Pr(Q(T+1)=J | Q(T)=I)
// OBSLIK(I,T) = Pr(Y(T) | Q(T)=I)
//
// For mixture models only:
// OBSLIK2(I,K,T) = Pr(Y(T) | Q(T)=I, M(T)=K)
// MIXMAT(I,K) = Pr(M(T)=K | Q(T)=I)
//
// Outputs:
// gamma(i,t) = Pr(X(t)=i | O(1:T))
// xi(i,j,t)  = Pr(X(t)=i, X(t+1)=j | O(1:T)) t <= T-1
// gamma2(j,k,t) = Pr(Q(t)=j, M(t)=k | O(1:T))

	bool filter_only = false;

	CVisDVector scale (T);
	scale = 1;
	double loglik = 0;

	// this is not returned.
	CVisDMatrix alpha (m_Q, T);
	alpha = 0;

	//gamma = 0;
	for (int i = 1; i <= T; i++)
		xi[i] = 0;

	int t = 1;
	for (int q = 1; q <= m_Q; q++)
		alpha(q, t) = m_init_state_prob(q) * obslik(q, t);

	//[alpha(:,t), scale(t)] = normalise(alpha(:,t)); the fw only was different??? Why?
	scale(t) = NormaliseColumn (alpha, t);

	CVisDVector tmp (m_Q);

	//transmat2 = transmat';
	for (t = 2; t <= T; t++)
	{  
		MultiplySpecial (m_trans_mat, alpha, t-1, tmp);
		for (q = 1; q <= m_Q; q++)
			alpha(q,t) = tmp(q) * obslik(q,t);

		scale(t) = NormaliseColumn (alpha, t);

		if (filter_only)
		{
			for (q = 1; q <= m_Q; q++)
				for (int q2 = 1; q2 <= m_Q; q2++)
				{
					xi[t-1](q,q2) = m_trans_mat(q,q2) * (alpha(q,t-1) * obslik(q2,t));
				}

			Normalise (xi[t-1]);
		}
	}

	bool impossible = false;
	for (i = 1; i <= T; i++)
	{
		loglik += (log(scale(i)));
		//if (scale(i) == 1.0)
		//{
		//	impossible = true;
		//}
	}

	// if impossible do something.
	if (impossible)
	{
		printf ("ForwardBackward - an almost impossible state encountered\n");
		loglik = -1e10;

		// it might also dump to file some diagnostics.
	}

	if (filter_only)
	{
		gamma = alpha;
		gamma2 = 0;
		return loglik;
	}

	CVisDMatrix beta (m_Q, T);
	beta = 0; 

	for (i = 1; i <= T; i++)
		gamma2[i] = 0;

	for (q = 1; q <= m_Q; q++)
	{
		beta(q,T) = 1;	
		gamma(q,T) = alpha(q,T) * beta(q,T);
	}
	
	NormaliseColumn (gamma, T);

	t = T;
	
	for (q = 1; q <= m_Q; q++)
		tmp(q) = (obslik(q,t) == 0.0) ? 1.0 : obslik(q,t);
		//denom = obslik(:,t) + (obslik(:,t)==0); % replace 0s with 1s before dividing

	for (q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
			gamma2[t] (q, m) = obslik2[t] (q, m) * m_mix_mat(q, m) * gamma(q,t) / tmp(q);

	//gamma2(:,:,t) = obslik2(:,:,t) .* mixmat .* repmat(gamma(:,t), [1 M]) ./ repmat(denom, [1 M]);
	CVisDVector b(m_Q);

	for (t = T-1; t >= 1; t--)
	{
		for (q = 1; q <= m_Q; q++)
			b(q) = beta(q,t+1) * obslik(q,t+1); 

		tmp = m_trans_mat * b;

		Normalise (tmp);
		for (q = 1; q <= m_Q; q++)
		{
			beta(q,t) = tmp(q);
			tmp(q) = alpha(q,t) * beta(q,t);
		}

		Normalise (tmp);
		for (q = 1; q <= m_Q; q++)
		{
			gamma(q,t) = tmp(q);
		}

		for (q = 1; q <= m_Q; q++)
			for (int q2 = 1; q2 <= m_Q; q2++)
			{
				xi[t](q,q2) = m_trans_mat(q,q2) * (alpha(q,t) * b(q2));
			}

		Normalise (xi[t]);

		//xi(:,:,t) = normalise((transmat .* (alpha(:,t) * b'))); 

		for (q = 1; q <= m_Q; q++)
			tmp(q) = (obslik(q,t) == 0.0) ? 1.0 : obslik(q,t);
		//denom = obslik(:,t) + (obslik(:,t)==0); % replace 0s with 1s before dividing

		for (q = 1; q <= m_Q; q++)
			for (int m = 1; m <= m_M; m++)
				gamma2[t] (q, m) = obslik2[t] (q, m) * m_mix_mat(q, m) * gamma(q,t) / tmp(q);

		//gamma2(:,:,t) = obslik2(:,:,t) .* mixmat .* repmat(gamma(:,t), [1 M]) ./ repmat(denom, [1 M]);
	}

	return loglik;
}


// compute the observation likelihood.
// data is the sequence to evaluate, T its length, and the results is in
//	matrix B sized [q][t]. B2 is a vector of length T of Q x M matrices.
void YARPMixtureHmm::ComputeObservationLikelihood (const CVisDVector *data, int T, CVisDMatrix& B, CVisDMatrix *B2)
{
	B = 0;

	switch (m_mix_comp)
	{
	case HmmFull:
		{
			printf ("ComputeObservationLikelihood - not implemented yet for full cov matrix\n");
			return;
		}
		break;

	case HmmConstant:
	case HmmSpherical:
	case HmmDiag:
		{
			for (int q = 1; q <= m_Q; q++)
			{
				for (int t = 1; t <= T; t++)
				{
					for (int m = 1; m <= m_M; m++)
					{
						B2[t] (q, m) = GaussianDiagonal (data[t], m_mu[q][m], m_sigma[q][m]);
						B (q, t) += (m_mix_mat(q, m) * B2[t](q,m));
					}
				}
			}
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////

// a bunch of methods to handle incremental likelihood computation.
// one sample at a time up to maxT temporal steps.
void YARPMixtureHmm::PrepareIncrementalObservation (int maxT)
{
	// allocate memory.
	m_io_maxt = maxT;
	m_io_obslik.Resize (m_Q, maxT);

	m_io_alpha.Resize (m_Q, maxT);
	m_io_alpha = 0;

	m_io_step = 1;

	m_io_loglik = 0;

	m_io_scale.Resize (maxT);
	m_io_scale = 1;
}

// just restart for a new sequence.
void YARPMixtureHmm::StartIncrementalObservation ()
{
	m_io_alpha = 0;
	m_io_step = 1;

	m_io_scale = 1;
	m_io_loglik = 0;
}

double YARPMixtureHmm::AddNewIncrementalObservation (const CVisDVector& data)
{
	ComputeOneSampleObservationLikelihood (data, m_io_step, m_io_obslik);
	double LL = OneSampleForwards (m_io_obslik, m_io_step, m_io_alpha);
	m_io_loglik += LL;

	m_io_step++;

	return m_io_loglik;
}

void YARPMixtureHmm::ClearIncrementalObservation ()
{
	// free memory.

	// not very much to do by now!
}

// compute the observation likelihood.
// data is the sequence to evaluate, T its length, and the results is in
//	matrix B sized [q][t]. B2 is a vector of length T of Q x M matrices.
void YARPMixtureHmm::ComputeOneSampleObservationLikelihood (const CVisDVector& data, int t, CVisDMatrix& B)
{
	for (int q = 1; q <= m_Q; q++)
		B(q, t) = 0;

	switch (m_mix_comp)
	{
	case HmmFull:
		{
			printf ("ComputeOneSampleObservationLikelihood - not implemented yet for full cov matrix\n");
			return;
		}
		break;

	case HmmConstant:
	case HmmSpherical:
	case HmmDiag:
		{
			for (q = 1; q <= m_Q; q++)
			{
				for (int m = 1; m <= m_M; m++)
				{
					const double B2 = GaussianDiagonal (data, m_mu[q][m], m_sigma[q][m]);
					B (q, t) += (m_mix_mat(q, m) * B2);
				}
			}
		}
		break;
	}
}

// Single step Forwards
double YARPMixtureHmm::OneSampleForwards (const CVisDMatrix& obslik, int t, CVisDMatrix& alpha)
{
	// scale(t) = 1/Pr(O(t) | O(1:t-1))
	// Hence prod_t 1/scale(t) = Pr(O(1)) Pr(O(2)|O(1)) Pr(O(3) | O(1:2)) ... = Pr(O(1), ... ,O(T)) = P
	// or log P = - sum_t log scale(t)
	//
	// Note, scale(t) = 1/c(t) as defined in Rabiner
	// Hence we divide beta(t) by scale(t).
	// Alternatively, we can just normalise beta(t) at each step.

	double loglik = 0;
	//prior = prior(:); 

	// alpha is Q x T
	if (t == 1)
	{
		for (int q = 1; q <= m_Q; q++)
			alpha(q, t) = m_init_state_prob(q) * obslik(q, t);

		// LATER: n can be removed.
		// [alpha(:,t), n] = normalise(alpha(:,t));
		const double n = NormaliseColumn (alpha, t);
		m_io_scale(t) = 1.0 / n; // scale(t) = 1/(sum_i alpha(i,t))
	}
	else
	{
		if (t > m_io_maxt)
		{
			printf ("***this is a bug in calling OneSampleForwards\n");
			printf ("***please check your code\n");
			exit(0);
		}

		CVisDVector tmp (m_Q);

		// transmat' * alpha (:,t-1).
		MultiplySpecial (m_trans_mat, alpha, t-1, tmp);

		for (int q = 1; q <= m_Q; q++)
		{
			alpha (q, t) = tmp (q) * obslik (q, t);
		}

		const double n = NormaliseColumn (alpha, t);
		m_io_scale(t) = 1.0 / n;
	}

	loglik -= (log(m_io_scale(t)));
	if (m_io_scale(t) == 1.0)
	{
		printf ("OneStepForwards - an almost impossible state encountered\n");
		// should be an impossible likelihood.
		loglik = -1e10;

		// dump to file.
		FILE * debug = fopen ("emergency.txt", "w");
		assert (debug != NULL);

		fprintf (debug, "a sequence is definitely close to be impossible\n");
		fprintf (debug, "obslik:\n");
		SerializeMatrix (debug, obslik);
		fprintf (debug, "alpha:\n");
		SerializeMatrix (debug, alpha);
		fprintf (debug, "scale:\n");
		SerializeVector (debug, m_io_scale);

		fclose (debug);
	}

	return loglik;
}

////////////////////////////////////////////////////////////////////


// as usual data[nex][T].
// compute the loglikelihood of the data for the hmm.
// here data are supposed to be all the same length (wouldn't be nice to handle variable length sequences?).
double YARPMixtureHmm::ComputeLogLikelihood (const CVisDVector** data, int nex, int T)
{
	CVisDMatrix obslik (m_Q, T);
	
	CVisDMatrix alpha (m_Q, T);
	alpha = 0;

	// not very useful now.
	CVisDMatrix *xi = new CVisDMatrix[T];
	assert (xi != NULL);
	xi--;
	for (int i = 1; i <= T; i++)
		xi[i].Resize (m_Q, m_Q);

	double loglik = 0;
	for (int m = 1; m <= nex; m++)
	{
		ComputeObservationLikelihood (data[m], T, obslik);
		//  [alpha, xi, LL] = forwards(prior, transmat, obslik);

		double LL = ForwardsDebug (obslik, T, alpha, xi);
		loglik += LL;
	}

	// LATER: optimize out the allocation.
	xi++;
	if (xi != NULL)
		delete[] xi;

	return loglik;
}

// Outputs:
// alpha(i,t) = Pr(X(t)=i | O(1:t))
// xi[t](i,j)  = Pr(X(t)=i, X(t+1)=j | O(1:t+1)), t <= T-1
//
double YARPMixtureHmm::ForwardsDebug (const CVisDMatrix& obslik, int T, CVisDMatrix& alpha, CVisDMatrix *xi)
{
	FILE *debug = fopen ("debug.txt", "w");
	assert (debug != NULL);

	bool scaled = true;
	CVisDVector scale(T);
	scale = 1;

	// scale(t) = 1/Pr(O(t) | O(1:t-1))
	// Hence prod_t 1/scale(t) = Pr(O(1)) Pr(O(2)|O(1)) Pr(O(3) | O(1:2)) ... = Pr(O(1), ... ,O(T)) = P
	// or log P = - sum_t log scale(t)
	//
	// Note, scale(t) = 1/c(t) as defined in Rabiner
	// Hence we divide beta(t) by scale(t).
	// Alternatively, we can just normalise beta(t) at each step.

	double loglik = 0;
	//prior = prior(:); 

	// alpha is Q x T
	alpha = 0;

	// xi is a vector of length T of matrices of size Q x Q (transmat)
	for (int i = 1; i <= T; i++)
	{
		xi[i] = 0;
	}

	fprintf (debug, "Initializing:\n");
	fprintf (debug, "obslik matrix\n");
	SerializeMatrix (debug, obslik);
	fprintf (debug, "m_init_state_prob\n");
	SerializeVector (debug, m_init_state_prob);

	int t = 1;
	for (int q = 1; q <= m_Q; q++)
		alpha(q, t) = m_init_state_prob(q) * obslik(q, t);

	fprintf (debug, "alpha step 1:\n");
	SerializeMatrix (debug, alpha);

	if (scaled)
	{
		// LATER: n can be removed.
		// [alpha(:,t), n] = normalise(alpha(:,t));
		double n = NormaliseColumn (alpha, t);
		scale(t) = 1.0 / n; // scale(t) = 1/(sum_i alpha(i,t))
	}

	fprintf (debug, "alpha after scaling step 1:\n");
	SerializeMatrix (debug, alpha);
	fprintf (debug, "vector scale:\n");
	SerializeVector (debug, scale);
	fprintf (debug, "\n\n\n\n");

	CVisDVector tmp (m_Q);

	// transmat2 = transmat'; Why?
	for (t = 2; t <= T; t++)
	{
		fprintf (debug, "init step %d\n", t);
		fprintf (debug, "before multiply spec\n");
		fprintf (debug, "transition matrix:\n");
		SerializeMatrix (debug, m_trans_mat);
		fprintf (debug, "alpha:\n");
		SerializeMatrix (debug, alpha);
		
		// transmat' * alpha (:,t-1).
		MultiplySpecial (m_trans_mat, alpha, t-1, tmp);

		fprintf (debug, "tmp\n");
		SerializeVector (debug, tmp);

		for (q = 1; q <= m_Q; q++)
		{
			//alpha(q,t) = (transmat2 * alpha(:,t-1)) .* obslik(:,t);
			alpha (q, t) = tmp (q) * obslik (q, t);
		}

		fprintf (debug, "new alpha:\n");
		SerializeMatrix (debug, alpha);

		if (scaled)
		{
			double n = NormaliseColumn (alpha, t);
			scale(t) = 1.0 / n;
		}

		fprintf (debug, "new alpha after normalization:\n");
		SerializeMatrix (debug, alpha);
		fprintf (debug, "vector scale:\n");
		SerializeVector (debug, scale);

		fprintf (debug, "\n\n\n\n");

		for (q = 1; q <= m_Q; q++)
			for (int q2 = 1; q2 <= m_Q; q2++)
			{
				xi[t-1](q,q2) = m_trans_mat(q,q2) * (alpha(q,t-1) * obslik(q2,t));
			}

		Normalise (xi[t-1]);

		//normalise((alpha(:,t-1) * obslik(:,t)') .* transmat);
	}

	if (scaled)
	{
		bool impossible = false;
		for (int i = 1; i <= T; i++)
		{
			loglik -= (log(scale(i)));
			if (scale(i) == 1.0)
			{
				impossible = true;
			}
		}

		if (impossible)
		{
			printf ("Forwards - an almost impossible state encountered\n");
			// should be an impossible likelihood.
			loglik = -1e10;
		}
	}
	else
	{
		double lik = NormaliseColumn (alpha, T);
		loglik = log(lik);
	}

	fclose (debug);

	return loglik;
}

// Forwards
double YARPMixtureHmm::Forwards (const CVisDMatrix& obslik, int T, CVisDMatrix& alpha, CVisDMatrix *xi)
{
	bool scaled = true;
	CVisDVector scale(T);
	scale = 1;

	// scale(t) = 1/Pr(O(t) | O(1:t-1))
	// Hence prod_t 1/scale(t) = Pr(O(1)) Pr(O(2)|O(1)) Pr(O(3) | O(1:2)) ... = Pr(O(1), ... ,O(T)) = P
	// or log P = - sum_t log scale(t)
	//
	// Note, scale(t) = 1/c(t) as defined in Rabiner
	// Hence we divide beta(t) by scale(t).
	// Alternatively, we can just normalise beta(t) at each step.

	double loglik = 0;
	//prior = prior(:); 

	// alpha is Q x T
	alpha = 0;

	// xi is a vector of length T of matrices of size Q x Q (transmat)
	for (int i = 1; i <= T; i++)
	{
		xi[i] = 0;
	}

	int t = 1;
	for (int q = 1; q <= m_Q; q++)
		alpha(q, t) = m_init_state_prob(q) * obslik(q, t);

	if (scaled)
	{
		// LATER: n can be removed.
		// [alpha(:,t), n] = normalise(alpha(:,t));
		double n = NormaliseColumn (alpha, t);
		scale(t) = 1.0 / n; // scale(t) = 1/(sum_i alpha(i,t))
	}

	CVisDVector tmp (m_Q);

	// transmat2 = transmat'; Why?
	for (t = 2; t <= T; t++)
	{		
		// transmat' * alpha (:,t-1).
		MultiplySpecial (m_trans_mat, alpha, t-1, tmp);

		for (q = 1; q <= m_Q; q++)
		{
			//alpha(q,t) = (transmat2 * alpha(:,t-1)) .* obslik(:,t);
			alpha (q, t) = tmp (q) * obslik (q, t);
		}

		if (scaled)
		{
			double n = NormaliseColumn (alpha, t);
			scale(t) = 1.0 / n;
		}

		for (q = 1; q <= m_Q; q++)
			for (int q2 = 1; q2 <= m_Q; q2++)
			{
				xi[t-1](q,q2) = m_trans_mat(q,q2) * (alpha(q,t-1) * obslik(q2,t));
			}

		Normalise (xi[t-1]);

		//normalise((alpha(:,t-1) * obslik(:,t)') .* transmat);
	}

	if (scaled)
	{
		bool impossible = false;
		for (int i = 1; i <= T; i++)
		{
			loglik -= (log(scale(i)));
			if (scale(i) == 1.0)
			{
				impossible = true;
			}
		}

		if (impossible)
		{
			printf ("Forwards - an almost impossible state encountered\n");
			// should be an impossible likelihood.
			loglik = -1e10;

			// dump to file.
			FILE * debug = fopen ("emergency.txt", "w");
			assert (debug != NULL);

			fprintf (debug, "a sequence is definitely close to be impossible\n");
			fprintf (debug, "obslik:\n");
			SerializeMatrix (debug, obslik);
			fprintf (debug, "alpha:\n");
			SerializeMatrix (debug, alpha);
			fprintf (debug, "scale:\n");
			SerializeVector (debug, scale);

			fclose (debug);
		}
	}
	else
	{
		double lik = NormaliseColumn (alpha, T);
		loglik = log(lik);
	}

	return loglik;
}

// compute the observation likelihood.
// data is the sequence to evaluate, T its length, and the results is in
//	matrix out sized [q][t].
void YARPMixtureHmm::ComputeObservationLikelihood (const CVisDVector *data, int T, CVisDMatrix& out)
{
	out = 0;

	switch (m_mix_comp)
	{
	case HmmFull:
		{
			printf ("ComputeObservationLikelihood - not implemented yet for full cov matrix\n");
			return;
		}
		break;

	case HmmConstant:
	case HmmSpherical:
	case HmmDiag:
		{
			for (int q = 1; q <= m_Q; q++)
			{
				for (int t = 1; t <= T; t++)
				{
					for (int m = 1; m <= m_M; m++)
					{
						out (q, t) += (m_mix_mat(q, m) * GaussianDiagonal (data[t], m_mu[q][m], m_sigma[q][m]));
					}
				}
			}
		}
		break;
	}
}

// generate nex sequences of length T from the hmm. returns also the state seq.
void YARPMixtureHmm::Sample (CVisDVector **data, int **hidden, int nex, int T)
{
	for (int i = 1; i <= nex; i++)
		memset (&hidden[i][1], 0, sizeof(int) * T);

	for (i = 1; i <= nex; i++)
		for (int j = 1; j <= T; j++)
			data[i][j] = 0;

	SampleState (hidden, nex, T);

	CVisDVector mix(m_M);
	
	for (i = 1; i <= nex; i++)
	{
		for (int t = 1; t <= T; t++)
		{
			int q = hidden[i][t];
			for (int k = 1; k <= m_M; k++)
				mix(k) = m_mix_mat(q,k);

			// mixture coefficients are interpreted as the prob of getting
			// the output from the m-th gaussian component. (as they sum to 1?)
			int m = SampleDiscrete (mix);

			// generate the observation.
			switch (m_mix_comp)
			{
			case HmmFull:
				SampleGaussianMixture (m_mu[q][m], m_sigma[q][m], data[i][t]);
				break;

			case HmmSpherical:
			case HmmDiag:
			case HmmConstant:
				SampleGaussianMixtureDiagonal (m_mu[q][m], m_sigma[q][m], data[i][t]);
				break;
			}
		}
	}
}

// protected?
void YARPMixtureHmm::SampleState (int **hidden, int nex, int T)
{
	CVisDVector row (m_Q);

	for (int i = 1; i <= nex; i++)
	{
		hidden[i][1] = SampleDiscrete (m_init_state_prob);
		for (int t = 2; t <= T; t++)
		{
			// get state row.
			for (int k = 1; k <= m_Q; k++)
				row(k) = m_trans_mat (hidden[i][t-1], k);

			hidden[i][t] = SampleDiscrete (row);
		}
	}
}

// Serialization.
void YARPMixtureHmm::Serialize (const char *filename) const
{
	if (!m_initialized)
	{
		printf ("Hmm serialize - nothing to serialize\n");
		return;
	}

	FILE * fp = fopen (filename, "w");
	assert (fp != NULL);

	// size of the all stuff.
	fprintf (fp, "%d %d %d ", m_Q, m_M, m_O);
	fprintf (fp, "%d %d\n", m_mix_comp, m_left_right);

	SerializeVector (fp, m_init_state_prob);
	SerializeMatrix (fp, m_trans_mat);
	SerializeMatrix (fp, m_mix_mat);

	for (int q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
		{
			SerializeVector (fp, m_mu[q][m]);
			SerializeMatrix (fp, m_sigma[q][m]);
		}

	m_mixture_model.Serialize (fp);

	fclose (fp);
}

void YARPMixtureHmm::Unserialize (const char *filename)
{
	FILE * fp = fopen (filename, "r");
	assert (fp != NULL);

	if (!m_initialized)
		Uninit ();

	fscanf (fp, "%d %d %d ", &m_Q, &m_M, &m_O);
	int tmp;
	fscanf (fp, "%d %d\n", &m_mix_comp, &tmp);
	m_left_right = (tmp) ? true : false;

	Init (m_Q, m_M, m_O, m_mix_comp);

	UnserializeVector (fp, m_init_state_prob, m_Q);
	UnserializeMatrix (fp, m_trans_mat, m_Q, m_Q);
	UnserializeMatrix (fp, m_mix_mat, m_Q, m_M);

	// read mu & sigma.
	for (int q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
		{
			UnserializeVector (fp, m_mu[q][m], m_O);
			UnserializeMatrix (fp, m_sigma[q][m], m_O, m_O);
		}

	// unserialize the gaussian mixture.
	m_mixture_model.Unserialize (fp);

	fclose (fp);
}

// initialize usign the K-means algorithm. Probably not adequate for
// real applications - Viterbi traning could be better (of course).

// data is an array (nex) of sequences of length T.
// data[nex][t]
void YARPMixtureHmm::KMeansInit (const CVisDVector **data, int nex, int T, int ncycles)
{
	m_init_state_prob = 1.0 / m_Q;		// normalized.
	m_trans_mat = 1.0 / (m_Q);			// each row normalized.

	m_mix_mat =	1.0 / (m_M);			// each row normalized.

	m_mixture_model.Create (m_O, m_Q*m_M, m_mix_comp);

	// LATER: add a flag and skip all this if not necessary.

	// this is just init... we can spend some time here.
	// tune this loop to keep trying on new solutions.
	double mem_error = -1;

	for (int i = 1; i <= ncycles; i++)
	{
		m_mixture_model.Initialize (data, nex, T);

		if (mem_error < 0)
		{
			mem_error = m_mixture_model.GetError ();
			int counter = 1;
			for (int q = 1; q <= m_Q; q++)
				for (int m = 1; m <= m_M; m++)
				{
					m_mu[q][m] = m_mixture_model.Centres[counter];
					m_sigma[q][m] = m_mixture_model.Covariance[counter];
					counter ++;
				}
		}
		else
		{
			if (mem_error > m_mixture_model.GetError())
			{
				int counter = 1;
				for (int q = 1; q <= m_Q; q++)
					for (int m = 1; m <= m_M; m++)
					{
						m_mu[q][m] = m_mixture_model.Centres[counter];
						m_sigma[q][m] = m_mixture_model.Covariance[counter];
						counter ++;
					}
			}
		}
	}

	// copy back to the gauss mix.
	int counter = 1;
	for (int q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
		{
			m_mixture_model.Centres[counter] = m_mu[q][m];
			m_mixture_model.Covariance[counter] = m_sigma[q][m];
			counter ++;
		}

	// I understand now the fuzziness of this initialization:
	//	- the clusters found need not to bear any resemblance to the
	//	- actual states (of the hmm) nor to the actual meaning of
	//	- the gaussian mixture output... 

	// admitting the clusters are correctly identified by the kmeans.
}

// initialize usign the K-means algorithm. Probably not adequate for
// real applications - Viterbi traning could be better (of course).

// data is an array (nex) of sequences of length T.
// data[nex][t]
// 
// centers is Q x M vectors.
void YARPMixtureHmm::NonRndKMeansInit (const CVisDVector **data, int nex, int T, const CVisDVector **centers)
{
	m_init_state_prob = 1.0 / m_Q;		// normalized.
	m_trans_mat = 1.0 / (m_Q);			// each row normalized.

	m_mix_mat =	1.0 / (m_M);			// each row normalized.

	m_mixture_model.Create (m_O, m_Q*m_M, m_mix_comp);

	m_mixture_model.NonRndInitialize (data, nex, T, centers, m_Q, m_M);

	int counter = 1;
	for (int q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
		{
			m_mu[q][m] = m_mixture_model.Centres[counter];
			m_sigma[q][m] = m_mixture_model.Covariance[counter];
			counter ++;
		}
}

//
// random init.
void YARPMixtureHmm::RandomInit (const CVisDVector **data, int nex, int T, double sigma)
{
	m_init_state_prob = 1.0 / m_Q;		// normalized.
	m_trans_mat = 1.0 / (m_Q);			// each row normalized.

	m_mix_mat =	1.0 / (m_M);			// each row normalized.

	m_mixture_model.Create (m_O, m_Q*m_M, m_mix_comp);

	// get ncentres random vectors out of data.
	const int range = nex * T;

	// initialize the centers from data.
	for (int q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
		{
			int index = YARPRandom::Rand (0, range-1);
			m_mu[q][m] = data[index/T+1][index%T+1];
			Diag(m_sigma[q][m], sigma);
		}
	
	int counter = 1;
	for (q = 1; q <= m_Q; q++)
		for (int m = 1; m <= m_M; m++)
		{
			m_mixture_model.Centres[counter] = m_mu[q][m];
			m_mixture_model.Covariance[counter] = m_sigma[q][m];
			counter ++;
		}
}

//
// initialize from bare data. this is used to create a particular hmm.
void YARPMixtureHmm::DataInit (const CVisDVector& prior,
							   const CVisDMatrix& a,
							   const CVisDMatrix& mix,
							   const CVisDVector **mu,
							   const CVisDMatrix **sigma)
{
	m_init_state_prob = prior;
	m_trans_mat = a;
	m_mix_mat = mix;

	m_mixture_model.Create (m_O, m_Q*m_M, m_mix_comp);

	int counter = 1;
	for (int q = 1; q <= m_Q; q++)
	{
		for (int m = 1; m <= m_M; m++)
		{
			m_mu[q][m] = mu[q][m];
			m_sigma[q][m] = sigma[q][m];

			m_mixture_model.Centres[counter] = mu[q][m];
			m_mixture_model.Covariance[counter] = sigma[q][m];
			counter ++;
		}
	}
}

// initialize empty.
YARPMixtureHmm::YARPMixtureHmm ()
{
	m_mu = NULL;
	m_sigma = NULL;
	m_initialized = false;

	m_mix_comp = HmmConstant;

#ifdef _DEBUG
	debug = NULL;
#endif
}

void YARPMixtureHmm::Init (int Q, int M, int O, YARPVarianceModel model)
{
	if (m_initialized)
		_uninit (m_Q, m_M, m_O);

	_init (Q, M, O);

	// other stuff.
	m_init_state_prob.Resize (Q);
	m_init_state_prob = 0;

	m_trans_mat.Resize (Q, Q);
	m_trans_mat = 0;

	m_mix_mat.Resize (Q, M);
	m_mix_mat = 0;

	m_mix_comp = model;
	m_left_right = false;
}

void YARPMixtureHmm::Uninit ()
{
	// other stuff.


	if (m_initialized)
		_uninit (m_Q, m_M, m_O);
}

YARPMixtureHmm::~YARPMixtureHmm ()
{
	if (m_initialized)
		_uninit (m_Q, m_M, m_O);
}

void YARPMixtureHmm::_init (int Q, int M, int O)
{
	m_Q = Q;
	m_M = M;
	m_O = O;

	m_mu = new CVisDVector *[Q];
	assert (m_mu != NULL);
	m_mu --;
	m_sigma = new CVisDMatrix *[Q];
	assert (m_sigma != NULL);
	m_sigma --;

	for (int q = 1; q <= Q; q++)
	{
		m_mu[q] = new CVisDVector[M];
		m_sigma[q] = new CVisDMatrix[M];
		assert (m_mu[q] != NULL && m_sigma[q] != NULL);
		
		m_mu[q]--;
		m_sigma[q]--;

		for (int m = 1; m <= M; m++)
		{
			m_mu[q][m].Resize (O);
			m_sigma[q][m].Resize (O, O);
		}
	}

	m_initialized = true;
}

void YARPMixtureHmm::_uninit (int Q, int M, int O)
{
	for (int q = 1; q <= Q; q++)
	{
		m_mu[q]++;
		if (m_mu[q] != NULL) delete[] m_mu[q];
		m_sigma[q]++;
		if (m_sigma[q] != NULL) delete[] m_sigma[q];
	}

	m_mu++;
	if (m_mu != NULL) delete[] m_mu;
	m_sigma++;
	if (m_sigma != NULL) delete[] m_sigma;

	m_initialized = false;
}
