//
//
//
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "YARPHmm.h"

const int DIM = 2;
const int M = 1;
const int NSTATES = 6;

const double a_data[NSTATES][NSTATES] = 
{ 
	{0.1, 0.9, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.1, 0.9, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.1, 0.9, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.1, 0.9, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.1, 0.9},
	{0.9, 0.0, 0.0, 0.0, 0.0, 0.1}
};

const double a2_data[NSTATES][NSTATES] = 
{ 
	{0.1, 0.2, 0.7, 0.0, 0.0},
	{0.0, 0.1, 0.2, 0.7, 0.0},
	{0.0, 0.0, 0.1, 0.2, 0.7},
	{0.7, 0.0, 0.0, 0.1, 0.2},
	{0.2, 0.7, 0.0, 0.0, 0.1}
};

const double mix_data[NSTATES][M] = 
{
	{ 1.0 },
	{ 1.0 },
	{ 1.0 },
	{ 1.0 },
	{ 1.0 },
	{ 1.0 }
};

#if 0
const double mix_data2[NSTATES][M] = 
{
	{ 0.1, 0.9 },
	{ 0.9, 0.1 },
	{ 0.1, 0.9 },
	{ 0.9, 0.1 },
	{ 0.1, 0.9 },
	{ 0.9, 0.1 }
};
#endif

// assuming M=1
const double mu_data[NSTATES*M][DIM] = 
{
	{ 1.0, 1.0 },
	{ 1.0, 2.0 },
	{ 1.0, 3.0 },
	{ 1.0, 4.0 },
	{ 1.0, 5.0 },
	{ 1.0, 6.0 },
};


//
//
//
void main ()
{
	YARPRandom::Seed (time(NULL));

	YARPMixtureHmm hmm1;
	YARPMixtureHmm hmm2;

	hmm1.Init (NSTATES, M, DIM, HmmDiag);
	hmm2.Init (NSTATES, M, DIM, HmmDiag);

	printf ("choose what to do:\n");
	printf ("1) prepare data and save it\n");
	printf ("2) train HMM (single step and save), load data from file\n");
	int choice;
	scanf ("%d", &choice);

	switch (choice)
	{
	case 1:
		{
			// prepare the model.
			CVisDVector prior (NSTATES);
			prior = 1.0 / NSTATES;
			CVisDMatrix a (NSTATES, NSTATES);
			memcpy (a.data()[0], a_data, sizeof(double) * NSTATES * NSTATES);
			CVisDMatrix mix (NSTATES, M);
			memcpy (mix.data()[0], mix_data, sizeof(double) * NSTATES * M);

			CVisDVector **mu = AllocVector (NSTATES, M, DIM);
			CVisDMatrix **sigma = AllocMatrix (NSTATES, M, DIM);

			for (int q = 1; q <= NSTATES; q++)
			{
				for (int m = 1; m <= M; m++)
				{
					//memcpy (mu[q][m].data(), mu_data[(q-1)*M+(m-1)], sizeof(double) * DIM);
					sigma[q][m] = 0;

					for (int xx = 1; xx <= DIM; xx++)
					{
						mu[q][m](xx) = q;
						sigma[q][m](xx,xx) = 0.1;
					}
				}
			}

			hmm1.DataInit (prior, a, mix, (const CVisDVector **)mu, (const CVisDMatrix **)sigma);
			hmm1.Serialize ("test_1.txt");

			// get a bunch of sequences.
			const int nex = 100;
			const int T = 40;
			
			int **hidden = AllocStates (nex, T);
			CVisDVector **observation = AllocVector (nex, T, DIM);

			hmm1.Sample (observation, hidden, nex, T);

			// save observation to file.
			FILE *fp = fopen ("observation.txt", "w");
			assert (fp != NULL);

			for (int n = 1; n <= nex; n++)
				for (int t = 1; t <= T; t++)
					SerializeVector (fp, observation[n][t]);
			fclose (fp);

		
			FreeStates (hidden, nex);
			FreeVector (observation, nex);

			// delete
			FreeVector (mu, NSTATES);
			FreeMatrix (sigma, NSTATES);
		}
		break;

	case 2:
		{
#ifdef _DEBUG
			hmm2.debug = fopen ("debug.txt", "w");
			assert (hmm2.debug != NULL);
#endif
			// get a bunch of sequences.
			const int nex = 100;
			const int T = 40;
			
			CVisDVector **observation = AllocVector (nex, T, DIM);
			FILE *fp = fopen ("observation.txt", "r");
			assert (fp != NULL);

			for (int n = 1; n <= nex; n++)
				for (int t = 1; t <= T; t++)
					UnserializeVector (fp, observation[n][t], DIM);
			fclose (fp);

			printf ("1) reinit, 2) unserialize from test_2.txt\n");
			scanf("%d", &choice);

			if (choice == 1)
			{
				// try to learn machine 2 as machine 1. T is constant in this example.
				hmm2.KMeansInit ((const CVisDVector **)observation, nex, T, 1);
				hmm2.Serialize ("test_2.txt");
			}
			else
				hmm2.Unserialize ("test_2.txt");

			CVisDVector LL;
			CVisDVector length (nex);
			length = T;

			const int max_iter = 20;

			// Baum-Welch or/and Viterbi training.
//			hmm2.LearnViterbi ((const CVisDVector **)observation, length, nex, LL, 1);
			hmm2.Learn ((const CVisDVector **)observation, length, nex, LL, 10);
			hmm2.Serialize ("learnt_2.txt");

			double lik1 = hmm2.ComputeLogLikelihood ((const CVisDVector **)observation, nex, T);
			printf ("likelihood : %lf\n", lik1);

			FreeVector (observation, nex);

#ifdef _DEBUG
			fclose (hmm2.debug);
#endif
		}
		break;
	}
}
 
/*
void main ()
{
	YARPRandom::Seed (time(NULL));

	YARPMixtureHmm hmm1;
	YARPMixtureHmm hmm2;

	hmm1.Init (NSTATES, M, DIM, HmmDiag);
	hmm2.Init (NSTATES, M, DIM, HmmDiag);

	// prepare the model.
	CVisDVector prior (NSTATES);
	prior = 1.0 / NSTATES;
	CVisDMatrix a (NSTATES, NSTATES);
	memcpy (a.data()[0], a_data, sizeof(double) * NSTATES * NSTATES);
	CVisDMatrix mix (NSTATES, M);
	memcpy (mix.data()[0], mix_data, sizeof(double) * NSTATES * M);

	CVisDVector **mu = AllocVector (NSTATES, M, DIM);
	CVisDMatrix **sigma = AllocMatrix (NSTATES, M, DIM);

	for (int q = 1; q <= NSTATES; q++)
	{
		memcpy (mu[q][1].data(), mu_data[q-1], sizeof(double) * DIM);

		sigma[q][1] = 0;
		sigma[q][1](1,1) = 0.001;
		sigma[q][1](2,2) = 0.001;
	}

	hmm1.DataInit (prior, a, mix, (const CVisDVector **)mu, (const CVisDMatrix **)sigma);

	// fill a again.
	memcpy (a.data()[0], a2_data, sizeof(double) * NSTATES * NSTATES);
	hmm2.DataInit (prior, a, mix, (const CVisDVector **)mu, (const CVisDMatrix **)sigma);

	hmm1.Serialize ("test_1.txt");
	hmm2.Serialize ("test_2.txt");

	// get a bunch of sequences.
	const int nex = 10;
	const int T = 30;
	
	int **hidden = AllocStates (nex, T);
	CVisDVector **observation = AllocVector (nex, T, DIM);

	hmm1.Sample (observation, hidden, nex, T);

	FILE * fp;
	fp = fopen ("test_seq.txt", "w");
	assert (fp != NULL);

	for (int i = 1; i <= nex; i++)
	{
		for (int m = 1; m <= DIM; m++)
		{
			for (int j = 1; j <= T; j++)
			{
				fprintf (fp, "%f ", observation[i][j](m));
			}
			fprintf (fp, "\n");
		}
	}

	for (i = 1; i <= nex; i++)
	{
		for (int j = 1; j <= T; j++)
		{
			fprintf (fp, "%d ", hidden[i][j]);
		}
		fprintf (fp, "\n");
	}

	fclose (fp);

	// test the likelihood of both machines.
	double lik1 = hmm1.ComputeLogLikelihood ((const CVisDVector **)observation, nex, T);
	double lik2 = hmm2.ComputeLogLikelihood ((const CVisDVector **)observation, nex, T);

	printf ("machine 1 : %lf\n", lik1);
	printf ("machine 2 : %lf\n", lik2);

	FreeStates (hidden, nex);
	FreeVector (observation, nex);

	// delete
	FreeVector (mu, NSTATES);
	FreeMatrix (sigma, NSTATES);
}
*/
/*
void main ()
{
	YARPRandom::Seed (time(NULL));

	YARPMixtureHmm hmm;

	// create 5 centres in a 2D space.
	const int DIM = 2;
	const int M = 1;
	const int NSTATES = 10;

	hmm.Init (NSTATES, M, DIM, HmmDiag);

	// prepare data set.
	const int EXAMPLES = 10;
	const int T = 20;

	// sum to 200 samples;
	CVisDVector **data;
	data = new CVisDVector *[EXAMPLES];
	assert (data != NULL);
	data --;
	for (int i = 1; i <= EXAMPLES; i++)
	{
		data[i] = new CVisDVector[T];
		assert (data[i] != NULL);
		data[i]--;
		for (int j = 1; j <= T; j++)
		{
			data[i][j].Resize (DIM);
			data[i][j] = 0;
		}
	}

	CVisDVector centres[5];
	for (i = 1; i <= 5; i++)
		centres[i-1].Resize (DIM);

	centres[0](1) = 0.0;
	centres[0](2) = 0.0;
	centres[1](1) = 10.0;
	centres[1](2) = 10.0;
	centres[2](1) = 20.0;
	centres[2](2) = 20.0;
	centres[3](1) = 30.0;
	centres[3](2) = 30.0;
	centres[4](1) = 40.0;
	centres[4](2) = 40.0;

	// draw data out of some gaussian distributions.
	for (i = 1; i <= EXAMPLES; i++)
	{
		for (int j = 1; j <= T; j++)
		{
			int tcenter = YARPRandom::Rand (0,4);

			data[i][j](1) = YARPRandom::RandN ();
			data[i][j](2) = YARPRandom::RandN ();
			data[i][j] += centres[tcenter];
		}
	}

	hmm.KMeansInit ((const CVisDVector **)data, EXAMPLES, T);

	hmm.Serialize ("test.txt");

	hmm.Unserialize ("test.txt");
	hmm.Unserialize ("test.txt");
	hmm.Unserialize ("test.txt");
	hmm.Unserialize ("test.txt");
	hmm.Unserialize ("test.txt");
	hmm.Unserialize ("test.txt");

	hmm.Serialize ("test2.txt");
}
*/

/*
void main ()
{
	YARPRandom::Seed (time(NULL));

	YARPGaussianMixture mixture;

	// create 5 centres in a 2D space.
	const int DIM = 2;
	const int CENTERS = 5;
	mixture.Create (DIM, CENTERS, HmmConstant);

	// prepare data set.
	const int EXAMPLES = 10;
	const int T = 20;

	// sum to 200 samples;
	CVisDVector **data;
	data = new CVisDVector *[EXAMPLES];
	assert (data != NULL);
	data --;
	for (int i = 1; i <= EXAMPLES; i++)
	{
		data[i] = new CVisDVector[T];
		assert (data[i] != NULL);
		data[i]--;
		for (int j = 1; j <= T; j++)
		{
			data[i][j].Resize (DIM);
			data[i][j] = 0;
		}
	}

	CVisDVector centres[5];
	for (i = 1; i <= 5; i++)
		centres[i-1].Resize (DIM);

	centres[0](1) = 0.0;
	centres[0](2) = 0.0;
	centres[1](1) = 10.0;
	centres[1](2) = 10.0;
	centres[2](1) = 20.0;
	centres[2](2) = 20.0;
	centres[3](1) = 30.0;
	centres[3](2) = 30.0;
	centres[4](1) = 40.0;
	centres[4](2) = 40.0;

	CVisDVector backup_run[CENTERS];
	for (i = 1; i <= CENTERS; i++)
		backup_run[i-1].Resize (DIM);

	// draw data out of some gaussian distributions.
	for (i = 1; i <= EXAMPLES; i++)
	{
		for (int j = 1; j <= T; j++)
		{
			int tcenter = YARPRandom::Rand (0,4);

			data[i][j](1) = YARPRandom::RandN ();
			data[i][j](2) = YARPRandom::RandN ();
			data[i][j] += centres[tcenter];
		}
	}

	FILE * fp;
	fp = fopen ("test.txt", "w");
	assert (fp != NULL);

	double error = -1;

	for (int turns = 1; turns <= 10; turns++)
	{
		mixture.Initialize ((const CVisDVector **)data, EXAMPLES, T);
	
		if (error < 0)
			error = mixture.GetError();
		else
		{
			if (error > mixture.GetError())
			{
				error = mixture.GetError();

				// save the all run somewhere.
				for (int kk = 1; kk <= CENTERS; kk++)
					backup_run[kk] = mixture.Centres[kk];
			}
		}
#if 0
		// save data points.
		for (i = 1; i <= EXAMPLES; i++)
		{
			for (int j = 1; j <= T; j++)
			{
				fprintf (fp, "%f %f\n", data[i][j](1), data[i][j](2));			
			}
		}

		fprintf (fp, "\n\n");

		for (i = 1; i <= CENTERS; i++)
		{
			fprintf (fp, "%f %f\n", mixture.Centres[i](1), mixture.Centres[i](2));
		}

		fprintf (fp, "\n\n");

		for (i = 1; i <= 100; i++)
		{
			fprintf (fp, "%f\n", mixture.GetKMeansError()(i));
		}
#endif
	}	// loop on turns.


	// save data points.
	for (i = 1; i <= EXAMPLES; i++)
	{
		for (int j = 1; j <= T; j++)
		{
			fprintf (fp, "%f %f\n", data[i][j](1), data[i][j](2));			
		}
	}

	fprintf (fp, "\n\n");

	for (i = 1; i <= CENTERS; i++)
	{
		fprintf (fp, "%f %f\n", backup_run[i-1](1), backup_run[i-1](2));
	}

	fprintf (fp, "\n\n");

	fclose (fp);
}

*/